
// This sky implementation is based on this paper:
// A.J.Preetham, Peter Shirley, Brian Smits. "A Practical Analytic Model for Daylight".
// (https://www2.cs.duke.edu/courses/cps124/spring08/assign/07_papers/p91-preetham.pdf,
// accessed May 1, 2021)

cbuffer SkyboxBuffer
{
    float3 inputSunDir;
    float inputTurbidity;

    int faceWidth;
    int faceHeight;

    int padding[2];
};

RWTexture2DArray<float4> destTexture : register(u0);

#define _PI 3.141592

float2x2 rot(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    return float2x2(c, s, -s, c);
}

// Calculates distribution coefficients A, B, C, D and E
void calcDistCoeffs(float turbidity, inout float3 A, inout float3 B, inout float3 C,
    inout float3 D, inout float3 E)
{
    A = float3(0.1787 * turbidity - 1.4630, -0.0193 * turbidity - 0.2592, -0.0167 * turbidity - 0.2608);
    B = float3(-0.3554 * turbidity + 0.4275, -0.0665 * turbidity + 0.0008, -0.0950 * turbidity + 0.0092);
    C = float3(-0.0227 * turbidity + 5.3251, -0.0004 * turbidity + 0.2125, -0.0079 * turbidity + 0.2102);
    D = float3(0.1206 * turbidity - 2.5771, -0.0641 * turbidity - 0.8989, -0.0441 * turbidity - 1.6537);
    E = float3(-0.0670 * turbidity + 0.3703, -0.0033 * turbidity + 0.0452, -0.0109 * turbidity + 0.0529);
}

float calcZenith_Y(float turbidity, float thetaS)
{
    float chi = (4.0 / 9.0 - turbidity / 120.0) * (_PI - 2.0 * thetaS);

    return (4.0453 * turbidity - 4.9710) * tan(chi) - 0.2155 * turbidity + 2.4192;
}

float calcZenith_x(float turbidity, float turbiditySquared,
    float thetaS, float thetaSSquared, float thetaSCubed)
{
    float t1 = 0.00166 * thetaSCubed - 0.00375 * thetaSSquared + 0.00209 * thetaS + 0.0;
    float t2 = -0.02903 * thetaSCubed + 0.06377 * thetaSSquared - 0.03202 * thetaS + 0.00394;
    float t3 = 0.11693 * thetaSCubed - 0.21196 * thetaSSquared + 0.06052 * thetaS + 0.25886;

    return turbiditySquared * t1 + turbidity * t2 + 1.0 * t3;
}

float calcZenith_y(float turbidity, float turbiditySquared,
    float thetaS, float thetaSSquared, float thetaSCubed)
{
    float t1 = 0.00275 * thetaSCubed - 0.00610 * thetaSSquared + 0.00317 * thetaS + 0.0;
    float t2 = -0.04214 * thetaSCubed + 0.08970 * thetaSSquared - 0.04153 * thetaS + 0.00516;
    float t3 = 0.15346 * thetaSCubed - 0.26756 * thetaSSquared + 0.06670 * thetaS + 0.26688;

    return turbiditySquared * t1 + turbidity * t2 + 1.0 * t3;
}

float3 YxyToRGB(float3 Yxy)
{
    // Yxy to XYZ
    float Y = Yxy.x;
    float x = Yxy.y;
    float y = Yxy.z;

    float Y_over_y = Y / y;

    float X = Y_over_y * x;
    float Z = Y_over_y * (1.0 - x - y);

    // XYZ to RGB

    // CIE RGB, reference white point E
    float3x3 M = {
        2.3706743, -0.9000405, -0.4706338,
        -0.5138850, 1.4253036, 0.0885814,
        0.0052982, -0.0146949, 1.0093968
    };

    return mul(M, float3(X, Y, Z));
}

float3 perezLuminance(float theta, float gamma, float3 A, float3 B, float3 C, float3 D, float3 E)
{
    float cosGamma = cos(gamma);

    return (1.0 + A * exp(B / cos(theta))) * (1.0 + C * exp(D * gamma) + E * cosGamma * cosGamma);
}

