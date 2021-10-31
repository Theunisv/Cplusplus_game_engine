#include <GL/glew.h>
#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

#include "Shader.h"

#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <tests/TestClearColor.h>

#include <Camera.h>

#include <assimp/Importer.hpp>
#include <vendor\stb_image.h>
#include <Model.h>
#include <SkyBox.h>
#include <CubeMap.h>

#include <dirent.h> 
#include <filesystem> 
#include <vector>

namespace fs = std::filesystem;
//using namespace std::experimental_filesystem;

//Funtion declaration
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void applyTexture(Texture texture, Shader shader);
void camMovement();
unsigned int loadTexture(char const* path);
void list_dir(const char* path);
std::string readFile(std::string path);
std::vector<std::string> split(const std::string& s, char delimiter);
void getIndex(std::vector<int> v, int K);
void applyLightUniforms(Shader& shader);
void loadFBXFiles(std::string levelPath);



int width = 1920;
int height = 1080;
const char* glsl_version = "#version 330 core";
bool keys[1024];

bool crouchClicked = false;
bool crouching = false;
GLfloat moveSpeed = 5.0f;

bool jumping = false;
bool falling = false;
float jumpLimit = 1.5f;
float airTime = 0;
float changeInCameraY = 0;

std::list<std::string> textureList;
std::list<Model> modelList;
std::list<unsigned int> modelLocations;
bool doorsAreOpen = false;
bool flashlightOn = true;
float spotlightConst = 1.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = width / 2.0;
GLfloat lastY = height / 2.0;

std::string triangleCount = "Total trianges:  ";
int totalDoors = 0;
float doorMovementAmount = 0;

int totalLights = 0;

bool engagingWithMenu = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::mat4 identityMatrix(1.0f);

std::string currProjection = "p";
bool hideMouse = true;

const char* namesList[2];

std::list<const char*> textureNames;
int numberOfTextures = 0;

int renderedTriangleCount;

