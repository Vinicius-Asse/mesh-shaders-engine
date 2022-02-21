#include<core/marching_cubes_compute_impl.hpp>

/***
 * Construtor do Programa
**/
MarchingCubesComputeImpl::MarchingCubesComputeImpl(Parameters *_param, Shader* _baseShader, Point* _points) : Program(_param, _baseShader, _points) {
    mesh = nullptr;
    computeShader = new Shader("resources/shaders/compute/marching.compute.glsl", ShaderType::COMPUTE_SHADER);
}

/***
 * Método Executado Quando o Programa é Iniciado
**/
void MarchingCubesComputeImpl::start() {
    LOG("Start from Compute Shader Implementation");

    wiredCube = Cube::getInstance(
        glm::vec3(0.0f, 0.0f, 0.0f),
        param->worldBounds,
        baseShader
    );
}

/***
 * Método Executado no Inicio de Cada Frame
**/
void MarchingCubesComputeImpl::input(SDL_Event* e) { }

/***
 * Método Executado Toda Frame
**/
void MarchingCubesComputeImpl::update() {
    qX = param->surfaceResolution;
    qY = param->surfaceResolution;
    qZ = param->surfaceResolution;

    unsigned __int64 startTime = Utils::currentTimeInMillis();
    generateMesh();
    meshInfo["timeGeneratingMesh"] = std::to_string(Utils::currentTimeInMillis() - startTime); 
}

/***
 * Método Executado ao Fim de Toda Frame
**/
void MarchingCubesComputeImpl::draw() {

    // Turn on wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    // Draw the box
    wiredCube->draw();

    // Turn off wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);

    // Draw Generated Mesh
    mesh->draw();
}


/***
 * 
 * Métodos Privados para execução do algoritmo Marching Cubes
 * 
**/

void MarchingCubesComputeImpl::generateMesh() {
    GLuint pointsCount = param->pointsCount;

    const int maxTrizQnt = qX * qY * qZ;

    computeShader->enable();

    // SURFACE LEVEL UNIFORM
    int slLoc = glGetUniformLocation(computeShader->uId, "u_surfaceLevel");
    glUniform1f(slLoc, param->surfaceLevel);

    // SMOOTH INTERSECTION UNIFORM
    int siLoc = glGetUniformLocation(computeShader->uId, "u_smooth");
    glUniform1f(siLoc, param->smoothIntersect);

    // LINEAR INTERSECTION UNIFORM
    int liLoc = glGetUniformLocation(computeShader->uId, "u_linear");
    glUniform1ui(liLoc, param->linearInterp ? 1 : 0);

    // POINTS COUNT UNIFORM
    int pcLoc = glGetUniformLocation(computeShader->uId, "u_pointsCount");
    glUniform1ui(pcLoc, pointsCount);

    // WORLD BOUNDS UNIFORM
    int wbLoc = glGetUniformLocation(computeShader->uId, "u_worldBounds");
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

    int ptsLoc = glGetUniformLocation(computeShader->uId, "u_points");
    glUniform4fv(ptsLoc, pointsCount, pts);

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

    // Setting up Triangles SSBO
    GLuint trianglesSSBO;
    {
        glGenBuffers(1, &trianglesSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Triangle) * maxTrizQnt, NULL, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trianglesSSBO);
    }

    // Setting up TrizTable SSBO
    GLuint trizTableSSBO;
    {
        glGenBuffers(1, &trizTableSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trizTableSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 4096 , NULL, GL_STATIC_READ);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, trizTableSSBO);

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

    // Dispatch Compute Shader
    glDispatchCompute(qX/8, qY/8, qZ/8);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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
    }

    // Releasing buffers
    glDeleteBuffers(1, &trianglesSSBO);
    glDeleteBuffers(1, &trizTableSSBO);
    glDeleteBuffers(1, &countBuff);

    computeShader->disable();

    if (param->smooth) 
        smoothShading(trianglesPtr, trizCount); 
    else 
        flatShading(trianglesPtr, trizCount);

    meshInfo["trizCount"] = std::to_string(trizCount);
    meshInfo["vertexCount"] = std::to_string(vertexBuff.size());
    meshInfo["indicesCount"] = std::to_string(indicesBuff.size());
    
    if (mesh != nullptr) mesh->free();
    
    mesh = new Mesh(indicesBuff, vertexBuff, baseShader);
}

void MarchingCubesComputeImpl::smoothShading(Triangle *triangles, int trizCount) {

    vertexBuff.clear();
    indicesBuff.clear();

    GLint curr = 0;
    std::unordered_map<glm::vec3, GLint> map;
    for (int i = 0; i < trizCount; i++) {
        Triangle t = triangles[i];

        glm::vec3 v0 = glm::vec3(t.ver0.x, t.ver0.y, t.ver0.z);
        glm::vec3 v1 = glm::vec3(t.ver1.x, t.ver1.y, t.ver1.z);
        glm::vec3 v2 = glm::vec3(t.ver2.x, t.ver2.y, t.ver2.z);
        glm::vec3 normal = glm::vec3(t.normal.x, t.normal.y, t.normal.z);

        if (pushUniqueVertices(&map, v0, normal, curr)) curr++;
        if (pushUniqueVertices(&map, v1, normal, curr)) curr++;
        if (pushUniqueVertices(&map, v2, normal, curr)) curr++;
    }
}

void MarchingCubesComputeImpl::flatShading(Triangle *triangles, int trizCount) {

    vertexBuff.clear();
    indicesBuff.clear();

    for (int i = 0; i < trizCount; i++)
    {
        vertexBuff.push_back(Utils::createVertex(triangles[i].ver0, triangles[i].normal));
        vertexBuff.push_back(Utils::createVertex(triangles[i].ver1, triangles[i].normal));
        vertexBuff.push_back(Utils::createVertex(triangles[i].ver2, triangles[i].normal));

        indicesBuff.push_back(i * 3 + 0);
        indicesBuff.push_back(i * 3 + 1);
        indicesBuff.push_back(i * 3 + 2);
    }
}

bool MarchingCubesComputeImpl::pushUniqueVertices(std::unordered_map<glm::vec3, GLint> *map, glm::vec3 position, glm::vec3 normal, GLint current) {
    if (map->find(position) == map->end()) {
        map->insert(std::make_pair(position, current));
        vertexBuff.push_back(Utils::createVertex({position.x, position.y, position.z, 1.0f}, {normal.x, normal.y, normal.z, 1.0f}));
        indicesBuff.push_back(current);

        return true;
    } else {
        indicesBuff.push_back(map->at(position));
        return false;
    }
}