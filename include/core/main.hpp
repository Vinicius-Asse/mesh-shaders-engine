#ifndef MAIN_HEADER
#define MAIN_HEADER

// Bibliotecas Principais
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/hash.hpp>

// Variaveis Globais
double TimeDeltaTime;

// Classes de Abstracoes
#include "../../src/mesh.cpp"
#include "../../src/program.cpp"
#include "../../src/shader.cpp"
#include "../../src/computeshader.cpp"
#include "../../src/cube.cpp"
#include "../../src/sphere.cpp"
#include "../../src/camera.cpp"

// Bibliotecas Auxiliares
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

// Constantes
#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

// Definicao de Funcoes
void setupWindow(const char*);
void finishError(std::string);
void mainLoop();
double timeControl();

void GLAPIENTRY MessageCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);



#endif