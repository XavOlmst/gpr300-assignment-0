#version 450
//Vertex attributes
layout(location = 0) in vec3 vPos; 
layout(location = 1) in vec3 vNormal; 
layout(location =2) in vec2 vTexCoords;

uniform mat4 _Model; 
uniform mat4 _ViewProjection; 

out Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoords;
}vs_out;

void main(){
	vs_out.WorldPos = vec3(_Model * vec4(vPos, 1.0f));
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;

	vs_out.TexCoords = vTexCoords;
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0f);
}
