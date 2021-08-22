
cbuffer BoidLogicBuffer : register(b0)
{
	float deltaTime;
	float halfVolumeSize;

	int numBoids;

	float padding;
};

RWStructuredBuffer<float3> boidVelocities : register(u0);
RWStructuredBuffer<float4x4> boidTransforms : register(u1);

uint wang_hash(uint seed)
{
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);

	return seed;
}

float randomFloat(float state)
{
	return float(wang_hash(uint(state))) / 4294967296.0;
}

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

// Values for boid behaviour
// (works best for 128 boids with a half play volume of 10)
#define accelerationScale 1.5f

#define minAccel 2.0f
#define maxAccel 0.25f

#define alignRadiusSqrd 4.0f
#define cohesionRadiusSqrd 25.0f
#define separationRadiusSqrd 1.0f

// Old separate functions for each rule
float3 getAlignment(uint id, float3 myPos, float3 myVelocity)
{
	float3 currentAcceleration = float3(0.0f, 0.0f, 0.0f);
	int numBoidsInRadius = 0;

	// Loop through all boids
	for (int i = 0; i < numBoids; ++i)
	{
		float3 deltaPos = getPos(i) - myPos;

		// Check if this boid is not my boid, and if it is close enough
		if (i != id && dot(deltaPos, deltaPos) <= alignRadiusSqrd)
		{
			currentAcceleration += boidVelocities[i];
			numBoidsInRadius++;
		}
	}

	// Average acceleration
	if (numBoidsInRadius > 0)
	{
		currentAcceleration /= float(numBoidsInRadius);
		currentAcceleration = setVecMag(currentAcceleration, minAccel);
		currentAcceleration -= myVelocity;
		currentAcceleration = limitVecMag(currentAcceleration, maxAccel);
	}

	return currentAcceleration;
}
float3 getCohesion(uint id, float3 myPos, float3 myVelocity)
{
	float3 currentAcceleration = float3(0.0f, 0.0f, 0.0f);
	int numBoidsInRadius = 0;

	// Loop through all boids
	for (int i = 0; i < numBoids; ++i)
	{
		float3 otherPos = getPos(i);
		float3 deltaPos = otherPos - myPos;

		// Check if this boid is not my boid, and if it is close enough
		if (i != id && dot(deltaPos, deltaPos) <= cohesionRadiusSqrd)
		{
			currentAcceleration += otherPos;
			numBoidsInRadius++;
		}
	}

	// Average acceleration
	if (numBoidsInRadius > 0)
	{
		currentAcceleration /= float(numBoidsInRadius);
		currentAcceleration -= myPos;
		currentAcceleration = setVecMag(currentAcceleration, minAccel);
		currentAcceleration -= myVelocity;
		currentAcceleration = limitVecMag(currentAcceleration, maxAccel);
	}

	return currentAcceleration;
}
float3 getSeparation(uint id, float3 myPos, float3 myVelocity)
{
	float3 currentAcceleration = float3(0.0f, 0.0f, 0.0f);
	int numBoidsInRadius = 0;

	// Loop through all boids
	for (int i = 0; i < numBoids; ++i)
	{
		float3 otherPos = getPos(i);
		float3 deltaPos = myPos - otherPos;
		float distSqrd = dot(deltaPos, deltaPos);

		// Check if this boid is not my boid, and if it is close enough
		if (i != id && distSqrd <= separationRadiusSqrd)
		{
			currentAcceleration += deltaPos / max(sqrt(distSqrd), 0.1f);
			numBoidsInRadius++;
		}
	}

	// Average acceleration
	if (numBoidsInRadius > 0)
	{
		currentAcceleration /= float(numBoidsInRadius);
		currentAcceleration = setVecMag(currentAcceleration, minAccel);
		currentAcceleration -= myVelocity;
		currentAcceleration = limitVecMag(currentAcceleration, maxAccel);
	}

	return currentAcceleration;
}

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
	for (int i = 0; i < numBoids; ++i)
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

[numthreads(16, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;

	float3 oldPos = getPos(id);
	float3 oldVelocity = boidVelocities[id];

	// Apply boid rules
	/*float3 acceleration = float3(0.0f, 0.0f, 0.0f);
	acceleration += getAlignment(id, oldPos, oldVelocity);
	acceleration += getCohesion(id, oldPos, oldVelocity);
	acceleration += getSeparation(id, oldPos, oldVelocity);*/
	float3 acceleration = getAcceleration(id, oldPos, oldVelocity);

	// Apply acceleration
	boidVelocities[id] += acceleration * deltaTime * accelerationScale;

	float3 newPos = oldPos + boidVelocities[id] * deltaTime;

	// Keep boid positions within [-halfVolumeSize, halfVolumeSize]
	float3 halfSize = float3(halfVolumeSize, halfVolumeSize, halfVolumeSize);
	newPos = frac((newPos + halfSize) /
		(halfSize * 2.0f)) * (halfSize * 2.0f) -
		halfSize;

	// Direction vectors
	float3 forwardDir = normalize(boidVelocities[id]);
	float3 leftDir = normalize(cross(forwardDir, float3(0.0f, 1.0f, 0.0f)));
	float3 upDir = cross(leftDir, forwardDir);

	// Apply final matrix
	boidTransforms[id] = float4x4(
		float4(leftDir.xyz, 0.0f),
		float4(upDir.xyz, 0.0f),
		float4(forwardDir.xyz, 0.0f),
		float4(newPos, 1.0f)
	);
}