#include<core/program.hpp>

    Camera*             camera;
    Cube*               cube;
    Sphere*             sphere;

/***
 * Construtor do Programa
**/
Program::Program(SDL_Window* _window, glm::vec3 _worldBounds, double _pointDencity) {
    window = _window;
    worldBounds = _worldBounds;
    pointDencity = _pointDencity;

    onCreate();
}

/***
 * Método Executado Quando o Programa é Criado
**/
void Program::onCreate() {
    int countX = worldBounds.x / pointDencity;
    int countY = worldBounds.y / pointDencity;
    int countZ = worldBounds.z / pointDencity;

    points = instantiatePoints(countX, countY, countZ);
    mesh   = generateMesh(new Shader("resources/shaders/base.glsl"));
}

/***
 * Método Executado Quando o Programa é Iniciado
**/
void Program::start() {
    Shader* baseShader = new Shader("resources/shaders/base.glsl");

    camera = new Camera(
        glm::vec3(0.0f, 0.0f, 2.5f),
        60.0f, window
    );

    cube = Cube::getInstance(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        baseShader
    );

    sphere = Sphere::getInstance(
        glm::vec3(0.0f, 1.5f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        16.0f, 8.0f, 
        baseShader
    );
}

/***
 * Método Executado no Inicio de Cada Frame
**/
void Program::input(SDL_Event* e) {
    camera->handleInputs(*e);
}

/***
 * Método Executado Toda Frame
**/
void Program::update() {
    camera->update();
    cube->rotate(glm::vec3(1.0f, 1.0f, 0.0f));
    sphere->rotate(glm::vec3(1.0f, 1.0f, 0.0f));
}

/***
 * Método Executado ao Fim de Toda Frame
**/
void Program::draw() {
    cube->draw();
    sphere->draw();

    mesh->draw();
}

Point*** Program::instantiatePoints(int countX, int countY, int countZ) {
    Point*** points = (Point***) malloc(sizeof(Point**) * countX);

    for (int i = 0; i < countX; i++) {
        points[i] = (Point**) malloc(sizeof(Point*) * countY);
        for (int j = 0; j < countY; j++) {
            points[i][j] = (Point*) malloc(sizeof(Point) * countZ);
            for (int k = 0; k < countZ; k++) {
                points[i][j][k] = {
                    glm::vec3(i, j, k),
                    generateRandomValue(i, j, k, 10.0f, glm::vec3(0, 0, 0)) //TODO: Parametrizar Scale e Displacement
                };
            }
        }
    }

    return points;
}

Mesh* Program::generateMesh(Shader* shader) {
    int countX = worldBounds.x / pointDencity;
    int countY = worldBounds.y / pointDencity;
    int countZ = worldBounds.z / pointDencity;

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
                    if (corners[l].value > -22.0f)
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

    for (int i = 0; i < triangles.size(); i++)
    {
        vertexBuff.push_back(createVertex(triangles[i].ver0, triangles[i].normal));
        vertexBuff.push_back(createVertex(triangles[i].ver1, triangles[i].normal));
        vertexBuff.push_back(createVertex(triangles[i].ver2, triangles[i].normal));

        indicesBuff.push_back(i * 3 + 0);
        indicesBuff.push_back(i * 3 + 1);
        indicesBuff.push_back(i * 3 + 2);
    }

    std::cout << "Terminou de Gerar Mesh. Triangulos: " << triangles.size() << std::endl;

    return new Mesh(indicesBuff, vertexBuff, shader);
}

float Program::generateRandomValue(int _x, int _y, int _z, float scale, glm::vec3 displacement) {
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

        return remap(result, 0, 1, -16, 32);
}

glm::vec3 Program::interpolate(Point a, Point b) {
    return (a.position + b.position) / 2.0f;
}

float Program::remap(float value, float low1, float high1, float low2, float high2) {
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

bool Program::isValidLine(int i, int j, int k)
{
    if (i < 0 || i >= worldBounds.x) return false;
    if (j < 0 || j >= worldBounds.y) return false;
    if (k < 0 || k >= worldBounds.z) return false;

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