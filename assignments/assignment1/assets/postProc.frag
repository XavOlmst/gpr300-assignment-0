#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _ColorBuffer;

//Didn't work... :(
/*float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}*/

void main()
{
//inverse color
	vec3 newColor = 1.0 - texture(_ColorBuffer, UV).rgb;

	//float depth = linearizeDepth(gl_FragCoord.z);

//greyscale
	FragColor = texture(_ColorBuffer, UV);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;

	FragColor = vec4(vec3(average), 1.0);
}