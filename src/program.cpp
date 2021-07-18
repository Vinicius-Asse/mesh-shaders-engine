#include<core/program.hpp>

Sphere*** instantiateSpheres(int, int, int, Shader*);

    Camera*  camera;
    Cube*    cube;
    Sphere***spheres;


/***
 * Construtor do Programa
**/
Program::Program(SDL_Window* _window) {
    window = _window;

    onCreate();
}

/***
 * Método Executado Quando o Programa é Criado
**/
void Program::onCreate() {

}

/***
 * Método Executado Quando o Programa é Iniciado
**/
void Program::start() {
    Shader* baseShader = new Shader("resources/shaders/base.shader");

    camera = new Camera(
        glm::vec3(0.0f, 0.0f, 2.5f),
        60.0f
    );

    cube = Cube::getInstance(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        baseShader);

    spheres = instantiateSpheres(10, 10, 10, baseShader);
}

/***
 * Método Executado no Inicio de Cada Frame
**/
void Program::input(SDL_Event* e) {
    camera->handleInputs(*e, window);
}

/***
 * Método Executado Toda Frame
**/
void Program::update() {
    camera->update(window);
}

/***
 * Método Executado ao Fim de Toda Frame
**/
void Program::draw() {
    cube->draw();

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                spheres[i][j][k].draw();
            }
        }
    }
}


Sphere*** instantiateSpheres(int countX, int countY, int countZ, Shader *shader) {
    Sphere*** spheres = (Sphere***) malloc(sizeof(Sphere**) * countX);
    float scale = 0.3f;

    for (int i = 0; i < countX; i++) {
        spheres[i] = (Sphere**) malloc(sizeof(Sphere*) * countY);
        for (int j = 0; j < countY; j++) {
            spheres[i][j] = (Sphere*) malloc(sizeof(Sphere) * countZ);
            for (int k = 0; k < countZ; k++) {
                Sphere* sphere = Sphere::getInstance(
                        glm::vec3(i * scale, j * scale, k * scale),
                        glm::vec3(0.05f, 0.05f, 0.05f),
                        16, 8, shader);
                spheres[i][j][k] = *sphere;
            }
        }
    }

    return spheres;
}