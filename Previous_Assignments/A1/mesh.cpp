#include "mesh.hpp"

vector<float> cube_verts = 
{
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

Mesh::Mesh(vector<float> vertices, vector<unsigned int> indices)
{
    initBuffers(vertices, indices);
}

void Mesh::initBuffers(vector<float> vertices, vector<unsigned int> indices)
{
    // Create the vertex array to record buffer assignments.
    glGenVertexArrays( 1, &mesh_vao );
    glBindVertexArray( mesh_vao );

    // Create the cube vertex buffer
    glGenBuffers( 1, &mesh_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, mesh_vbo );
    glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(float),
        &vertices[0], GL_STATIC_DRAW );

    glGenBuffers( 1, &mesh_ebo );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    // GLint posAttrib = shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( VERT );
    glVertexAttribPointer( VERT, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

    // Reset state to prevent rogue code from messing with *my* 
    // stuff!
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // OpenGL has the buffer now, there's no need for us to keep a copy.
    // delete [] verts;

    this->vertices = vertices;
    this->indicies = indices;

    CHECK_GL_ERRORS;
}

void Mesh::applyTranslation(glm::vec3 t)
{
    this->translation += t;
}

void Mesh::draw(ShaderProgram &shader, glm::mat4 parent)
{
    // // Create a global transformation for the model (centre it).
    mat4 W;
    W = glm::translate( W, this->translation ) * glm::toMat4(rotation) * glm::scale(mat4(1.0f), scale);
    W = parent*W;
    
    shader.enable();
        glEnable( GL_DEPTH_TEST );

        GLint M_uni = shader.getUniformLocation( "M" );
        glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

        // Just draw the mesh for now.
        glBindVertexArray( mesh_vao );

        col_uni = shader.getUniformLocation( "colour" );
        glUniform3f( col_uni, color.r, color.g, color.b );

        // Indicies are ready to use but disabled for now.  
        // Uncomment line below and comment glDrawArrays to enable.
        // glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);

        // Draw the cubes
        // Highlight the active square.
    shader.disable();

    // Restore defaults
    glBindVertexArray( 0 );

    CHECK_GL_ERRORS;
}

vector<float> Cube::initVertices()
{
    vector<float> verts = cube_verts;

    for(size_t i = 0; i < 108; i++)
    {
        verts[i] += 1.0f;
        verts[i] *= 0.5f;
    }

    return verts;
}

vector<float> Square::initVertices(size_t width){  vector<float> result{-1, -1, 0,
                                                                        1, -1, 0,
                                                                        1, 1, 0,
                                                                        
                                                                        -1, -1, 0,
                                                                        1, 1, 0,
                                                                        -1, 1, 0 };
                                                for(float &x : result){ x *=width/2; }
                                                return result; }

vector<float> Sphere::initVertices(float radius, size_t longs, size_t lats)
{  
    vector<float> result;

    float lat_angle = 2.0f*PI/lats;
    float lon_angle = PI/longs;

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