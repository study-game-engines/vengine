static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;
static const float PI = 3.14159265f;

struct InstanceData
{
    float4x4 modelMatrix;
    float4 colour;
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 weights : WEIGHTS;
    uint4 boneIndices : BONEINDICES;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 posWS : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float4 shadowPos : TEXCOORD1;
    uint instanceID : SV_InstanceID;
};

Texture2D t : register(t0);
Texture2D shadowMap : register(t1);
SamplerState s : register(s0);
SamplerComparisonState shadowSampler : register(s1);
StructuredBuffer<InstanceData> instanceData : register(t3);

cbuffer cbMatrices : register(b0)
{
	float4x4 model;
	float4x4 view;
	float4x4 proj;
	float4x4 mvp;
	float4x4 texMatrix;
	float4x4 lightMVP;
	float4x4 lightViewProj;
};

struct Material
{
	float4 ambient;
	float4 emissive;
	float4 diffuse;
	float4 specular;
	float2 uvOffset;
	float2 uvScale;
	float uvRotation;
    float smoothness;
    float metallic;
	bool useTexture;
};

cbuffer cbMaterials : register(b1)
{
	Material material;
}

cbuffer cbSkiningData : register(b2)
{
	float4x4 boneTransforms[96];
};

struct Light
{
	float4 position;
	float4 direction;
	float4 colour;
	float spotAngle;
    float intensity;
	int lightType;
	bool enabled;
};

static const int MAX_LIGHTS = 8;
static const int DIRECTIONAL_LIGHT = 0;
static const int POINT_LIGHT = 1;
static const int SPOT_LIGHT = 2;

struct LightingResult
{
	float4 diffuse;
	float4 specular;
};

cbuffer cbLights : register(b3)
{
	float4 eyePosition;
	float4 globalAmbient;
	int numLights;
	bool shadowsEnabled;
	int2 pad;
	Light lights[MAX_LIGHTS];
}

cbuffer cbTime : register(b4)
{
	float deltaTime;
	float timeSinceStartup;
}

cbuffer cbMeshData : register(b5)
{
	float3 meshPosition;
}

//Stole all this PBR code from Frostbite https://www.ea.com/frostbite/news/moving-frostbite-to-pb
//because it's specular calcs looked the simplest.
//Some more good references from https://blog.selfshadow.com/publications/s2013-shading-course/#course_content (UE4, Blackops)
//And also the dudes at Tri-Ace https://research.tri-ace.com/
float3 F_Schlick(in float3 f0, in float f90, in float u)
{
	return f0 + (f90 - f0) * pow(1.f - u , 5.f);
}

float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
	float alphaG2 = alphaG * alphaG;
	float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
	float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);
	return 0.5f / (Lambda_GGXV + Lambda_GGXL);
}

float D_GGX(float NdotH, float m)
{
	float m2 = m * m;
	float f = (NdotH * m2 - NdotH) * NdotH + 1;
	return m2 / (f * f);
}

float4 CalcDiffuse(Light light, float3 L, float3 N, float LdotH, float NdotL, float NdotV)
{
    return (light.colour * NdotL) / PI;
}

float4 CalcSpecularPBR(Light light, float NdotV, float NdotL, float NdotH, float LdotH)
{
    float smoothness = material.smoothness;

    float3 F = F_Schlick(NdotH, NdotV, LdotH);
    float Vis = V_SmithGGXCorrelated(NdotV, NdotL, smoothness);
    float D = D_GGX(NdotH, smoothness);
    float3 Fr = D * F * Vis;
    return float4(Fr, 1.0f) * light.colour;
}

float4 CalcSpecularBlinnPhong(Light light, float3 normal, float3 lightDir, float3 view, float specPower)
{
    float3 reflectDir = reflect(-lightDir, normal);
    float4 spec = pow(max(dot(view, reflectDir), 0.0), specPower);
    return spec * light.colour;
}

