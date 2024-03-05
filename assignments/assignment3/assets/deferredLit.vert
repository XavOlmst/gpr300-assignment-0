#version 450

out vec2 UV;
out vec4 LightSpacePos;

uniform mat4 _Model;
uniform mat4 _ViewProjection; 
uniform mat4 _LightViewProjection;

vec4 vertices[3] = 
{
	vec4(-1, -1, 0, 0), //bottom left (X,Y,U,V)
	vec4(3, -1, 2, 0), //bottom right
	vec4(-1, 3, 0, 2) //top left
};

void main()
{
	UV = vertices[gl_VertexID].zw;

	gl_Position = vec4(vertices[gl_VertexID].xy, 0, 1);

	vec3 worldPos = vec3(_Model * gl_Position);
	_LightSpacePos = _LightViewProjection * _Model * vec4(worldPos, 1.0f);
}