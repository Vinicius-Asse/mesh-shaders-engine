#pragma once

// Bibliotecas Principais
#include<imgui/imgui.h>
#include<imgui/imgui_impl_sdl.h>
#include<imgui/imgui_impl_opengl3.h>

#include<glad/glad.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_opengl.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/string_cast.hpp>
#include<glm/gtx/hash.hpp>
#include<glm/gtc/random.hpp>

// Variaveis Globais
#include<core/global.hpp>

// Classes de Abstracoes
#include<core/parameters.hpp>

#include<core/utils.hpp>
#include<core/mesh.hpp>
#include<core/marching_cubes.hpp>
#include<core/marching_cubes_compute_impl.hpp>
#include<core/marching_cubes_mesh_impl.hpp>
#include<core/shader.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/camera.hpp>

// Bibliotecas Auxiliares
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<cstdlib> 
#include<ctime>
#include<queue>
#include<map>

// Definicao de Funcoes
void setupWindow(const char*);
void finishError(std::string);
void mainLoop(ImGuiIO&);
void updatePoints(Point*, Parameters*);
Point* createPoints(Parameters*);
double timeControl(Uint32*);
void clearScreen(float, float, float, float);
void setupImGuiFrame(ImGuiIO& io, SDL_Event *e);
void GLAPIENTRY MessageCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);
void drawImGuiElements(Program*, ImGuiIO&, Parameters*, Camera);
Program* getProgram(Parameters*, Program*, Program*, Program*);
Mesh* setupWorldBounds(Shader*);
void resize(int, int);