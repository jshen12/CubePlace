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
	int vert_size, int ind_size, int count, bool needsRebinding) {
	glBindVertexArray(vertex_array);               // bind array first

	// only bind to buffer if verticies (i.e. world) changes
	if (needsRebinding) {
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);  // bind to gl_array_buffer
		glBufferData(GL_ARRAY_BUFFER, vert_size, vert, GL_STATIC_DRAW);  // write to buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);    // for vertex array buffers
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_size, ind, GL_STATIC_DRAW);
		// Tell OpenGL how to interpret vertex buffer  (index, size(x,y,z), dtype, normalized?, stride, offset) 
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// normal attribute
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

static void drawLines(GLuint vertex_array, GLuint vertex_buffer) {
	glBindVertexArray(vertex_array);
	glLineWidth(4 * 1);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);  // bind to gl_array_buffer
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), crosshair_vertices, GL_STATIC_DRAW);  // write to buffer

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 12);
}

#endif 