int main(void)
{
    GLFWwindow* window;    

    /* Initialize the library */
    if (!glfwInit())
        return -1;    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Task 1 Prototype", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Glew Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;   
   

    Shader lightingShader("res/shaders/light_vs.shader", "res/shaders/light_fs.shader");

    lightingShader.Bind();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    

    Shader cubeShader("res/shaders/cube_maps_vs.shader", "res/shaders/cube_maps_fs.shader");
    Shader skyboxShader("res/shaders/sky_box_vs.shader", "res/shaders/sky_box_fs.shader");

    Shader materialShader("res/shaders/model_shader_vs.shader", "res/shaders/model_shader_fs.shader");
    materialShader.Bind();
    CubeMap cubeBox;
    SkyBox skyBox;


    std::vector<std::string> faces
    {
        "res/textures/skybox/right.jpg",
        "res/textures/skybox/left.jpg",
        "res/textures/skybox/top.jpg",
        "res/textures/skybox/bottom.jpg",
        "res/textures/skybox/front.jpg",
        "res/textures/skybox/back.jpg"
    };
    unsigned int cubemapTexture = cubeBox.loadCubemap(faces);

    cubeShader.Bind();
    cubeShader.setInt("texture1", 0);

    skyboxShader.Bind();
    skyboxShader.setInt("skybox", 0);


    Shader shader("res/shaders/basic_model_vs.shader", "res/shaders/basic_model_fs.shader");
    shader.Bind();        

    //shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    //ASSERT(location != -1);



    Renderer renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    float xPos = 0.0f;
    float yPos = 0.0f;

    glm::vec3 objTranslation(xPos, yPos, 1);
    glm::vec3 objTwoTranslation(xPos, yPos, 1);

    float camX = 0.0f;
    float camY = 0.0f;

    glm::vec3 camTranslation(camX, camY, 0);
    
    glEnable(GL_DEPTH_TEST);


    //Model ourModel;
    //ourModel.loadModel("res/models/Stone_Wall.obj");
    //Model ourModel("res/level1/default_textures/Brick_Wall1Object.obj");
    //renderedTriangleCount += ourModel.triangleCount / 3;

    float objectCoords[3] = { 1.0, 1.0, 1.0 };
    float objectRot[3] = { 1.0, 1.0, 1.0 };
    float objectScal[3] = { 1.0, 1.0, 1.0 };
    triangleCount += std::to_string(renderedTriangleCount);

    bool textureSelected = false;
    /* Loop until the user closes the window */
    glm::vec3 oldTranslate;
    glm::mat4 model = glm::mat4(1.0f);
   // glm::vec3 pointLightPositions(glm::vec3(-16.32f, 6.17f, 29.54f));

    shader.setVec3("viewPos", cameraPos);

    std::vector<glm::vec3> pointLightPositions;
    pointLightPositions.push_back(cameraPos);

    pointLightPositions.push_back(glm::vec3(142.692f, 2.88692f, -0.820805f));
    pointLightPositions.push_back(glm::vec3(142.692f, 2.88692f, -16.579495f));
    pointLightPositions.push_back(glm::vec3(128.87f, 2.88692f, -0.820805f));
    pointLightPositions.push_back(glm::vec3(128.87f, 2.88692f, -17.78f));
    pointLightPositions.push_back(glm::vec3(119.44f, 2.88692f, -11.90f));
    pointLightPositions.push_back(glm::vec3(107.78f, 2.88692f, -6.088f));
    pointLightPositions.push_back(glm::vec3(96.87f, 2.88692f, -11.95f));
    pointLightPositions.push_back(glm::vec3(96.87f, 2.88692f, -6.84f));
    pointLightPositions.push_back(glm::vec3(84.16f, 2.88692f, -6.87f));
    pointLightPositions.push_back(glm::vec3(84.16f, 2.88692f, -11.93f));
    pointLightPositions.push_back(glm::vec3(85.93f, 2.88692f, -28.245f));
    pointLightPositions.push_back(glm::vec3(91.43f, 2.88692f, -49.78f));
    pointLightPositions.push_back(glm::vec3(107.07f, 2.88692f, -24.86f));
    pointLightPositions.push_back(glm::vec3(118.78f, 2.88692f, -49.90f));

    pointLightPositions.push_back(glm::vec3(69.61f, 2.88692f, -7.22f));
    pointLightPositions.push_back(glm::vec3(47.05f, 2.88692f, -18.044f));
    pointLightPositions.push_back(glm::vec3(47.05f, 2.88692f, -1.20f));
    pointLightPositions.push_back(glm::vec3(49.44f, 2.88692f, 25.77f));

    materialShader.Bind();

    int index = 0;
    for (glm::vec3 var : pointLightPositions)
    {
        std::string pointLightNum = "pointLights[";
        pointLightNum += std::to_string(index) + "]";
        materialShader.setVec3(pointLightNum + ".position", var);
        materialShader.setVec3(pointLightNum + ".ambient", 0.05f, 0.05f, 0.05f);
        materialShader.setVec3(pointLightNum + ".diffuse", 0.8f, 0.8f, 0.8f);
        materialShader.setVec3(pointLightNum + ".specular", 1.0f, 1.0f, 1.0f);
        materialShader.setFloat(pointLightNum + ".constant", 0.8f);
        materialShader.setFloat(pointLightNum + ".linear", 0.09);
        materialShader.setFloat(pointLightNum + ".quadratic", 0.032);
        index++;

        std::cout << pointLightNum << " posted" << std::endl;
    }



    while (!glfwWindowShouldClose(window))
    {
        IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f,0.8f,1.0f);
        camMovement();
        if (hideMouse)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();       
        
        if (jumping == true)
        {
            if (airTime <= jumpLimit && !falling)
            {
                cameraPos.y += 0.1f;
                airTime += 0.1f;
                changeInCameraY += 0.1f;
                std::cout << airTime << std::endl;
            }

            if (airTime >= jumpLimit)
            {
                falling = true;
            }

            if (falling == true)
            {
                cameraPos.y -= 0.1f;
                changeInCameraY -= 0.1f;
            }
            if (changeInCameraY <= 0)
            {
                jumping = false;
                airTime = 0;
                falling = false;
                changeInCameraY = 0;
            }            
        }

        if (crouchClicked)
        {
            if (crouching)
            {
                cameraPos.y += .3f;
                crouching = false;
                moveSpeed = 5.0f;
            }
            else
            {
                cameraPos.y -= .3f;
                crouching = true;
                moveSpeed = 3.0f;
            }
            crouchClicked = false;
        }


        //glfwGetWindowSize(window, (int*)&width, (int*)&height);
        /* Render here */
        renderer.Clear();

        //Draw imgui Menu
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(400, 800));

        ImGui::Begin("Texture Load"); 
           
        int changeAmount;
        static int item_current = 1;
        static int selected = 0;        

            ImGui::BeginChild("left pane", ImVec2(400, 400), true);
            if (ImGui::Button("Load Level 1"))
            {
                modelList.clear();
                textureList.clear();
                renderedTriangleCount = 0;
                totalDoors = 0;
                totalLights = 0;
                loadFBXFiles("wolflevel1");
                cameraPos = glm::vec3(145.446f,2.69121f,-9.15258f);                
            }

            ImGui::SameLine();
            if (ImGui::Button("Load Level 2"))
            {
                modelList.clear();
                textureList.clear();
                renderedTriangleCount = 0;
                totalDoors = 0;
                totalLights = 0;
                loadFBXFiles("wolflevel2");
                cameraPos = glm::vec3(145.446f, 2.69121f, -9.15258f);
            }
            
                ImGui::SameLine();
            if (ImGui::Button("Load High Poly Example"))
            {
                modelList.clear();
                textureList.clear();
                renderedTriangleCount = 0;
                totalDoors = 0;
                totalLights = 0;
                loadFBXFiles("highpolyimport");
                cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            if (ImGui::Button("Clear Level"))
            {
                modelList.clear();
                textureList.clear();
                renderedTriangleCount = 0;              
                totalDoors = 0;
            }

            std::string triangleStr = std::to_string(renderedTriangleCount) + " total drawn triangles";
            ImGui::Text(triangleStr.c_str());
            std::string doorsStr = "Total doors loaded :" +  std::to_string(totalDoors);
            ImGui::Text(doorsStr.c_str());


            std::string lightsStr = "Total lights loaded :" + std::to_string(totalLights);
            ImGui::Text(lightsStr.c_str());
           
            //int texSize = index + 1;
            const char* texNames[2];
            int k = 0;
            for (Model var : modelList)
            {
                const char* name;
                name = var.name.c_str();
                texNames[k] = name;
            }

            ImGui::ListBoxHeader("Select Texture");
                for (Model var : modelList)
                {
                    std::string& item_name = var.name;
                    if (ImGui::Selectable(item_name.c_str(), true))
                    {
                        item_current = var.id;
                    }
                }
                ImGui::ListBoxFooter();

            ImGui::EndChild();    


            if (ImGui::Button("Apply Selected Texture"))
            {
                if (item_current == 0)
                {
                    shader.Bind();
                    Texture texture2("res/textures/index.jpg", "Stone Rock Wall");
                    texture2.Bind(0);
                    shader.SetUniform1i("texture_diffuse1", 0);
                }
                if (item_current == 1)
                {
                    shader.Bind();
                    //texture2.Bind(1);
                    shader.SetUniform1i("texture_diffuse1", 1);
                }
                std::cout << item_current << std::endl;
            }
            

            if (ImGui::Button("Select Texture"))
            {
                selected = item_current;
                for (Model var : modelList)
                {                   
                    if (var.id == selected)
                    {
                        objectCoords[0] = var.getTranslationUniform().x;
                        objectCoords[1] = var.getTranslationUniform().y;
                        objectCoords[2] = var.getTranslationUniform().z;
                        std::cout << "true" << std::endl;
                        objectCoords[0] = var.scaleUniform.x;
                        objectCoords[1] = var.scaleUniform.y;
                        objectCoords[2] = var.scaleUniform.z;
                        std::cout << "true" << std::endl;
                        objectCoords[0] = var.rotationUniform.x;
                        objectCoords[1] = var.rotationUniform.y;
                        objectCoords[2] = var.rotationUniform.z;
                        std::cout << "true" << std::endl;
                        textureSelected = true;      
                    }
                }                
                
                
            }
            if (ImGui::Button("Save Transform"))
            {                
                for (Model var : modelList)
                {
                    if (var.id == selected)
                    {
                        glm::vec3 trans(objectCoords[0], objectCoords[1], objectCoords[2]);
                        //var.setTranslateUniform(trans);
                        var.translateUniform = trans;
                        var.position = glm::translate(glm::mat4(1), trans);
                        textureSelected = false;
                        selected = -1;
                    }
                }             
                
            }
              
            ImGui::Text("Object Translation");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.20f);
            ImGui::SliderFloat("x", &objectCoords[0], -20.0f, 20.0f);
            ImGui::SliderFloat("y", &objectCoords[1], -20.0f, 20.0f);
            ImGui::SliderFloat("z", &objectCoords[2], -20.0f, 20.0f);

            ImGui::Text("Object Rotation");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.20f);
            ImGui::SliderFloat("x degrees", &objectRot[0], -180.0f, 180.0f);
            ImGui::SliderFloat("y degrees", &objectRot[1], -180.0f, 180.0f);
            ImGui::SliderFloat("z degrees", &objectRot[2], -180.0f, 180.0f);

            ImGui::Text("Object Scale");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.20f);
            ImGui::SliderFloat("x scale", &objectScal[0], 0, 10.0f);
            ImGui::SliderFloat("y scale", &objectScal[1], 0, 10.0f);
            ImGui::SliderFloat("z scale", &objectScal[2], 0, 10.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 250.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate * 2.5);

            std::stringstream s;
            s << "Current cam position: " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z;
            // assign to std::string
            std::string str = s.str();
            ImGui::Text(str.c_str());
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("next pane", ImVec2(200, 200), true, window_flags);
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank ;
            static char str0[128] = "";
            std::string helpcom = "/help";
            std::string levelcom = "/level";
            std::string clearcom = "/clear";

            static char consoleOutput[3000] = "Console Output goes here";
            std::string output;
            if (ImGui::InputText("Console Input", str0, IM_ARRAYSIZE(str0), flags))
            {
                std::string strInput = "";

                    for (int i = 0; i < sizeof(str0)/sizeof(char); i++) {
                        strInput = strInput + str0[i];
                    }         

                
                    std::cout << strInput << std::endl;
                if (strInput.find(helpcom) != std::string::npos)
                {
                    std::cout << strInput << std::endl;
                    output = "To spawn a level type /level  \n followed by the corresponding \n level number e.g /level2";
                    int i;
                    for (i = 0; i < sizeof(output); i++) {
                        consoleOutput[i] = output[i];                       
                    }                  
                }

                if (strInput.find(levelcom) != std::string::npos)
                {
                    if (strInput.find("1") != std::string::npos)
                    {
                        modelList.clear();
                        textureList.clear();
                        renderedTriangleCount = 0;
                        totalDoors = 0;
                        loadFBXFiles("wolflevel1");
                        cameraPos = glm::vec3(145.446f, 2.69121f, -9.15258f);
                    }

                    else if (strInput.find("2") != std::string::npos)
                    {
                        modelList.clear();
                        textureList.clear();
                        renderedTriangleCount = 0;
                        totalDoors = 0;
                        loadFBXFiles("wolflevel2");
                        cameraPos = glm::vec3(145.446f, 2.69121f, -9.15258f);
                    }
                    else if (strInput.find("high") != std::string::npos)
                    {
                        modelList.clear();
                        textureList.clear();
                        renderedTriangleCount = 0;
                        totalDoors = 0;
                        loadFBXFiles("highpolyimport");
                        cameraPos = glm::vec3(145.446f, 2.69121f, -9.15258f);
                    }
                }

                if (strInput.find(clearcom) != std::string::npos)
                {
                    modelList.clear();
                    textureList.clear();
                    renderedTriangleCount = 0;
                    totalDoors = 0;
                }
            }  

            ImGui::TextWrapped(consoleOutput);

            ImGui::EndChild();

            engagingWithMenu = ImGui::IsAnyItemActive();
        ImGui::End();  

        glm::vec3 lightPos(cameraPos.x, cameraPos.y , cameraPos.z);

        
       
       

        
                
        //set up ortho proj
        //glm::mat4 proj = activeCam.getProjection();
        //glm::mat4 proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // Projection 

        glm::mat4 proj;
        proj = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
        //if (currProjection == "p")
        //{
        //    
        //}
        //else
        //{
        //    proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
        //}
       
        //set up camera
        //glm::mat4 view = activeCam.getView();
        //glm::mat4 view = glm::translate(glm::mat4(1.0f), camTranslation);
        

        
        // world transformation
        model = glm::mat4(1.0f);
        shader.Bind();

        shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);


        // point light 1
        shader.setVec3("pointLight.position", 0.0f, 0.0f,0.0f);
        shader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLight.constant", 1.0f);
        shader.setFloat("pointLight.linear", 0.09);
        shader.setFloat("pointLight.quadratic", 0.032);

        if (flashlightOn)
            spotlightConst = 1.0f;

        else
            spotlightConst = 0.0f;
        // spotLight
        shader.setVec3("spotLight.position", cameraPos);
        shader.setVec3("spotLight.direction", cameraFront);
        shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("spotLight.constant", spotlightConst);
        shader.setFloat("spotLight.linear", 0.09);
        shader.setFloat("spotLight.quadratic", 0.032);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


        // translate it down so it's at the center of the scene

         //std::cout << " display " << var.getTranslationUniform().x << std::endl;
         //model = glm::scale(model, var.scaleUniform);	// it's a bit too big for our scene, so scale it down
         //model = glm::rotate(model, glm::radians(var.rotationUniform.x), glm::vec3(1, 0, 0));
         //model = glm::rotate(model, glm::radians(var.rotationUniform.y), glm::vec3(0, 1, 0));
         //model = glm::rotate(model, glm::radians(var.rotationUniform.z), glm::vec3(0, 0, 1));
        shader.SetUniformMat4f("model", model);
        shader.SetUniformMat4f("view", view);
        shader.SetUniformMat4f("projection", proj);
        //var.Draw(shader);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scenec
       // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        
        //shader.SetUniformMat4f("model", model);
        //shader.SetUniformMat4f("view", view);
        //shader.SetUniformMat4f("projection", proj); 
        //ourModel.Draw(shader);
        //shader.Bind();

        materialShader.Bind();



        for(Model var : modelList)
        {    

            int varId = var.id;
            
            model = glm::mat4(1.0f);
            
            //model = var.
            //var.position = glm::translate(var.position, var.getTranslationUniform()); // translate it down so it's at the center of the scene
            
            //model = var.position;
            //
            //glm::vec3 trans(var.translateUniform[0], var.translateUniform[1], var.translateUniform[2]);
            //var.setTranslateUniform(trans);
            if (var.id == selected && textureSelected)
            {
                glm::vec3 trans(objectCoords[0], objectCoords[1], objectCoords[2]);
                glm::vec3 rot(objectRot[0], objectRot[1], objectRot[2]);
                glm::vec3 scale(objectScal[0], objectScal[1], objectScal[2]);
                var.setTranslateUniform(trans);                  
                var.scaleUniform = scale;
                var.rotationUniform = rot;
                model = glm::translate(model, trans);
                model = glm::scale(model, scale);
                model = glm::rotate(model, glm::radians(rot.x) ,glm::vec3(1.0,0.0,0.0));
                model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0, 1.0, 0.0));
                model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0, 0.0, 1.0));
                var.position = model;
            }
            else
            {
                if (var.isDoor)
                {
                    if (doorsAreOpen)
                    {
                        if (doorMovementAmount > -3.85)
                        {
                            doorMovementAmount -= 0.1f;
                        }                        
                        std::cout << "lifting door : " << var.name << std::endl;
                        model = glm::translate(model, glm::vec3(0.0f, doorMovementAmount, 0.0f));
                    }
                    else if (!doorsAreOpen && doorMovementAmount < 0)
                    {
                        doorMovementAmount += 0.2f;
                        std::cout << "lifting door : " << var.name << std::endl;
                        model = glm::translate(model, glm::vec3(0.0f, doorMovementAmount, 0.0f));
                    }
                }

                
                    model = glm::translate(model, var.getTranslationUniform());
                    var.position = glm::translate(model, var.getTranslationUniform());
                
            }
            //glm::vec3 translation = var.translateUniform;
            //std::cout << var.getTranslatex() << std::endl;
            
           
            
            if (oldTranslate != var.getTranslationUniform())
            {
                oldTranslate = var.getTranslationUniform();
                std::cout << oldTranslate.x<< std::endl;
            }
           
            materialShader.setVec3("material.ambient", glm::vec3(0.5f, 2.0f, 2.0f));
            //materialShader.setInt("material.diffuse", 0);        
          //  unsigned int diffuseMap = var.diffuseTexture;           
  
            //lightingShader.setInt("material.specular", 1);

            materialShader.setVec3("viewPos", cameraPos);
            materialShader.setFloat("material.shininess", 32.0f);

            materialShader.SetUniformMat4f("model", var.position);
            materialShader.SetUniformMat4f("view", view);
            materialShader.SetUniformMat4f("projection", proj);
            var.Draw(materialShader);
            var.position = model;


            // directional light
            
            //var.position = model;
            //shader.Unbind();   
        }
       
       /* cubeShader.Bind();
        glm::mat4 cubemodel = glm::mat4(1.0f);
        cubeShader.setMat4("model", cubemodel);
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", proj);*/

        //cubeBox.Draw();

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Bind();
        view = glm::mat4(glm::mat3(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", proj);
        // skybox cube
        skyBox.Draw(cubemapTexture);
        glDepthFunc(GL_LESS); // set depth function back to default



        ImGui::Render();
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        //glfwPollEvents();        
    }


    

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }

    if (!engagingWithMenu)
    {


        if (key == GLFW_KEY_C && action == GLFW_RELEASE)
        {
            std::cout << hideMouse << std::endl;
            if (hideMouse)
                hideMouse = false;
            else
                hideMouse = true;
        }
        if (key == GLFW_KEY_E && action == GLFW_RELEASE)
        {
            doorsAreOpen = !doorsAreOpen;
        }

        if (key == GLFW_KEY_F && action == GLFW_RELEASE)
        {
            flashlightOn = !flashlightOn;
        }

        if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        {
            if (!jumping)
            {
                jumping = true;
            }
        }

        if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)
        {
            crouchClicked = !crouchClicked;
        }
    }
}

