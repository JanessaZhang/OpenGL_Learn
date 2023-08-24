#shader vertex
#version 330 core

layout(location = 0)in vec4 position;
layout(location = 1)in vec4 a_color;

out vec4 v_color;

uniform mat4 u_MVP;
// uniform float pointsize;

void main()
{
	gl_Position = u_MVP*position;
	// gl_PointSize = pointsize;
	v_color=a_color;
};

#shader fragment
#version 330 core

layout(location = 0)out vec4 color;

in vec4 v_color;

uniform vec4 u_color;

void main()
{
	color = v_color;
};