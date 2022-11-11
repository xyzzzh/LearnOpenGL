# LearnOpenGL

# note

## 01_hello_window

## 02_hello_triangle

### VBO: 顶点缓冲对象(Vertex Buffer Objects, VBO)
使用glGenBuffers函数和一个缓冲ID生成一个VBO对象,
使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上,
调用glBufferData函数，它会把之前定义的顶点数据复制到缓冲的内存中

```c++
// 使用glGenBuffers函数和一个缓冲ID生成一个VBO对象
unsigned int VBO;
glGenBuffers(1, &VBO);
// 使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上,
glBindBuffer(GL_ARRAY_BUFFER, VBO);  
// 调用glBufferData函数，它会把之前定义的顶点数据复制到缓冲的内存中
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

### VAO: 顶点数组对象(Vertex Array Object, VAO)
首先绑定VAO，
然后绑定和设置vertex buffer，
最后设置vertex attributes(s)

```c++
// ..:: 初始化代码（只运行一次 (除非你的物体频繁改变)） :: ..
// 1. 绑定VAO
glBindVertexArray(VAO);
// 2. 把顶点数组复制到缓冲中供OpenGL使用
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 3. 设置顶点属性指针
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

[...]

// ..:: 绘制代码（渲染循环中） :: ..
// 4. 绘制物体
glUseProgram(shaderProgram);
glBindVertexArray(VAO);
someOpenGLFunctionThatDrawsOurTriangle();
```

### EBO: 元素缓冲对象(Element Buffer Object，EBO)

用于减少重复顶点绘制，按索引绘制顶点。
EBO是一个缓冲区，就像一个顶点缓冲区对象一样，它存储 OpenGL 用来决定要绘制哪些顶点的索引。
这种所谓的索引绘制(Indexed Drawing)正是我们问题的解决方案。
首先，我们先要定义（不重复的）顶点，和绘制出矩形所需的索引：

```c++
float vertices[] = {
    0.5f, 0.5f, 0.0f,   // 右上角
    0.5f, -0.5f, 0.0f,  // 右下角
    -0.5f, -0.5f, 0.0f, // 左下角
    -0.5f, 0.5f, 0.0f   // 左上角
};
unsigned int indices[] = {
    // 注意索引从0开始! 
    // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
    // 这样可以由下标代表顶点组合成矩形
    0, 1, 3, // 第一个三角形
    1, 2, 3  // 第二个三角形
};
```
你可以看到，当使用索引的时候，我们只定义了4个顶点，而不是6个。下一步我们需要创建元素缓冲对象：
```c++
unsigned int EBO;
glGenBuffers(1, &EBO);
```
与VBO类似，我们先绑定EBO然后用glBufferData把索引复制到缓冲里。
同样，和VBO类似，我们会把这些函数调用放在绑定和解绑函数调用之间，只不过这次我们把缓冲的类型定义为GL_ELEMENT_ARRAY_BUFFER。
```c++
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
```
注意：我们传递了GL_ELEMENT_ARRAY_BUFFER当作缓冲目标。
最后一件要做的事是用glDrawElements来替换glDrawArrays函数，表示我们要从索引缓冲区渲染三角形。
使用glDrawElements时，我们会使用当前绑定的索引缓冲对象中的索引进行绘制：
```c++
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```
最后的初始化和绘制代码现在看起来像这样：
```c++
// ..:: 初始化代码 :: ..
// 1. 绑定顶点数组对象
glBindVertexArray(VAO);
// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
// 4. 设定顶点属性指针
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

[...]

// ..:: 绘制代码（渲染循环中） :: ..
glUseProgram(shaderProgram);
glBindVertexArray(VAO);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
glBindVertexArray(0);
```

## 03_shaders
着色器(Shader)是运行在**GPU**上的小程序。
这些小程序为图形渲染管线的某个特定部分而运行。
从基本意义上来说，着色器只是一种把**输入转化为输出**的程序。
着色器也是一种非常独立的程序，因为它们之间不能相互通信；
它们之间**唯一的沟通只有通过输入和输出**。

### GLSL
一个典型的着色器有下面的结构：
```c++
#version version_number
in type in_variable_name;
in type in_variable_name;

out type out_variable_name;

uniform type uniform_name;

