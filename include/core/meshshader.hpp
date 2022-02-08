#pragma once

#include<glad/glad.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<regex>


class MeshShader 
{
private:
    ComposedShader composedShader;
    ComposedShader parseShader(const std::string& filePath);
    unsigned int createShader(ComposedShader ComposedShader);
    unsigned int compileShader(unsigned int type, const std::string& source);

public:
    MeshShader(const std::string& shaderPath);
    ~MeshShader();

    void enable();
    void disable();
    
    unsigned int uId;
    unsigned int pipeline;
};