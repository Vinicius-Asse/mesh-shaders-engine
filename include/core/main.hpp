#ifndef MAIN_HEADER
#define MAIN_HEADER

// Bibliotecas Principais
#include <glad/glad.h>
#include <SDL2/SDL.h>

// Classes de Abstracoes
#include "../../src/mesh.cpp"

// Bibliotecas Auxiliares
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Constantes
#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

struct ParsedShader;

// Definicao de Funcoes
void setupWindow(const char*);
void finishError(std::string);
void mainLoop();
ParsedShader parseShader(const std::string&);
unsigned int createShader(const std::string&, const std::string&);



#endif