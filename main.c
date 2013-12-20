#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

static void gl_log(GLenum category, GLenum severity, GLuint id, GLenum e, 
                   GLsizei length, const GLchar* message, GLvoid* userParam)
{
	puts(message);
}
	
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

int main(void)
{
	if (!glfwInit())
	{
		fputs("Failed to initialize glfw", stderr);
		exit(EXIT_FAILURE);
	}
	
	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	#endif
	
	GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		fputs("Failed to open window", stderr);
		exit(EXIT_FAILURE);
	}
	
	glfwMakeContextCurrent(window);
	
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK)
	{
		fputs("Failed to initilize glew", stderr);
		exit(EXIT_FAILURE);
	}
	glGetError(); //Clears an error set by GLEW
	
	#ifndef NDEBUG
	glDebugMessageCallbackARB(gl_log, NULL);
	#endif
	
	glUseProgram(worldgen_program);
	glViewport(0, 0, 64, 64);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, 64);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, faces);
	glBufferData(GL_ARRAY_BUFFER, GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS * sizeof(int), NULL, GL_STATIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glUseProgram(facegen_program);
	glUniform1i(glGetUniformLocation(facegen_program, "world"), 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, texture);
	
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, faces);
	
	
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, count_query);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 63*63*63);
	glEndTransformFeedback();
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glGetQueryObjectuiv(count_query, GL_QUERY_RESULT, &faces_count);
	
	
	printf("%d faces generated\n", faces_count);
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glUseProgram(render_program);
	
	GLint transform_uniform = glGetUniformLocation(render_program, "transform");
	glViewport(0, 0, 640, 480);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	float rotate = 0;
	while (!glfwWindowShouldClose(window))
	{
		rotate += 0.01f;
		mat4x4 transform, tmpa, tmpb;
		mat4x4_perspective(tmpb, 70, 1, 0.1f, 1000);
		mat4x4_look_at(tmpa, (vec3){sin(rotate)*64 + 32,100,cos(rotate)*64 + 32}, (vec3){32,32,32}, (vec3){0,64,0});
		mat4x4_mul(transform, tmpb, tmpa);
		glUniformMatrix4fv(transform_uniform, 1, GL_FALSE, (const float*)transform);
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, faces);
		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_POINTS, 0, faces_count);
		glDisableVertexAttribArray(0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		glFlush();
		usleep(16000);
	}
	
	
	
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}
