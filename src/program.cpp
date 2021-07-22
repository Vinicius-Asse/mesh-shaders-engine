#include<core/program.hpp>

struct Point
{
    glm::vec3 position;
    float value;
    bool visible;
};

Point*** instantiatePoints(int countX, int countY, int countZ, bool visible, Shader* shader);

    Camera*             camera;
    Cube*               cube;
    Sphere*             sphere;
    Point***            points;
    std::vector<Sphere> spheres;


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

    int pCount = 10;
    points = instantiatePoints(pCount, pCount, pCount, true, baseShader);
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

    // for (Sphere s : spheres) {
    //     s.draw();
    // }

    // glm::mat4 mvpMatrix = camera->getMVPMatrix(glm::mat4(1.0f));
    // for (int i = 0; i < 10; i++) {
    //     for (int j = 0; j < 10; j++) {
    //         for (int k = 0; k < 10; k++) {
    //             Point p = points[i][j][k];

    //             glm::vec3 pos = glm::vec4(p.position.x, p.position.y, p.position.z, 1.0f) * mvpMatrix;

    //             glBegin(GL_POINTS);
    //             glVertex3f(pos.x, pos.y, pos.z);
    //             glEnd();
    //         }
    //     }
    // }
}


Point*** instantiatePoints(int countX, int countY, int countZ, bool visible, Shader * shader) {
    Point*** points = (Point***) malloc(sizeof(Point**) * countX);
    float scale = 0.001f;

    for (int i = 0; i < countX; i++) {
        points[i] = (Point**) malloc(sizeof(Point*) * countY);
        for (int j = 0; j < countY; j++) {
            points[i][j] = (Point*) malloc(sizeof(Point) * countZ);
            for (int k = 0; k < countZ; k++) {
                Point p = {
                    glm::vec3(i, j, k),
                    1.0f,
                    visible
                };

                points[i][j][k] = p;
                // Sphere* s = Sphere::getInstance(
                //         glm::vec3(p.position.x, p.position.y, p.position.z),
                //         glm::vec3(.05f, .05f, .05f),
                //         8.0f, 4.0f, 
                //         shader
                // );

                // spheres.push_back(*s);
            }
        }
    }

    return points;
}