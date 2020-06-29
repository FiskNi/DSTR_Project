#version 430
#define LIGHTS_MAX 64

layout(std430, binding = 0) readonly buffer LightIndexBuffer {
    //Point light indexes
    int index[LIGHTS_MAX];
} lightIndexBuffer;

struct P_LIGHT {
    vec3 position;
    vec3 color; //Light that is sent out from the light
    float radius;
};

in vec2 f_UV;
in vec3 f_normal; //Comes in normalized
in vec4 f_position;

out vec4 color;
out vec4 brightColor;

vec3 GLOBAL_lightDirection = vec3(0.2, -0.7, 0.0);
vec3 GLOBAL_lightColor = normalize(vec3(109, 196, 199));
float ambientStr = 0.3f;


uniform vec3 CameraPosition;

uniform vec3 Ambient_Color;
uniform vec3 Diffuse_Color;
uniform vec3 Specular_Color;
uniform vec2 TexAndRim;
uniform bool NormalMapping;
uniform float time;

uniform int LightCount;
uniform sampler2D shieldTexture;

uniform int grayscale = 0;
uniform P_LIGHT pLights[LIGHTS_MAX];

vec3 calcDirLight(vec3 normal, vec3 diffuseColor);
vec2 rotate(float magnitude, vec2 p);

void main() {
    vec3 pivot = vec3(0.5, -0.5, 1.);
    vec2 p = f_UV - pivot.xy;
    p = rotate(3.14 * time * 0.1, p);
    float a = atan(p.y, p.x) * 1;
    float r = sqrt(dot(p,p));
    vec2 finalUV;
    finalUV.x = (time * -.5) - 1/(r + 1.7);
    finalUV.y = pivot.z * a/3.1416;

    vec3 position = vec3(0);
    //Create the diffuse color once
    vec3 diffuse = Diffuse_Color;
    vec4 alphaTexture = texture(shieldTexture, f_UV);

    //vec3 ambientCol = (Ambient_Color + ambientStr);
    if (TexAndRim.x == 1) {
        diffuse *= alphaTexture.rgb;
    }

    vec3 result = diffuse;

    //HOLDERS (Values that we take in but not currently use: aka CLEAN UP)
    int grayHolder = grayscale;
    vec3 ambientHolder = Ambient_Color;
    int lightHolder = LightCount;
    vec3 cameraHolder = CameraPosition;
	bool normalMapHolder = NormalMapping;
    //-------------------------

    //Add light calculations for better effect

    vec3 viewDir = normalize(cameraHolder - f_position.xyz);
    float fresnel = 1 - dot(viewDir,  f_normal);
  //  result += fresnel;
    if(alphaTexture.a < 0.6f)
      discard;
    color = vec4(result, alphaTexture.a);
}

vec2 rotate(float magnitude, vec2 p)
{
    float sinTheta = sin(magnitude);
    float cosTheta = cos(magnitude);
    mat2 rotationMat = mat2(cosTheta, -sinTheta, sinTheta, cosTheta);
    return p * rotationMat;
}

vec3 calcDirLight(vec3 normal, vec3 diffuseColor)
{
    float lightStr = 0.5;
    vec3 lightDir = normalize(-GLOBAL_lightDirection);
    float diff = smoothstep(0.0, 0.01, (max(dot(normal, lightDir), 0.0)));

    diffuseColor = (Diffuse_Color * texture(shieldTexture, f_UV).rgb) * diff * lightStr * GLOBAL_lightColor;

    float specularStr = 0.5;

    vec3 viewDir = normalize(CameraPosition - f_position.xyz);
    vec3 reflectDir = reflect(-lightDir, f_normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);

    vec3 specular = specularStr * spec * GLOBAL_lightColor;

    return diffuseColor + specular;
}
