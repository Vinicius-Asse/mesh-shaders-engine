#ifndef SHADER_CLASS
#define SHADER_CLASS

#include<glad/glad.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

struct ComposedShader
{
    std::string VertexShader;
    std::string FragmentShader;
};

class Shader 
{
private:
    ComposedShader composedShader;
    ComposedShader parseShader(const std::string& filePath);
    unsigned int createShader(ComposedShader composedShader);
    unsigned int compileShader(unsigned int type, const std::string& source);

public:
    Shader(const std::string& shaderPath);
    ~Shader();

    void enable();
    void disable();
    
    unsigned int uId;
};

#endif