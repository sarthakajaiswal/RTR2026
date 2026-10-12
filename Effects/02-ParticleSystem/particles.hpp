#pragma once 

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <GL\glew.h> 
#include <GL\GL.h> 
#include <iostream> 
#include <vector> 

#include "vmath.h" 

#define MAX_PARTICLES       100 
#define EMMITER_POSITION    vmath::vec2(0.0f, 0.0f) 

typedef struct 
{
    vmath::vec2 position;     // position 
    vmath::vec2 velocity;    // direction and speed 
    float lifespan; 
    vmath::vec4 color; 
} Particle; 

unsigned int nr_particles = 100; 
std::vector<Particle> particles; 

GLuint particleVAO; 

void initParticle(Particle* p) 
{
    p->position = vmath::vec2(0.0f); 
    p->velocity = vmath::vec2(1.0f); 
    p->lifespan = 0.0f; 
    p->color = vmath::vec4(1.0f);  
}  

extern FILE* gpFile; 
void initParticles() 
{
    srand((unsigned int)time(0));

    for(unsigned int i = 0; i < nr_particles; ++i) 
    {
        Particle p; 
        initParticle(&p); 
        particles.push_back(p); 
    } 

    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    }; 
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
} 

int FirstUnusedParticle() 
{
    static unsigned int lastUsedParticle = 0; 

    for(unsigned int i = lastUsedParticle; i < nr_particles; ++i) 
    {
        if(particles[i].lifespan <= 0.0f) 
        {
            lastUsedParticle = i; 
            return (i); 
        } 
    } 

    // otherwise do linear search 
    for(unsigned int i = 0; i < lastUsedParticle; ++i) 
    {
        if(particles[i].lifespan <= 0.0f) 
        {
            lastUsedParticle = i; 
            return (i); 
        } 
    } 

    // override first particles if all others are alive 
    lastUsedParticle = 0; 
    return (0); 
} 

void RespawnParticle(Particle *particle)
{
    float randX = ((rand() % 100) - 50) / 50.0f;   // -1.0 .. 1.0
    float randY = ((rand() % 100) - 50) / 50.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);

    particle->position  = EMMITER_POSITION + vmath::vec2(randX, randY) * 0.1f; // small spread around emitter
    particle->color     = vmath::vec4(rColor, rColor, rColor, 1.0f);
    particle->lifespan  = (rand()%100)/100.0f;
    particle->velocity  = vmath::vec2(randX, randY) * 0.5f;
}

void updateParticles() 
{
    unsigned int nr_new_particles = 10; 

    // add new particles 
    for(unsigned int i = 0; i < nr_new_particles; ++i) 
    {
        int unusedParticle = FirstUnusedParticle(); 
        RespawnParticle(&particles[unusedParticle]); 
    } 

    // update all particles 
    float dt = 0.01; 
    for(unsigned int i = 0; i < nr_particles; ++i) 
    {
        Particle &p = particles[i]; 
        p.lifespan -= 0.001; 
        if(p.lifespan > 0.0f) 
        {
            p.position -= p.velocity * dt; 
            p.color[3] -= 2.5f * dt; 
        } 
    } 
} 

void renderParticles(GLuint _shaderProgram, GLuint _texture) 
{
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 

    glUseProgram(_shaderProgram); 

    for(int i = 0; i < particles.size(); ++i) 
    { 
        Particle particle = particles[i]; 
        if(particle.lifespan > 0.0f) 
        {
            glUniform2fv(glGetUniformLocation(_shaderProgram, "offset"), 1, particle.position); 
            glUniform4fv(glGetUniformLocation(_shaderProgram, "color"), 1, particle.color); 
            glUniform1f(glGetUniformLocation(_shaderProgram, "life"), particle.lifespan); 

            glActiveTexture(GL_TEXTURE0); 
            glBindTexture(GL_TEXTURE_2D, _texture); 
            glUniform1i(glGetUniformLocation(_shaderProgram, "sprite"), 0); 

            glBindVertexArray(particleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        } 
    } 

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(0); 
} 
