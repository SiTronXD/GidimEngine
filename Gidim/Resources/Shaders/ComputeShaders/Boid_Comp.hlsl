
#define THREAD_GROUP_SIZE 128

cbuffer BoidLogicBuffer : register(b0)
{
	float deltaTime;
	float halfVolumeSize;
	float maxSearchRadius;

	int numBoids;
};

RWStructuredBuffer<float3> boidVelocities : register(u0);
RWStructuredBuffer<float4x4> boidTransforms : register(u1);
RWStructuredBuffer<uint2> boidSortedList : register(u2);
RWStructuredBuffer<uint> boidListOffsets : register(u3);

// Set magnitude of vector
float3 setVecMag(float3 vec, float mag)
{
	return normalize(vec) * mag;
}

// Limit magnitude of vector
float3 limitVecMag(float3 vec, float maxMag)
{
	float magSqrd = dot(vec, vec);

	if (magSqrd > maxMag * maxMag)
	{
		vec /= sqrt(magSqrd);
		vec *= maxMag;
	}

	return vec;
}

// Extract position from transformation matrix
float3 getPos(uint boidIndex)
{
	return float3(
		boidTransforms[boidIndex]._41, 
		boidTransforms[boidIndex]._42, 
		boidTransforms[boidIndex]._43
	);
}

// Get cell ID that the boid resides within
uint getCellID(float3 worldPos)
{
	// From [-halfVolumeSize, halfVolumeSize] to [0.0, volumeSize]
	worldPos += float3(halfVolumeSize, halfVolumeSize, halfVolumeSize);

	// From [0, volumeSize] to [0, maxNumCells - 1]
	worldPos = floor(worldPos / maxSearchRadius);

	float numCellsSingleSide = halfVolumeSize * 2.0f / maxSearchRadius;

	uint foundID = uint(
		worldPos.x +
		worldPos.y * numCellsSingleSide +
		worldPos.z * numCellsSingleSide * numCellsSingleSide
	);

	return clamp(foundID, 0, numBoids);
}

// Values for boid behaviour
// (works best for 128 boids with a half play volume of 10)
#define accelerationScale 1.5f

#define minAccel 2.0f
#define maxAccel 0.25f

#define alignRadiusSqrd 4.0f
#define cohesionRadiusSqrd 25.0f
#define separationRadiusSqrd 1.0f

