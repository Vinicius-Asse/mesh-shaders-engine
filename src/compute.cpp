#include<core/compute.hpp>

/***
 * Construtor do Programa
**/
Compute::Compute(Parameters *_param) {
    param = _param;

    onCreate();
}

/***
 * Método Executado Quando o Programa é Criado
**/
void Compute::onCreate() {
    computeShader = new ComputeShader("resources/shaders/marching.compute");
    meshShader    = new Shader("resources/shaders/base.glsl");

    wiredCube = Cube::getInstance(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        meshShader
    );
}

/***
 * Método Executado Quando o Programa é Iniciado
**/
void Compute::start() {
    int countX = param->surfaceResolution;
    int countY = param->surfaceResolution;
    int countZ = param->surfaceResolution;

    unsigned __int64 startTime = Utils::currentTimeInMillis();
    //points = instantiatePoints(countX, countY, countZ);
    mesh = generateMesh(countX, countY, countZ);

    std::cout << "Tempo para gerar mesh: " << Utils::currentTimeInMillis() - startTime << "ms. ";
    std::cout << 
        "{ " << 
                "surfaceLevel: '" << std::to_string(param->surfaceLevel)           << "', " <<
                "smooth: '"       << std::to_string(param->smooth)                 << "', " <<
                "linearInterp: '" << std::to_string(param->linearInterp)           << "', " <<
                // "distX:  '"       << std::to_string(param->noiseDisplacement.x)    << "', " <<
                // "distY: '"        << std::to_string(param->noiseDisplacement.y)    << "', " <<
                // "distZ: '"        << std::to_string(param->noiseDisplacement.z)    << "', " <<
                "GPU: '"          << std::to_string(param->useGPU)                 <<
        " }" << 
        std::endl;

    meshInfo["timeGeneratingMesh"] = std::to_string(Utils::currentTimeInMillis() - startTime);
}

/***
 * Método Executado no Inicio de Cada Frame
**/
void Compute::input(SDL_Event* e) { }

/***
 * Método Executado Toda Frame
**/
void Compute::update() { }

/***
 * Método Executado ao Fim de Toda Frame
**/
void Compute::draw() {

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

Point* Compute::instantiatePoints(int countX, int countY, int countZ) {

    const int buffSize = countX * countY * countZ * sizeof(Point);
    
    GLuint vertexSSbo;

    GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

    ComputeShader* compute = new ComputeShader("resources/shaders/points.compute");

    compute->enable();

    glGenBuffers(1, &vertexSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexSSbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, buffSize, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexSSbo);

    Point* ssboPoints = (Point*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffSize, bufMask);

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glDispatchCompute(countX, countY, countZ);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexSSbo);
    Point* ppt = (Point*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    compute->disable();

    return ppt;
}

Mesh* Compute::generateMesh(int countX, int countY, int countZ) {

    std::cout << "Gerando Mesh via Compute Shaders" << std::endl;

    const int maxTrizQnt = countX * countY * countZ;

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

    // POINTS LEVEL UNIFORM
    GLfloat pts[12] = {
          0.0f,  3.0f,  0.0f,  0.0f,
          5.0f,  -3.0f,  0.0f,  0.0f,
          0.0f,  -2.5f, -3.5f,  0.0f
    };

    int ptsLoc = glGetUniformLocation(computeShader->uId, "u_points");
    glUniform4fv(ptsLoc, 12, pts);

    // Setting up Atomic Counter Buffer
    GLuint countBuff = 0;
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
    glDispatchCompute(countX/8, countY/8, countZ/8);
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

    return new Mesh(indicesBuff, vertexBuff, meshShader);
}

void Compute::smoothShading(Triangle *triangles, int trizCount) {

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

void Compute::flatShading(Triangle *triangles, int trizCount) {

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

bool Compute::pushUniqueVertices(std::unordered_map<glm::vec3, GLint> *map, glm::vec3 position, glm::vec3 normal, GLint current) {
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