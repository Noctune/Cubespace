#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform mat4 transform;

in uint facecode[];

flat out vec4 frag_normal;
out vec2 frag_texpos;

const vec3 normals[] = vec3[](
	vec3(-1,  0,  0),
	vec3( 1,  0,  0),
	vec3( 0, -1,  0),
	vec3( 0,  1,  0),
	vec3( 0,  0, -1),
	vec3( 0,  0,  1)
);

const vec2 vertices[] = vec2[](
	vec2( 0.5,  0.5),
	vec2(-0.5,  0.5),
	vec2( 0.5, -0.5),
	vec2(-0.5, -0.5)
);

const vec2 texpos[] = vec2[](
	vec2(1, 1),
	vec2(0, 1),
	vec2(1, 0),
	vec2(0, 0)
);

void main()
{
	vec4 normal = vec4(normals[facecode[0] & 7], 0);
	
	vec4 position = vec4(
		(facecode[0] >> 8)  & 255,
		(facecode[0] >> 16) & 255,
		(facecode[0] >> 24) & 255,
		1
	);
	
	mat4 supertransform = transform * mat4(
		normal.zxyw,
		abs(normal.yzxw),
		normal,
		position
	);
	
	frag_normal = normal;
	for(uint i = 0; i < vertices.length(); i++)
	{
		frag_texpos = texpos[i];
		gl_Position = supertransform * vec4(vertices[i], 0.5f, 1);
		EmitVertex();
	}
}

