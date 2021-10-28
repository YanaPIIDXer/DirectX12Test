struct Output {
	float4 pos : POSITION;
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};

Output BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD) {
	Output output;
	output.pos = pos;
	output.svpos = pos;
	output.uv = uv;
	return output;
}
