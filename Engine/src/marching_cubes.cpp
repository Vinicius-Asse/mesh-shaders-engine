#include<core/marching_cubes.hpp>

/***
 * Construtor do Programa
**/
MarchingCubesCPUImpl::MarchingCubesCPUImpl(Parameters *_param, Shader* _baseShader, Point* _points) : Program(_param, _baseShader, _points) {
    name = "cpu_impl";

    mesh = nullptr;
    qX = param->surfaceResolution;
    qY = param->surfaceResolution;
    qZ = param->surfaceResolution;
}

/***
 * Método Executado Quando o Programa é Iniciado
**/
void MarchingCubesCPUImpl::start() {
    LOG("Start from CPU Implementation");
}

/***
 * Método Executado no Inicio de Cada Frame
**/
void MarchingCubesCPUImpl::input(SDL_Event* e) { }

/***
 * Método Executado Toda Frame
**/
void MarchingCubesCPUImpl::update() {

    qX = param->surfaceResolution;
    qY = param->surfaceResolution;
    qZ = param->surfaceResolution;

    unsigned __int64 startTime = Utils::currentTimeInMillis();
    mesh = generateMesh(baseShader);

    meshInfo["timeGeneratingMesh"] = std::to_string(Utils::currentTimeInMillis() - startTime);
}

/***
 * Método Executado ao Fim de Toda Frame
**/
void MarchingCubesCPUImpl::draw() {
    // Draw Generated Mesh
    mesh->draw();
}


/***
 * 
 * Métodos Privados para execução do algoritmo Marching Cubes
 * 
**/

Mesh* MarchingCubesCPUImpl::generateMesh(Shader* shader) {
    std::vector<Triangle> triangles;
    
    for (int i = 0; i < qX - 1; i++) {
        for (int j = 0; j < qY - 1; j++) {
            for (int k = 0; k < qZ - 1; k++) {
                Point corners[8] = {
                    getPoint(i  , j  , k  ),
                    getPoint(i+1, j  , k  ),
                    getPoint(i+1, j  , k+1),
                    getPoint(i  , j  , k+1),
                    getPoint(i  , j+1, k  ),
                    getPoint(i+1, j+1, k  ),
                    getPoint(i+1, j+1, k+1),
                    getPoint(i  , j+1, k+1) 
                };

                int index = 0;
                if (corners[0].value < param->surfaceLevel) index |=   1;
                if (corners[1].value < param->surfaceLevel) index |=   2;
                if (corners[2].value < param->surfaceLevel) index |=   4;
                if (corners[3].value < param->surfaceLevel) index |=   8;
                if (corners[4].value < param->surfaceLevel) index |=  16;
                if (corners[5].value < param->surfaceLevel) index |=  32;
                if (corners[6].value < param->surfaceLevel) index |=  64;
                if (corners[7].value < param->surfaceLevel) index |= 128;

                if (index == 0 || index == 255) continue;

                for (int l = 0; triTable[index][l] !=-1; l+=3)
                {
                    int idx = triTable[index][l + 0];
                    int idy = triTable[index][l + 1];
                    int idz = triTable[index][l + 2];

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
                        { vec0.x, vec0.y, vec0.z, 1.0f }, 
                        { vec1.x, vec1.y, vec1.z, 1.0f },
                        { vec2.x, vec2.y, vec2.z, 1.0f },
                        { normal.x, normal.y, normal.z, 1.0f },
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

    meshInfo["trizCount"] = std::to_string(triangles.size());
    meshInfo["vertexCount"] = std::to_string(vertexBuff.size());
    meshInfo["indexCount"] = std::to_string(indicesBuff.size());

    if (mesh != nullptr) mesh->free();

    return new Mesh(indicesBuff, vertexBuff, shader);
}

Point MarchingCubesCPUImpl::getPoint(int _x, int _y, int _z) {
    float x = Utils::remap(_x, 0, qX-1, -param->worldBounds.x/2.0f, param->worldBounds.x/2.0f);
    float y = Utils::remap(_y, 0, qY-1, -param->worldBounds.y/2.0f, param->worldBounds.y/2.0f);
    float z = Utils::remap(_z, 0, qZ-1, -param->worldBounds.z/2.0f, param->worldBounds.z/2.0f);

    float value = 2.5;
    for (int i = 0; i < param->pointsCount; i++) {
        float distanteToPoint = glm::distance(glm::vec3(x, y, z), glm::vec3(points[i].x, points[i].y, points[i].z));
        value = Utils::smoothMin(Utils::remap(std::abs(distanteToPoint), 0, 5.5, -1, 1), value, param->smoothIntersect);
    }

    return {
        x, y, z,         //Posição
        value,           //Valor
        0.0f, 0.0f, 0.0f //Velocidade (Nao utilizado)
    };
}

glm::vec3 MarchingCubesCPUImpl::interpolate(Point a, Point b) {
    glm::vec3 aPos = glm::vec3(a.x, a.y, a.z);
    glm::vec3 bPos = glm::vec3(b.x, b.y, b.z);

    if (param->linearInterp) {
        float t = (param->surfaceLevel - a.value) / (b.value - a.value);
        return aPos + t * (bPos - aPos);
    } else {
        return (aPos + bPos) / 2.0f;
    }
}

void MarchingCubesCPUImpl::smoothShading(std::vector<Triangle> triangles) {
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

void MarchingCubesCPUImpl::flatShading(std::vector<Triangle> triangles) {

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

bool MarchingCubesCPUImpl::pushUniqueVertices(std::unordered_map<glm::vec3, GLint> *map, glm::vec3 position, glm::vec3 normal, GLint current) {
    //TODO: Alterar implementação para melhor controle da implementação de geometria suave
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