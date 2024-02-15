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
uniform sampler2D mainTex;
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

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos)
{
	vec3 sampleCoord = lightSpacePos.xyz / lightSpacePos.w;
	sampleCoord = sampleCoord * 0.5 + 0.5;

	float myDepth = sampleCoord.z;
	float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;

	return step(shadowMapDepth, myDepth);
}

void main(){
	//Shade with 0-1 normal
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 toLight = -_LightDirection;

	vec3 ambient = _Material.Ka * _LightColor;

	float difuseFactor = max(dot(normal, toLight),0.0);
	vec3 diffuse = difuseFactor * _LightColor;

	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,h),0.0), _Material.Shininess);
	vec3 specular = specularFactor * _LightColor;

	float shadow = calcShadow(_ShadowMap, fs_in.LightSpacePos);

	vec3 lighting = ambient + ((diffuse + specular) * (1.0 - shadow));

	vec3 objColor = texture(mainTex, fs_in.TexCoords).rgb;

	//objColor = pow(objColor, vec3(1.0/2.2));

	FragColor = vec4(objColor * lighting, 1.0);
}