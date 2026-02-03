// Tilt Five First Light - Minimal Stereo Rendering
// A clean starting point for T5 native development

#include <iostream>
#include <cmath>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <t5wrapper/Context.h>
#include <t5wrapper/Glasses.h>
#include <t5wrapper/Conversions.h>
#include <glwrapper/Shader.h>
#include <glwrapper/Buffer.h>
#include <glwrapper/VertexArray.h>
#include <glwrapper/Framebuffer.h>
#include <glapplication/Transform.h>
#include <util/ChangeDetector.h>

namespace GLW = GLWrapper;
namespace T5W = T5Wrapper;

// T5 display parameters
constexpr int T5_WIDTH = 1216;
constexpr int T5_HEIGHT = 768;
constexpr float T5_FOV = 48.0f;

// Court dimensions (meters) - 3D Pong playfield
constexpr float COURT_WIDTH  = 0.40f;  // X: left/right
constexpr float COURT_HEIGHT = 0.25f;  // Y: up/down
constexpr float COURT_DEPTH  = 0.60f;  // Z: toward/away

// Half dimensions for convenience
constexpr float COURT_HALF_W = COURT_WIDTH / 2.0f;
constexpr float COURT_HALF_D = COURT_DEPTH / 2.0f;

// Paddle dimensions
constexpr float PADDLE_WIDTH  = 0.08f;  // X size
constexpr float PADDLE_HEIGHT = 0.06f;  // Y size
constexpr float PADDLE_HALF_W = PADDLE_WIDTH / 2.0f;
constexpr float PADDLE_HALF_H = PADDLE_HEIGHT / 2.0f;

// Simple vertex with position and color
struct Vertex {
    float x, y, z;
    float r, g, b;
};

// A colored triangle floating above the gameboard (kept for reference)
const Vertex triangleVertices[] = {
    // Position              Color
    { 0.0f,  0.1f, 0.0f,    1.0f, 0.0f, 0.0f },  // Top - Red
    {-0.1f, -0.05f, 0.0f,   0.0f, 1.0f, 0.0f },  // Bottom Left - Green
    { 0.1f, -0.05f, 0.0f,   0.0f, 0.0f, 1.0f },  // Bottom Right - Blue
};

// Court wireframe vertices - 12 edges, 2 vertices per edge = 24 vertices
// Color: cyan for visibility
const Vertex courtVertices[] = {
    // Near face (player side, z = -depth/2) - 4 edges
    {-COURT_HALF_W, 0,            -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, 0,            -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, 0,            -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, COURT_HEIGHT, -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, COURT_HEIGHT, -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, COURT_HEIGHT, -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, COURT_HEIGHT, -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, 0,            -COURT_HALF_D,   0.0f, 0.8f, 0.8f},

    // Far face (opponent side, z = +depth/2) - 4 edges
    {-COURT_HALF_W, 0,            COURT_HALF_D,    0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, 0,            COURT_HALF_D,    0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, 0,            COURT_HALF_D,    0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, COURT_HEIGHT, COURT_HALF_D,    0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, COURT_HEIGHT, COURT_HALF_D,    0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, COURT_HEIGHT, COURT_HALF_D,    0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, COURT_HEIGHT, COURT_HALF_D,    0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, 0,            COURT_HALF_D,    0.0f, 0.8f, 0.8f},

    // Connecting edges (depth lines) - 4 edges
    {-COURT_HALF_W, 0,            -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, 0,             COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, 0,            -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, 0,             COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, COURT_HEIGHT, -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    { COURT_HALF_W, COURT_HEIGHT,  COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, COURT_HEIGHT, -COURT_HALF_D,   0.0f, 0.8f, 0.8f},
    {-COURT_HALF_W, COURT_HEIGHT,  COURT_HALF_D,   0.0f, 0.8f, 0.8f},
};
constexpr int COURT_VERTEX_COUNT = sizeof(courtVertices) / sizeof(courtVertices[0]);

// Paddle vertices - quad centered at origin (will be translated by model matrix)
// Color: bright green for player paddle
const Vertex paddleVertices[] = {
    // First triangle
    {-PADDLE_HALF_W, -PADDLE_HALF_H, 0.0f,   0.2f, 1.0f, 0.2f},
    { PADDLE_HALF_W, -PADDLE_HALF_H, 0.0f,   0.2f, 1.0f, 0.2f},
    { PADDLE_HALF_W,  PADDLE_HALF_H, 0.0f,   0.2f, 1.0f, 0.2f},
    // Second triangle
    {-PADDLE_HALF_W, -PADDLE_HALF_H, 0.0f,   0.2f, 1.0f, 0.2f},
    { PADDLE_HALF_W,  PADDLE_HALF_H, 0.0f,   0.2f, 1.0f, 0.2f},
    {-PADDLE_HALF_W,  PADDLE_HALF_H, 0.0f,   0.2f, 1.0f, 0.2f},
};
constexpr int PADDLE_VERTEX_COUNT = sizeof(paddleVertices) / sizeof(paddleVertices[0]);

const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 fragColor;
uniform mat4 MVP;
void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
    fragColor = aColor;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 fragColor;
out vec4 outColor;
void main() {
    outColor = vec4(fragColor, 1.0);
}
)";

