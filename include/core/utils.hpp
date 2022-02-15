#pragma once

#include<chrono>
#include<glm/vec3.hpp>
#include<glm/gtc/noise.hpp>

#define LOG_TRACE 1
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(msg) std::cout << "[" << __DATE__ << "]" << "[" << __TIME__ << "][INFO]\t: " << msg << std::endl;
#define TRACE(msg) if (LOG_TRACE) std::cout << "[" << __DATE__ << "]" << "[" << __TIME__ << "][TRACE]\t: " <<  msg << std::endl;

#include<core/mesh.hpp>
#include<core/marching_cubes.hpp>

class Utils {
public:
    static unsigned __int64 currentTimeInMillis();
    static float remap(float value, float fromLow, float fromHigh, float toLow, float toHigh);
    static Vertex createVertex(Vec4 position, Vec4 normal);
    static glm::vec3 getNormalVector(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    static float generateNoise(float, float, float, float, glm::vec3);
    static float randomFloat(float, float);
    static std::string readFile(std::string&);
    static int getLineId(int x, int y, int z, int max_x, int max_y, int max_z);
    static float smoothMin(float a, float b, float k);
};

