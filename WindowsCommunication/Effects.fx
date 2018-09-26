float4 VS(float4 inPos : POSITION) : SV_POSITION
{
	return inPos;
}

float4 PS() : SV_TARGET
{
	return float4(1.0, 0.0, 0.0, 1.0);
}