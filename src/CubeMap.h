#pragma once
#include <vector>
#include <string>

class CubeMap {
private:	
	unsigned int cubeVAO, cubeVBO, cubeTexture, cubeMapTexture;
	std::vector<std::string> faces;
	

public:
	CubeMap();
	~CubeMap();

	void Draw();
	unsigned int loadCubemap(std::vector<std::string> faces);
};