void camMovement()
{
    // Camera controls
    if (hideMouse)
    {    

        GLfloat cameraSpeed = moveSpeed * deltaTime;
        if (keys[GLFW_KEY_W])
            cameraPos += cameraSpeed * cameraFront;
        if (keys[GLFW_KEY_S])
            cameraPos -= cameraSpeed * cameraFront;
        if (keys[GLFW_KEY_A])
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (keys[GLFW_KEY_D])
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (keys[GLFW_KEY_O])
        currProjection = "o";
    if (keys[GLFW_KEY_P])
        currProjection = "p";
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (hideMouse)
    {   
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos; 
        lastX = xpos;
        lastY = ypos;

        GLfloat sensitivity = 0.05;	
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }   
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void list_dir(const char* path) {
    struct dirent* entry;
    DIR* dir = opendir(path);

    if (dir == NULL) {
        std::cout << "there is no such path " << path  << std::endl;
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        std::cout << entry->d_name << std::endl;
        std::string fileName = entry->d_name;

        if (fileName.find(".fbx") != -1)
        {
            textureList.push_back(fileName);
        }
    }
    closedir(dir);
}

std::string readFile(std::string path) {
    std::size_t fbxPosition = path.find(".fbx");
    
    std::string uniformPath = path.substr(0, fbxPosition);
    uniformPath.append(".txt");

    char text[200];

    std::ifstream file;
    if (!file)
    {
        std::ofstream outfile(uniformPath, std::ios::out);
        outfile << "0.0,0.0,0.0#0.0,0.0,0.0#1.0,1.0,1.0";
        outfile.close();

        file.open(uniformPath, std::ios::out | std::ios::in);
        file >> text;
    }
    else
    {
        file.open(uniformPath, std::ios::out | std::ios::in);
        file >> text;        
    }
   
    // Reading from file
    
    std::cout << text << std::endl;
    std::cout << uniformPath << std::endl;

    //closing the file
    file.close();
    return text;
}
std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void getIndex(std::vector<int> v, int K)
{
    auto it = find(v.begin(),
        v.end(), K);

    // If element was found 
    if (it != v.end()) {
        // calculating the index 
        // of K 
        int index = distance(v.begin(),
            it);
        std::cout << index << std::endl;
    }
    else {
        // If the element is not 
        // present in the vector 
        std::cout << "-1" << std::endl;
    }
}

void loadFBXFiles(std::string levelPath) {

    fs::path path{ fs::current_path() };
    std::string path_string{ path.u8string() };
    list_dir((path_string + "/res/"+ levelPath).c_str());

    stbi_set_flip_vertically_on_load(true);
    int index = 0;
    for (std::string var : textureList)
    {
        Model ab;
        ab.id = index;
        ab.SetName(var);
        ab.isDoor = false;
        ab.isLight = false;
        std::string doorStr1 = "door";
        std::string doorStr2 = "Door";

        if (var.find(doorStr1) != std::string::npos || var.find(doorStr2) != std::string::npos)
        {
            std::cout << var << " contains the word door" << std::endl;
            ab.isDoor = true;
            totalDoors++;
        }

        std::string lightStr1 = "light";
        std::string lightStr2 = "Light";

        if (var.find(lightStr1) != std::string::npos || var.find(lightStr2) != std::string::npos)
        {
            std::cout << var << " contains the word light" << std::endl;
            ab.isLight = true;
            totalLights++;
        }

        std::string gunStr1 = "gun";
        std::string gunStr2 = "Gun";

        if (var.find(gunStr1) != std::string::npos || var.find(gunStr2) != std::string::npos)
        {
            std::cout << var << " contains the word light" << std::endl;
            ab.isGun = true;
        }


        std::cout << var.c_str() << std::endl;
        //ab.shader.Bind();
        ab.loadModel("res/" + levelPath +"/"+ var);
        modelList.emplace_back(ab);
        //modelLocations.push_back(modelList.back());
        textureNames.push_back(var.c_str());
        //namesList[index] = var.c_str();
        renderedTriangleCount += 12;
        // Model currModel = modelList.back();
        //std::string text = readFile("res/" + levelPath + "/" + var);
        //std::vector<std::string> result;
        //char delim = '#';
        //result = split(text, delim);
        //delim = ',';
        std::cout << "Translate Uniform val: " << "0.0, 0.0, 0.0" << std::endl;
        std::vector<std::string> transResult = {"0.0", "0.0", "0.0"} ;//split(result[0], delim);
        std::cout << "Rotate Uniform val: " << "0.0, 0.0, 0.0" << std::endl;
        std::vector<std::string> rotResult = { "0.0", "0.0", "0.0" };//split(result[1], delim);
        std::cout << "Scale Uniform val: " << "1.0, 1.0, 1.0" << std::endl;
        std::vector<std::string> scalResult = { "1.0", "0.0", "1.0" };//split(result[2], delim);

        float transx = std::stof(transResult[0]);
        float transy = std::stof(transResult[1]);
        float transz = std::stof(transResult[2]);
        glm::vec3 transVals = glm::vec3(transx, transy, transz);

        float rotx = std::stof(rotResult[0]);
        float roty = std::stof(rotResult[1]);
        float rotz = std::stof(rotResult[2]);
        glm::vec3 rotVals = glm::vec3(rotx, roty, rotz);

        float scalx = std::stof(scalResult[0]);
        float scaly = std::stof(scalResult[1]);
        float scalz = std::stof(scalResult[2]);
        glm::vec3 scalVals = glm::vec3(scalx, scaly, scalz);
        std::cout << ab.getTranslatex() << std::endl;
        //ab.translateUniform = transVals;
        //ab.scaleUniform = rotVals;
        //ab.rotationUniform = scalVals;
        //ab.position = glm::translate(glm::mat4(1), ab.translateUniform);
        std::cout << ab.getTranslatex() << std::endl;
        index++;
        for (Mesh mesh : ab.meshes)
        {
            renderedTriangleCount += 12;
        }
    }
}

void applyLightUniforms(Shader &shader) {

    
    //materialShader.setVec3("light.position", lightPos);
   
}


