#include<core/mesh.hpp>

Mesh::Mesh(std::vector<GLint> indices, std::vector<Vertex> vertex, Shader *_shader) {

    indicesCount = indices.size();

    shader = _shader;
    model = glm::mat4(1.0f);
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    shader->enable();
    bind();

    // Setting Up Data Buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex.size(), vertex.data(), GL_STATIC_DRAW);

    // Setting Up Index Buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indicesCount, indices.data(), GL_STATIC_DRAW);

    // Setting Positions Attribute
    addAttribute(GL_FLOAT, 3);

    // Setting Color Attribute
    addAttribute(GL_FLOAT, 3);

    // Setting Normal Attribute
    addAttribute(GL_FLOAT, 3);

    unbind();

    shader->disable();
}
 
// Mesh::~Mesh() {
//     std::cout << "Deleting Mesh" << std::endl;
//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteBuffers(1, &EBO);
// }

void Mesh::draw() {
    Camera* camera = Camera::MainCamera;
    glm::mat4 mvpMatrix = camera->getMVPMatrix(model);
    glm::vec3 lightDir = camera->lightDir;
    glm::vec3 ligthCol = camera->lightColor;

    shader->enable();

    // MVP MATRIX UNIFORM
    int mvpLoc = glGetUniformLocation(shader->uId, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    // MODEL MATRIX UNIFORM
    int modelLoc = glGetUniformLocation(shader->uId, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // LIGTH DIRECTION UNIFORM
    int lightDirLoc = glGetUniformLocation(shader->uId, "lightDir");
    glUniform4f(lightDirLoc, lightDir.x, lightDir.y, lightDir.z, 1.0f);

    // LIGTH COLOR UNIFORM
    int ligthColLoc = glGetUniformLocation(shader->uId, "ligthCol");
    glUniform4f(ligthColLoc, ligthCol.x, ligthCol.y, ligthCol.z, 1.0f);

    glBindVertexArray(VAO);
    glDrawElements(type, indicesCount, GL_UNSIGNED_INT, 0);

    shader->disable();
}

void Mesh::bind() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void Mesh::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::addAttribute(GLenum _type, int count, bool normalized)
{
    glVertexAttribPointer(currAttr, count, _type, normalized? GL_FALSE : GL_TRUE, sizeof(Vertex), (void*) nxtAttr);
    glEnableVertexAttribArray(currAttr);

    nxtAttr += sizeof(float) * count;
    currAttr++;
}

void Mesh::translate(glm::vec3 _position) {
    model = glm::translate(model, _position);
}

void Mesh::rotate(glm::vec3 _rotation) {
    model = glm::rotate(model, glm::radians(10.0f * (float) TimeDeltaTime), _rotation);
}

void Mesh::scale(glm::vec3 _scale) {
    model = glm::scale(model, _scale);
}

void Mesh::free() {
    //std::cout << "Frreing Mesh" << std::endl;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}