// Framebuffer for one eye
struct EyeFramebuffer {
    GLuint fbo = 0;
    GLuint colorTex = 0;
    GLuint depthTex = 0;
    int width = 0;
    int height = 0;

    bool create(int w, int h) {
        width = w;
        height = h;

        glGenTextures(1, &colorTex);
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer not complete!" << std::endl;
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, width, height);
    }

    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void destroy() {
        if (fbo) glDeleteFramebuffers(1, &fbo);
        if (colorTex) glDeleteTextures(1, &colorTex);
        if (depthTex) glDeleteTextures(1, &depthTex);
        fbo = colorTex = depthTex = 0;
    }
};

int main() {
    std::cout << "=== Tilt Five First Light ===" << std::endl;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(T5_WIDTH, T5_HEIGHT, "Tilt Five First Light", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 1;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

    // Initialize Tilt Five
    T5W::Context t5Context;
    auto t5Result = t5Context.Create("TiltFiveFirstLight", "1.0.0");
    if (t5Result != T5_SUCCESS) {
        std::cerr << "Failed to create T5 context: " << t5GetResultMessage(t5Result) << std::endl;
        return 1;
    }
    std::cout << "T5 Service: " << t5Context.GetServiceVersion() << std::endl;

    // Find glasses
    std::vector<std::string> glassesList;
    auto glassesResult = t5Context.GetConnectedGlasses();
    if (!glassesResult.TryGet(glassesList) || glassesList.empty()) {
        std::cerr << "No glasses found!" << std::endl;
        return 1;
    }
    std::cout << "Found glasses: " << glassesList[0] << std::endl;

    // Connect to glasses
    T5W::Glasses glasses;
    t5Result = glasses.Create(t5Context, glassesList[0]);
    if (t5Result != T5_SUCCESS) {
        std::cerr << "Failed to create glasses: " << t5GetResultMessage(t5Result) << std::endl;
        return 1;
    }

    t5Result = glasses.Acquire("TiltFiveFirstLight");
    if (t5Result != T5_SUCCESS) {
        std::cerr << "Failed to acquire glasses: " << t5GetResultMessage(t5Result) << std::endl;
        return 1;
    }

    // Wait for glasses to be ready
    std::cout << "Waiting for glasses..." << std::flush;
    while ((t5Result = glasses.EnsureReady()) == T5_ERROR_TRY_AGAIN) {
        std::cout << "." << std::flush;
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) return 0;
    }
    if (t5Result != T5_SUCCESS) {
        std::cerr << "\nFailed to ready glasses: " << t5GetResultMessage(t5Result) << std::endl;
        return 1;
    }
    std::cout << " Ready!" << std::endl;

    // Initialize graphics context for glasses
    t5Result = glasses.InitGlassesOpenGLContext();
    if (t5Result != T5_SUCCESS) {
        std::cerr << "Failed to init GL context: " << t5GetResultMessage(t5Result) << std::endl;
        return 1;
    }

    float ipd = glasses.GetIpd();
    std::cout << "IPD: " << ipd << "m" << std::endl;

    // Configure wand stream
    t5Result = glasses.ConfigureWandStream(true);
    if (t5Result != T5_SUCCESS) {
        std::cerr << "Failed to configure wand stream: " << t5GetResultMessage(t5Result) << std::endl;
        // Non-fatal - continue without wand
    } else {
        std::cout << "Wand stream enabled" << std::endl;
    }

    // Create framebuffers for each eye
    EyeFramebuffer leftEye, rightEye;
    if (!leftEye.create(T5_WIDTH, T5_HEIGHT) || !rightEye.create(T5_WIDTH, T5_HEIGHT)) {
        std::cerr << "Failed to create eye framebuffers" << std::endl;
        return 1;
    }

    // Create shader program
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fragShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");

    // Create vertex buffer and array for triangle (kept for reference)
    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Create vertex buffer and array for court wireframe
    GLuint courtVbo, courtVao;
    glGenBuffers(1, &courtVbo);
    glGenVertexArrays(1, &courtVao);

    glBindVertexArray(courtVao);
    glBindBuffer(GL_ARRAY_BUFFER, courtVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(courtVertices), courtVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Create vertex buffer and array for paddle
    GLuint paddleVbo, paddleVao;
    glGenBuffers(1, &paddleVbo);
    glGenVertexArrays(1, &paddleVao);

    glBindVertexArray(paddleVao);
    glBindBuffer(GL_ARRAY_BUFFER, paddleVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(paddleVertices), paddleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    // Transforms
    GLApplication::Transform headPose;
    GLApplication::Transform leftEyePose, rightEyePose;
    leftEyePose.SetPosition(-ipd / 2.0f, 0, 0);
    rightEyePose.SetPosition(ipd / 2.0f, 0, 0);

    ChangeDetector<bool> isPoseValid;
    ChangeDetector<bool> isFrameSent;
    ChangeDetector<bool> isWandConnected;

    // Paddle state - position in court space
    glm::vec2 paddlePos(0.0f, COURT_HEIGHT / 2.0f);  // Start centered

    std::cout << "Starting render loop. Press ESC to exit." << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            break;

        // Get glasses pose
        auto poseResult = glasses.GetGlassesPose();
        T5_GlassesPose pose;
        isPoseValid = poseResult.TryGet(pose);

        if (isPoseValid) {
            headPose.SetPosition(T5W::toGLM(pose.posGLS_GBD));
            headPose.SetOrientation(T5W::toGLM(pose.rotToGLS_GBD));
        }

        if (isPoseValid.IsChanged()) {
            std::cout << (isPoseValid ? "Tracking acquired" : "Tracking lost") << std::endl;
        }

        // Read wand events (drain the queue)
        T5_WandStreamEvent wandEvent;
        while (true) {
            auto wandResult = glasses.ReadWandStream(0);
            if (!wandResult.TryGet(wandEvent)) break;

            if (wandEvent.type == kT5_WandStreamEventType_Connect) {
                isWandConnected = true;
            } else if (wandEvent.type == kT5_WandStreamEventType_Disconnect) {
                isWandConnected = false;
            } else if (wandEvent.type == kT5_WandStreamEventType_Report) {
                if (wandEvent.report.poseValid) {
                    // Map wand aim position to paddle position
                    // Wand reports position in gameboard space (GBD)
                    glm::vec3 wandPos = T5W::toGLM(wandEvent.report.posAim_GBD);

                    // Use wand X/Y directly, clamp to court bounds
                    paddlePos.x = glm::clamp(wandPos.x,
                        -COURT_HALF_W + PADDLE_HALF_W,
                         COURT_HALF_W - PADDLE_HALF_W);
                    paddlePos.y = glm::clamp(wandPos.y,
                        PADDLE_HALF_H,
                        COURT_HEIGHT - PADDLE_HALF_H);
                }
            }
        }

        if (isWandConnected.IsChanged()) {
            std::cout << (isWandConnected ? "Wand connected" : "Wand disconnected") << std::endl;
        }

        // Animation: rotate triangle over time
        float time = (float)glfwGetTime();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.15f)); // Hover above board
        model = glm::rotate(model, time, glm::vec3(0, 0, 1)); // Spin on Z axis

        // Projection matrix
        float aspect = (float)T5_WIDTH / T5_HEIGHT;
        glm::mat4 projection = glm::perspective(glm::radians(T5_FOV), aspect, 0.1f, 100.0f);

        // View matrices for each eye
        glm::mat4 viewLeft = leftEyePose.MatrixToLocalFrame() * headPose.MatrixToLocalFrame();
        glm::mat4 viewRight = rightEyePose.MatrixToLocalFrame() * headPose.MatrixToLocalFrame();

        glm::mat4 mvpLeft = projection * viewLeft * model;
        glm::mat4 mvpRight = projection * viewRight * model;

        // Court MVP (identity model - court is fixed in world space)
        glm::mat4 courtMvpLeft = projection * viewLeft;
        glm::mat4 courtMvpRight = projection * viewRight;

        // Paddle model matrix - position at player's end of court
        glm::mat4 paddleModel = glm::translate(glm::mat4(1.0f),
            glm::vec3(paddlePos.x, paddlePos.y, -COURT_HALF_D));
        glm::mat4 paddleMvpLeft = projection * viewLeft * paddleModel;
        glm::mat4 paddleMvpRight = projection * viewRight * paddleModel;

        // Render to left eye
        leftEye.bind();
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // Draw court wireframe (fixed in world space)
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(courtMvpLeft));
        glBindVertexArray(courtVao);
        glDrawArrays(GL_LINES, 0, COURT_VERTEX_COUNT);

        // Draw paddle
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(paddleMvpLeft));
        glBindVertexArray(paddleVao);
        glDrawArrays(GL_TRIANGLES, 0, PADDLE_VERTEX_COUNT);

        // Draw triangle (animated) - kept for reference
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpLeft));
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        leftEye.unbind();

        // Render to right eye
        rightEye.bind();
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // Draw court wireframe (fixed in world space)
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(courtMvpRight));
        glBindVertexArray(courtVao);
        glDrawArrays(GL_LINES, 0, COURT_VERTEX_COUNT);

        // Draw paddle
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(paddleMvpRight));
        glBindVertexArray(paddleVao);
        glDrawArrays(GL_TRIANGLES, 0, PADDLE_VERTEX_COUNT);

        // Draw triangle (animated) - kept for reference
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpRight));
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        rightEye.unbind();

        // Mirror right eye to window
        glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEye.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        int winW, winH;
        glfwGetFramebufferSize(window, &winW, &winH);
        glBlitFramebuffer(0, 0, T5_WIDTH, T5_HEIGHT, 0, 0, winW, winH, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window);

        // Send frames to glasses
        if (isPoseValid) {
            T5_FrameInfo frameInfo = {};

            frameInfo.vci.startY_VCI = -tan(glm::radians(T5_FOV) * 0.5f);
            frameInfo.vci.startX_VCI = frameInfo.vci.startY_VCI * T5_WIDTH / (float)T5_HEIGHT;
            frameInfo.vci.width_VCI = -2.0f * frameInfo.vci.startX_VCI;
            frameInfo.vci.height_VCI = -2.0f * frameInfo.vci.startY_VCI;

            frameInfo.texWidth_PIX = T5_WIDTH;
            frameInfo.texHeight_PIX = T5_HEIGHT;
            frameInfo.leftTexHandle = (void*)(uintptr_t)leftEye.colorTex;
            frameInfo.rightTexHandle = (void*)(uintptr_t)rightEye.colorTex;

            auto leftPos = leftEyePose.TransformPointToParentFrame(glm::vec3(0, 0, 0));
            leftPos = headPose.TransformPointToParentFrame(leftPos);
            frameInfo.posLVC_GBD = T5W::toT5(leftPos);
            frameInfo.rotToLVC_GBD = T5W::toT5(headPose.GetOrientation());

            auto rightPos = rightEyePose.TransformPointToParentFrame(glm::vec3(0, 0, 0));
            rightPos = headPose.TransformPointToParentFrame(rightPos);
            frameInfo.posRVC_GBD = T5W::toT5(rightPos);
            frameInfo.rotToRVC_GBD = T5W::toT5(headPose.GetOrientation());

            frameInfo.isUpsideDown = false;
            frameInfo.isSrgb = false;

            t5Result = glasses.SendFrameToGlasses(frameInfo);
            isFrameSent = (t5Result == T5_SUCCESS);

            if (isFrameSent.IsChanged()) {
                if (isFrameSent) {
                    std::cout << "Started sending frames" << std::endl;
                } else {
                    std::cerr << "Stopped sending frames: " << t5GetResultMessage(t5Result) << std::endl;
                }
            }
        }
    }

    // Cleanup
    std::cout << "Shutting down..." << std::endl;
    leftEye.destroy();
    rightEye.destroy();
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &courtVao);
    glDeleteBuffers(1, &courtVbo);
    glDeleteVertexArrays(1, &paddleVao);
    glDeleteBuffers(1, &paddleVbo);
    glDeleteProgram(shaderProgram);
    glasses.Release();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Done!" << std::endl;
    return 0;
}
