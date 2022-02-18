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

    GLuint trizTableSSBO;
    {
        glGenBuffers(1, &trizTableSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trizTableSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 4096 , NULL, GL_STATIC_READ);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trizTableSSBO);

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

    // Setting up Triangles SSBO
    GLuint trianglesSSBO;
    {
        glGenBuffers(1, &trianglesSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Triangle) * qX * qY * qZ, NULL, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, trianglesSSBO);
    }

    //TODO: Adicionar controle de Meshlets.
    //Primeiro argumento: offset. 
    //Segundo argumento: quantidade de WorkGroups.
    glDrawMeshTasksNV(0, (qX * qY * qZ));

    // Retrieving Triangles Count
    GLuint trizCount;
    {
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, countBuff);
        GLuint* trizCountPtr = (GLuint*) glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
        trizCount = trizCountPtr[0];
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    }

    // Retrieving Triangles Buffer Data
    Triangle* trianglesPtr = nullptr;
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
        trianglesPtr = (Triangle*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        for(int i = 0; i < 1; i++) {
            Triangle t = trianglesPtr[i];
            LOG("X: " << t.ver0.x << ", Y: " << t.ver0.y << ", Z: " << t.ver0.z << ", V: " << t.ver0.w);
        }
    }

    // Releasing buffers
    glDeleteBuffers(1, &trianglesSSBO);
    glDeleteBuffers(1, &trizTableSSBO);
    glDeleteBuffers(1, &countBuff);

    GLint verticesOutQnd, primitivesOutQnt;
    glGetProgramiv(meshShader->uId, GL_MESH_VERTICES_OUT_NV, &verticesOutQnd);
    glGetProgramiv(meshShader->uId, GL_MESH_PRIMITIVES_OUT_NV, &primitivesOutQnt);

    meshInfo["trizCount"] = std::to_string(trizCount);
    meshInfo["vertexCount"] = std::to_string(verticesOutQnd);
    meshInfo["indexCount"] = std::to_string(primitivesOutQnt);

    meshShader->disable();
}