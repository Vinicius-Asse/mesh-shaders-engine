#include<core/meshshader.hpp>

MeshShader::MeshShader(const std::string& shaderPath) {
    composedShader = parseShader(shaderPath);
    uId = createShader(composedShader);
}

MeshShader::~MeshShader() {
    glDeleteProgram(uId);
}

void MeshShader::enable() {
    glUseProgram(uId);
    glBindProgramPipeline(pipeline);
}

void MeshShader::disable() {
    glBindProgramPipeline(0);
    glUseProgram(0);
}

ComposedShader MeshShader::parseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1, MESH = 2
    };

    std::string line;
    std::stringstream ss[3];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#!") != std::string::npos) {
            if (line.find("vertex shader") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment shader") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            } else if (line.find("mesh shader") != std::string::npos) {
                type = ShaderType::MESH;
            }
        } else {
            ss[(int)type] << line << '\n';
        }
    }
    
    return { ss[0].str(), ss[1].str(), ss[2].str() };
}

unsigned int MeshShader::createShader(ComposedShader composedShader) {
    unsigned int program = glCreateProgram();

    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);

    unsigned int ms = compileShader(GL_MESH_SHADER_NV, composedShader.MeshShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, composedShader.FragmentShader);

    glAttachShader(program, ms);
    glAttachShader(program, fs);

    glLinkProgram(program);
    
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
    {
        std::cout << "Failed to link shader program!" << std::endl;
        GLint logLen;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0)
        {
            GLsizei written;
            std::string(logLen, ' ');
            std::vector<GLchar> infoLog(logLen);
            glGetProgramInfoLog(program, logLen, &written, &infoLog[0]);
            std::cout << "Program log: " << std::endl << infoLog.data() << std::endl;

            glDeleteProgram(program);
            abort();
        }
    }
    
    glValidateProgram(program);

    glDetachShader(program, ms);
    glDetachShader(program, fs);
    glDeleteShader(ms);
    glDeleteShader(fs);


    glGenProgramPipelines(1, &pipeline);
    glUseProgramStages(pipeline, GL_MESH_SHADER_BIT_NV | GL_FRAGMENT_SHADER_BIT, program);
    
    return program;
}

unsigned int MeshShader::compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(sizeof(char) * length);
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile compute shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}