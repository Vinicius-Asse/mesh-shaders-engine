#include<core/utils.hpp>

unsigned __int64 Utils::currentTimeInMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()
        .time_since_epoch())
        .count();
}

float Utils::remap(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    return toLow + (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
}

Vertex Utils::createVertex(Vec4 position, Vec4 normal) {
    return 
    {
        { position.x, position.y, position.z},
        { 1.0f, 1.0f, 1.0f },
        { normal.x, normal.y, normal.z }
    };
}

glm::vec3 Utils::getNormalVector(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm::vec3 dir = glm::cross((b - a), (c - a));
    return glm::normalize(dir);
}

float Utils::generateNoise(float _x, float _y, float _z, float scale, glm::vec3 displacement = glm::vec3(0,0,0)) {
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

float Utils::randomFloat(float min, float max) {
    return glm::linearRand(min, max);
}
