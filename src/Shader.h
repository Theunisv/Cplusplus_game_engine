#pragma once
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

struct ShaderProgramSource
{
	const char* VertexSource;
	const char* FragmentSource;
	const char* GometrySource;
};

class Shader
{
private:
    const char* m_vertexPath;
	const char* m_fragmentPath;
	const char* m_geometryPath;
	
	std::unordered_map<std::string, int> m_UniformLocationCache;

    void ParseShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
	void CompileShader(const char* vShaderCode, const char* fShaderCode, const char* gShaderCode);
	void CreateShader(const GLuint vertexShader, const GLuint fragmentShader, const GLuint geometryShader);
	int GetUniformLocation(const std::string& name);
	void checkCompileErrors(unsigned int shader, std::string type);
public:
	Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	GLuint ID;

	//Util Functions

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w);
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
};

