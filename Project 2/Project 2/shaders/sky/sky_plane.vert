#version 460

layout (location = 0) in vec4 inPos;

uniform mat4 inverseMatrix;

out vec4 near4;
out vec4 far4;

void main()
{
	gl_Position = vec4(vec2(inPos.x, inPos.y), 1.0, 1.0);

	vec2 pos = gl_Position.xy / gl_Position.w;

	near4 = inverseMatrix * (vec4(pos, -1.0, 1.0));
	far4 = inverseMatrix * (vec4(pos, 1.0, 1.0));
}