#version 330 core

layout(points) in;
layout(points, max_vertices=3) out;

uniform usampler3D world;

const ivec3 offsets[3] = ivec3[3](
	ivec3(1, 0, 0),
	ivec3(0, 1, 0),
	ivec3(0, 0, 1)
);

out uint facecode;

void main()
{
	ivec3 position = ivec3(
		gl_PrimitiveIDIn % 63,
		gl_PrimitiveIDIn / 63 % 63,
		gl_PrimitiveIDIn / (63 * 63));
	
	uint self = texelFetch(world, position, 0).r;
	bool self_solid = self != 0;
	
	for(uint i = 0; i < 3; i++)
	{
		uint other = texelFetch(world, position + offsets[i], 0).r;
		bool other_solid = other != 0;
		if (self_solid != other_solid)
		{
			ivec3 facepos = position + (self_solid ? ivec3(0) : offsets[i]);
			facecode =
				(self_solid ? 1 : 0) |
				(i << 1)             |
				(facepos.x << 8)     |
				(facepos.y << 16)    |
				(facepos.z << 24);
			
			EmitVertex();
		}
	}
}
