#version 330 core

float snoise(vec3 v);

layout(location = 0) out uint Value;

in vec3 position;

void main(void){
	Value = snoise(position) + position.y < 0 ? 1 : 0;
}
