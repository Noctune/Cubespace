#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

out vec3 position;

const vec2 corners[] = vec2[](
	vec2(-1, -1),
	vec2( 1, -1),
	vec2(-1,  1),
	vec2( 1,  1)
);

void main()
{
	gl_Layer = gl_PrimitiveIDIn;
	float z = float(gl_PrimitiveIDIn) / 63.0 * 2.0 - 1.0;
	for (int i = 0; i < corners.length(); i++)
	{
		position = vec3(corners[i], z);
		gl_Position = vec4(position, 1);
		EmitVertex();
	}
}
