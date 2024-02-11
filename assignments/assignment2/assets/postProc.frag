#version 450

out vec4 FragColor;

in vec2 UV;

uniform sampler2D _ColorBuffer;
uniform int _KernalIndex;
//Didn't work... :(
/*float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}*/

const float offset = 1.0f / 300.0f;

vec2 offsets[9] = vec2[](
	vec2(-offset, -offset), //bottom left
	vec2(0.0f, -offset), //bottom center
	vec2(offset, -offset), //bottom right
	vec2(offset, 0.0), //right
	vec2(offset, offset), //top right
	vec2(0.0, offset), //top center
	vec2(-offset, offset), //top left
	vec2(-offset, 0.0), //left
	vec2(0.0, 0.0) //center
	);

float sharpenKernal[9] = {
	-1, -1, -1,
	-1, 9, -1,
	-1, -1, -1
};

float blurKernal[9] = {
	1/16f, 1/8f, 1/16f,
	1/8f, 1/4f, 1/8f,
	1/16f, 1/8f, 1/16f
};

float edgeDetectionKernal[9] = {
	1, 1, 1,
	1, -8, 1,
	1, 1, 1
};
void main()
{
	float kernal[9];

	switch(_KernalIndex)
	{
		case 0: 
			kernal = sharpenKernal;
			break;
		case 1:
			kernal = edgeDetectionKernal;
			break;
		case 2:
			kernal = blurKernal;
			break;
		default:
			kernal = blurKernal;
	}

//inverse color
	//vec3 newColor = 1.0 - texture(_ColorBuffer, UV).rgb;
	//FragColor = vec4(newColor, 1.0);

	//float depth = linearizeDepth(gl_FragCoord.z);

//greyscale
	//FragColor = texture(_ColorBuffer, UV);
    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	//FragColor = vec4(vec3(average), 1.0);

//Kernals
	vec3 sampleTex[9];

	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(_ColorBuffer, UV.st + offsets[i])); 
	}

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
	{
		col += sampleTex[i] * kernal[i];
	}

	FragColor = vec4(col, 1.0);
}