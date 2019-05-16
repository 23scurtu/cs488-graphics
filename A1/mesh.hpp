# pragma once

#include <stddef.h>
#include <cstdio>
#include <algorithm>
#include <vector>
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;
using namespace std;

const float PI = 3.14159265f;

// float cube_verts[ 24 ] = {
//     // front
//     -1.0, -1.0,  1.0,
//     1.0, -1.0,  1.0,
//     1.0,  1.0,  1.0,
//     -1.0,  1.0,  1.0,
//     // back
//     -1.0, -1.0, -1.0,
//     1.0, -1.0, -1.0,
//     1.0,  1.0, -1.0,
//     -1.0,  1.0, -1.0
// };

// int cube_indicies[36] = {
//     // front
//     0, 1, 2,
//     2, 3, 0,
//     // right
//     1, 5, 6,
//     6, 2, 1,
//     // back
//     7, 6, 5,
//     5, 4, 7,
//     // left
//     4, 0, 3,
//     3, 7, 4,
//     // bottom
//     4, 5, 1,
//     1, 0, 4,
//     // top
//     3, 2, 6,
//     6, 7, 3
// };

class Mesh
{
    vector<float> vertices;
    vector<unsigned int> indicies;

    GLuint mesh_vao; // Vertex Array Object
	GLuint mesh_vbo; // Vertex Buffer Object

    GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint col_uni;   // Uniform location for cube colour.

    int VERT = 0;

    glm::vec3 translation;
    glm::vec3 color;
    glm::vec3 scale = vec3(1,1,1);

public:
    // TODO Move to private
    glm::quat rotation;

    Mesh() = default;
    Mesh(vector<float> vertices, vector<unsigned int> indices);
    void initBuffers(vector<float> vertices, vector<unsigned int> indices);
    void applyTranslation(glm::vec3 t);
    void setTranslation(glm::vec3 t){ translation = t; };
    void setScale(glm::vec3 s){ scale = s; }
    void setColor(glm::vec3 color){ this->color = color; }
    glm::vec3 getTranslation(){ return translation; }
    glm::vec3 getColor(){ return color; }
    void draw(ShaderProgram &shader, glm::mat4 parent = glm::mat4(1.0));
};

class Cube: public Mesh
{
    // vector<float> verts;
    // vector<unsigned int> indicies;

    public:
    Cube(): Mesh(initVertices(), initIndices()) {}
    ~Cube() {}

    vector<unsigned int> initIndices(){ vector<unsigned int> indicies; for(size_t i = 0; i < 36; i++) indicies.push_back(i); return indicies; }
    vector<float> initVertices();

    // void draw(ShaderProgram &shader) { draw(shader); }

};

class Square: public Mesh
{
    public:
    Square(size_t width): Mesh(initVertices(width), initIndices()) { setColor(glm::vec3(1,0,0)); }
    ~Square() {}

    vector<unsigned int> initIndices(){ vector<unsigned int> indicies; for(size_t i = 0; i < 4; i++) indicies.push_back(i); return indicies; }
    vector<float> initVertices(size_t width){  vector<float> result{-1, -1, 0,
                                                                    1, -1, 0,
                                                                    1, 1, 0,
                                                                    
                                                                    -1, -1, 0,
                                                                    1, 1, 0,
                                                                    -1, 1, 0 };
                                                for(float &x : result){ x *=width/2; }
                                                return result; }

    
};

class Sphere: public Mesh
{
    public:
    Sphere(float radius, size_t longs, size_t lats): Mesh(initVertices(radius, longs, lats), initIndices()) { setColor(glm::vec3(1,0,0)); }
    ~Sphere() {}

    vector<unsigned int> initIndices(){ vector<unsigned int> indicies; for(size_t i = 0; i < 4; i++) indicies.push_back(i); return indicies; }
    vector<float> initVertices(float radius, size_t longs, size_t lats)
    {  
        // vector<float> result{-1, -1, 0,
        //                                                             1, -1, 0,
        //                                                             1, 1, 0,
                                                                    
        //                                                             -1, -1, 0,
        //                                                             1, 1, 0,
        //                                                             -1, 1, 0 };
        vector<float> result;

        float lat_angle = 2.0f*PI/lats;
        float lon_angle = 2.0f*PI/longs;

        for(size_t x = 0; x < lats; x++)
        {
            for(size_t y = 0; y < longs; y++)
            {
                float lat = lat_angle * x;
                float lon = lon_angle * y;

                auto p1 = glm::vec3(radius * cos(lat) * cos(lon),
                                    radius * cos(lat) * sin(lon),
                                    radius * sin(lat));
                auto p2 = glm::vec3(radius * cos(lat + lat_angle) * cos(lon),
                                    radius * cos(lat + lat_angle) * sin(lon),
                                    radius * sin(lat + lat_angle));
                auto p3 = glm::vec3(radius * cos(lat + lat_angle) * cos(lon + lon_angle),
                                    radius * cos(lat + lat_angle) * sin(lon + lon_angle),
                                    radius * sin(lat + lat_angle));
                auto p4 = glm::vec3(radius * cos(lat) * cos(lon + lon_angle),
                                    radius * cos(lat) * sin(lon + lon_angle),
                                    radius * sin(lat));
                
                result.push_back(p1.x);
                result.push_back(p1.y);
                result.push_back(p1.z);
                result.push_back(p2.x);
                result.push_back(p2.y);
                result.push_back(p2.z);
                result.push_back(p3.x);
                result.push_back(p3.y);
                result.push_back(p3.z);

                result.push_back(p1.x);
                result.push_back(p1.y);
                result.push_back(p1.z);
                result.push_back(p3.x);
                result.push_back(p3.y);
                result.push_back(p3.z);
                result.push_back(p4.x);
                result.push_back(p4.y);
                result.push_back(p4.z);
            }
        }
        return result; 
    }
};