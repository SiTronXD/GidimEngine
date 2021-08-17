
cbuffer BoidBuffer : register(b0)
{
	float deltaTime;

	int numBoids;

	float2 padding;
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

float3 getPos(uint boidIndex)
{
	return float3(
		boidTransforms[boidIndex]._41, 
		boidTransforms[boidIndex]._42, 
		boidTransforms[boidIndex]._43
	);
}

float3 getAlignment(uint id, float3 myPos, float3 myVelocity)
{
	const float alignRadiusSqrd = 5.0f * 5.0f;
	const float minAccel = 15.0f;
	const float maxAccel = 0.5f;

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
	const float cohesionRadiusSqrd = 2.0f * 2.0f;
	const float minAccel = 10.0f;
	const float maxAccel = 2.0f;

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
	const float separationRadiusSqrd = 1.0f * 1.0f;
	const float minAccel = 3.0f;
	const float maxAccel = 2.5f;

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

[numthreads(32, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;

	float3 oldPos = getPos(id);
	float3 oldVelocity = boidVelocities[id];

	// Apply boid rules
	float3 acceleration = float3(0.0f, 0.0f, 0.0f);
	acceleration += getAlignment(id, oldPos, oldVelocity);
	acceleration += getCohesion(id, oldPos, oldVelocity);
	acceleration += getSeparation(id, oldPos, oldVelocity);

	// Apply acceleration
	boidVelocities[id] += acceleration * deltaTime;

	float3 newPos = oldPos + boidVelocities[id] * deltaTime;

	// Keep boid positions within [-5, 5]
	const float halfSize = 5.0f;
	newPos = frac((newPos + float3(halfSize, halfSize, halfSize)) / 
		(float3(halfSize, halfSize, halfSize) * 2.0f)) * 
		(float3(halfSize, halfSize, halfSize) * 2.0f) - 
		float3(halfSize, halfSize, halfSize);

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