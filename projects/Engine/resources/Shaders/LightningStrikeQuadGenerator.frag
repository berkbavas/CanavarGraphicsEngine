#version 330 core
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

uniform vec4 color = vec4(1);

in vec3 fsColor; 

void main()
{
	fragColor = vec4(fsColor, 1.0f);
    brightColor = fragColor;
}