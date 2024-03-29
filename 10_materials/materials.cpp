//
// Created by Chamelz on 2022/10/25.
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <shader_m.h>
#define STB_IMAGE_IMPLEMENTATION
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stb_image.h>
#include <camera.h>

#define RESOURCES_ROOT_PATH "/Volumes/ZHITAI/resources"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void set_UI();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

enum MATERIAL
{
    EMERALD,
    PEARL,
    BRONZE,
    GOLD,
    GREEN_RUBBER,
    RED_PLASTIC
};
struct MaterialStruct
{
    ImVec4 ambient;
    ImVec4 diffuse;
    ImVec4 specular;
    float shininess;
};
MaterialStruct materialArray[6];
static int item_current = 0;

ImVec4 objectColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
ImVec4 lightColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
static float f = 0.0f;
static int counter = 0;
float ambientStrength = 0.1;
float diffuseStrength = 0.5;
float specularStrength = 0.5;

ImVec4 material_ambient = ImVec4(1.0f, 0.5f, 0.31f, 10.f);
ImVec4 material_diffuse = ImVec4(1.0f, 0.5f, 0.31f, 1.0f);
ImVec4 material_specular = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // specular lighting doesn't have full effect on this object's material
float material_shininess = 32.0f;

glm::vec3 defaultLightColor = glm::vec3(1.0);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 初始化ImGui
    ImGui::CreateContext();     // Setup Dear ImGui context
    ImGui::StyleColorsDark();       // Setup Dear ImGui style
    ImGui_ImplGlfw_InitForOpenGL(window, true);     // Setup Platform/Renderer backends
    ImGui_ImplOpenGL3_Init("#version 330");
    materialArray[EMERALD].ambient = ImVec4(0.0215, 0.1745, 0.0215, 1.0);
    materialArray[EMERALD].diffuse = ImVec4(0.07568, 0.61424, 0.07568, 1.0);
    materialArray[EMERALD].specular = ImVec4(0.633, 0.727811, 0.633, 1.0);
    materialArray[EMERALD].shininess = 76.8;

    materialArray[PEARL].ambient = ImVec4(0.25, 0.20725, 0.20725, 1.0);
    materialArray[PEARL].diffuse = ImVec4(1, 0.829, 0.829, 1.0);
    materialArray[PEARL].specular = ImVec4(0.296648, 0.296648, 0.296648, 1.0);
    materialArray[PEARL].shininess = 11.26;

    materialArray[BRONZE].ambient = ImVec4(0.2125, 0.1275, 0.054, 1.0);
    materialArray[BRONZE].diffuse = ImVec4(0.714, 0.4284, 0.18144, 1.0);
    materialArray[BRONZE].specular = ImVec4(0.393548, 0.271906, 0.166721, 1.0);
    materialArray[BRONZE].shininess = 25.6;

    materialArray[GOLD].ambient = ImVec4(0.24725, 0.1995, 0.0745, 1.0);
    materialArray[GOLD].diffuse = ImVec4(0.75164, 0.60648, 0.22648, 1.0);
    materialArray[GOLD].specular = ImVec4(0.628281, 0.555802, 0.366065, 1.0);
    materialArray[GOLD].shininess = 51.2;

    materialArray[GREEN_RUBBER].ambient = ImVec4(0.0, 0.05, 0, 1.0);
    materialArray[GREEN_RUBBER].diffuse = ImVec4(0.4, 0.5, 0.4, 1.0);
    materialArray[GREEN_RUBBER].specular = ImVec4(0.04, 0.7, 0.04, 1.0);
    materialArray[GREEN_RUBBER].shininess = 10;

    materialArray[RED_PLASTIC].ambient = ImVec4(0.0, 0.0, 0.0, 1.0);
    materialArray[RED_PLASTIC].diffuse = ImVec4(0.5, 0.0, 0.0, 1.0);
    materialArray[RED_PLASTIC].specular = ImVec4(0.7, 0.6, 0.6, 1.0);
    materialArray[RED_PLASTIC].shininess = 32;

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("../shaders/10_01_v.glsl", "../shaders/10_01_f.glsl");
    Shader lightCubeShader("../shaders/10_02_v.glsl", "../shaders/10_02_f.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Swap front and back buffers */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        set_UI();

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // light properties
        glm::vec3 diffuseColor = glm::vec3(lightColor.x, lightColor.y, lightColor.z) * glm::vec3(diffuseStrength); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(ambientStrength); // low influence
        lightingShader.setVec3("light.ambient",  ambientColor);
        lightingShader.setVec3("light.diffuse",  diffuseColor); // 将光照调暗了一些以搭配场景
        lightingShader.setVec3("light.specular", specularStrength, specularStrength, specularStrength);

        // material properties
        lightingShader.setVec3("material.ambient", material_ambient.x, material_ambient.y, material_ambient.z);
        lightingShader.setVec3("material.diffuse", material_diffuse.x, material_diffuse.y, material_diffuse.z);
        lightingShader.setVec3("material.specular", material_specular.x, material_specular.y, material_specular.z); // specular lighting doesn't have full effect on this object's material
        lightingShader.setFloat("material.shininess", material_shininess);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwDestroyWindow(window);
    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessMouseMovement(xoffset, yoffset);
}
void set_UI(){
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

    ImGui::Begin("Editor"); // Create a window called "Hello, world!" and append into it.

    // ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
    ImGui::Text("Camera.Position:");
    ImGui::SliderFloat("camera.Position.x", &camera.Position.x, -10.0f, 10.0f);
    ImGui::SliderFloat("camera.Position.y", &camera.Position.y, -10.0f, 10.0f);
    ImGui::SliderFloat("camera.Position.z", &camera.Position.z, -10.0f, 10.0f);

    ImGui::Text("Material properties:");
    ImGui::ColorEdit3("ambient", (float *) &material_ambient);
    ImGui::ColorEdit3("diffuse", (float *) &material_diffuse);
    ImGui::ColorEdit3("specular", (float *) &material_specular);
    ImGui::SliderFloat("material_shininess", &material_shininess, 0.0f, 100.0f);
    {
        const char *items[] = {
                "Emerald",
                "Pearl",
                "Bronze",
                "Gold",
                "Green rubber",
                "Red Plastic",
        };
        if(ImGui::Combo("Material", &item_current, items, IM_ARRAYSIZE(items))){
            material_ambient = materialArray[item_current].ambient;
            material_diffuse = materialArray[item_current].diffuse;
            material_specular = materialArray[item_current].specular;
            material_shininess = materialArray[item_current].shininess;
        }
    }

    ImGui::Text("Light properties:");
    ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0f, 1.0f);
    ImGui::SliderFloat("diffuseStrength", &diffuseStrength, 0.0f, 1.0f);
    ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 1.0f);

    ImGui::ColorEdit3("lightColor", (float *) &lightColor); // Edit 3 floats representing a color

    // ImGui::ColorEdit3("lightColor", (float *) &lightColor); // Edit 3 floats representing a color
    if (ImGui::Button("counter"))                        // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}