#version 450 core
out vec4 FragColor; 
in vec2 UV; //From fsTriangle.vert
in vec4 _LightSpacePos;

//All your material and lighting uniforms go here!

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

//layout(binding = i) can be used as an alternative to shader.setInt()
//Each sampler will always be bound to a specific texture unit
uniform layout(binding = 0) sampler2D _gPositions;
uniform layout(binding = 1) sampler2D _gNormals;
uniform layout(binding = 2) sampler2D _gAlbedo;
uniform layout(binding = 3) sampler2D _ShadowMap;

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

vec3 calculateLighting(vec3 normal, vec3 pos, vec3 albedo)
{
	vec3 lightColor;

	//vec3 normal = normalize(fs_in.WorldNormal);
	vec3 toLight = -_LightDirection;

	vec3 ambient = _Material.Ka * _LightColor;

	float difuseFactor = max(dot(normal, toLight),0.0f);
	vec3 diffuse = difuseFactor * _LightColor;

	vec3 toEye = normalize(_EyePos - pos);
	vec3 h = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,h),0.0f), _Material.Shininess);
	vec3 specular = specularFactor * _LightColor;

	float bias = max(0.05 * (1.0 - dot(normal, _LightDirection)), 0.005);
	float shadow = calcShadow(_ShadowMap, _LightSpacePos, bias);

	vec3 lighting = ambient + diffuse + specular;

	return lightColor;
}


void main(){
	//Sample surface properties for this screen pixel
	vec3 normal = texture(_gNormals,UV).xyz;
	vec3 worldPos = texture(_gPositions,UV).xyz;
	vec3 albedo = texture(_gAlbedo,UV).xyz;

	//Worldspace lighting calculations, same as in forward shading
	vec3 lightColor = calculateLighting(normal,worldPos,albedo);
	FragColor = vec4(albedo * lightColor,1.0);
}
