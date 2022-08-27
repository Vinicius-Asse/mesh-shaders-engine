#include <core/marching_cubes_mesh_impl.hpp>

/***
 * Construtor do Programa
 **/
MarchingCubesMeshImpl::MarchingCubesMeshImpl(Parameters *_param, Shader *_baseShader, Point *_points) : Program(_param, _baseShader, _points)
{
    name = "mesh_impl";

    meshShader = new Shader("resources/shaders/marching.mesh", ShaderType::MESH_SHADER);
    qX = param->surfaceResolution;
    qY = param->surfaceResolution;
    qZ = param->surfaceResolution;
}

/***
 * Método Executado Quando o Programa é Iniciado
 **/
void MarchingCubesMeshImpl::start()
{
    LOG("Start from Compute Shader Implementation");
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
    unsigned __int64 startTime = Utils::currentTimeInMillis();
    executeMeshShader();
    meshInfo["timeGeneratingMesh"] = std::to_string(Utils::currentTimeInMillis() - startTime);
}

/***
 *
 * Métodos Privados para execução do algoritmo Marching Cubes
 *
 **/

void MarchingCubesMeshImpl::executeMeshShader()
{
    int TOTAL_WORKGROUPS = (qX * qY * qZ) / 8;

    GLuint sphereCount = param->pointsCount;

    meshShader->enable();
    glm::mat4 mvpMatrix = Camera::MainCamera->getMVPMatrix(glm::mat4(1.0f));
    glm::vec3 lightDir = Camera::MainCamera->lightDir;

    // MVP MATRIX UNIFORM
    int mvpLoc = glGetUniformLocation(meshShader->uId, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    // LIGTH DIRECTION UNIFORM
    int lightDirLoc = glGetUniformLocation(meshShader->uId, "u_lightDir");
    glUniform4f(lightDirLoc, lightDir.x, lightDir.y, lightDir.z, 1.0f);

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
    int srLoc = glGetUniformLocation(meshShader->uId, "u_spheresCount");
    glUniform1ui(srLoc, sphereCount);

    // POINTS COUNT UNIFORM
    int pnLoc = glGetUniformLocation(meshShader->uId, "u_pointsCount");
    glUniform3i(pnLoc, qX, qY, qZ);

    // WORLD BOUNDS UNIFORM
    int wbLoc = glGetUniformLocation(meshShader->uId, "u_worldBounds");
    glUniform3f(wbLoc, param->worldBounds.x, param->worldBounds.y, param->worldBounds.z);

    // SPHERES LEVEL UNIFORM
    GLfloat* pts = (GLfloat*)malloc(sizeof(float) * 4 * sphereCount);
    int curr = 0;
    for (int i = 0; i < sphereCount; i++) {
        pts[curr + 0] = points[i].x;
        pts[curr + 1] = points[i].y;
        pts[curr + 2] = points[i].z;
        pts[curr + 3] = 1.0f;
        curr += 4;
    }

    int ptsLoc = glGetUniformLocation(meshShader->uId, "u_spheres");
    glUniform4fv(ptsLoc, sphereCount, pts);

    // Setting up Atomic Counter Buffer
    GLuint countBuff;
    {
        GLuint zero = 0;
        glGenBuffers(1, &countBuff);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuff);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, countBuff);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_STATIC_COPY);
        glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero);
    }

    // Setting up TrizTable SSBO
    GLuint trizTableSSBO;
    {
        glGenBuffers(1, &trizTableSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trizTableSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 4096, NULL, GL_STATIC_READ);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trizTableSSBO);

        GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
        int* trizTablePtr = (int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * 4096, bufMask);

        int c = 0;
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 16; j++) {
                trizTablePtr[c++] = triTable[i][j];
            }
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    // Setting up triangles count SSBO
    GLuint trizCountSSBO;
    {
        glGenBuffers(1, &trizCountSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trizCountSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * TOTAL_WORKGROUPS, NULL, GL_STATIC_READ);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, trizCountSSBO);

        GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
        int* trizCountPtr = (int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * TOTAL_WORKGROUPS, bufMask);

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    //TODO: Adicionar controle de Meshlets.
    //Primeiro argumento: offset. 
    //Segundo argumento: quantidade de WorkGroups.
    glDrawMeshTasksNV(0, TOTAL_WORKGROUPS);

    // Retrieving Triangles Count
    GLuint trizCount;
    {
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuff);
        GLuint* trizCountPtr = (GLuint*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
        trizCount = trizCountPtr[0];
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    }

    // Releasing buffers
    glDeleteBuffers(1, &countBuff);
    glDeleteBuffers(1, &trizTableSSBO);
    glDeleteBuffers(1, &trizCountSSBO);

    //GLint verticesOutQnd, primitivesOutQnt;
    //glGetProgramiv(meshShader->uId, GL_MESH_VERTICES_OUT_NV, &verticesOutQnd);
    //glGetProgramiv(meshShader->uId, GL_MESH_PRIMITIVES_OUT_NV, &primitivesOutQnt);

    meshInfo["trizCount"] = std::to_string(trizCount);
    meshInfo["vertexCount"] = std::to_string(trizCount * 3);
    meshInfo["indexCount"] = std::to_string(trizCount * 3);

    meshShader->disable();
}