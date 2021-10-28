struct Input {
	float4 pos : POSITION;
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 BasicPS(Input input) : SV_TARGET{
	return float4(input.uv, 1.0f, 1.0f);
}
