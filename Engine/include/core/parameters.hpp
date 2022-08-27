#pragma once

#include<glm/vec3.hpp>

enum class Implementation { CPU=0, COMPUTE_SHADER=1, MESH_SHADER=2 };

class Parameters
{
    public:
        Parameters() {};
        
        glm::vec3 worldBounds       = glm::vec3(10.0f, 10.0f, 10.0f);
        glm::vec3 lightDirection    = glm::vec3(0.2f, -0.8f, -0.6f);
        int   pointsCount           = 10;
        float surfaceLevel          = -0.75f;
        float surfaceResolution     = 16.0;
        float smoothIntersect       = 2.0;
        float simulationSpeed       = 0.0f;
        float gravityForce          = 0.1f;
        bool  smooth                = false;
        bool  linearInterp          = true;
        bool  fullscreen            = false;
        bool  fixedLight            = false;
        bool  lockAspectRatio       = true;
        bool  wiredMesh             = false;
        bool  showWorldBounds       = false;
        Implementation  impl        = Implementation::CPU;
};