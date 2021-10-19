#include<core/program.hpp>

    Camera*             camera;
    Cube*               cube;

    float distX, distY, distZ   = 0.0f;
    float noiseScale            = 10.0f;
    float surfaceLevel          = -17.0f;
    float pointDencity          = 1.0f;
    bool smooth                 = false;
    bool linearInterp           = false;

/***
 * Construtor do Programa
**/
Program::Program(SDL_Window* _window, glm::vec3 _worldBounds) {
    window = _window;
    worldBounds = _worldBounds;

    onCreate();
}

/***
 * Método Executado Quando o Programa é Criado
**/
void Program::onCreate() {
    int countX = worldBounds.x * pointDencity;
    int countY = worldBounds.y * pointDencity;
    int countZ = worldBounds.z * pointDencity;

    points = instantiatePoints(countX, countY, countZ);
    mesh   = generateMesh(countX, countY, countZ, new Shader("resources/shaders/base.glsl"));
}

/***
 * Método Executado Quando o Programa é Iniciado
**/
void Program::start() {
    Shader* baseShader = new Shader("resources/shaders/base.glsl");

    camera = new Camera(
        glm::vec3(0.0f, 0.0f, 20.5f),
        60.0f, window
    );

    cube = Cube::getInstance(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        baseShader
    );
}

/***
 * Método Executado no Inicio de Cada Frame
**/
void Program::input(SDL_Event* e) {
    camera->handleInputs(*e);

    bool changedMesh   = false;
    bool changedPoints = false;
    switch (e->type) {
    case SDL_KEYDOWN:
        switch(e->key.keysym.sym) {
            case SDLK_q:     surfaceLevel -= 0.5f;         changedMesh = true; break;
            case SDLK_e:     surfaceLevel += 0.5f;         changedMesh = true; break;
            case SDLK_k:     smooth = !smooth;             changedMesh = true; break;
            case SDLK_j:     linearInterp = !linearInterp; changedMesh = true; break;
            case SDLK_LEFT:  distX += 0.5f;                changedMesh = true;   changedPoints = true; break;
            case SDLK_RIGHT: distX -= 0.5f;                changedMesh = true;   changedPoints = true; break;
            case SDLK_UP:    distZ += 0.5f;                changedMesh = true;   changedPoints = true; break;
            case SDLK_DOWN:  distZ -= 0.5f;                changedMesh = true;   changedPoints = true; break;
            case SDLK_i:     noiseScale += 0.5f;           changedMesh = true;   changedPoints = true; break;
            case SDLK_o:     noiseScale -= 0.5f;           changedMesh = true;   changedPoints = true; break;
            case SDLK_t:     pointDencity += 1.0f;         changedMesh = true;   changedPoints = true; break;
            case SDLK_y:     pointDencity -= 1.0f;         changedMesh = true;   changedPoints = true; break;
        }
        break;
    default:
        break;
    }

    if (changedMesh) {
        int countX = worldBounds.x * pointDencity;
        int countY = worldBounds.y * pointDencity;
        int countZ = worldBounds.z * pointDencity;

        if (changedPoints) {
            points = instantiatePoints(countX, countY, countZ);
        }

        mesh = generateMesh(countX, countY, countZ, new Shader("resources/shaders/base.glsl"));

        std::cout << 
        "{ " << 
                "surfaceLevel: '" << std::to_string(surfaceLevel) << "', " <<
                "smooth: '"       << std::to_string(smooth)       << "', " <<
                "linearInterp: '" << std::to_string(linearInterp) << "', " <<
                "distX:  '"       << std::to_string(distX)        << "', " <<
                "distY: '"        << std::to_string(distY)        << "', " <<
                "distZ: '"        << std::to_string(distZ)                 <<
        " }" << 
        std::endl;
    }
}

/***
 * Método Executado Toda Frame
**/
void Program::update() {
    camera->update();
    //cube->rotate(glm::vec3(1.0f, 1.0f, 0.0f));
    //sphere->rotate(glm::vec3(1.0f, 1.0f, 0.0f));
}

/***
 * Método Executado ao Fim de Toda Frame
**/
void Program::draw() {

    // Turn on wireframe mode
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    // Draw the box
    cube->draw();

    // Turn off wireframe mode
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
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
                float x = remap(i, 0, countX - 1, -5.0f, 5.0f);
                float y = remap(j, 0, countY - 1, -5.0f, 5.0f);
                float z = remap(k, 0, countZ - 1, -5.0f, 5.0f);
                points[i][j][k] = {
                    glm::vec3(x, y, z),
                    generateRandomValue(x, y, z, noiseScale, glm::vec3(distX, distY, distZ))
                };
            }
        }
    }

    return points;
}

