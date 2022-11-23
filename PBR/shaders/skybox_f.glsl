#version 330
out vec4 FragColor;

in vec3 localPos;

uniform samplerCube envMap;

void main() {
    vec3 envColor = texture(envMap, localPos).rgb;

    envColor = envColor / (envColor + vec3(1.0f));
    envColor = pow(envColor, vec3(1.0f/2.2f));

    FragColor = vec4(envColor, 1.0f);
}
