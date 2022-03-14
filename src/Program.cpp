#include "Program.h"
#include <string>
constexpr double MY_PI = 3.1415926;

glm::mat4 GetViewMatrix(glm::vec3 eyePos)
{
    glm::mat4 view(1.0f);
    glm::mat4 translate(1.0f);

    translate[3] = { -eyePos[0], -eyePos[1], -eyePos[2], 1.0f };
    view = translate * view;
    return view;
}

glm::mat4 GetModelMatrix(float rotation_angle)
{
    glm::mat4 rotation(1.0f);
    float c = cos((rotation_angle * MY_PI) / 180.0f);
    float s = sin((rotation_angle * MY_PI) / 180.0f);

    rotation[0] = { c, 0, -s, 0 };
    rotation[1] = { 0, 1.0f, 0, 0 };
    rotation[2] = { s, 0, c, 0 };
    rotation[3] = { 0, 0, 0, 1.0f };

    glm::mat4 scale(1.0f);
    scale[0][0] = 2.5f;
    scale[1][1] = 2.5f;
    scale[2][2] = 2.5f;


    return rotation * scale;
}

glm::mat4 GetProjectionMatrix(float eye_fov, float aspect_ratio,
    float zNear, float zFar)
{
    // Students will implement this function

    glm::mat4 projection(1.0f);

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    glm::mat4 projectionToOrtho;
    float n = zNear;
    float f = zFar;
    float t = tan(eye_fov / 2.0f * MY_PI / 180.0f) * abs(n);
    float b = -t;
    float r = aspect_ratio * t;
    float l = -r;

    projectionToOrtho[0] = { n, 0, 0, 0 };
    projectionToOrtho[1] = { 0, n, 0, 0 };
    projectionToOrtho[2] = { 0, 0, n + f, 1.0f };
    projectionToOrtho[3] = { 0, 0, -n * f, 0 };

    glm::mat4 orthoTranslate(1.0f);
    glm::mat4 orthoLinear;

    orthoLinear[0] = { 2.0f / (l - r), 0,              0,              0 };
    orthoLinear[1] = { 0,              2.0f / (t - b), 0,              0 };
    orthoLinear[2] = { 0,              0,              2.0f / (n - f), 0 };
    orthoLinear[3] = { 0,              0,              0,              1 };

    orthoTranslate[3] = { -(r + l) / 2.0f, -(t + b) / 2.0f, -(n + f) / 2.0f, 1.0f };
    glm::mat4 ortho = orthoLinear * orthoTranslate;
    projection = ortho * projectionToOrtho;
    // Then return it.
    return projection;
}

glm::mat4 GetViewportMatrix(int width, int height)
{
    int w = width;
    int h = height;
    glm::mat4 viewportMatrix(1.0f);
    viewportMatrix[0] = { w / 2.0f, 0, 0, 0 };
    viewportMatrix[1] = { 0, h / 2.0f, 0, 0 };
    viewportMatrix[2] = { 0, 0, 1.0f, 0 };
    viewportMatrix[3] = { w / 2.0f, h / 2.0f, 0, 1.0f };
    return viewportMatrix;
}

int Program::Init()
{
    //Initialise GLFW, make sure it works. Put an error message here if you like.
    if (!glfwInit())
        return -1;

    //Set resolution here, and give your window a different title.

    window = glfwCreateWindow(1400, 1400, "OpenGL Boilerplate", nullptr, nullptr);
    //GLFW_SCALE_TO_MONITOR;
    if (!window)
    {
        glfwTerminate(); //Again, you can put a real error message here.
        return -1;
    }

    //This tells GLFW that the window we created is the one we should render to.
    glfwMakeContextCurrent(window);


    //Tell GLAD to load all its OpenGL functions.
    if (!gladLoadGL())
        return -1;

    viewportSize = { 1400, 1400 };
    CPURenderer* r = CPURenderer::CreateInstance(viewportSize.x, viewportSize.y);
    r->SetClippingPlane(0.1, 50);
    InitGUI();
}

void Program::Update()
{
    CPURenderer* r = CPURenderer::GetInstance();

    float angles = 180;
    Model* model = LoadModel("soulspear/soulspear.obj");
    //r->fragmentShader = PhongFragmentShader;
    r->vertexShader = NormalMapVertexShader;
    r->fragmentShader = PhongNormalMapFragmentShader;
    for (int i = 0; i < model->meshes.size(); i++)
    {
        r->UploadVertices(model->meshes[i].vertices);
        r->UploadIndices(model->meshes[i].indices);
    }

    Texture* texture = new Texture("soulspear\\soulspear_diffuse.tga");
    int diffuseId = r->UploadTexture(texture);
    r->BindTextureUint(0, diffuseId);

    Texture* normalTexture = new Texture("soulspear\\soulspear_normal.tga");
    int normalId = r->UploadTexture(normalTexture);
    r->BindTextureUint(1, normalId);

    Texture* specularTexture = new Texture("soulspear\\soulspear_specular.tga");
    int specularId = r->UploadTexture(specularTexture);
    r->BindTextureUint(2, specularId);

    //unsigned char* color = texture->GetColor(0, 0);
    //std::cout << (short)color[0] << ", " << (short)color[1] << ", " << (short)color[2] << std::endl;
    

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            angles -= 5;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            angles += 5;
        }

        glm::vec3 eyePos = { 0, 5, 30 };
        r->SetUniform("modelMatrix", GetModelMatrix(angles));
        r->SetUniform("viewMatrix", GetViewMatrix(eyePos));
        r->SetUniform("projectionMatrix", GetProjectionMatrix(45, 1, 0.1, 50));
        r->SetUniform("viewportMatrix", GetViewportMatrix(viewportSize.x, viewportSize.y));
        r->SetUniform("diffuseTexture", 0);
        r->SetUniform("normalTexture", 1);
        r->SetUniform("specularTexture", 2);
        r->SetUniform("eyePosition", eyePos);
        //r->SetUniform("")
        //r->SetUniform("spotTexture", )
        //Clear the screen ?eventually do rendering code here.
        glClear(GL_COLOR_BUFFER_BIT);

        Draw();

        //Swapping the buffers ?this means this frame is over.
        glfwSwapBuffers(window);

        //Tell GLFW to check if anything is going on with input, etc.
    }
}

void Program::End()
{
    //If we get to this point, the window has closed, so clean up GLFW and exit.
    glfwTerminate();
    CPURenderer::DeleteInstance();
    // Cleanup GUI related
    EndGUI();
}

void Program::Draw()
{
    CPURenderer* r = CPURenderer::GetInstance();
    r->Clear();
    r->BindVertexBuffer(1);
    r->BindIndexBuffer(2);
    r->Draw();
    //r->DrawLine({ 0, 0, 0 }, { 1279, 719, 0 }, { 1, 0, 0, 1 });
    r->UpdateTexture();
}

void Program::InitGUI()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void Program::UpdateGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Make window dockable
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);


    // begin imgui window
    ImGui::Begin("Imgui window");
    // draw ui element in between
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void Program::EndGUI()
{
    // Cleanup GUI related
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
