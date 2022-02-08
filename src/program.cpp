#include<core/program.hpp>

/***
 * Construtor do Programa
**/
Program::Program(Parameters *_param) {
    param = _param;

    onCreate();
}

/***
 * Método Executado Quando o Programa é Criado
**/
void Program::onCreate() {
    Shader* baseShader = new Shader("resources/shaders/base.glsl", ShaderType::VERTEX_SHADER);

    wiredCube = Cube::getInstance(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        baseShader
    );
}

/***
 * Método Executado Quando o Programa é Iniciado
**/
void Program::start() {
    int countX = param->surfaceResolution;
    int countY = param->surfaceResolution;
    int countZ = param->surfaceResolution;

    unsigned __int64 startTime = Utils::currentTimeInMillis();
    points = instantiatePoints(countX, countY, countZ);
    mesh = generateMesh(countX, countY, countZ, new Shader("resources/shaders/base.glsl", ShaderType::VERTEX_SHADER));

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
}

/***
 * Método Executado no Inicio de Cada Frame
**/
void Program::input(SDL_Event* e) { }

/***
 * Método Executado Toda Frame
**/
void Program::update() { }

/***
 * Método Executado ao Fim de Toda Frame
**/
void Program::draw() {

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

Point*** Program::instantiatePoints(int countX, int countY, int countZ) {
    Point*** points = (Point***) malloc(sizeof(Point**) * countX);
    for (int i = 0; i < countX; i++) {
        points[i] = (Point**) malloc(sizeof(Point*) * countY);
        for (int j = 0; j < countY; j++) {
            points[i][j] = (Point*) malloc(sizeof(Point) * countZ);
            for (int k = 0; k < countZ; k++) {
                float x = Utils::remap(i, 0, countX - 1, -5.0f, 5.0f);
                float y = Utils::remap(j, 0, countY - 1, -5.0f, 5.0f);
                float z = Utils::remap(k, 0, countZ - 1, -5.0f, 5.0f);
                points[i][j][k] = {
                    x, y, z,
                    Utils::generateNoise(x, y, z, param->noiseScale, param->noiseDisplacement)
                };
            }
        }
    }

    return points;
}

Mesh* Program::generateMesh(int countX, int countY, int countZ, Shader* shader) {

    std::cout << "Gerando Mesh em CPU" << std::endl;
    std::cout << "Comecou a gerar a Mesh. X = " << countX << " Y = " << countY << " Z = " << countZ << std::endl;

    std::vector<Triangle> triangles;
    
    for (int i = 0; i < countX - 1; i++) {
        for (int j = 0; j < countY - 1; j++) {
            for (int k = 0; k < countZ - 1; k++) {
                Point corners[8] = {
                    points[i  ][j  ][k  ],
                    points[i+1][j  ][k  ],
                    points[i+1][j  ][k+1],
                    points[i  ][j  ][k+1],
                    points[i  ][j+1][k  ],
                    points[i+1][j+1][k  ],
                    points[i+1][j+1][k+1],
                    points[i  ][j+1][k+1] 
                };

                int cubeIndex = 0;
                for (int l = 0; l < 8; l++)
                {
                    if (corners[l].value > param->surfaceLevel)
                    {
                        cubeIndex |= 1 << l;
                    }
                }

                if (cubeIndex == 0 || cubeIndex == 255) continue;

                for (int l = 0; triTable[cubeIndex][l] !=-1; l+=3)
                {
                    int idx = triTable[cubeIndex][l + 0];
                    int idy = triTable[cubeIndex][l + 1];
                    int idz = triTable[cubeIndex][l + 2];

                    int a0 = cornerTupleTable[idx][0];
                    int b0 = cornerTupleTable[idx][1];

                    int a1 = cornerTupleTable[idy][0];
                    int b1 = cornerTupleTable[idy][1];

                    int a2 = cornerTupleTable[idz][0];
                    int b2 = cornerTupleTable[idz][1];

                    glm::vec3 vec0 = interpolate(corners[a0], corners[b0]);
                    glm::vec3 vec1 = interpolate(corners[a1], corners[b1]);
                    glm::vec3 vec2 = interpolate(corners[a2], corners[b2]);
                    glm::vec3 normal = Utils::getNormalVector(vec0, vec1, vec2);

                    triangles.push_back({
                        {vec0.x, vec0.y, vec0.z, 1.0f}, 
                        {vec1.x, vec1.y, vec1.z, 1.0f},
                        {vec2.x, vec2.y, vec2.z, 1.0f},
                        {normal.x, normal.y, normal.z, 1.0f},
                    });
                }
            }
        }
    }

    if (param->smooth) {
        smoothShading(triangles);
    } else {
        flatShading(triangles);
    }

    std::cout << "Terminou de Gerar Mesh. Triangulos: " << triangles.size() << " Vertices: " << vertexBuff.size() << "  Indices: " << indicesBuff.size() << std::endl;

    return new Mesh(indicesBuff, vertexBuff, shader);
}

glm::vec3 Program::interpolate(Point a, Point b) {
    glm::vec3 aPos = glm::vec3(a.x, a.y, a.z);
    glm::vec3 bPos = glm::vec3(b.x, b.y, b.z);

    if (param->linearInterp) {
        float t = (param->surfaceLevel - a.value) / (b.value - a.value);
        return aPos + t * (bPos - aPos);
    } else {
        return (aPos + bPos) / 2.0f;
    }
}

void Program::smoothShading(std::vector<Triangle> triangles) {

    vertexBuff.clear();
    indicesBuff.clear();

    GLint curr = 0;
    std::unordered_map<glm::vec3, GLint> map;
    for (int i = 0; i < triangles.size(); i++) {
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

void Program::flatShading(std::vector<Triangle> triangles) {

    vertexBuff.clear();
    indicesBuff.clear();

    for (int i = 0; i < triangles.size(); i++)
    {
        vertexBuff.push_back(Utils::createVertex(triangles[i].ver0, triangles[i].normal));
        vertexBuff.push_back(Utils::createVertex(triangles[i].ver1, triangles[i].normal));
        vertexBuff.push_back(Utils::createVertex(triangles[i].ver2, triangles[i].normal));

        indicesBuff.push_back(i * 3 + 0);
        indicesBuff.push_back(i * 3 + 1);
        indicesBuff.push_back(i * 3 + 2);
    }
}

bool Program::pushUniqueVertices(std::unordered_map<glm::vec3, GLint> *map, glm::vec3 position, glm::vec3 normal, GLint current) {
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