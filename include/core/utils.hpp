#pragma once

#include<chrono>
#include<glm/vec3.hpp>
#include<glm/gtc/noise.hpp>

#include<core/mesh.hpp>
#include<core/program.hpp>

class Utils {
public:
    static unsigned __int64 currentTimeInMillis();
    static float remap(float value, float fromLow, float fromHigh, float toLow, float toHigh);
    static Vertex createVertex(Vec4 position, Vec4 normal);
    static glm::vec3 getNormalVector(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    static float generateNoise(float, float, float, float, glm::vec3);
    static float randomFloat(float, float);
};

