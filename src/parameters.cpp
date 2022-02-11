#include<core/parameters.hpp>

// Default Parameters
Parameters::Parameters()
{
    worldBounds         = glm::vec3(10.0f, 10.0f, 10.0f);
    noiseDisplacement   = glm::vec3(0.0f , 0.0f , 0.0f);
    noiseScale          = 0.001f;
    surfaceLevel        = -0.75f;
    surfaceResolution   = 16.0f;
    smoothIntersect     = 2.0f;
    pointsCount         = 10;
    simulationSpeed     = 1.0f;
    gravityForce        = .01f;
    smooth              = false;
    linearInterp        = false;
    useGPU              = true;
}

Parameters::~Parameters() {}