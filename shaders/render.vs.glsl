#version 330 core

layout(location = 0) in uint fcode;
out uint facecode;

void main()
{
	facecode = fcode;
}
