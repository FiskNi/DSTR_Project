#version 430
#define LIGHTS_MAX 64

layout(std430, binding = 0) readonly buffer LightIndexBuffer {
    //Point light indexes
    int index[LIGHTS_MAX];
} lightIndexBuffer;

struct P_LIGHT {
    vec3 position;
    vec3 color; //Light that is sent out from the light
    vec4 attenAndRadius;
    float strength;
};

in vec2 f_UV;
in vec3 f_normal; //Comes in normalized
in vec4 f_position;
in vec3 f_tangent;

out vec4 color;
out vec4 brightColor;

vec3 GLOBAL_lightDirection = vec3(0.3f, -0.5f, -0.5f);      // 1 Directional light
vec3 GLOBAL_lightColor = normalize(vec3(1, 1, 1));          // Directional light color (white)

vec3 GLOBAL_lightDirection2 = vec3(0.8f, -0.5f, 0.4f);      // 1 Directional light

float dirlightStr = 0.1f;     // Modifier for brightness (dirlight)
float ambientStr = 0.1f;      // Global light strength (ambient)

uniform vec3 CameraPosition;

uniform vec3 Ambient_Color;             // Change to emmisive
uniform vec3 Diffuse_Color;             // Material diffuse
uniform vec3 Specular_Color;            // Material specular
uniform vec2 TexAndRim;                 // Booleans --- Textures & Rimlighting
uniform int LightCount;                 // Used when lightculling. To know how many lights there are in total in the scene
uniform sampler2D albedoTexture;        // Texture diffuse

uniform bool NormalMapping;				// Use normal mapping or not
uniform sampler2D normalMap;

uniform int grayscale = 0;
uniform P_LIGHT pLights[LIGHTS_MAX];

const int screenX = 1280;
const int screenY = 720;


vec3 normalSampleToWorldSpace(vec3 normalMapSample, vec3 normal, vec3 tangent);
vec3 calcPointLights(P_LIGHT pLight, vec3 normal, vec3 position, float distance, vec3 diffuse);
//Calculate the directional light... Returns the diffuse color, post calculations
vec3 calcDirLight(vec3 normal, vec3 diffuseColor, vec3 lightDirection);
// To simulate death
vec3 grayscaleColour(vec3 col);

void main() {

	vec3 finalNormal = f_normal;
	if(NormalMapping == true)
	{
		//Obtain normal from normal map in range [0, 1]
		vec3 normalMapSample = texture(normalMap, f_UV).rgb;
		finalNormal = normalSampleToWorldSpace(normalMapSample, f_normal, f_tangent);
	}

    //Makes the material full solid color (basically fully lit). Needs bloom for best effect.
    vec3 emissive = Ambient_Color; // Temp used as emmisve, should rename all ambient names to emmisive.

    // Create the diffuse color once
    vec3 diffuseColor = Diffuse_Color;  // Material color

    // Texture slot
    vec4 finalTexture = texture(albedoTexture, f_UV);
    if (TexAndRim.x == 1)
        diffuseColor = vec3(finalTexture);

    // Ambient light
    vec3 ambientLight = diffuseColor * ambientStr;     // Material color

    // Directional light
    vec3 directionalLight = calcDirLight(finalNormal, diffuseColor, GLOBAL_lightDirection);
    directionalLight += calcDirLight(finalNormal, diffuseColor, GLOBAL_lightDirection2);

    //This is a light accumilation over the point lights
    vec3 pointLights = vec3(0.0f);
    for(int i = 0; i < LightCount; i++) {
        //uint lightIndex = lightIndexBuffer.index[i];
        //position += pLights[lightIndex].position;
        float distance = length(f_position.xyz - pLights[i].position);
        //if we are within the light position
        if(distance > pLights[i].attenAndRadius.w) {
            continue;
        }
        else {
            pointLights += calcPointLights(pLights[i], f_normal, f_position.xyz, distance, diffuseColor) * pLights[i].strength;
        }
    }

    // Resulting light
    vec4 result = vec4(ambientLight + directionalLight + pointLights + emissive, finalTexture.a); // We see light, so add only and all the lights together to get color

    if(grayscale == 1){
    	result.xyz = grayscaleColour(result.xyz);
    }

    color = result;
    
    // Simple crosshair (hardcoded)
    if (abs(gl_FragCoord.x - (screenX / 2)) < 1 && abs(gl_FragCoord.y - (screenY / 2)) < 7 )
    {
        color.xyz = 1 - color.xyz;
    }
    if (abs(gl_FragCoord.x - (screenX / 2)) < 7 && abs(gl_FragCoord.y - (screenY / 2)) < 1 )
    {
        if (abs(gl_FragCoord.x - (screenX / 2)) > 1)
        {
           color.xyz = 1 - color.xyz;
        }
    }
}

