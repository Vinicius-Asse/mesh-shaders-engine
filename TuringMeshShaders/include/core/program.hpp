#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>
#include<unordered_map>
#include<iostream>

#include<core/structs.h>
#include<core/utils.hpp>
#include<core/camera.hpp>
#include<core/mesh.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/parameters.hpp>
#include<core/shader.hpp>
#include<core/constants/tables.hpp>

class Program
{
    public:
        Program(Parameters* _param, Shader* _baseShader, Point* _points) { 
            param = _param;
            baseShader = _baseShader;
            points = _points;
        };

        std::string name;

        std::map<std::string, std::string> meshInfo;

        /***
         * Método Executado Quando o Programa é Iniciado
        **/
        virtual void start() { LOG("Método Start não implementado!"); }

        /***
         * Método Executado no Inicio de Cada Frame para recebimento de entradas do usuário
        **/
        virtual void input(SDL_Event*) { LOG("Método Input não implementado!"); }

        /***
         * Método Executado Toda Frame
        **/
        virtual void update() { LOG("Método Update não implementado!"); }

        /***
         * Método Executado ao Fim de Toda Frame
        **/
        virtual void draw() { LOG("Método Draw não implementado!"); }

        int getPointsCount() {
            return sizeof(points)/sizeof(*points);
        }

    protected:
        Parameters* param;
        Shader* baseShader;
        Point* points;
};