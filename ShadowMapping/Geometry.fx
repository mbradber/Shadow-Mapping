
//definition of Light source struct
struct Light
{
	float3 position;
	float3 direction;
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float range;
};

//global variables
float4x4 wvp;
float4x4 lwvp;
float4x4 worldMatrix;
float3 eyePos;
Light lightSource;
Texture2D shadowMap;

SamplerState shadowSample
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct vsOut
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float4 diff	   : DIFFUSE;
    float4 spec    : SPECULAR;
	float4 projTex : TEXCOORD0;
};

vsOut VS(float3 pos:POSITION, float3 norm:NORMAL, float4 diff:DIFFUSE,
	float4 spec:SPECULAR)
{
	vsOut vOut;
	vOut.posH = mul(float4(pos, 1.0f), wvp);
	vOut.posW = mul(float4(pos, 1.0f), worldMatrix).rgb;
	vOut.normalW = mul(float4(norm, 0.0f), worldMatrix).rgb;
	vOut.diff = diff;
	vOut.spec = spec;
	vOut.projTex = mul(float4(pos, 1.0f), lwvp);

	return vOut;
}

float4 PS(vsOut pIn):SV_TARGET
{
	float3 calculatedColor = float3(0, 0, 0);

	//add the ambient light
	calculatedColor += (pIn.diff * lightSource.ambient).rgb;

	//calculate the dot product of the light source direction
	//and the face normal of this pixel
	float diffuseFactor = dot(-lightSource.direction, pIn.normalW);

	if(diffuseFactor > 0)
	{
		float sp = max(pIn.spec.a, 1);
		float3 eyeVec = normalize(eyePos - pIn.posW);
		float3 reflection = reflect(lightSource.direction, pIn.normalW);

		float sFactor = dot(reflection, eyeVec);
		sFactor = max(sFactor, 0);
		sFactor = pow(sFactor, sp);

		calculatedColor += (sFactor * pIn.spec * lightSource.specular).rgb;
		calculatedColor += (diffuseFactor * pIn.diff * lightSource.diffuse).rgb;
	}

	float4 projC = float4(pIn.projTex);
	//execute the homogenous divide
	projC.xyz /= projC.w;

	/*bool invalid = false;
	if(projC.x < -1 || projC.x > 1 || projC.y < -1 || projC.y > 1 || projC.z < 0)
		invalid = true;	*/

	//if(!invalid)
	//{
		projC.x = 0.5f * projC.x + 0.5f;
		projC.y = -1 * 0.5f * projC.y + 0.5f;

		float depth = projC.z;
		float sampleDepth = shadowMap.Sample(shadowSample, projC.xy).r;

		if(depth > sampleDepth + 0.0007)
			return pIn.diff * lightSource.ambient;
	//}

	return float4(calculatedColor, 1.0f);
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