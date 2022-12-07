#version 330
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir){
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // projCoords的xyz分量都是[-1,1]，将其转换至[0,1]范围
    projCoords = projCoords * 0.5 + 0.5;
    // 得到光的位置视野下最近的深度
    float cloestDepth = texture(shadowMap, projCoords.xy).r;
    // 为了得到片段的当前深度，我们简单获取投影向量的z坐标，它等于来自光的透视视角的片段的深度
    float currDepth = projCoords.z;
    // 使用偏移量
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // 实际的对比就是简单检查currentDepth是否高于closetDepth，如果是，那么片段就在阴影中
    // float shadow = currDepth - bias > cloestDepth ? 1.0 : 0.0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int i=-1;i<=1;i++){
        for(int j=-1;j<=1;j++){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i,j)*texelSize).r;
            shadow += currDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)  shadow = 0.0;
    return shadow;
}

void main() {
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);

    // ambient
    vec3 ambient = 0.9 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);

    vec3 lighting = (ambient + (1.0 - shadow)*(diffuse+specular)) * color;
    FragColor = vec4(lighting, 1.0);
}
