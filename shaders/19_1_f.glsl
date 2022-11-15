#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform int mode;
uniform float refractRatio;

void main(){
    switch(mode){
        case 0:{
            // black
            FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        }
        case 1:{
            // reflect
            vec3 I = normalize(Position - cameraPos);
            vec3 R = reflect(I, normalize(Normal));
            FragColor = vec4(texture(skybox, R).rgb, 1.0);
            break;
        }
        case 2:{
            vec3 I = normalize(Position - cameraPos);
            vec3 R = refract(I, normalize(Normal), refractRatio);
            FragColor = vec4(texture(skybox, R).rgb, 1.0);
        }
        default:
            break;
    }

}