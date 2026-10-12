#pragma once 

#include <stdio.h> 
#include <iostream> 
#include <string> 
#include <vector> 

#include <GL\glew.h>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h> 

#include "vmath.h" 
#include "mesh.hpp"
#include "stb_image.h"  

GLuint TextureFromFile(const char* path, const std::string& directory, bool gamma=false); 

extern FILE* gpFile; 

class Model 
{
public: 
    Model(char* path); 
    void draw(GLuint& shader, vmath::mat4 _modelMat, vmath::mat4 _viewMat, vmath::mat4 _projectionMat); 

private: 
    std::vector<Texture> textures_loaded; 
    std::vector<Mesh> meshes; 
    std::string directory; 

    void loadModel(std::string path); 
    void processNode(aiNode* node, const aiScene* scene, vmath::mat4 accumulatedTransform); 
    Mesh processMesh(aiMesh* mesh, const aiScene* scene); 
    std::vector<Texture> loadMaterialTextures(aiMaterial* _material, aiTextureType _type, std::string _typeName); 
}; 

Model::Model(char* path) 
{
    loadModel(path); 
    fprintf(gpFile, "Model Loaded Succesfully : %s\n", path); 
} 

void Model::draw(GLuint& shader, vmath::mat4 _modelMatrix, vmath::mat4 _viewMatrix, vmath::mat4 _projectionMatrix) 
{ 
    for(unsigned int i = 0; i < meshes.size(); ++i) 
    {
        meshes[i].draw(shader, _modelMatrix, _viewMatrix, _projectionMatrix);  
    } 
} 

extern char gszLogFileName[]; 
void Model::loadModel(std::string path) 
{
    Assimp::Importer import; 
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); 

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP:: " << import.GetErrorString() << std::endl; 
        return; 
    } 

    directory = path.substr(0, path.find_last_of('/')); 

    processNode(scene->mRootNode, scene, vmath::mat4::identity());     
} 

void Model::processNode(aiNode* node, const aiScene* scene, vmath::mat4 accumulatedTransform) 
{
    aiMatrix4x4 t = node->mTransformation;
    t.Transpose();  // aiMatrix4x4 is row-major; transposing gives us column-major for OpenGL
    vmath::mat4 localTransform = vmath::mat4(
        vmath::vec4(t.a1, t.a2, t.a3, t.a4),   
        vmath::vec4(t.b1, t.b2, t.b3, t.b4),   
        vmath::vec4(t.c1, t.c2, t.c3, t.c4),   
        vmath::vec4(t.d1, t.d2, t.d3, t.d4)    
    );

    vmath::mat4 currentTransformation = accumulatedTransform * localTransform;  

    // process all the node's meshes (if any) 
    for(unsigned int i = 0; i < node->mNumMeshes; ++i) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        Mesh m = processMesh(mesh, scene);
        m.transformMatrix = currentTransformation;  
        meshes.push_back(m); 
    } 

    // do same of each of his children 
    for(unsigned int i = 0; i < node->mNumChildren; ++i) 
    {
        processNode(node->mChildren[i], scene, currentTransformation); 
    } 
} 

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) 
{
    std::vector<vertex> vertices; 
    std::vector<unsigned int> indices; 
    std::vector<Texture> textures; 

    for(int i = 0; i < mesh->mNumVertices; ++i) 
    {
        vertex vertex; 

        // process vertex position, normal and texture coordinates 
        vmath::vec3 vector; 

        // 1] position 
        vector[0] = mesh->mVertices[i].x; 
        vector[1] = mesh->mVertices[i].y; 
        vector[2] = mesh->mVertices[i].z; 
        vertex.position = vector; 

        // 2] Normals 
        if(mesh->HasNormals()) 
        {
            vector[0] = mesh->mNormals[i].x; 
            vector[1] = mesh->mNormals[i].y; 
            vector[2] = mesh->mNormals[i].z; 
            vertex.normal = vector; 
        } 

        // 3] TexCoords 
        for(int channelNo = 0; channelNo < MAX_UV_CHANNELS; ++channelNo) 
        {
            if(mesh->mTextureCoords[channelNo]) 
            {
                vmath::vec2 vec; 

                vec[0] = mesh->mTextureCoords[channelNo][i].x; 
                vec[1] = mesh->mTextureCoords[channelNo][i].y; 
                vertex.texCoords[channelNo] = vec; 
            }
            else 
            {
                vertex.texCoords[channelNo] = vmath::vec2(0.0f); 
            }
        } 

        // 4] tangents and bitangents 
        if(mesh->HasTangentsAndBitangents())
        {
            // tangent 
            vector[0] = mesh->mTangents[i].x; 
            vector[1] = mesh->mTangents[i].y; 
            vector[2] = mesh->mTangents[i].z; 
            vertex.tangent = vector; 

            // bitangent 
            vector[0] = mesh->mBitangents[i].x; 
            vector[1] = mesh->mBitangents[i].y; 
            vector[2] = mesh->mBitangents[i].z; 
            vertex.bitangent = vector; 
        }  

        vertices.push_back(vertex); 
    } 

    // indices 
    for(unsigned int i = 0; i < mesh->mNumFaces; ++i) 
    {
        aiFace face = mesh->mFaces[i]; 
        for(unsigned int j = 0; j < face.mNumIndices; ++j) 
            indices.push_back(face.mIndices[j]); 
    } 

    // materials 
    if(mesh->mMaterialIndex >= 0) 
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; 
        
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); 
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); 

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"); 
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end()); 
    } 

    return Mesh(vertices, indices, textures); 
} 

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName) 
{
    std::vector<Texture> textures; 
    int uvIndex = 0; 

    for(unsigned int i = 0; i < material->GetTextureCount(type); ++i) 
    {
        aiString str; 
        material->GetTexture(type, i, &str); 

        // check if texture was already loaded and if so continue to next iteration 
        bool skip = false; 
        for(unsigned int j = 0; j < textures_loaded.size(); ++j) 
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) 
            {
                textures.push_back(textures_loaded[j]); 
                skip = true;    // a texture with the same filepath has already been loaded, continue to next one
                break; 
            } 
        } 

        if(skip == false) 
        {
            Texture texture; 
            texture.id = TextureFromFile(str.C_Str(), directory); 
            texture.type = typeName; 
            texture.path = str.C_Str(); 
            texture.uvChannel = material->Get(AI_MATKEY_UVWSRC(type, i), uvIndex); 

            textures.push_back(texture); 
            textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        } 
    } 

    return textures; 
} 

GLuint TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = std::string(path); 
    filename = directory + '/' + filename; 

    unsigned int textureID; 
    glGenTextures(1, &textureID); 

    int width, height, nrComponents; 
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        fprintf(gpFile, "Texture loaded at path: %s\n", path); 
    }
    else
    {
        fprintf(gpFile, "Texture failed to load at path: %s\n", path); 
        stbi_image_free(data);
    }

    return textureID;
} 
