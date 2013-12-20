run: cubespace shaders/noise3D.glsl
	./cubespace

cubespace: main.c
	clang -std=c99 -Wall `pkg-config --cflags glfw3` -o cubespace main.c `pkg-config --static --libs glfw3 glew` -D_BSD_SOURCE -Ilinmath.h

shaders/noise3D.glsl:
	echo '#version 330 core' >shaders/noise3D.glsl
	cat webgl-noise/src/noise3D.glsl >>shaders/noise3D.glsl
