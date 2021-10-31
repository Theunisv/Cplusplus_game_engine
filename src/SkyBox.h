#pragma once
#include <vector>
#include <string>
class SkyBox
{

private:
	unsigned int skyboxVAO, skyboxVBO;

public:
	SkyBox();
	~SkyBox();

	void Draw(unsigned int cubemapTexture);
};

