#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "shader.hpp"

#define Vao unsigned int

struct Window {
    int width;
    int height;
    GLFWwindow* handle;
};

static Window globalWindow;
static Vao squareVao;

static void updateViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

static void frameBufferResizeCallback(GLFWwindow *window, int width, int height) {
    updateViewport(width, height);
    globalWindow.width = width;
    globalWindow.height = height;
    // camera.setScreenSize((float) width, (float) height);
}

static void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    globalWindow.width = 1280;
    globalWindow.height = 720;
    globalWindow.handle = 
        glfwCreateWindow(
            globalWindow.width, 
            globalWindow.height,
            "Physics",
            NULL,
            NULL
        );
    if (!globalWindow.handle) {
        printf("Failed to create window\n");
    }
    glfwSetFramebufferSizeCallback(globalWindow.handle, frameBufferResizeCallback);
    glfwMakeContextCurrent(globalWindow.handle);
}

static void setupSquareVao() {
    float vertices[] = {
        -1,  1,
         1,  1,
        -1, -1,
         1, -1,
    };

    unsigned int indices[] = {
        1, 3, 2,
        0, 1, 2,
    };

    glGenVertexArrays(1, &squareVao);
    glBindVertexArray(squareVao);

    unsigned int buffers[2];
    glGenBuffers(2, buffers);
    unsigned int vertexBuffer = buffers[0];
    unsigned int indexBuffer = buffers[1];

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * 8,
        vertices,
        GL_STATIC_DRAW
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * 6,
        indices,
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
}

int main() {
    initWindow();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("failed to load required extensions\n");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setupSquareVao();

    glm::mat4 projection = glm::ortho(
        (float) (-globalWindow.width / 2),
        (float) (globalWindow.width / 2),
        (float) (-globalWindow.height / 2),
        (float) (globalWindow.height / 2),
        0.1f,
        100.0f
    );

    glm::mat4 view = 
        glm::lookAt(glm::vec3(0, 0, 100), glm::vec3(0), glm::vec3(0, 1, 0));

    GridShader gridShader = createGridShader(
        "../shader/gridVert.glsl",
        "../shader/gridFrag.glsl"
    );
    CircleShader cirlceShader = createCircleShader(
        "../shader/circleVert.glsl",
        "../shader/circleFrag.glsl"
    );

    const float radius = 30;
    glm::mat4 model = glm::scale(glm::mat4(1), glm::vec3(radius));

    float delta = 0.0f;
    float lastFrame = 0.0f;
    glClearColor(0.1, 0.1, 0.1, 1);

    while (!glfwWindowShouldClose(globalWindow.handle)) {
        if (glfwGetKey(globalWindow.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(globalWindow.handle, true);
        }

        float currentFrame = glfwGetTime();
        delta = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(gridShader.id);
        glBindVertexArray(squareVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(cirlceShader.id);
        glBindVertexArray(squareVao);
        setUniformMat4(cirlceShader.uModel, &model);
        setUniformMat4(cirlceShader.uView, &view);
        setUniformMat4(cirlceShader.uProjection, &projection);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(globalWindow.handle);
        glfwPollEvents();
    }
}
