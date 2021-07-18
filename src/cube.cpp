#include <core/cube.hpp>

Cube* Cube::getInstance(glm::vec3 _position, glm::vec3 _scale, Shader *shader) {
    std::vector<Vertex> transformedVertices;

    for (Vertex v : vertices) {
        Vertex newVertice = {
           v.position[0] * _scale.x,
           v.position[1] * _scale.y,
           v.position[2] * _scale.z,
           v.color[0],
           v.color[1],
           v.color[2],
           v.normals[0],
           v.normals[1],
           v.normals[2]
        };
        transformedVertices.push_back(newVertice);
    }

    Cube* instance = new Cube(indexes, transformedVertices, shader);
    instance->translate(_position);

    return instance;
}

Cube::Cube(std::vector<GLint> indices, 
           std::vector<Vertex> vertex,  
           Shader *shader) : Mesh(indices, vertex, shader) { }