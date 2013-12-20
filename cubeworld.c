#include <GL/glew.h>
#include <stdbool.h>

#ifndef CUBEWORLD_TERRAIN_LENGTH
#define CUBEWORLD_TERRAIN_LENGTH 8
#endif

struct cubeworld
{
	GLuint worldgen;
	GLuint facegen;
	GLuint render;
	GLuint worldgen_framebuffer;
	GLuint worldgen_texture;
	GLuint face_buffer;
	GLuint count_query;
	GLuint terrain[CUBEWORLD_TERRAIN_LENGTH][CUBEWORLD_TERRAIN_LENGTH];
};

static GLuint load_shader(const char* filename, GLenum type)
{
	FILE *file = fopen(filename, "r");
	
	if (file == NULL)
	{
		printf("Failed to load %s\n", filename);
		return 0;
	}
	
	printf("Compiling %s\n", filename);
	
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	rewind(file);
	char source[size];
	fread(source, 1, size, file);
	fclose(file);
	
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char*[]){ source }, (const int[]){ size });
	glCompileShader(shader);
	return shader;
}

static GLuint build_program(int shadercount, const GLuint shaders[shadercount], const char *feedback)
{
	GLuint program = glCreateProgram();
	if (shaders != NULL)
		for (int i = 0; i < shadercount; i++)
			glAttachShader(program, shaders[i]);
	
	if (feedback != NULL)
	{
		int i;
		for (i = 0; feedback[i] != NULL; i++);
		glTransformFeedbackVaryings(program, feedback, GL_INTERLEAVED_ATTRIBS);
	}
	
	glLinkShader(program);
}

bool init_cubeworld(struct cubeworld *world)
{
	enum
	{
		noise = 0, worldgenvs, worldgengs, worldgenfs, facegenvs,
		facegengs, rendervs, rendergs, renderfs, shadercount
	};
	
	*world = (struct cubeworld){0};
	
	bool success = true;
	
	GLuint shaders[] = (GLuint[]) { 
		[worldgennoise] = load_shader("shaders/noise3D.glsl",     GL_FRAGMENT_SHADER),
		[worldgenvs]    = load_shader("shaders/worldgen.vs.glsl", GL_VERTEX_SHADER),
		[worldgengs]    = load_shader("shaders/worldgen.gs.glsl", GL_GEOMETRY_SHADER),
		[worldgenfs]    = load_shader("shaders/worldgen.fs.glsl", GL_FRAGMENT_SHADER),
		[facegenvs]     = load_shader("shaders/facegen.vs.glsl",  GL_VERTEX_SHADER),
		[facegengs]     = load_shader("shaders/facegen.gs.glsl",  GL_GEOMETRY_SHADER),
		[rendervs]      = load_shader("shaders/render.vs.glsl",   GL_VERTEX_SHADER),
		[rendergs]      = load_shader("shaders/render.gs.glsl",   GL_GEOMETRY_SHADER),
		[renderfs]      = load_shader("shaders/render.fs.glsl",   GL_FRAGMENT_SHADER)
	};
	
	for (int i = 0; i < shadercount; i++)
		if (shaders[i] == 0)
			return false;
	
	puts("Linking worldgen program");
	world->worldgen = build_program(4, &shaders[worldgennoise], NULL);
	puts("Linking facegen program");
	world->facegen = build_program(2, &shaders[facegenvs], (const char*[]){ "facecode", NULL });
	puts("Linking render program");
	world->render = build_program(3, &shaders[rendervs], NULL);
	
	if (world->worldgen == 0 || world->facegen == 0 || world->render == 0)
		return false;
		
	glGenTextures(1, &world->worldgen_texture);
	glBindTexture(GL_TEXTURE_3D, world->worldgen_texture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, 64, 64, 64, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, world->worldgen_texture, 0);
	glDrawBuffers(1, (GLenum[]){ GL_COLOR_ATTACHMENT0 });
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		puts("Failed to create framebuffer");
		return false;
	}
	
	glGenBuffers(1, &world->face_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, world->face_buffer);
	glBufferData(GL_ARRAY_BUFFER, 32768 * sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenQueries(1, &world->count_query);
	
	for (int i = 0; i < shadercount; i++)
		if (shaders[i] != 0)
			glDeleteShader(i);
	
	return true;
}

void deinit_cubeworld(struct cubeworld *world)
{
	glDeleteProgram(world->worldgen);
	glDeleteProgram(world->facegen);
	glDeleteProgram(world->render);
	glDeleteFramebuffers(1, world->worldgen_framebuffer);
	glDeleteTextures(1, world->worldgen_texture);
	glDeleteBuffers(1, world->face_buffer);
	glDeleteQueries(1, world->count_query);
}
