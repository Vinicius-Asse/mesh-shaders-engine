#include<core/parameters.hpp>

// Default Parameters
Parameters::Parameters()
{
    worldBounds         = glm::vec3(10.0f, 10.0f, 10.0f);
    noiseDisplacement   = glm::vec3(0.0f , 0.0f , 0.0f);
    noiseScale          = 10.0f;
    surfaceLevel        = 0.0f;
    pointDencity        = 1.0f;
    smooth              = false;
    linearInterp        = false;
    useGPU              = false;
}

Parameters::~Parameters() {}