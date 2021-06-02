#include <core/cube.hpp>

Cube Cube::getInstance(glm::vec3 position, glm::vec3 scale, Shader *shader) {
    Vertex newVertices[36];

    for (int i = 0; i < 36; i++) {
        newVertices[i] = {
            vertices[i].position[0] + position.x * scale.x,
            vertices[i].position[1] + position.y * scale.y,
            vertices[i].position[2] + position.z * scale.z,
            vertices[i].color[0],
            vertices[i].color[1],
            vertices[i].color[2]
        };
    }

    Cube instance(indexes, 36, newVertices, 8, shader);

    return instance;
}

Cube::Cube(GLint indices[], 
           unsigned int indicesCount,
           Vertex *vertex,  
           unsigned int vertexCount, 
           Shader *shader) : Mesh(indices, indicesCount, vertex, vertexCount, shader) { }