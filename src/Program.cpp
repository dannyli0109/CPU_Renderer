#include "Program.h"
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
    glm::mat4 model(1.0f);
    float c = cos((rotation_angle * MY_PI) / 180.0f);
    float s = sin((rotation_angle * MY_PI) / 180.0f);

    model[0] = { c, s, 0, 0 };
    model[1] = { -s, c, 0, 0 };
    model[2] = { 0, 0, 1.0f, 0 };
    model[3] = { 0, 0, 0, 1.0f };

    return model;
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

    orthoLinear[0] = { 2.0f / (r - l), 0,              0,              0 };
    orthoLinear[1] = { 0,              2.0f / (t - b), 0,              0 };
    orthoLinear[2] = { 0,              0,              2.0f / (n - f), 0 };
    orthoLinear[3] = { 0,              0,              0,              1 };

    orthoTranslate[3] = { -(r + l) / 2.0f, -(t + b) / 2.0f, -(n + f) / 2.0f, 1.0f };
    glm::mat4 ortho = orthoLinear * orthoTranslate;
    projection = ortho * projectionToOrtho;
    // Then return it.
    return projection;
}

int Program::Init()
{
    //Initialise GLFW, make sure it works. Put an error message here if you like.
    if (!glfwInit())
        return -1;

    //Set resolution here, and give your window a different title.

    window = glfwCreateWindow(700, 700, "OpenGL Boilerplate", nullptr, nullptr);
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

    CPURenderer::CreateInstance(700, 700);
    InitGUI();
}

void Program::Update()
{
    CPURenderer* r = CPURenderer::GetInstance();

    unsigned int indexBuffer = r->UploadVertices({
        Vertex({{2, 0, -2}, {1, 1, 1, 1}}),
        Vertex({{0, 2, -2}, {1, 1, 1, 1}}),
        Vertex({{-2, 0, -2}, {1, 1, 1, 1}})
    });

    unsigned int vertexBuffer = r->UploadIndices({
        0, 1, 2
    });

    r->SetUniform("modelMatrix", GetModelMatrix(0));
    r->SetUniform("viewMatrix", GetViewMatrix({0, 0, 5}));
    r->SetUniform("projectionMatrix", GetProjectionMatrix(45, 1, 0.1, 50));

    while (!glfwWindowShouldClose(window))
    {
        //Clear the screen ?eventually do rendering code here.
        glClear(GL_COLOR_BUFFER_BIT);
        
        Draw();

        //Swapping the buffers ?this means this frame is over.
        glfwSwapBuffers(window);

        //Tell GLFW to check if anything is going on with input, etc.
        glfwPollEvents();
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
