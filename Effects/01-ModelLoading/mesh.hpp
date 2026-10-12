#pragma once 

#include <stdio.h> 
#include <iostream> 
#include <string> 
#include <vector> 

#include <GL\glew.h>

#include "vmath.h" 
#include "global.hpp" 

#define MAX_UV_CHANNELS 8 

extern FILE* gpFile; 

struct vertex 
{
    vmath::vec3 position; 
    vmath::vec3 normal; 
    vmath::vec2 texCoords[MAX_UV_CHANNELS]; 
    vmath::vec3 tangent; 
    vmath::vec3 bitangent; 
}; 

struct Texture 
{
    GLuint id; 
    std::string type; 
    std::string path; 
    unsigned short uvChannel; 
}; 

class Mesh 
{
public: 
    std::vector<vertex>         vertices; 
    std::vector<unsigned int>   indices; 
    std::vector<Texture>        textures;
    vmath::mat4     transformMatrix;  

    Mesh(std::vector<vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures); 
    void draw(GLuint& shader, vmath::mat4 _modelMatrix, vmath::mat4 _viewMatrix, vmath::mat4 _projMatrix); 

private: 
    unsigned int vao, vbo, ebo; 

    void setupMesh(); 
}; 

Mesh::Mesh(std::vector<vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures) 
{
    vertices = _vertices; 
    indices = _indices; 
    textures = _textures; 

    setupMesh(); 
} 

void Mesh::setupMesh() 
{
    glGenVertexArrays(1, &vao); 
    glGenBuffers(1, &vbo); 
    glGenBuffers(1, &ebo); 

    glBindVertexArray(vao); 
    glBindBuffer(GL_ARRAY_BUFFER, vbo); 

    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertex), &vertices[0], GL_STATIC_DRAW); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW); 

    // vertex position 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)OFFSET_OF(vertex, position)); 
    // vertex normals 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL); 
    glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)OFFSET_OF(vertex, normal)); 
    // vertex texture coordinates 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0); 
    glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)OFFSET_OF(vertex, texCoords[0])); 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE1); 
    glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)OFFSET_OF(vertex, texCoords[1])); 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE2); 
    glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)OFFSET_OF(vertex, texCoords[2])); 

    glBindVertexArray(0); 
} 

void Mesh::draw(GLuint& _shader, vmath::mat4 _modelMatrix, vmath::mat4 _viewMatrix, vmath::mat4 _projectionMatrix)  
{
    unsigned int diffuseNr = 1; 
    unsigned int specularNr = 1; 

    glUseProgram(_shader); 

    _modelMatrix *= transformMatrix; 
	glUniformMatrix4fv(glGetUniformLocation(_shader, "uMVPMatrix"), 1, 
		GL_FALSE, _projectionMatrix*_viewMatrix*_modelMatrix); 

    for(unsigned int i = 0; i < textures.size(); ++i) 
    {
        glActiveTexture(GL_TEXTURE0 + i); 

        std::string number; 
        std::string name = textures[i].type; 
        if(name == "texture_diffuse") 
        {
            number = std::to_string(diffuseNr); 
            diffuseNr += 1; 
        } 
        else if(name == "texture_specular") 
        {
            number = std::to_string(specularNr); 
            specularNr += 1; 
        } 

        glUniform1i(glGetUniformLocation(_shader, ("material."+name+number).c_str()), i); 
        glUniform1i(glGetUniformLocation(_shader, "channel"), textures[i].uvChannel); 
        glBindTexture(GL_TEXTURE_2D, textures[i].id); 
    } 
    glActiveTexture(GL_TEXTURE0); 

    glBindVertexArray(vao); 
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0); 

    glUseProgram(0); 
} 

