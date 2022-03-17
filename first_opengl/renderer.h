#ifndef RENDERER_H_
#define RENDERER_H_

#include <glad.h>
#include <GLFW/glfw3.h>

static void drawBufferData(GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer,
    static float* vert, static unsigned int* ind,
    int vert_size, int ind_size, int count)
{
    glBindVertexArray(vertex_array);               // bind array first
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);  // bind to gl_array_buffer
    glBufferData(GL_ARRAY_BUFFER, vert_size, vert, GL_STATIC_DRAW);  // write to buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);    // for vertex array buffers
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_size, ind, GL_STATIC_DRAW);
    // Tell OpenGL how to interpret vertex buffer  (index, size(x,y,z), dtype, normalized?, stride, offset) 
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}


static void loadTexture(GLuint texture, unsigned char* data, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // generate texture(target, mipmap_level, format, width, height, 0, format, datatype, data)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

}


#endif 