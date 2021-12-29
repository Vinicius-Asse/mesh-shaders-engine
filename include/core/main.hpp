#ifndef MAIN_HEADER
#define MAIN_HEADER

// Bibliotecas Principais
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/random.hpp>

// Variaveis Globais
double TimeDeltaTime;

// Classes de Abstracoes
#include "../../src/utils.cpp"
#include "../../src/mesh.cpp"
#include "../../src/program.cpp"
#include "../../src/compute.cpp"
#include "../../src/shader.cpp"
#include "../../src/computeshader.cpp"
#include "../../src/cube.cpp"
#include "../../src/sphere.cpp"
#include "../../src/camera.cpp"
#include "../../src/parameters.cpp"

// Bibliotecas Auxiliares
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib> 
#include <ctime>

// Definicao de Funcoes
void setupWindow(const char*);
void finishError(std::string);
void mainLoop(ImGuiIO&);
void updatePoints(Point*, Parameters*);
Point* createPoints(Parameters*);
double timeControl();
int getFramerate(Uint32);

void GLAPIENTRY MessageCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);



#endif