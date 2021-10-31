#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Renderer.h"
#include "Shader.h"



Shader::Shader()
{
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
        : m_vertexPath(vertexPath), m_fragmentPath(fragmentPath), ID(0)
    {

        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        //bool geometryShaderGiven;
        //if (geometryPath == nullptr)
        //    geometryShaderGiven = false;
        //else
        //    geometryShaderGiven = true;
    
        //// 1. retrieve the vertex/fragment source code from filePath
        //std::string vertexCode;
        //std::string fragmentCode;
        //std::string geometryCode;
        //std::ifstream vShaderFile;
        //std::ifstream fShaderFile;
        //std::ifstream gShaderFile;
        //// ensure ifstream objects can throw exceptions:
        //vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        //fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        //gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        //try
        //{
        //    // open files
        //    vShaderFile.open(vertexPath);
        //    fShaderFile.open(fragmentPath);
        //    std::stringstream vShaderStream, fShaderStream;
        //    // read file's buffer contents into streams
        //    vShaderStream << vShaderFile.rdbuf();
        //    fShaderStream << fShaderFile.rdbuf();
        //    // close file handlers
        //    vShaderFile.close();
        //    fShaderFile.close();
        //    // convert stream into string
        //    vertexCode = vShaderStream.str();
        //    fragmentCode = fShaderStream.str();
        //    // if geometry shader path is present, also load a geometry shader
        //    if (geometryShaderGiven)
        //    {
        //        gShaderFile.open(geometryPath);
        //        std::stringstream gShaderStream;
        //        gShaderStream << gShaderFile.rdbuf();
        //        gShaderFile.close();
        //        geometryCode = gShaderStream.str();

        //    }
        //}
        //catch (std::ifstream::failure& e)
        //{
        //    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        //}

        ////CompileShader(returnSource.VertexSource, returnSource.FragmentSource, returnSource.GometrySource);
        ////return returnSource;

        //GLuint vertexShader, fragmentShader, geometryShader;
        //const char* vertexSource = vertexCode.c_str();
        //const char* fragmentSource = fragmentCode.c_str();
        //const char* geometrySource = geometryCode.c_str();

        //// vertex shader
        //vertexShader = glCreateShader(GL_VERTEX_SHADER);
        //glShaderSource(vertexShader, 1, &vertexSource, NULL);
        //glCompileShader(vertexShader);
        //// fragment Shader
        //fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        //glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        //glCompileShader(fragmentShader);

        //// if geometry shader is given, compile geometry shader
        //if (geometryShaderGiven)
        //{
        //    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        //    glShaderSource(geometryShader, 1, &geometrySource, NULL);
        //    glCompileShader(geometryShader);

        //}

        //ID = glCreateProgram();
        //glAttachShader(ID, vertexShader);
        //glAttachShader(ID, fragmentShader);
        //if (geometryShaderGiven)
        //    glAttachShader(ID, geometryShader);
        //glLinkProgram(ID);
        //// delete the shaders as they're linked into our program now and no longer necessery
        //glDeleteShader(vertexShader);
        //glDeleteShader(fragmentShader);
        //if (geometryShaderGiven)
        //    glDeleteShader(geometryShader);

        //glUseProgram(ID);
    }

    Shader::~Shader()
{
    GLCall(glDeleteProgram(ID));
}

void Shader::Bind() const
{
    GLCall(glUseProgram(ID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}




void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0,v1,v2,v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1 , GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(ID, name.c_str()));


    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    
    m_UniformLocationCache[name] = location;
    return location;    
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void Shader::setBool(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