int main()
{
// 处理输入并进行一些图形操作
...
// 输出处理过的结果到输出变量
out_variable_name = weird_stuff_we_processed;
}
```
关于顶点着色器，
每个输入变量也叫顶点属性(Vertex Attribute)。
我们能声明的顶点属性是有上限的，它一般由硬件来决定（至少16个包含4分量）。

### 向量

| 类型    | 含义                    |
|-------|-----------------------|
| vecn  | 包含n个float分量的默认向量      |
| bvecn | 包含n个bool分量的向量         |
| ivecn | 包含n个int分量的向量          |
| uvecn | 包含n个unsigned int分量的向量 |
| dvecn | 包含n个double分量的向量       |

重组
```c++
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
```
### 输入与输出
**in** & **out**
在发送方着色器中声明一个输出，在接收方着色器中声明一个类似的输入。当类型和名字都一样的时候，OpenGL就会把两个变量链接到一起，它们之间就能发送数据了（这是在链接程序对象时完成的）。

特例：

顶点着色器：layout (location = 0)
片段着色器：它需要一个vec4颜色输出变量，因为片段着色器需要生成一个最终输出的颜色。

### Uniform

Uniform是一种**从CPU中的应用向GPU**中的着色器发送数据的方式，
但uniform和顶点属性有些不同。

首先，uniform是**全局的(Global)**。
全局意味着uniform变量必须在每个着色器程序对象中都是独一无二的，
而且它可以被着色器程序的任意着色器在任意阶段访问。
第二，无论你把uniform值设置成什么， 
uniform会**一直保存**它们的数据，直到它们被重置或更新。

## 04_textures
纹理是一个2D图片（也有1D和3D的纹理)。
为了能够把纹理映射(Map)到三角形上，我们需要指定三角形的每个顶点各自对应纹理的哪个部分。
这样每个顶点就会关联着一个纹理坐标(Texture Coordinate)，用来标明该从纹理图像的哪个部分采样。
之后在图形的其它片段上进行片段插值(Fragment Interpolation)。


纹理坐标在x和y轴上，范围为0到1之间（注意我们使用的是2D纹理图像）。
使用纹理坐标获取纹理颜色叫做采样(Sampling)。
纹理坐标起始于(0, 0)，也就是纹理图片的左下角，
终始于(1, 1)，即纹理图片的右上角。
下面的图片展示了我们是如何把纹理坐标映射到三角形上的。
![image](https://learnopengl-cn.github.io/img/01/06/tex_coords.png)
我们为三角形指定了3个纹理坐标点。
如上图所示，我们希望三角形的左下角对应纹理的左下角，因此我们把三角形左下角顶点的纹理坐标设置为(0, 0)；
三角形的上顶点对应于图片的上中位置所以我们把它的纹理坐标设置为(0.5, 1.0)；
同理右下方的顶点设置为(1, 0)。
我们只要给顶点着色器传递这三个纹理坐标就行了，
接下来它们会被传片段着色器中，它会为每个片段进行纹理坐标的插值。

纹理坐标看起来就像这样：
```c++
float texCoords[] = {
    0.0f, 0.0f, // 左下角
    1.0f, 0.0f, // 右下角
    0.5f, 1.0f // 上中
};
```
### 纹理环绕方式

| 环绕方式               | 	描述                         |
|--------------------|-----------------------------|
| GL_REPEAT | 对纹理的默认行为。重复纹理图像。 |
| GL_MIRRORED_REPEAT | 和GL_REPEAT一样，但每次重复图片是镜像放置的。 |
| GL_CLAMP_TO_EDGE	  |纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。|
| GL_CLAMP_TO_BORDER |超出的坐标为用户指定的边缘颜色。|
![image](https://learnopengl-cn.github.io/img/01/06/texture_wrapping.png)
使用glTexParameter*函数对单独的一个坐标轴设置（s、t（如果是使用3D纹理那么还有一个r）它们和x、y、z是等价的）
第一个参数指定了纹理目标；我们使用的是2D纹理，因此纹理目标是GL_TEXTURE_2D。
第二个参数需要我们指定设置的选项与应用的纹理轴。我们打算配置的是WRAP选项，并且指定S和T轴。
最后一个参数需要我们传递一个环绕方式(Wrapping)，在这个例子中OpenGL会给当前激活的纹理设定纹理环绕方式为GL_MIRRORED_REPEAT。
如果我们选择GL_CLAMP_TO_BORDER选项，我们还需要指定一个边缘的颜色。
这需要使用glTexParameter函数的fv后缀形式，用GL_TEXTURE_BORDER_COLOR作为它的选项，并且传递一个float数组作为边缘的颜色值：
```c++
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
```
### 纹理过滤
纹理坐标不依赖于分辨率(Resolution)，它可以是任意浮点值，所以OpenGL需要知道怎样将纹理像素(Texture Pixel，也叫Texel，译注1)映射到纹理坐标。
当有一个很大的物体但是纹理的分辨率很低的时候，这就变得很重要了。

主要讨论：GL_NEAREST和GL_LINEAR

GL_NEAREST （也叫邻近过滤，Nearest Neighbor Filtering）是OpenGL默认的纹理过滤方式。
当设置为GL_NEAREST的时候，OpenGL会选择中心点最接近纹理坐标的那个像素。
下图中你可以看到四个像素，加号代表纹理坐标。
左上角那个纹理像素的中心距离纹理坐标最近，所以它会被选择为样本颜色：
![image](https://learnopengl-cn.github.io/img/01/06/filter_nearest.png)

GL_LINEAR（也叫线性过滤，(Bi)linear Filtering）
它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色。
一个纹理像素的中心距离纹理坐标越近，那么这个纹理像素的颜色对最终的样本颜色的贡献越大。
下图中你可以看到返回的颜色是邻近像素的混合色：
![](https://learnopengl-cn.github.io/img/01/06/filter_linear.png)

二者效果对比：
![](https://learnopengl-cn.github.io/img/01/06/texture_filtering.png)
- GL_NEAREST产生了颗粒状的图案
- GL_LINEAR能够产生更平滑的图案

当进行放大(Magnify)和缩小(Minify)操作的时候可以设置纹理过滤的选项，
比如你可以在纹理被缩小的时候使用邻近过滤，被放大时使用线性过滤。
使用glTexParameter*函数为放大和缩小指定过滤方式。
```c++
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