vec3 normalSampleToWorldSpace(vec3 normalMapSample, vec3 normal, vec3 tangent) {
	vec3 bumpedNormal;

	//Uncompress each component from [0,1] to [-1, 1]
	vec3 normalT = normalize(2.0f*normalMapSample - 1.0f);

	//Build orthonormal basis and TBN matrix
	vec3 N = normal;
	vec3 T = normalize(tangent - dot(tangent,N)*N);
	vec3 B = cross(N,T);

	mat3 TBN = mat3(T, B, N);

	//Transform from tangent space to world space
	bumpedNormal = normalT * TBN;

	return bumpedNormal;
}

vec3 calcPointLights(P_LIGHT pLight, vec3 normal, vec3 position, float distance, vec3 diffuse) {
    vec3 lightDir = normalize(pLight.position - position); //From the surface to the light
    float diff = max(dot(normal, lightDir), 0);
    vec3 diffuseLight = diffuse * diff * normalize(pLight.color);

    vec3 newDiffuse = diffuseLight;
    float f = 0.15; // desaturate by %
    float L = 0.3 * newDiffuse.r + 0.6 * newDiffuse.g + 0.1 * newDiffuse.b;
    float new_r = newDiffuse.r + f * (L - newDiffuse.r);
    float new_g = newDiffuse.g + f * (L - newDiffuse.g);
    float new_b = newDiffuse.b + f * (L - newDiffuse.b);
    newDiffuse = vec3(new_r, new_g, new_b);
    diffuseLight = newDiffuse;

    float attenuation = 1.0 / (pLight.attenAndRadius.x + pLight.attenAndRadius.y * distance +
  			     pLight.attenAndRadius.z * (distance * distance));

    return (diffuseLight) * attenuation;
}

vec3 calcDirLight(vec3 normal, vec3 diffuseColor, vec3 lightDirection) {
    /* --- DIFFUSE SHADING --- */
    float lightStr = dirlightStr;
    vec3 lightDir = normalize(-lightDirection);
    float nDotL = dot(normal, lightDir);

    float diff = smoothstep(0.0, 0.01, (max(dot(normal, lightDir), 0.0)));

    vec3 newDiffuse = diffuseColor;

    float f = 0.0; // desaturate by %
    float L = 0.3 * newDiffuse.r + 0.6 * newDiffuse.g + 0.1 * newDiffuse.b;
    float new_r = newDiffuse.r + f * (L - newDiffuse.r);
    float new_g = newDiffuse.g + f * (L - newDiffuse.g);
    float new_b = newDiffuse.b + f * (L - newDiffuse.b);
    newDiffuse = vec3(new_r, new_g, new_b);

    diffuseColor = newDiffuse * (max(dot(normal, lightDir), 0.0)) * lightStr * GLOBAL_lightColor;

    /* --- SPECULAR SHADING --- */
    if(TexAndRim.y == 1) {
        float specularStr = 0.5f;

        vec3 viewDir = normalize(CameraPosition - f_position.xyz); //normalize(CameraPosition - f_position.xyz);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
        //Lock specular
        spec = smoothstep(0.005, 0.01, spec);
        vec3 specular = specularStr * spec * GLOBAL_lightColor;

        vec3 rimColor = vec3(1.0);
        float rimThreshold = 0.1;
        float rimDot = 1 - dot(viewDir, normal); //Rim value
        float rimIntensity = rimDot * pow(nDotL, rimThreshold);
        rimIntensity = smoothstep(0.7 - 0.01, 0.7 - 0.01, rimIntensity);
        rimColor = diffuseColor * rimIntensity;

        return diffuseColor + specular + rimColor;
    }
    else {
        return diffuseColor;
    }
}

vec3 grayscaleColour(vec3 col) {
    float colourValue = (col.r + col.g + col.b) / 3; //Calculate the average pixel colour
    return vec3(colourValue);
}
