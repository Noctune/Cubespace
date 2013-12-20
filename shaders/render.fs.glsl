#version 330 core
 
out vec4 color;
flat in vec4 frag_normal;
in vec2 frag_texpos;

void main()
{
	color = max(dot(frag_normal, -normalize(vec4(0, -1, -1, -1))), 0) * vec4(0.5, 0.5, 0.5, 0) +
	        vec4(0.5, 0.5, 0.5, 1);
}
