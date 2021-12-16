struct TransformOut
{
	VS_OUT Transform(VS_IN i)
	{
		VS_OUT o;

		float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		weights[0] = i.weights.x;
		weights[1] = i.weights.y;
		weights[2] = i.weights.z;
		weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

		float3 posL = float3(0.0f, 0.0f, 0.0f);
		float3 normalL = float3(0.0f, 0.0f, 0.0f);
		for (int index = 0; index < 4; ++index)
		{
			//no nonuniform scaling
			posL += weights[index] * mul(float4(i.pos, 1.0f),
				boneTransforms[i.boneIndices[index]]).xyz;
			normalL += weights[index] * mul(i.normal,
				(float3x3)boneTransforms[i.boneIndices[index]]);
		}

		o.pos = mul(mvp, float4(posL, 1.0f));
		o.posWS = mul(model, float4(i.pos, 1.0f));
		float4 newUv = mul(texMatrix, float4(i.uv, 0.f, 1.0f));
		o.uv = float2(newUv.x, newUv.y);
		o.normal = mul((float3x3)model, i.normal);
		o.shadowPos = mul(lightMVP, o.posWS);

		return o;
	}
};
