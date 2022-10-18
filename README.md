# LearnOpenGL

# note

## 01_hello_window

## 02_hello_triangle

### 2.1 VBO: 顶点缓冲对象(Vertex Buffer Objects, VBO)
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

### 2.2 VAO: 顶点数组对象(Vertex Array Object, VAO)
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

### 2.3 EBO: 元素缓冲对象(Element Buffer Object，EBO)

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

### 3.1 GLSL
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

### 3.2 数据类型

#### 向量

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
#### 输入与输出
**in** & **out**
在发送方着色器中声明一个输出，在接收方着色器中声明一个类似的输入。当类型和名字都一样的时候，OpenGL就会把两个变量链接到一起，它们之间就能发送数据了（这是在链接程序对象时完成的）。

特例：

顶点着色器：layout (location = 0)
片段着色器：它需要一个vec4颜色输出变量，因为片段着色器需要生成一个最终输出的颜色。

#### Uniform

Uniform是一种**从CPU中的应用向GPU**中的着色器发送数据的方式，
但uniform和顶点属性有些不同。

首先，uniform是**全局的(Global)**。
全局意味着uniform变量必须在每个着色器程序对象中都是独一无二的，
而且它可以被着色器程序的任意着色器在任意阶段访问。
第二，无论你把uniform值设置成什么， 
uniform会**一直保存**它们的数据，直到它们被重置或更新。
