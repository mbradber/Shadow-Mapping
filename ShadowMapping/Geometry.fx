float4x4 wvp;

struct vsOut
{
	float4 hPos: SV_POSITION;
	float4 color: COLOR;
};

vsOut VS(float4 Pos:POSITION, float4 Col:COLOR)
{
	vsOut vOut;
	vOut.hPos = mul(Pos, wvp);
	vOut.color = Col;
	return vOut;
}

float4 PS(vsOut pIn):SV_TARGET
{
	return pIn.color;
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