Mesh* Program::generateMesh(int countX, int countY, int countZ, Shader* shader) {

    std::cout << "Comecou a gerar a Mesh. X = " << countX << " Y = " << countY << " Z = " << countZ << std::endl;

    std::vector<Triangle> triangles;
    
    for (int i = 0; i < countX; i++) {
        for (int j = 0; j < countY; j++) {
            for (int k = 0; k < countZ; k++) {
                if (!isValidCube(i, j, k)) continue;

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
                    if (corners[l].value > surfaceLevel)
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

                    triangles.push_back({
                        vec0, vec1, vec2,
                        getNormalVector(vec0, vec1, vec2)
                    });
                }
            }
        }
    }

    if (smooth) {
        smoothShading(triangles);
    } else {
        flatShading(triangles);
    }

    std::cout << "Terminou de Gerar Mesh. Triangulos: " << triangles.size() << " Vertices: " << vertexBuff.size() << "  Indices: " << indicesBuff.size() << std::endl;

    return new Mesh(indicesBuff, vertexBuff, shader);
}

GLfloat Program::generateRandomValue(float _x, float _y, float _z, float scale, glm::vec3 displacement) {
        float x = _x / 100.0f * scale + displacement.x;
        float y = _y / 100.0f * scale + displacement.y;
        float z = _z / 100.0f * scale + displacement.z;

        float ab = glm::perlin(glm::vec2(x, y));
        float bc = glm::perlin(glm::vec2(y, z));
        float ac = glm::perlin(glm::vec2(x, z));
        float ba = glm::perlin(glm::vec2(y, x));
        float cb = glm::perlin(glm::vec2(z, y));
        float ca = glm::perlin(glm::vec2(z, x));

        float result = (ab + bc + ac + ba + cb + ca) / 6.0f;
        //float result = remap(glm::distance(glm::vec3(_x, _y, _z) + displacement, glm::vec3(0, 0, 0)) * scale, 0, 100.0f, 0, 1);
        //float result = (_x == -5 || _x == 5 || _y == -5 || _y == 5 || _z == -5 || _z == 5) ? 1 : 0;

        //std::cout << result << std::endl;

        return remap(result, 0, 1.0f, -16, 32);
}

glm::vec3 Program::interpolate(Point a, Point b) {
    if (linearInterp) {
        float t = (surfaceLevel - a.value) / (b.value - a.value);
        return a.position + t * (b.position - a.position);
    } else {
        return (a.position + b.position) / 2.0f;
    }
}

GLfloat Program::remap(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    return toLow + (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
}

bool Program::isValidLine(int i, int j, int k)
{
    if (i < 0 || i >= worldBounds.x * pointDencity) return false;
    if (j < 0 || j >= worldBounds.y * pointDencity) return false;
    if (k < 0 || k >= worldBounds.z * pointDencity) return false;

    return true;
}

bool Program::isValidCube(int i, int j, int k)
{
    return isValidLine(i+1, j, k) && isValidLine(i, j+1, k) && isValidLine(i, j, k+1) &&
            isValidLine(i+1, j, k+1) && isValidLine(i, j+1, k+1) && isValidLine(i+1, j+1, k) &&
            isValidLine(i+1, j+1, k+1);
}

Vertex Program::createVertex(glm::vec3 position, glm::vec3 normal) {
    return {
        { position.x, position.y, position.z},
        { 1.0f, 1.0f, 1.0f },
        { normal.x, normal.y, normal.z }
    };
}

glm::vec3 Program::getNormalVector(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm::vec3 dir = glm::cross((b - a), (c - a));
    return glm::normalize(dir);
}

void Program::smoothShading(std::vector<Triangle> triangles) {

    vertexBuff.clear();
    indicesBuff.clear();

    GLint curr = 0;
    std::unordered_map<glm::vec3, GLint> map;
    for (int i = 0; i < triangles.size(); i++) {
        Triangle t = triangles[i];

        if (pushUniqueVertices(&map, t.ver0, t.normal, curr)) curr++;
        if (pushUniqueVertices(&map, t.ver1, t.normal, curr)) curr++;
        if (pushUniqueVertices(&map, t.ver2, t.normal, curr)) curr++;
    }
}

void Program::flatShading(std::vector<Triangle> triangles) {

    vertexBuff.clear();
    indicesBuff.clear();

    for (int i = 0; i < triangles.size(); i++)
    {
        vertexBuff.push_back(createVertex(triangles[i].ver0, triangles[i].normal));
        vertexBuff.push_back(createVertex(triangles[i].ver1, triangles[i].normal));
        vertexBuff.push_back(createVertex(triangles[i].ver2, triangles[i].normal));

        indicesBuff.push_back(i * 3 + 0);
        indicesBuff.push_back(i * 3 + 1);
        indicesBuff.push_back(i * 3 + 2);
    }
}

bool Program::pushUniqueVertices(std::unordered_map<glm::vec3, GLint> *map, glm::vec3 position, glm::vec3 normal, GLint current) {
    if (map->find(position) == map->end()) {
        map->insert(std::make_pair(position, current));
        vertexBuff.push_back(createVertex(position, normal));
        indicesBuff.push_back(current);

        return true;
    } else {
        indicesBuff.push_back(map->at(position));
        return false;
    }
}