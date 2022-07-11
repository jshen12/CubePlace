#ifndef RENDERER_H_
#define RENDERER_H_

#include <glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

static float text_vertices[] = {
    // positions               // texture coords
     0.45f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
     0.45f, -1.0f, 0.0f,    1.0f, 0.0f, // bottom right
    -0.45f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
    -0.45f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
};
static unsigned int text_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

static float crosshair_vertices[] = {
    -0.01f, 0.0f, 0.0f,
    0.01f, 0.0f, 0.0f,
    0.0f, -0.02f, 0.0f,
    0.0f, 0.02f, 0.0f
};


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
    //glDisableVertexAttribArray(0);
    //glDisableVertexAttribArray(1);
}

static void drawText(GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer, std::string text, float x, float y)
{
    // TODO: MAKE ONE TRIANGLE MESH, INSTEAD OF MULTIPLE ONES
    glBindVertexArray(vertex_array);
    std::vector<float> verts = {};
    std::vector<unsigned int> inds = {};
    int offsetX;
    int offsetY;
    int indCount = 0;
    float scale = 0.05f;
    for (int i = 0; i < text.length(); i++) {
        offsetX = (text[i] - 32) % 16;
        offsetY = 7 - (text[i] - 32) / 16;
        for (int v = 0; v < 4; v++)
        {

            // vertecies
            verts.push_back( (text_vertices[v * 5]) * scale + x + (scale * i * 0.75f));
            verts.push_back( (text_vertices[v * 5 + 1]) * scale + y);
            verts.push_back(text_vertices[v * 5 + 2]);
            // uv tex coords
            verts.push_back(text_vertices[v * 5 + 3] / 16.0f + (offsetX / 16.0f));
            verts.push_back(text_vertices[v * 5 + 4] / 8.0f + (offsetY / 8.0f));

        }

        inds.push_back(indCount);
        inds.push_back(indCount + 1);
        inds.push_back(indCount + 3);
        inds.push_back(indCount + 1);
        inds.push_back(indCount + 2);
        inds.push_back(indCount + 3);
        indCount += 4;

    }

    drawBufferData(vertex_array, vertex_buffer, element_buffer, &verts[0], &inds[0],
        sizeof(float) * verts.size(), sizeof(unsigned int) * inds.size(), inds.size());
}

static void drawLines(GLuint vertex_array, GLuint vertex_buffer)
{
    glBindVertexArray(vertex_array); 
    glLineWidth(4 * 1);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);  // bind to gl_array_buffer
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), crosshair_vertices, GL_STATIC_DRAW);  // write to buffer

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, 12);
}

#endif 