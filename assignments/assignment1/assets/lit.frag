#version 450
out vec4 FragColor; //The color of this fragment

in Surface
{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoords;
	mat3 TBN;
}fs_in; //Interpolated of this fragment 

uniform sampler2D mainTex;
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0); //White light
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

struct Material{
	float Ka; //ambient coefficent
	float Kd; //diffuce coefficent
	float Ks; //specular coefficent
	float Shininess; //ooooh shiny
};

uniform Material _Material;
uniform sampler2D normalMap;

void main(){
	//Shade with 0-1 normal
	vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
	normal = normalize(normal * 2.0f - 1.0f);
	normal = normalize(fs_in.TBN * normal);

	vec3 toLight = -_LightDirection;
	float difuseFactor = max(dot(normal, toLight),0.0);

	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,h),0.0), _Material.Shininess);

	vec3 lightColor = (_Material.Kd * difuseFactor +  _Material.Ks * specularFactor) * _LightColor;
	lightColor += _AmbientColor * _Material.Ka;
	vec3 objColor = texture(mainTex, fs_in.TexCoords).rgb;

	FragColor = vec4(objColor * lightColor, 1.0);
}