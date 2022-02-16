#include <core/marching_cubes_mesh_impl.hpp>

/***
 * Construtor do Programa
 **/
MarchingCubesMeshImpl::MarchingCubesMeshImpl(Parameters *_param, Shader *_baseShader, Point *_points) : Program(_param, _baseShader, _points)
{
    meshShader = new Shader("resources/shaders/mesh/marching.mesh", ShaderType::MESH_SHADER);
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
void MarchingCubesMeshImpl::update() 
{
    qX = param->surfaceResolution;
    qY = param->surfaceResolution;
    qZ = param->surfaceResolution;
}

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
    GLuint pointsCount = param->pointsCount;

    meshShader->enable();
    glm::mat4 mvpMatrix = Camera::MainCamera->getMVPMatrix(glm::mat4(1.0f));

    // MVP MATRIX UNIFORM
    int mvpLoc = glGetUniformLocation(meshShader->uId, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    // SURFACE LEVEL UNIFORM
    int slLoc = glGetUniformLocation(meshShader->uId, "u_surfaceLevel");
    glUniform1f(slLoc, param->surfaceLevel);

    // SMOOTH INTERSECTION UNIFORM
    int siLoc = glGetUniformLocation(meshShader->uId, "u_smooth");
    glUniform1f(siLoc, param->smoothIntersect);

    // LINEAR INTERSECTION UNIFORM
    int liLoc = glGetUniformLocation(meshShader->uId, "u_linear");
    glUniform1ui(liLoc, param->linearInterp ? 1 : 0);

    // POINTS COUNT UNIFORM
    int pcLoc = glGetUniformLocation(meshShader->uId, "u_pointsCount");
    glUniform1ui(pcLoc, pointsCount);

    // WORLD BOUNDS UNIFORM
    int wbLoc = glGetUniformLocation(meshShader->uId, "u_worldBounds");
    glUniform3f(wbLoc, param->worldBounds.x, param->worldBounds.y, param->worldBounds.z);

    // POINTS LEVEL UNIFORM
    GLfloat* pts = (GLfloat*) malloc(sizeof(float) * 4 * pointsCount);
    int curr = 0;
    for(int i = 0; i < pointsCount; i++) {
        pts[curr+0] = points[i].x;
        pts[curr+1] = points[i].y;
        pts[curr+2] = points[i].z;
        pts[curr+3] = 1.0f;
        curr+=4;
    }

    int ptsLoc = glGetUniformLocation(meshShader->uId, "u_points");
    glUniform4fv(ptsLoc, pointsCount, pts);

    // TrizTable UNIFORM
    const int tableSize = 4096; // 258 * 16
    GLint* triPts = (GLint*) malloc(sizeof(GLint) * tableSize);
    int c = 0;
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 16; j++) {
            triPts[c++] = triTable[i][j];
        }
    }

    int triLoc = glGetUniformLocation(meshShader->uId, "u_trizTable");
    glUniform1iv(triLoc, tableSize, triPts);

    //TODO: Adicionar controle de Meshlets.
    //Primeiro argumento: offset. 
    //Segundo argumento: quantidade de WorkGroups.
    glDrawMeshTasksNV(0, qX * qY * qZ);

    meshShader->disable();
}