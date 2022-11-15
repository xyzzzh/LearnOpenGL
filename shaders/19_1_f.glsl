#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform int mode;
uniform float refractRatio;
uniform sampler2D texture_diffuse1;

void main(){
    switch(mode){
        case 0:{
            // black
//            vec3 viewDir = normalize(cameraPos - Position);
//            vec3 normal = normalize(Normal);
//
//            vec3 R = reflect(- viewDir, normal);
//            vec3 reflectMap = vec3(texture(material.texture_reflection1, TexCoord));
//            vec3 reflection = vec3(texture(material.texture1, R).rgb) * reflectMap;
//
//            float diff = max(normalize(dot(normal, viewDir)), 0.0f);
//            vec3 diffuse = diff * vec3(texture(material.texture_diffuse1, TexCoord));
//
//            FragColor = vec4(diffuse + reflection, 1.0f);
            FragColor = vec4(1.0f);
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
            break;
        }
        default:
            break;
    }

}