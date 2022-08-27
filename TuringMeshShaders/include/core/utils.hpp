#pragma once

#include<chrono>
#include<glm/gtc/random.hpp>
#include<glm/vec3.hpp>
#include<glm/gtc/noise.hpp>

#define LOG_TRACE 1
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(msg) std::cout << "INFO  - [" << __DATE__ << "]" << "[" << __TIME__ << "] : " << msg << std::endl;
#define TRACE(msg) if (LOG_TRACE) std::cout << "TRACE - [" << __DATE__ << "]" << "[" << __TIME__ << "] : " <<  msg << std::endl;

#include<core/structs.h>

namespace Utils {
    static unsigned __int64 currentTimeInMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()
            .time_since_epoch())
            .count();
    }

    static float remap(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
        return toLow + (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
    }

    static Vertex createVertex(Vec4 position, Vec4 normal) {
        return {
            { position.x, position.y, position.z},
            { 1.0f, 1.0f, 1.0f },
            { normal.x, normal.y, normal.z }
        };
    }

    static glm::vec3 getNormalVector(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        glm::vec3 dir = glm::cross((b - a), (c - a));
        return glm::normalize(dir);
    }

    static float generateNoise(float _x, float _y, float _z, float scale, glm::vec3 displacement = glm::vec3(0, 0, 0)) {
        float x = _x / 100.0f * scale + displacement.x;
        float y = _y / 100.0f * scale + displacement.y;
        float z = _z / 100.0f * scale + displacement.z;

        float ab = glm::perlin(glm::vec2(x, y));
        float bc = glm::perlin(glm::vec2(y, z));
        float ac = glm::perlin(glm::vec2(x, z));
        float ba = glm::perlin(glm::vec2(y, x));
        float cb = glm::perlin(glm::vec2(z, y));
        float ca = glm::perlin(glm::vec2(z, x));

        float result = (ab + bc + ac + ba + cb + ca) / 6.0f;

        return Utils::remap(result, 0, 1.0f, 1, -1);
    }

    static float randomFloat(float min, float max) {
        return glm::linearRand(min, max);
    }

    static int getLineId(int x, int y, int z, int max_x, int max_y, int max_z) {
        return x + (max_y * y) + (max_y * max_z * z);
    }

    static float smoothMin(float a, float b, float k) {
        float h = std::max(k - std::abs(a - b), 0.0f) / k;
        return glm::min(a, b) - h * h * h * k * 1 / 6.0f;
    }
};

