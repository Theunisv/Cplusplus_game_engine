#pragma once
#include <string>

#include "Texture.h"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <matrix4x4.h>
#include <matrix4x4.inl>
#include <assimp/postprocess.h>


unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:
    std::vector<TextureStruct> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;
    int triangleCount;
    TextureStruct diffuseTexture;
    glm::vec3 translateUniform;
    glm::vec3 rotationUniform;
    glm::vec3 scaleUniform;

    bool isDoor = false;
    bool isLight = false;
    bool isGun = false;

    glm::mat4 position;
   
    void init();
    int id;
    std::string name;
    void SetName(std::string name);

    float getTranslatex();
    void setTranslateUniform(glm::vec3 translation);
    void setRotationeUniform(glm::vec3 rotation);
    void setScaleUniform(glm::vec3 scale);
    glm::vec3 getTranslationUniform();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureStruct> textures;

    Model();
    // constructor, expects a filepath to a 3D model.
    //Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
    //{
     //   loadModel(path);
    //}
    
    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    void loadModel(std::string path);

private:
    
    void processNode(aiNode* node, const aiScene* scene);
    void scale(float scaleFactor);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureStruct> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);
};

