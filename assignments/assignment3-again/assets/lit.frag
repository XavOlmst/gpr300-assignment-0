#version 450
out vec4 FragColor; //The color of this fragment
in Surface
{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoords;
	vec4 LightSpacePos;
}fs_in; //Interpolated of this fragment 

uniform sampler2D _ShadowMap;
uniform sampler2D _MainTex;
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0); //White light

struct Material{
	float Ka; //ambient coefficent
	float Kd; //diffuce coefficent
	float Ks; //specular coefficent
	float Shininess; //ooooh shiny
};

uniform Material _Material;

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos, float bias)
{
	vec3 sampleCoord = lightSpacePos.xyz / lightSpacePos.w;
	sampleCoord = sampleCoord * 0.5f + 0.5f;

	float myDepth = sampleCoord.z;
	float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;

	if(myDepth > 1.0f)
		return 0.0f;

	float totalShadow = 0.0f;

	vec2 texelOffset = 1.0 / textureSize(shadowMap, 0);

	for(int y = -1; y <= 1; y++)
	{
		for(int x = -1; x <= 1; x++)
		{
			float pcfDepth = texture(shadowMap, sampleCoord.xy + vec2(x, y) * texelOffset).r;
			totalShadow += step(pcfDepth, myDepth - bias);
		}
	}

	totalShadow /= 9.0f;

	return totalShadow;
}

void main(){
	//Shade with 0-1 normal
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 toLight = -_LightDirection;

	vec3 ambient = _Material.Ka * _LightColor;

	float difuseFactor = max(dot(normal, toLight),0.0f);
	vec3 diffuse = difuseFactor * _LightColor;

	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,h),0.0f), _Material.Shininess);
	vec3 specular = specularFactor * _LightColor;

	float bias = max(0.05 * (1.0 - dot(normal, _LightDirection)), 0.005);
	float shadow = calcShadow(_ShadowMap, fs_in.LightSpacePos, bias);

	vec3 lighting = ambient + (diffuse + specular) * (1.0f - shadow);

	vec3 objColor = texture(_MainTex, fs_in.TexCoords).rgb;

	FragColor = vec4(lighting * objColor, 1.0f);
}
