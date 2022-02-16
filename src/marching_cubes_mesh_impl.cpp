#include <core/marching_cubes_mesh_impl.hpp>

/***
 * Construtor do Programa
 **/
MarchingCubesMeshImpl::MarchingCubesMeshImpl(Parameters *_param, Shader *_baseShader, Point *_points) : Program(_param, _baseShader, _points)
{
    meshShader = new Shader("resources/shaders/mesh/test.glsl", ShaderType::MESH_SHADER);
}

/***
 * Método Executado Quando o Programa é Iniciado
 **/
void MarchingCubesMeshImpl::start()
{
    LOG("Start from Compute Shader Implementation");

    wiredCube = Cube::getInstance(
        glm::vec3(0.0f, 0.0f, 0.0f),
        param->worldBounds,
        baseShader);
}

/***
 * Método Executado no Inicio de Cada Frame
 **/
void MarchingCubesMeshImpl::input(SDL_Event *e) {}

/***
 * Método Executado Toda Frame
 **/
void MarchingCubesMeshImpl::update() {}

/***
 * Método Executado ao Fim de Toda Frame
 **/
void MarchingCubesMeshImpl::draw()
{
    // Turn on wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    // Draw the box
    wiredCube->draw();

    // Turn off wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);

    executeMeshShader();
}

/***
 *
 * Métodos Privados para execução do algoritmo Marching Cubes
 *
 **/

void MarchingCubesMeshImpl::executeMeshShader()
{
    meshShader->enable();
    glm::mat4 mvpMatrix = Camera::MainCamera->getMVPMatrix(glm::mat4(1.0f));

    // MVP MATRIX UNIFORM
    int mvpLoc = glGetUniformLocation(meshShader->uId, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    glDrawMeshTasksNV(0, 1);
    meshShader->disable();
}