float3 calculateZenithLuminanceYxy(in float t, in float thetaS)
{
    float chi = (4.0 / 9.0 - t / 120.0) * (_PI - 2.0 * thetaS);
    float Yz = (4.0453 * t - 4.9710) * tan(chi) - 0.2155 * t + 2.4192;

    float theta2 = thetaS * thetaS;
    float theta3 = theta2 * thetaS;
    float T = t;
    float T2 = t * t;

    float xz =
        (0.00165 * theta3 - 0.00375 * theta2 + 0.00209 * thetaS + 0.0) * T2 +
        (-0.02903 * theta3 + 0.06377 * theta2 - 0.03202 * thetaS + 0.00394) * T +
        (0.11693 * theta3 - 0.21196 * theta2 + 0.06052 * thetaS + 0.25886);

    float yz =
        (0.00275 * theta3 - 0.00610 * theta2 + 0.00317 * thetaS + 0.0) * T2 +
        (-0.04214 * theta3 + 0.08970 * theta2 - 0.04153 * thetaS + 0.00516) * T +
        (0.15346 * theta3 - 0.26756 * theta2 + 0.06670 * thetaS + 0.26688);

    return float3(Yz, xz, yz);
}

float3 getSkyColor(float3 viewDir, float3 sunDir, float turbidity)
{
    float3 A, B, C, D, E;
    calcDistCoeffs(turbidity, A, B, C, D, E);

    float3 Yxy = float3(0.0, 0.0, 0.0);

    float thetaS = acos(saturate(dot(sunDir, float3(0.0, 1.0, 0.0))));
    float theta = acos(saturate(dot(viewDir, float3(0.0, 1.0, 0.0))));
    float gamma = acos(clamp(dot(viewDir, sunDir), -1.0, 1.0));

    float thetaSSquared = thetaS * thetaS;
    float thetaSCubed = thetaSSquared * thetaS;
    float turbiditySquared = turbidity * turbidity;

    // Y
    Yxy.x = calcZenith_Y(turbidity, thetaS);

    // x
    Yxy.y = calcZenith_x(turbidity, turbiditySquared, thetaS, thetaSSquared, thetaSCubed);

    // y
    Yxy.z = calcZenith_y(turbidity, turbiditySquared, thetaS, thetaSSquared, thetaSCubed);

    float3 YP = Yxy * perezLuminance(theta, gamma, A, B, C, D, E) /
        perezLuminance(0.0, thetaS, A, B, C, D, E);

    return clamp(YxyToRGB(YP) * 0.05, 0.0, 1.0);
}

[numthreads(16, 16, 2)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // Sun
    float3 sunDir = normalize(inputSunDir);

    // XY: [-1, 1], Z: 0
    float3 uv = 
        float3(
            float2(dispatchThreadID.xy) / float2(faceWidth, faceHeight), 0.0
        ) * 2.0 - float3(1.0, 1.0, 0.0);
    uv.y *= -1.0; // Invert Y

    // Determine view based on current face
    float3 viewDir = float3(1.0, 0.0, 0.0);
    switch (dispatchThreadID.z)
    {
    case 0: // +X

        uv.x *= -1.0;
        viewDir = float3(1.0, 0.0, 0.0) + uv.zyx;

        break;

    case 1: // -X

        viewDir = float3(-1.0, 0.0, 0.0) + uv.zyx;

        break;

    case 2: // +Y

        uv.y *= -1.0;
        viewDir = float3(0.0, 1.0, 0.0) + uv.xzy;

        break;

    case 3: // -Y

        viewDir = float3(0.0, -1.0, 0.0) + uv.xzy;

        break;

    case 4: // +Z

        viewDir = float3(0.0, 0.0, 1.0) + uv.xyz;

        break;

    case 5: // -Z

        viewDir = float3(0.0, 0.0, -1.0) + uv.xyz;

        break;
    }

    // Normalize view direction
    viewDir = normalize(viewDir);

    // Color
    float3 col = getSkyColor(viewDir, sunDir, inputTurbidity);

    // "Sun"
    col = lerp(
        col, 
        float3(1.0, 1.0, 1.0), 
        smoothstep(0.993, 1.0, saturate(dot(sunDir, viewDir))) * 
            saturate(dot(sunDir, float3(0.0, 1.0, 0.0)))
    );

    // Set color in skybox
    destTexture[dispatchThreadID.xyz] = float4(col, 1.0);
}