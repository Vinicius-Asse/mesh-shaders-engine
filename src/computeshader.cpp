#include<core/computeshader.hpp>

ComputeShader::ComputeShader(const std::string& shaderPath) {
    std::string shaderStr = parseShader(shaderPath);
    uId = createShader(shaderStr);
    //std::cout << "[" << std::to_string(uId) << "] * Compute Shader Program Created * " << std::endl;
}

ComputeShader::~ComputeShader() {
    //std::cout << "[" << std::to_string(uId) << "] ~ Deleting Shader Data" << std::endl;
    glDeleteProgram(uId);
}

void ComputeShader::enable() {
    glUseProgram(uId);
}

void ComputeShader::disable() {
    glUseProgram(0);
}

std::string ComputeShader::parseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    std::string line;
    std::stringstream ss;
    while (getline(stream, line)) {
        std::regex includeRegex("#include\\s+\"(.*)\"");

        if (std::regex_search(line, includeRegex)) {
            std::smatch sm;
            std::string includePath = "";
            if (std::regex_match(line, sm, includeRegex)) {
                includePath = sm[1].str();
            }
            ss << parseShader(includePath) << "\n";
        } else {
            ss << line << "\n";
        }
    }

    return ss.str();
}

unsigned int ComputeShader::createShader(std::string shaderStr) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_COMPUTE_SHADER, shaderStr);

    glAttachShader(program, vs);

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

    glDeleteShader(vs);

    return program;
}

unsigned int ComputeShader::compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)alloca(sizeof(char) * lenght);
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "Failed to compile compute shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}