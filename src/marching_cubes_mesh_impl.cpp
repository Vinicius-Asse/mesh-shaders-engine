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
    GLuint sphereCount = param->pointsCount;

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

    // SPHERE COUNT UNIFORM
    int srLoc = glGetUniformLocation(meshShader->uId, "u_sphereCount");
    glUniform1ui(srLoc, sphereCount);

    // WORLD BOUNDS UNIFORM
    int wbLoc = glGetUniformLocation(meshShader->uId, "u_worldBounds");
    glUniform3f(wbLoc, param->worldBounds.x, param->worldBounds.y, param->worldBounds.z);

    // POINTS COUNT UNIFORM
    int pnLoc = glGetUniformLocation(meshShader->uId, "u_pointsCount");
    glUniform3ui(pnLoc, qX, qY, qZ);

    // SPHERES LEVEL UNIFORM
    GLfloat* pts = (GLfloat*) malloc(sizeof(float) * 4 * sphereCount);
    int curr = 0;
    for(int i = 0; i < sphereCount; i++) {
        pts[curr+0] = points[i].x;
        pts[curr+1] = points[i].y;
        pts[curr+2] = points[i].z;
        pts[curr+3] = 1.0f;
        curr+=4;
    }

    int ptsLoc = glGetUniformLocation(meshShader->uId, "u_spheres");
    glUniform4fv(ptsLoc, sphereCount, pts);

    // Setting up TrizTable SSBO
    GLuint trizTableSSBO;
    {
        glGenBuffers(1, &trizTableSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trizTableSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 4096 , NULL, GL_STATIC_READ);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, trizTableSSBO);

        GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
        int* trizTablePtr = (int*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * 4096, bufMask);

        int c = 0;
        for(int i = 0; i < 256; i++) {
            for(int j = 0; j < 16; j++) {
                trizTablePtr[c++] = triTable[i][j];
            }
        }
        
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    // Setting up Atomic Counter Buffer
    GLuint countBuff;
    {
        GLuint zero = 0;
        glGenBuffers(1, &countBuff);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuff);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, countBuff);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_STATIC_COPY);
        glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero);
    }

    //TODO: Adicionar controle de Meshlets.
    //Primeiro argumento: offset. 
    //Segundo argumento: quantidade de WorkGroups.
    glDrawMeshTasksNV(0, (qX * qY * qZ) / 32);

    meshShader->disable();
}