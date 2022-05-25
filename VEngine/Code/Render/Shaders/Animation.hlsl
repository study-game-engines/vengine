#include "Include/Common.hlsli"
#include "Include/TransformOut.hlsli"

VS_OUT VSMain(VS_IN i)
{
	TransformOut o;
	return o.TransformAnimation(i);
}

//@Todo: need to consolidate DefaultShader's PSMain and this one eventually
float4 PSMain(VS_OUT i) : SV_Target
{
    float4 texColour = float4(0.f, 0.f, 0.f, 0.f);
    
    if (material.useTexture)
    {
        texColour = t.Sample(s, i.uv);
        clip(texColour.a - 0.10f);
    }
    else
    {
        texColour = material.ambient;
    }

    float3 normal = normalize(i.normal);
    float4 position = i.posWS;

    float3 V = normalize(eyePosition - position).xyz;

    LightingResult endResult = CalcForwardLighting(V, position, normal);

    endResult.diffuse *= material.ambient;
    endResult.specular *= material.ambient;

    //LIGHT PROBES AND SHADOWS COMMENTED OUT FOR NOW
    
    //float4 shadowColour = float4(0.f, 0.f, 0.f, 0.f);
    //if (shadowsEnabled)
    //{
    //    shadowColour = CalcShadowFactor(i.shadowPos);
    //}
    
    //float4 shIrradiance = float4(GetSHIrradiance(i.normal, SH), 1.0f);
    
    float4 finalColour = (globalAmbient + endResult.diffuse + endResult.specular) * texColour;
    finalColour.a = material.ambient.a;
    return finalColour;
}
