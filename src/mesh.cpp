#include<core/mesh.hpp>

Mesh::Mesh(Vertex *vertex, unsigned int count) {
    glGenBuffers(1, &uId);
    std::cout << "[" << std::to_string(uId) << "] * Mesh Buffer Created * " << std::endl;

    bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * count, vertex, GL_STATIC_DRAW);

    // Setting Positions Attribute
    addAttribute(0, GL_FLOAT, 3);

    unbind();
}

void Mesh::bind() {
    std::cout << "[" << std::to_string(uId) << "] Mesh Buffer Binded" << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, uId);
}

void Mesh::unbind() {
    std::cout << "[" << std::to_string(uId) << "] Mesh Buffer Unbinded" << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::addAttribute(unsigned int pos, unsigned int type, int count, bool normalized)
{
    glVertexAttribPointer(pos, count, GL_FLOAT, normalized? GL_FALSE : GL_TRUE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(pos);
}