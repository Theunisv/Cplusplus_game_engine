#pragma once

#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	std::string m_Name;
public:
	Texture(const std::string& path, const std::string& name);
	~Texture();

	//unsigned int TextureFromFile();

	void Bind(unsigned int slot = 0) const;
	void Unbind();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	std::string GetName() const {return m_Name;}
	unsigned int getID() const {return m_RendererID;}
};

struct TextureStruct {
	unsigned int id;
	std::string type;
	std::string path;
};
