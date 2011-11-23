float4x4 wvp;

struct vsOut
{
	float4 posH: SV_POSITION;
};

vsOut VS(float3 pos:POSITION, float3 norm:NORMAL, float4 diff:DIFFUSE,
	float4 spec:SPECULAR)
{
	vsOut vOut;
	vOut.posH = mul(float4(pos, 1.0f), wvp);

	return vOut;
}

void PS(vsOut pIn)
{
	//return float4(0, 0, 0, 1);
}

technique10 T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}