### 多级渐远纹理
假设我们有一个包含着上千物体的大房间，每个物体上都有纹理。
有些物体会很远，但其纹理会拥有与近处物体同样高的分辨率。
由于远处的物体可能只产生很少的片段，OpenGL从高分辨率纹理中为这些片段获取正确的颜色值就很困难，
因为它需要对一个跨过纹理很大部分的片段只拾取一个纹理颜色。
在小物体上这会产生不真实的感觉，更不用说对它们使用高分辨率纹理浪费内存的问题了。

- 效果不真实
- 性能开销大

因此引入多级渐远纹理(Mipmap)。
![](https://learnopengl-cn.github.io/img/01/06/mipmaps.png)
OpenGL提供glGenerateMipmaps函数，在创建完一个纹理后调用该函数，自动生成Mipmap。

在渲染中切换多级渐远纹理级别(Level)时，OpenGL在两个不同级别的多级渐远纹理层之间会产生不真实的生硬边界。
就像普通的纹理过滤一样，切换多级渐远纹理级别时你也可以在两个不同多级渐远纹理级别之间使用NEAREST和LINEAR过滤。
为了指定不同多级渐远纹理级别之间的过滤方式，你可以使用下面四个选项中的一个代替原有的过滤方式：

| 过滤方式 |描述|
|------|-----|
|GL_NEAREST_MIPMAP_NEAREST|使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样|
|GL_LINEAR_MIPMAP_NEAREST|使用最邻近的多级渐远纹理级别，并使用线性插值进行采样|
|GL_NEAREST_MIPMAP_LINEAR|在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样|
|GL_LINEAR_MIPMAP_LINEAR|在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样|

我们可以使用glTexParameteri将过滤方式设置为前面四种提到的方法之一：
```c++
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```
### 生成纹理

纹理是使用ID引用的。生成后需要绑定，让之后任何的纹理指令都可以配置当前绑定的纹理:
```c++
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
```
现在纹理已经绑定了，我们可以使用前面载入的图片数据生成一个纹理了。
纹理可以通过**glTexImage2D**来生成:
```c++
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
```
> - 第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。 
> - 第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。 
> - 第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。 
> - 第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。 
> - 下个参数应该总是被设为0（历史遗留的问题）。 
> - 第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。 
> - 最后一个参数是真正的图像数据。

生成一个纹理的过程应该看起来像这样：
```c++
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
// 为当前绑定的纹理对象设置环绕、过滤方式
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 加载并生成纹理
int width, height, nrChannels;
unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
if (data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "Failed to load texture" << std::endl;
}
stbi_image_free(data);
```

### 纹理单元

sampler2D变量是个uniform，我们却不用glUniform给它赋值。
使用glUniform1i，我们可以给纹理采样器分配一个位置值，
这样的话我们能够在一个片段着色器中设置多个纹理。
一个纹理的位置值通常称为一个纹理单元(Texture Unit)。
一个纹理的默认纹理单元是0，它是默认的激活纹理单元，所以教程前面部分我们没有分配一个位置值。
纹理单元的主要目的是让我们在着色器中可以使用多于一个的纹理。
通过把纹理单元赋值给采样器，我们可以一次绑定多个纹理，只要我们首先激活对应的纹理单元。
就像glBindTexture一样，我们可以使用glActiveTexture激活纹理单元，传入我们需要使用的纹理单元：
```c++
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture1);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, texture2);
```

我们还要通过使用glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元。
我们只需要设置一次即可，所以这个会放在渲染循环的前面：
```c++
ourShader.use(); // 不要忘记在设置uniform变量之前激活着色器程序！
glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // 手动设置
ourShader.setInt("texture2", 1); // 或者使用着色器类设置

while(...) 
{
    [...]
}

```


## 05_变换
通过Uniform向顶点着色器vshader传递trans矩阵。并在render loop中定义trans矩阵，实现实时变换位置的效果。

```c++
# v.glsl
#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, 1.0-aTexCoord.y);
}
```
```c++
// render loop in transformations.cpp
unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
```

## 06_坐标系统
## 07_摄像机类
camera.h

## 08_颜色

## 09_基础光照
冯氏光照模型(Phong Lighting Model)
冯氏光照模型的主要结构由3个分量组成：环境(Ambient)、漫反射(Diffuse)和镜面(Specular)光照。
![](https://learnopengl-cn.github.io/img/02/02/basic_lighting_phong.png)
- 环境光照(Ambient Lighting)：即使在黑暗的情况下，世界上通常也仍然有一些光亮（月亮、远处的光），所以物体几乎永远不会是完全黑暗的。为了模拟这个，我们会使用一个环境光照常量，它永远会给物体一些颜色。
- 漫反射光照(Diffuse Lighting)：模拟光源对物体的方向性影响(Directional Impact)。它是冯氏光照模型中视觉上最显著的分量。物体的某一部分越是正对着光源，它就会越亮。
- 镜面光照(Specular Lighting)：模拟有光泽物体上面出现的亮点。镜面光照的颜色相比于物体的颜色会更倾向于光的颜色。

### 1. 环境光照ambient
使用一个很小的常量（光照）颜色，添加到物体片段的最终颜色中，
这样子的话即便场景中没有直接的光源也能看起来存在有一些发散的光。

把环境光照添加到场景里非常简单。
我们用光的颜色乘以一个很小的常量环境因子，再乘以物体的颜色，然后将最终结果作为片段的颜色：
```glsl
void main(){
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * objectColor;
    FragColor = vec4(result, 1.0);
}
```
### 2. 漫反射diffuse
漫反射光照使物体上与光线方向越接近的片段,能从光源处获得更多的亮度。

![](https://learnopengl-cn.github.io/img/02/02/diffuse_light.png)
两个单位向量的夹角越小，它们点乘的结果越倾向于1。
当两个向量的夹角为90度的时候，点乘会变为0。
这同样适用于θ，θ越大，光对片段颜色的影响就应该越小。

所以，计算漫反射光照需要:
- 法向量：一个垂直于顶点表面的向量。 
- 定向的光线：作为光源的位置与片段的位置之间向量差的方向向量。为了计算这个光线，我们需要光的位置向量和片段的位置向量。

```glsl
void main(){
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
```
### 3. 高光specular
和漫反射光照一样，镜面光照也决定于光的方向向量和物体的法向量，
但是它也决定于观察方向，例如玩家是从什么方向看向这个片段的。
镜面光照决定于表面的反射特性。
如果我们把物体表面设想为一面镜子，那么镜面光照最强的地方就是我们看到表面上反射光的地方。
![](https://learnopengl-cn.github.io/img/02/02/basic_lighting_specular_theory.png)
我们先计算视线方向与反射方向的点乘（并确保它不是负值），然后取它的32次幂。
这个32是高光的反光度(Shininess)。
一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小。
在下面的图片里，你会看到不同反光度的视觉效果影响：
![](https://learnopengl-cn.github.io/img/02/02/basic_lighting_specular_shininess.png)
```glsl
void main(){
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
```

### 4. Gouraud着色
在光照着色器的早期，开发者曾经在顶点着色器中实现冯氏光照模型。
在顶点着色器中做光照的优势是，相比片段来说，顶点要少得多，因此会更高效，
所以（开销大的）光照计算频率会更低。
然而，顶点着色器中的最终颜色值是仅仅只是那个顶点的颜色值，
片段的颜色值是由插值光照颜色所得来的。
结果就是这种光照看起来不会非常真实，除非使用了大量顶点。
![](https://learnopengl-cn.github.io/img/02/02/basic_lighting_gouruad.png)
在顶点着色器中实现的冯氏光照模型叫做Gouraud着色(Gouraud Shading)，
而不是冯氏着色(Phong Shading)。
记住，由于插值，这种光照看起来有点逊色。冯氏着色能产生更平滑的光照效果。

## 10_材质

### 1. 材质 Material
在上一节中，我们定义了一个物体和光的颜色，并结合环境光与镜面强度分量，来决定物体的视觉输出。
当描述一个表面时，我们可以分别为三个光照分量定义一个材质颜色(Material Color)：环境光照(Ambient Lighting)、漫反射光照(Diffuse Lighting)和镜面光照(Specular Lighting)。
通过为每个分量指定一个颜色，我们就能够对表面的颜色输出有细粒度的控制了。
现在，我们再添加一个反光度(Shininess)分量，结合上述的三个颜色，我们就有了全部所需的材质属性了：
```glsl
#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

uniform Material material;
```
如你所见，我们为冯氏光照模型的每个分量都定义一个颜色向量。
ambient材质向量定义了在环境光照下这个表面反射的是什么颜色，通常与表面的颜色相同。
diffuse材质向量定义了在漫反射光照下表面的颜色。漫反射颜色（和环境光照一样）也被设置为我们期望的物体颜色。
specular材质向量设置的是表面上镜面高光的颜色（或者甚至可能反映一个特定表面的颜色）。
最后，shininess影响镜面高光的散射/半径。

### 2. 光的属性
物体过亮的原因是环境光、漫反射和镜面光这三个颜色对任何一个光源都全力反射。
光源对环境光、漫反射和镜面光分量也分别具有不同的强度。
前面的章节中，我们通过使用一个强度值改变环境光和镜面光强度的方式解决了这个问题。
我们想做类似的事情，但是这次是要为每个光照分量分别指定一个强度向量。
```glsl
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
```
一个光源对它的ambient、diffuse和specular光照分量有着不同的强度。
环境光照通常被设置为一个比较低的强度，因为我们不希望环境光颜色太过主导。
光源的漫反射分量通常被设置为我们希望光所具有的那个颜色，通常是一个比较明亮的白色。
镜面光分量通常会保持为vec3(1.0)，以最大强度发光。
注意我们也将光源的位置向量加入了结构体。

## 11 光照贴图
在上一节中，我们将整个物体的材质定义为一个整体，
但现实世界中的物体通常并不只包含有一种材质，而是由多种材质所组成。
这样的物体在不同的部件上都有不同的材质属性。

所以，上一节中的那个材质系统是肯定不够的，它只是一个最简单的模型，
所以我们需要拓展之前的系统，引入漫反射和镜面光贴图(Map)。
这允许我们对物体的**漫反射**分量（以及间接地对环境光分量，它们几乎总是一样的）和**镜面光**分量有着更精确的控制。

### 1. 漫反射贴图
我们希望通过某种方式对物体的每个片段单独设置漫反射颜色。
我们可以使用纹理来达到这一点，在光照场景中，它通常叫做一个漫反射贴图(Diffuse Map).
在着色器中使用漫反射贴图的方法和纹理教程中是完全一样的。
但这次我们会将纹理储存为Material结构体中的一个sampler2D。
我们将之前定义的vec3漫反射颜色向量替换为漫反射贴图。
我们也移除了环境光材质颜色向量，因为环境光颜色在几乎所有情况下都等于漫反射颜色，
所以我们不需要将它们分开储存.
```glsl
#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
```

```c++
// lighting_maps.cpp
lightingShader.setInt("material.diffuse", 0);
...
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, diffuseMap);
```

## 14 depth testing

在这一节中，我们将会更加深入地讨论这些储存在深度缓冲（或z缓冲(z-buffer)）中的深度值(Depth Value)，
以及它们是如何确定一个片段是处于其它片段后方的。

当深度测试(Depth Testing)被启用的时候，
OpenGL会将一个片段的深度值与深度缓冲的内容进行对比。
OpenGL会执行一个深度测试，如果这个测试通过了的话，深度缓冲将会更新为新的深度值。
如果深度测试失败了，片段将会被丢弃。

深度缓冲是在片段着色器运行之后，在屏幕空间中运行的。
屏幕空间坐标与通过OpenGL的glViewport所定义的视口密切相关，
并且可以直接使用GLSL内建变量gl_FragCoord从片段着色器中直接访问。
gl_FragCoord的x和y分量代表了片段的屏幕空间坐标（其中(0, 0)位于左下角）。
gl_FragCoord中也包含了一个z分量，它包含了片段真正的深度值。
z值就是需要与深度缓冲内容所对比的那个值。

深度测试默认是禁用的，需要用GL_DEPTH_TEST选项来启用它。
当它启用的时候，如果一个片段通过了深度测试的话，OpenGL会在深度缓冲中储存该片段的z值；
如果没有通过深度缓冲，则会丢弃该片段。
如果你启用了深度缓冲，你还应该在每个渲染迭代之前使用GL_DEPTH_BUFFER_BIT来清除深度缓冲，
否则你会仍在使用上一次渲染迭代中的写入的深度值：

```c++
glEnable(GL_DEPTH_TEST);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

### 深度测试函数
OpenGL允许我们修改深度测试中使用的比较运算符。
这允许我们来控制OpenGL什么时候该通过或丢弃一个片段，什么时候去更新深度缓冲。
我们可以调用glDepthFunc函数来设置比较运算符（或者说深度函数(Depth Function):

|函数| 描述                      |
|---|-------------------------|
|GL_ALWAYS| 永远通过深度测试                |
|GL_NEVER| 	永远不通过深度测试              |
|GL_LESS| 	在片段深度值小于缓冲的深度值时通过测试    |
|GL_EQUAL| 	在片段深度值等于缓冲区的深度值时通过测试   |
|GL_LEQUAL| 	在片段深度值小于等于缓冲区的深度值时通过测试 |
|GL_GREATER| 	在片段深度值大于缓冲区的深度值时通过测试   |
|GL_NOTEQUAL| 	在片段深度值不等于缓冲区的深度值时通过测试  |
|GL_GEQUAL| 	在片段深度值大于等于缓冲区的深度值时通过测试 |

### 深度缓冲的可视化

我们知道片段着色器中，内建gl_FragCoord向量的z值包含了那个特定片段的深度值。
如果我们将这个深度值输出为颜色，我们可以显示场景中所有片段的深度值。
我们可以根据片段的深度值返回一个颜色向量来完成这一工作：
```glsl
void main()
{
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
```

## 15 stencil testing

当片段着色器处理完一个片段之后，模板测试(Stencil Test)会开始执行，和深度测试一样，它也可能会丢弃片段。
接下来，被保留的片段会进入深度测试，它可能会丢弃更多的片段。
模板测试是根据又一个缓冲来进行的，它叫做模板缓冲(Stencil Buffer)，
我们可以在渲染的时候更新它来获得一些很有意思的效果。

一个模板缓冲中，（通常）每个模板值(Stencil Value)是8位的。
所以每个像素/片段一共能有256种不同的模板值。
我们可以将这些模板值设置为我们想要的值，然后当某一个片段有某一个模板值的时候，我们就可以选择丢弃或是保留这个片段了。

模板缓冲的一个简单的例子如下：
![](https://learnopengl-cn.github.io/img/04/02/stencil_buffer.png)
> 模板缓冲首先会被清除为0，之后在模板缓冲中使用1填充了一个空心矩形。场景中的片段将会只在片段的模板值为1的时候会被渲染（其它的都被丢弃了）。

模板缓冲操作允许我们在渲染片段时将模板缓冲设定为一个特定的值。
通过在渲染时修改模板缓冲的内容，我们写入了模板缓冲。
在同一个（或者接下来的）渲染迭代中，我们可以读取这些值，来决定丢弃还是保留某个片段。
使用模板缓冲的大体步骤如下：

- 启用模板缓冲的写入。
- 渲染物体，更新模板缓冲的内容。
- 禁用模板缓冲的写入。
- 渲染（其它）物体，这次根据模板缓冲的内容丢弃特定的片段。

启用GL_STENCIL_TEST来启用模板测试。在这一行代码之后，所有的渲染调用都会以某种方式影响着模板缓冲。
```c++
glEnable(GL_STENCIL_TEST);
```

注意，和颜色和深度缓冲一样，你也需要在每次迭代之前清除模板缓冲。
```c++
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
```

和深度测试的glDepthMask函数一样，模板缓冲也有一个类似的函数。
glStencilMask允许我们设置一个位掩码(Bitmask)，它会与将要写入缓冲的模板值进行与(AND)运算。
默认情况下设置的位掩码所有位都为1，不影响输出，但如果我们将它设置为0x00，写入缓冲的所有模板值最后都会变成0.
这与深度测试中的glDepthMask(GL_FALSE)是等价的。
```c++
glStencilMask(0xFF); // 每一位写入模板缓冲时都保持原样
glStencilMask(0x00); // 每一位在写入模板缓冲时都会变成0（禁用写入）
```

### 模板函数

和深度测试一样，我们对模板缓冲应该通过还是失败，以及它应该如何影响模板缓冲，也是有一定控制的。
一共有两个函数能够用来配置模板测试：glStencilFunc和glStencilOp。

glStencilFunc(GLenum func, GLint ref, GLuint mask)一共包含三个参数：
- func：设置模板测试函数(Stencil Test Function)。这个测试函数将会应用到已储存的模板值上和glStencilFunc函数的ref值上。可用的选项有：GL_NEVER、GL_LESS、GL_LEQUAL、GL_GREATER、GL_GEQUAL、GL_EQUAL、GL_NOTEQUAL和GL_ALWAYS。它们的语义和深度缓冲的函数类似。
- ref：设置了模板测试的参考值(Reference Value)。模板缓冲的内容将会与这个值进行比较。
- mask：设置一个掩码，它将会与参考值和储存的模板值在测试比较它们之前进行与(AND)运算。初始情况下所有位都为1。

在一开始的那个简单的模板例子中，函数被设置为：
```c++
glStencilFunc(GL_EQUAL, 1, 0xFF)
```
但是glStencilFunc仅仅描述了OpenGL应该对模板缓冲内容做什么，而不是我们应该如何更新缓冲。这就需要glStencilOp这个函数了。

glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)一共包含三个选项，我们能够设定每个选项应该采取的行为：
- sfail：模板测试失败时采取的行为。
- dpfail：模板测试通过，但深度测试失败时采取的行为。
- dppass：模板测试和深度测试都通过时采取的行为。

每个选项都可以选用以下的其中一种行为：

|行为|描述|
|---|---|
|GL_KEEP|	保持当前储存的模板值|
|GL_ZERO|	将模板值设置为0|
|GL_REPLACE|	将模板值设置为glStencilFunc函数设置的ref值|
|GL_INCR|	如果模板值小于最大值则将模板值加1|
|GL_INCR_WRAP|	与GL_INCR一样，但如果模板值超过了最大值则归零|
|GL_DECR|	如果模板值大于最小值则将模板值减1|
|GL_DECR_WRAP|	与GL_DECR一样，但如果模板值小于0则将其设置为最大值|
|GL_INVERT|	按位翻转当前的模板缓冲值|

默认情况下glStencilOp是设置为(GL_KEEP, GL_KEEP, GL_KEEP)的，
所以不论任何测试的结果是如何，模板缓冲都会保留它的值。
默认的行为不会更新模板缓冲，所以如果你想写入模板缓冲的话，你需要至少对其中一个选项设置不同的值。

### 物体轮廓

效果：
![](https://learnopengl-cn.github.io/img/04/02/stencil_object_outlining.png)
为物体创建轮廓的步骤如下：
- 在绘制（需要添加轮廓的）物体之前，将模板函数设置为GL_ALWAYS，每当物体的片段被渲染时，将模板缓冲更新为1。
- 渲染物体。
- 禁用模板写入以及深度测试。
- 将每个物体缩放一点点。
- 使用一个不同的片段着色器，输出一个单独的（边框）颜色。
- 再次绘制物体，但只在它们片段的模板值不等于1时才绘制。
- 再次启用模板写入和深度测试。

```c++
glEnable(GL_DEPTH_TEST);
glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

glStencilMask(0x00); // 记得保证我们在绘制地板的时候不会更新模板缓冲
normalShader.use();
DrawFloor()  

glStencilFunc(GL_ALWAYS, 1, 0xFF); 
glStencilMask(0xFF); 
DrawTwoContainers();

glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
glStencilMask(0x00); 
glDisable(GL_DEPTH_TEST);
shaderSingleColor.use(); 
DrawTwoScaledUpContainers();
glStencilMask(0xFF);
glEnable(GL_DEPTH_TEST); 
```

总结：
模板测试的目的：利用已经本次绘制的物体，产生一个区域，在下次绘制中利用这个区域做一些效果。

模板测试的有两个要点：
- 模板测试，用于剔除片段
- 模板缓冲更新，用于更新出一个模板区域出来，为下次绘制做准备

模板缓冲区(Stencil Buffer)：与颜色缓冲区和深度缓冲区类似，模板缓冲区可以为屏幕上的每个像素点保存一个无符号整数值(8位，最大是256)。

模板掩码函数，这里指定了一个像素点在模板缓冲区中的模板值哪些位是可以被修改的

glStencilMask(0xFF); // 每一位都可以被修改，即启用模板缓冲写入

glStencilMask(0x00); // 每一位都不可以被修改，即禁用模板缓冲写入

模板测试的函数，这里指定是什么情况下通过模板测试

比较流程：掩码值 mask 和参考值 ref 值先做与操作，
再把当前模板中的值 stencil 与掩码值 mask 做与操作，
然后参考 func 中的方法是否可以通过，这个比较方式使用了第三个参数 mask。
例如 
- GL_LESS 通过，当且仅当满足: ( stencil & mask ) ref < ( stencil & mask )。
- GL_GEQUAL通过，当且仅当( stencil & mask ) >= ( ref & mask )。

物体轮廓理解：
1. 开启并设置模板测试条件为：总是通过测试，即本次绘制的所有片段都会通过测试并更新模板值；
2. 绘制物体并更新模板值；
3. 禁用模板缓冲写入；
4. 修改模板测试条件：没有模板值得片段才通过测试，意味着这次绘制会丢弃掉之前绘制的物体区域（并不影响上次的绘制，只会影响接下来的绘制）

## 16 Blending

OpenGL中，混合(Blending)通常是实现物体透明度(Transparency)的一种技术。
虽然直接丢弃片段很好，但它不能让我们渲染半透明的图像。
我们要么渲染一个片段，要么完全丢弃它。
要想渲染有多个透明度级别的图像，我们需要启用混合(Blending)。
和OpenGL大多数的功能一样，我们可以启用GL_BLEND来启用混合：
```c++
glEnable(GL_BLEND);
```
OpenGL中的混合是通过下面这个方程来实现的：
> C_res = C_source * F_source + C_destination * F_destination
> - C_source ：源颜色向量。这是源自纹理的颜色向量。 
> - F_source：源因子值。指定了alpha值对源颜色的影响。
> - C_destination：目标颜色向量。这是当前储存在颜色缓冲中的颜色向量。
> - F_destination：目标因子值。指定了alpha值对目标颜色的影响。

如何让OpenGL使用这样的因子呢？正好有一个专门的函数，叫做glBlendFunc。
glBlendFunc(GLenum sfactor, GLenum dfactor)函数接受两个参数，
来设置源因子值F_source和目标因子值F_destination。

OpenGL为我们定义了很多个选项，我们将在下面列出大部分最常用的选项。注意常数颜色向量C_constant可以通过glBlendColor函数来另外设置。

| 选项                         |值|
|-----------------------------|---|
| GL_ZERO                     |因子等于0|
| GL_ONE                      |因子等于1|
| GL_SRC_COLOR                |因子等于源颜色向量C_source|
| GL_ONE_MINUS_SRC_COLOR      |因子等于1−C_source|
| GL_DST_COLOR                |因子等于目标颜色向量C_destination|
| GL_ONE_MINUS_DST_COLOR      |因子等于1−C_destination|
| GL_SRC_ALPHA                |因子等于C_source的alpha分量|
| GL_ONE_MINUS_SRC_ALPHA      |因子等于1− C_source的alpha分量|
| GL_DST_ALPHA                |因子等于C¯destination的alpha分量|
| GL_ONE_MINUS_DST_ALPHA      |因子等于1− C_destination的alpha分量|
| GL_CONSTANT_COLOR           |因子等于常数颜色向量C_constant|
| GL_ONE_MINUS_CONSTANT_COLOR |因子等于1−C_constant|
| GL_CONSTANT_ALPHA           |因子等于C_constant的alpha分量|
| GL_ONE_MINUS_CONSTANT_ALPHA |因子等于1− C_constant的alpha分量|

为了获得之前两个方形的混合结果，我们需要使用源颜色向量的alpha作为源因子，使用1−alpha作为目标因子。这将会产生以下的glBlendFunc：
```c++
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```
