#include<core/shader.hpp>

Shader::Shader(const std::string& shaderPath, ShaderType _type) {
    type = _type;
    composedShader = parseShader(shaderPath);
    uId = createShader(composedShader);
}

Shader::~Shader() {
    glDeleteProgram(uId);
}

void Shader::enable() {
    glUseProgram(uId);
}

void Shader::disable() {
    glUseProgram(0);
}

ComposedShader Shader::parseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    enum class SubType {
        NONE=-1, VERTEX=0, FRAGMENT=1, COMPUTE=2, MESH=3
    };

    std::string line;
    std::stringstream ss[4];
    SubType lineType = SubType::NONE;
    while (getline(stream, line)) {
        // Segrega arquivo por '#! ... shader'
        if (line.find("#!") != std::string::npos) {
            if (line.find("vertex shader") != std::string::npos) {
                lineType = SubType::VERTEX;
            } else if (line.find("fragment shader") != std::string::npos) {
                lineType = SubType::FRAGMENT;
            } else if (line.find("compute shader") != std::string::npos) {
                lineType = SubType::COMPUTE;
            } else if (line.find("mesh shader") != std::string::npos) {
                lineType = SubType::MESH;
            }
        } else {
            // Substitui '#include "caminho/include"' pelo conteudo de caminho/include
            std::regex includeRegex("#include\\s+\"(.*)\"");
            if (std::regex_search(line, includeRegex)) {
                std::smatch sm;
                std::string includePath = "";
                if (std::regex_match(line, sm, includeRegex)) {
                    includePath = sm[1].str();
                }
                ss[(int)lineType] << Utils::readFile(includePath) << "\n";
            }else {
                ss[(int)lineType] << line << '\n';
            }
        }
    }
    return { ss[0].str(), ss[1].str(), ss[2].str(), ss[3].str() };
}

unsigned int Shader::createShader(ComposedShader composedShader) {
    GLuint program = glCreateProgram();

    switch(type) {
        case ShaderType::VERTEX_SHADER: {
            GLuint vs = compileShader(GL_VERTEX_SHADER, composedShader.VertexShader);
            GLuint fs = compileShader(GL_FRAGMENT_SHADER, composedShader.FragmentShader);

            glAttachShader(program, vs);
            glAttachShader(program, fs);
            glLinkProgram(program);

            validateProgram(program);

            glDeleteShader(vs);
            glDeleteShader(fs);

            break;
        }
        case ShaderType::COMPUTE_SHADER: {
            GLuint cs = compileShader(GL_COMPUTE_SHADER, composedShader.ComputeShader);

            glAttachShader(program, cs);
            glLinkProgram(program);

            validateProgram(program);

            glDeleteShader(cs);

            break;
        }
        case ShaderType::MESH_SHADER: {
            glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);

            GLuint ms = compileShader(GL_MESH_SHADER_NV, composedShader.MeshShader);
            GLuint fs = compileShader(GL_FRAGMENT_SHADER, composedShader.FragmentShader);

            glAttachShader(program, ms);
            glAttachShader(program, fs);

            glLinkProgram(program);
            
            validateProgram(program);

            glDetachShader(program, ms);
            glDetachShader(program, fs);
            glDeleteShader(ms);
            glDeleteShader(fs);

            GLuint pipeline;
            glGenProgramPipelines(1, &pipeline);
            glUseProgramStages(pipeline, GL_MESH_SHADER_BIT_NV | GL_FRAGMENT_SHADER_BIT, program);
        }
    }

    return program;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
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
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

void Shader::validateProgram(GLint program) {
    GLint status;
    glValidateProgram(program);
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
            
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", infoLog.data(), NULL);
            SDL_Quit();
        }
    }

}