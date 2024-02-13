#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D depthMap;

void main()
{
	float depthValue = texture(depthMap, UV).r;

	FragColor = vec4(vec3(depthValue), 1.0);
}