#pragma once

#include<glm/vec3.hpp>

class Parameters
{
private:
    /* data */
public:
    Parameters();
    ~Parameters();
    
    glm::vec3 worldBounds;
    glm::vec3 noiseDisplacement;
    float noiseScale;
    float surfaceLevel;
    float surfaceResolution;
    float smoothIntersect;
    bool  smooth;
    bool  linearInterp;
    bool  useGPU;

};
