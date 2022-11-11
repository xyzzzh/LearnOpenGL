#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform bool isVisual;

void main()
{
    if(isVisual){
        FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    }
    else{
        FragColor = texture(texture1, TexCoords);
    }
}
