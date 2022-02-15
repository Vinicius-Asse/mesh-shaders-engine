#ifndef SHADER_CLASS
#define SHADER_CLASS

#include<glad/glad.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<regex>

struct ComposedShader
{
    std::string VertexShader;
    std::string FragmentShader;
    std::string ComputeShader;
    std::string MeshShader;
};

enum class ShaderType { NONE=-1, VERTEX_SHADER=0, COMPUTE_SHADER=1, MESH_SHADER=2 };
inline const char* ToString(ShaderType st) {
    switch (st) {
        case ShaderType::VERTEX_SHADER: return "VERTEX_SHADER";
        case ShaderType::COMPUTE_SHADER:return "COMPUTE_SHADER";
        case ShaderType::MESH_SHADER:   return "MESH_SHADER";
        default:      return "NONE";
    }
}

class Shader 
{
private:
    ShaderType type;
    ComposedShader composedShader;
    ComposedShader parseShader(const std::string& filePath);
    unsigned int createShader(ComposedShader composedShader);
    unsigned int compileShader(unsigned int type, const std::string& source);
    void validateProgram(GLint);

public:
    Shader(const std::string& shaderPath, ShaderType type);
    ~Shader();

    void enable();
    void disable();
    
    unsigned int uId;

};

#endif