float CalcFalloff(Light light, float distance)
{
    return light.intensity / max(pow(distance, 2.f), 0.001f);
}

LightingResult CalcDirectionalLight(Light light, float3 normal, float3 V, float3 L,
	float distance, float NdotV, float NdotL, float NdotH, float LdotH)
{
    LightingResult result;
	result.diffuse = CalcDiffuse(light, L, normal, LdotH, NdotL, NdotV);
	result.specular = CalcSpecularPBR(light, NdotV, NdotL, NdotH, LdotH);
	return result;
}

LightingResult CalcPointLight(Light light, float3 V, float4 P, float3 N, float3 L,
	float distance, float NdotV, float NdotL, float NdotH, float LdotH)
{
	float falloff = CalcFalloff(light, distance);

    LightingResult result;
    result.diffuse = CalcDiffuse(light, L, N, LdotH, NdotL, NdotV) * falloff;
    result.specular = CalcSpecularPBR(light, NdotV, NdotL, NdotH, LdotH) * falloff;
	return result;
}

float CalcSpotCone(Light light, float3 L)
{
	float minCos = cos(light.spotAngle);
	float maxCos = (minCos + 1.0f) / 2.0f;
	float cosAngle = dot(light.direction.xyz, -L);
	return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult CalcSpotLight(Light light, float3 V, float4 P, float3 N, float3 L,
	float distance, float NdotV, float NdotL, float NdotH, float LdotH)
{
	LightingResult result;

	float falloff = CalcFalloff(light, distance);
	float spotIntensity = CalcSpotCone(light, L);

    result.diffuse = CalcDiffuse(light, L, N, LdotH, NdotL, NdotV) * falloff * spotIntensity;
    result.specular = CalcSpecularPBR(light, NdotV, NdotL, NdotH, LdotH) * falloff * spotIntensity;

	return result;
}

LightingResult CalcForwardLighting(float3 V, float4 position, float3 normal)
{
	LightingResult endResult;
	endResult.diffuse = float4(0.f, 0.f, 0.f, 0.f);
	endResult.specular = float4(0.f, 0.f, 0.f, 0.f);

    float NdotV = abs(dot(normal, V));

	[unroll]
	for (int i = 0; i < numLights; i++)
	{
		if (!lights[i].enabled)
		{
			continue;
		}

		//Dot products and vectors
        float3 L = (lights[i].position - position).xyz;
        float distance = length(L);
        L = L / distance;

        float3 H = normalize(V + L);

        float NdotL = saturate(dot(normal, L));
        float NdotH = saturate(dot(normal, H));
        float LdotH = saturate(dot(L, H));

		LightingResult result;
		result.diffuse = float4(0.f, 0.f, 0.f, 0.f);
		result.specular = float4(0.f, 0.f, 0.f, 0.f);

		//Main light switch calc
		switch (lights[i].lightType)
		{
		case POINT_LIGHT:
			result = CalcPointLight(lights[i], V, position, normal, L, distance, NdotV, NdotL, NdotH, LdotH);
			break;

		case SPOT_LIGHT:
            result = CalcSpotLight(lights[i], V, position, normal, L, distance, NdotV, NdotL, NdotH, LdotH);
			break;

		case DIRECTIONAL_LIGHT:
            result = CalcDirectionalLight(lights[i], normal, V, L, distance, NdotV, NdotL, NdotH, LdotH);
			break;
		}

		endResult.diffuse += result.diffuse;
		endResult.specular += result.specular;
	}

	endResult.diffuse = saturate(endResult.diffuse);
	endResult.specular = saturate(endResult.specular);

	return endResult;
}

float CalcShadowFactor(float4 shadowPos)
{
	shadowPos.xyz /= shadowPos.w;
	float depth = shadowPos.z;

	const float dx = SMAP_DX;
	float percentLit = 0.0f;

	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += shadowMap.SampleCmpLevelZero(shadowSampler, shadowPos.xy + offsets[i], depth).r;
	}

	return percentLit /= 9.f;
}
