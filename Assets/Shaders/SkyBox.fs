#version 440 core
out vec4 FragColor;

in vec3 TexCoords;
uniform samplerCube skyBox;

const int screenX = 1280;
const int screenY = 720;
void main()
{
    FragColor = texture(skyBox, TexCoords);
    FragColor.xyz = vec3(0);
    //// Simple crosshair (hardcoded)
    //if (abs(gl_FragCoord.x - (screenX / 2)) < 1 && abs(gl_FragCoord.y - (screenY / 2)) < 7 )
    //{
    //    FragColor.xyz = 1 - FragColor.xyz;
    //    FragColor.xyz = vec3(1);
    //}
    //if (abs(gl_FragCoord.x - (screenX / 2)) < 7 && abs(gl_FragCoord.y - (screenY / 2)) < 1 )
    //{
    //    if (abs(gl_FragCoord.x - (screenX / 2)) > 1)
    //    {
    //        color.xyz = 1 - color.xyz;
    //        FragColor.xyz = 1 - FragColor.xyz;
    //        FragColor.xyz = vec3(1);
    //    }
    //}

    // Simple crosshair (hardcoded)
    if (abs(gl_FragCoord.x - (screenX / 2)) < 1 && abs(gl_FragCoord.y - (screenY / 2)) < 7 )
    {
        FragColor.xyz = 1 - FragColor.xyz;
    }
    if (abs(gl_FragCoord.x - (screenX / 2)) < 7 && abs(gl_FragCoord.y - (screenY / 2)) < 1 )
    {
        if (abs(gl_FragCoord.x - (screenX / 2)) > 1)
        {
            FragColor.xyz = 1 - FragColor.xyz;
        }
    }
}