#pragma once

#include<glm/glm.hpp>
#include<vector>

#include<core/shader.hpp>
#include<core/mesh.hpp>

class Sphere : public Mesh {
private:

    static std::vector<Vertex> getVertices(float radius, int sectorCount, int stackCount);
    static std::vector<GLint> getIndices(int sectorCount, int stackCount);

    static float map(float value, float low1, float high1, float low2, float high2);

public:
    Sphere();
    Sphere(std::vector<GLint> indices, std::vector<Vertex> vertex, Shader *shader);
    //~Sphere();

    static Sphere* getInstance(glm::vec3 position, glm::vec3 scale, int sectorCount, int stackCount, Shader *shader);

    void onUpdate();
};