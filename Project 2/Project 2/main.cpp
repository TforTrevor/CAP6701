#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.hpp"
#include "assets.hpp"
#include "post_processing.hpp"
#include "hdr_fbo.hpp"
#include "sky.hpp"

double mouseX = 0;
double mouseY = 0;
bool wireframeToggle = false;
bool phongToggle = false;
bool quadsToggle = false;
bool cursorToggle = false;

const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

void mouseCallback(GLFWwindow* window, double x, double y)
{
    mouseX = x;
    mouseY = y;
}

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        wireframeToggle = !wireframeToggle;

    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        phongToggle = !phongToggle;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        quadsToggle = !quadsToggle;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        cursorToggle = !cursorToggle;
        if (cursorToggle)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::cout << message << std::endl;
}

std::vector<ParticleSystem> loadParticles(std::shared_ptr<ModelAssets>& models, std::shared_ptr<MaterialAssets>& materials)
{
    std::vector<ParticleSystem> objects;

    ParticleSystem front{ models->smallSphere, materials->pbrWhite, 16 };
    front.setPosition(glm::vec3(0, 0, -1.75f));
    front.setSpawnRate(0.05f);
    objects.push_back(std::move(front));

    return objects;
}

int main()
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Renderer", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    std::cout << glGetString(GL_VERSION) << std::endl;

    glDebugMessageCallback(errorCallback, nullptr);

    //Init IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    std::shared_ptr<ShaderAssets> shaderAssets = std::make_shared<ShaderAssets>();
    std::shared_ptr<TextureAssets> textureAssets = std::make_shared<TextureAssets>();
    std::shared_ptr<MaterialAssets> materialAssets = std::make_shared<MaterialAssets>(shaderAssets, textureAssets);
    std::shared_ptr<ModelAssets> modelAssets = std::make_shared<ModelAssets>();

    std::shared_ptr<Camera> camera = std::make_shared<Camera>(WINDOW_WIDTH, WINDOW_HEIGHT);

    RenderObject pbrBoat = { modelAssets->hull, materialAssets->pbrBoat };
    RenderObject phongBoat = { modelAssets->hull, materialAssets->phongBoat };
    RenderObject pbrBoatQuad = { modelAssets->hullQuads, materialAssets->pbrBoatQuad };
    RenderObject phongBoatQuad = { modelAssets->hullQuads, materialAssets->phongBoatQuad };

    RenderObject pbrSail = { modelAssets->sail, materialAssets->pbrBoat };
    RenderObject phongSail = { modelAssets->sail, materialAssets->phongBoat };
    RenderObject pbrSailQuad = { modelAssets->sailQuads, materialAssets->pbrBoatQuad };
    RenderObject phongSailQuad = { modelAssets->sailQuads, materialAssets->phongBoatQuad };

    RenderObject pbrBall = { modelAssets->sphere, materialAssets->pbrBall };
    pbrBall.setPosition(glm::vec3(5, 0.5f, 0));
    RenderObject phongBall = { modelAssets->sphere, materialAssets->phongBall };
    phongBall.setPosition(glm::vec3(5, 0.5f, 0));
    RenderObject pbrBallQuad = { modelAssets->sphereQuads, materialAssets->pbrBallQuad };
    pbrBallQuad.setPosition(glm::vec3(5, 0.5f, 0));
    RenderObject phongBallQuad = { modelAssets->sphereQuads, materialAssets->phongBallQuad };
    phongBallQuad.setPosition(glm::vec3(5, 0.5f, 0));

    RenderObject pbrWaves = { modelAssets->water, materialAssets->pbrWaves };
    RenderObject phongWaves = { modelAssets->water, materialAssets->phongWaves };
    RenderObject pbrWavesQuad = { modelAssets->waterQuads, materialAssets->pbrWavesQuad };
    RenderObject phongWavesQuad = { modelAssets->waterQuads, materialAssets->phongWavesQuad };

    //std::shared_ptr<Sky> sky = std::make_shared<Sky>("textures/spruit_sunrise_2k.exr");
    std::shared_ptr<Sky> sky = std::make_shared<Sky>("textures/shudu_lake_2k.exr");
    //std::shared_ptr<Sky> sky = std::make_shared<Sky>("textures/lakeside_2k.exr");

    Renderer renderer{ camera, sky };
    PostProcessing postProcessing { WINDOW_WIDTH, WINDOW_HEIGHT };

    std::vector<RenderObject> pbrObjects = { pbrBoat, pbrSail, pbrBall, pbrWaves };
    std::vector<RenderObject> phongObjects = { phongBoat, phongSail, phongBall, phongWaves };
    std::vector<RenderObject> pbrQuadObjects = { pbrBoatQuad, pbrSailQuad, pbrBallQuad, pbrWavesQuad };
    std::vector<RenderObject> phongQuadObjects = { phongBoatQuad, phongSailQuad, phongBallQuad, phongWavesQuad };
    std::vector<ParticleSystem> particleSystems = loadParticles(modelAssets, materialAssets);
    
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float worldTime = 4.0f;

    HDRFBO hdrFBO{ WINDOW_WIDTH, WINDOW_HEIGHT };
    Sky::AtmosphereProperties atmosphereProperties{};

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        if (wireframeToggle)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (!cursorToggle)
        {
            camera->processInput(window, deltaTime);
            camera->processMouse(mouseX, mouseY);
        }
        
        for (ParticleSystem& system : particleSystems)
        {
            system.update(deltaTime);
        }

        for (RenderObject& object : pbrObjects)
            object.setEnabled(false);
        for (RenderObject& object : pbrQuadObjects)
            object.setEnabled(false);
        for (RenderObject& object : phongObjects)
            object.setEnabled(false);
        for (RenderObject& object : phongQuadObjects)
            object.setEnabled(false);

        if (phongToggle)
        {
            if (quadsToggle)
            {
                for (RenderObject& object : phongQuadObjects)
                    object.setEnabled(true);
            }
            else
            {
                for (RenderObject& object : phongObjects)
                    object.setEnabled(true);
            }
        }
        else
        {
            if (quadsToggle)
            {
                for (RenderObject& object : pbrQuadObjects)
                    object.setEnabled(true);
            }
            else
            {
                for (RenderObject& object : pbrObjects)
                    object.setEnabled(true);
            }
        }

        renderer.begin(hdrFBO.frameBufferObject);
        sky->drawPBR(atmosphereProperties, camera, worldTime);
        renderer.toggleTessellation(true);
        renderer.setPatchSize(3);
        renderer.drawObjects(pbrObjects, currentFrame);
        renderer.drawObjects(phongObjects, currentFrame);
        renderer.setPatchSize(4);
        renderer.drawObjects(pbrQuadObjects, currentFrame);
        renderer.drawObjects(phongQuadObjects, currentFrame);
        renderer.toggleTessellation(false);
        renderer.drawObjects(particleSystems, currentFrame);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //sky->draw(camera);
        
        renderer.end();

        postProcessing.begin(hdrFBO);
        postProcessing.render(shaderAssets->ppHDR);
        postProcessing.render(shaderAssets->ppACES);
        postProcessing.render(shaderAssets->ppLinearToGamma);
        postProcessing.end();

        if (cursorToggle)
            ImGui::Begin("Atmosphere");
        else
            ImGui::Begin("Atmosphere", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoInputs);
        ImGui::DragFloat("Time", &worldTime, 0.1f, 0.0f);
        ImGui::DragFloat2("Sky View LUT size", (float*)&atmosphereProperties.skyViewSize, 1.0f, 1.0f, 2048.0f);
        ImGui::Text("Atmosphere Properties");
        ImGui::DragFloat("Ground Radius", &atmosphereProperties.groundRadius, 0.001f, 0.0f, atmosphereProperties.atmosphereRadius);
        ImGui::DragFloat("Atmosphere Radius", &atmosphereProperties.atmosphereRadius, 0.001f, atmosphereProperties.groundRadius, 100.0f);
        ImGui::DragFloat3("Rayleigh Scattering Base", (float*)&atmosphereProperties.rayleighScatteringBase, 0.01f, 0.0f);
        ImGui::DragFloat("Rayleigh Absorption Base", &atmosphereProperties.rayleighAbsorptionBase, 0.01f, 0.0f);
        ImGui::DragFloat("Mie Scattering Base", &atmosphereProperties.mieScatteringBase, 0.01f, 0.0f);
        ImGui::DragFloat("Mie Absorption Base", &atmosphereProperties.mieAbsorptionBase, 0.01f, 0.0f);
        ImGui::DragFloat3("Ozone Absorption Base", (float*)&atmosphereProperties.ozoneAbsorptionBase, 0.01f, 0.0f);
        ImGui::DragFloat3("Ground Albedo", (float*)&atmosphereProperties.groundAlbedo, 0.01f, 0.0f);
        ImGui::SetWindowSize({ 0, 0 }, 0);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}