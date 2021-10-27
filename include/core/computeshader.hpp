#pragma once

#include<glad/glad.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<regex>


class ComputeShader 
{
private:
    std::string parseShader(const std::string& filePath);
    unsigned int createShader(std::string shaderStr);
    unsigned int compileShader(unsigned int type, const std::string& source);

public:
    ComputeShader(const std::string& shaderPath);
    ~ComputeShader();

    void enable();
    void disable();
    
    unsigned int uId;
};