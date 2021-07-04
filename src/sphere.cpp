#include <core/sphere.hpp>

#define MATH_PI 3.14159265359

Sphere Sphere::getInstance(glm::vec3 _position, glm::vec3 _scale, int sectorCount, int stackCount, Shader *shader) {
    std::vector<Vertex> vertices = getVertices(1.0f, sectorCount, stackCount);
    std::vector<GLint> indices = getIndices(sectorCount, stackCount);
    std::vector<Vertex> transformedVertices;

    float r = 1.0f;

    for (Vertex v : vertices) {
        Vertex newVertice = {
           v.position[0] * _scale.x,
           v.position[1] * _scale.y,
           v.position[2] * _scale.z,
           v.color[0],
           v.color[1],
           v.color[2]
        };
        transformedVertices.push_back(newVertice);
    }

    Sphere instance(indices, transformedVertices, shader);
    instance.translate(_position);

    return instance;
}

Sphere::Sphere(std::vector<GLint> indices, 
           std::vector<Vertex> vertex,
           Shader *shader) : Mesh(indices, vertex, shader) { }

std::vector<Vertex> Sphere::getVertices(float r, int sectorCount, int stackCount) {
    std::vector<Vertex> vertices;

    float x, y, z, xy;

    float sectorStep = 2 * MATH_PI / sectorCount;
    float stackStep = MATH_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i) {
        stackAngle = MATH_PI / 2 - i * stackStep;       // starting from pi/2 to -pi/2
        xy = r * cosf(stackAngle);                      // r * cos(u)
        z = r * sinf(stackAngle);                       // r * sin(u)

        for(int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;               // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);                 // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);                 // r * cos(u) * sin(v)
            
            Vertex v = {
                x,    y,    z,   // POSITION
                map(x, -1.0f, 1.0f, 0, 1.0f),
                map(y, -1.0f, 1.0f, 0, 1.0f),
                map(z, -1.0f, 1.0f, 0, 1.0f)
            };

            vertices.push_back(v);
        }
    }
    return vertices;
}

std::vector<GLint> Sphere::getIndices(int sectorCount, int stackCount) {
    std::vector<GLint> indices;

    unsigned int k1, k2;
    for(int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {

            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    return indices;
}

float Sphere::map(float value, float low1, float high1, float low2, float high2) {
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}