// One function incorporating all rules
float3 getAcceleration(uint id, float3 myPos, float3 myVelocity)
{
	float3 alignmentAccel = float3(0.0f, 0.0f, 0.0f);
	int alignmentBoidsInRadius = 0;
	float3 cohesionAccel = float3(0.0f, 0.0f, 0.0f);
	int cohesionBoidsInRadius = 0;
	float3 separationAccel = float3(0.0f, 0.0f, 0.0f);
	int separationBoidsInRadius = 0;

	// Loop through all boids
	/*for (int i = 0; i < numBoids; ++i)
	{
		float3 otherPos = getPos(i);
		float3 deltaPos = myPos - otherPos;
		float distSqrd = dot(deltaPos, deltaPos);

		// Make sure this boid is not my boid
		if (i != id)
		{
			// Alignment
			if (distSqrd <= alignRadiusSqrd)
			{
				alignmentAccel += boidVelocities[i];
				alignmentBoidsInRadius++;
			}

			// Cohesion
			if (distSqrd <= cohesionRadiusSqrd)
			{
				cohesionAccel += otherPos;
				cohesionBoidsInRadius++;
			}

			// Separation
			if (distSqrd <= separationRadiusSqrd)
			{
				separationAccel += deltaPos / max(sqrt(distSqrd), 0.1f);
				separationBoidsInRadius++;
			}
		}
	}*/

	// Loop through all neighboring boids
	for (int zo = -1; zo <= 1; ++zo)
	{
		for (int yo = -1; yo <= 1; ++yo)
		{
			for (int xo = -1; xo <= 1; ++xo)
			{
				// Create temporary position
				float3 tempPos = myPos + float3(xo, yo, zo) * maxSearchRadius;

				// Ignore boids outside play volume
				if (tempPos.x < -halfVolumeSize || tempPos.x > halfVolumeSize ||
					tempPos.y < -halfVolumeSize || tempPos.y > halfVolumeSize ||
					tempPos.z < -halfVolumeSize || tempPos.z > halfVolumeSize)
				{
					continue;
				}

				// Find IDs from temporary position
				uint tempCellID = getCellID(tempPos);
				uint boidOffsetID = boidListOffsets[tempCellID];

				// Iterate through all boids within cell
				while (boidOffsetID < 0xFFFFFFFF && boidOffsetID < numBoids)
				{
					// uint(cell ID, boid ID)
					uint2 neighborBoidIDs = boidSortedList[boidOffsetID];

					// Not iterating within same cell anymore
					if (neighborBoidIDs.x != tempCellID)
					{
						break;
					}

					// ---------- Start of evaluation ----------
					int i = neighborBoidIDs.y;
					float3 otherPos = getPos(i);
					float3 deltaPos = myPos - otherPos;
					float distSqrd = dot(deltaPos, deltaPos);

					// Make sure this boid is not my boid
					if (i != id)
					{
						// Alignment
						if (distSqrd <= alignRadiusSqrd)
						{
							alignmentAccel += boidVelocities[i];
							alignmentBoidsInRadius++;
						}

						// Cohesion
						if (distSqrd <= cohesionRadiusSqrd)
						{
							cohesionAccel += otherPos;
							cohesionBoidsInRadius++;
						}

						// Separation
						if (distSqrd <= separationRadiusSqrd)
						{
							separationAccel += deltaPos / max(sqrt(distSqrd), 0.1f);
							separationBoidsInRadius++;
						}
					}

					// ---------- End of evaluation ----------


					// Next boid within cell
					boidOffsetID++;
				}
			}
		}
	}

	// ---------- Alignment ----------
	if (alignmentBoidsInRadius > 0)
	{
		alignmentAccel /= float(alignmentBoidsInRadius);
		alignmentAccel = setVecMag(alignmentAccel, minAccel);
		alignmentAccel -= myVelocity;
		alignmentAccel = limitVecMag(alignmentAccel, maxAccel);
	}

	// ---------- Cohesion ----------
	if (cohesionBoidsInRadius > 0)
	{
		cohesionAccel /= float(cohesionBoidsInRadius);
		cohesionAccel -= myPos;
		cohesionAccel = setVecMag(cohesionAccel, minAccel);
		cohesionAccel -= myVelocity;
		cohesionAccel = limitVecMag(cohesionAccel, maxAccel);
	}

	// ---------- Separation ----------
	if (separationBoidsInRadius > 0)
	{
		separationAccel /= float(separationBoidsInRadius);
		separationAccel = setVecMag(separationAccel, minAccel);
		separationAccel -= myVelocity;
		separationAccel = limitVecMag(separationAccel, maxAccel);
	}

	return alignmentAccel + cohesionAccel + separationAccel;
}

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;

	float3 oldPos = getPos(id);
	float3 velocity = boidVelocities[id];

	// Apply boid rules
	float3 acceleration = getAcceleration(id, oldPos, velocity);

	// Apply acceleration to velocity
	velocity += acceleration * deltaTime * accelerationScale;

	float3 newPos = oldPos + velocity * deltaTime;

	// Keep boid positions wrapped within [-halfVolumeSize, halfVolumeSize]
	float3 halfSize = float3(halfVolumeSize, halfVolumeSize, halfVolumeSize);
	newPos = frac((newPos + halfSize) /
		(halfSize * 2.0f)) * (halfSize * 2.0f) -
		halfSize;

	// Direction vectors
	float3 forwardDir = normalize(velocity);
	float3 leftDir = normalize(cross(forwardDir, float3(0.0f, 1.0f, 0.0f)));
	float3 upDir = cross(leftDir, forwardDir);

	// Apply new velocity to the old one
	boidVelocities[id] = velocity;

	// Apply translation and rotation to the final matrix
	boidTransforms[id] = float4x4(
		float4(leftDir.xyz, 0.0f),
		float4(upDir.xyz, 0.0f),
		float4(forwardDir.xyz, 0.0f),
		float4(newPos, 1.0f)
	);
}