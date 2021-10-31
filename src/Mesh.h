#pragma once
#include <vector>
#include "Vertex.h"
#include "Texture.h"

class Mesh {
public:
    std::vector<Vertex>         vertices;
    std::vector<unsigned int>   indices;
    std::vector<TextureStruct>  textures;

    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureStruct> textures);
    void Draw(Shader& shader);
private:
    //  render data
    unsigned int VBO, EBO;

    void setupMesh();
};