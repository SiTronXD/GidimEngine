
// This foam implementation is based on this presentation:
// Tim Tcheblokov, NVIDIA. "Ocean simulation and rendering in War Thunder".
// (http://developer.download.nvidia.com/assets/gameworks/downloads/regular/events/cgdc15/CGDC2015_ocean_simulation_en.pdf,
// accessed May 28, 2021)

cbuffer FoamMaskBuffer : register(b0)
{
	int gridWidth;
	int gridHeight;

	int padding[2];
};

RWTexture2D<float4> displacementTexture : register(u0);
RWTexture2D<float4> foamMaskTexture : register(u1);

// Repeat position within grid
uint2 repPos(uint2 pos)
{
	pos += uint2(gridWidth, gridHeight);
	pos.x = pos.x % gridWidth;
	pos.y = pos.y % gridHeight;

	return pos;
}

#define LAMBDA 1.2
#define U_SCALE 8.0

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	const float foamScale = 1.0;
	const float foamBias = 0.6;

	uint2 pos = dispatchThreadID.xy;

	// Estimate partial derivatives
	float2 dX =	(displacementTexture[repPos(pos + uint2(-1, 0))].rb -
				displacementTexture[repPos(pos + uint2(1, 0))].rb) * 0.5 * LAMBDA * U_SCALE;
	float2 dY = (displacementTexture[repPos(pos + uint2(0, -1))].rb -
				displacementTexture[repPos(pos + uint2(0, 1))].rb) * 0.5 * LAMBDA * U_SCALE;

	// Calculate the determinant of the jacobian matrix
	float jacobianDet = (1.0 + dX.x) * (1.0 + dY.y) - dX.y * dY.x;

	// Interpret determinant to create mask
	float col = saturate(foamScale * (-jacobianDet + foamBias)); //jacobianDet < foamBias ? 1.0 : 0.0;

	foamMaskTexture[dispatchThreadID.xy] = float4(col, col, col, 1.0);
}