#ifndef MAIN_HEADER
#define MAIN_HEADER

// Bibliotecas Principais
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Classes de Abstracoes
#include "../../src/mesh.cpp"
#include "../../src/shader.cpp"
#include "../../src/cube.cpp"

// Bibliotecas Auxiliares
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Constantes
#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

// Definicao de Funcoes
void setupWindow(const char*);
void finishError(std::string);
void mainLoop();


#endif