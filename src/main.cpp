#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Game variables
int playerLane = 1; // 0=left, 1=center, 2=right
float playerY = -0.7f;
float carWidth = 0.15f;
float carHeight = 0.25f;
float lanePositions[3] = {-0.5f, 0.0f, 0.5f}; // X positions for lanes

struct EnemyCar {
    float x, y;
    float speed;
    bool active;
};

EnemyCar enemyCars[10]; // Static array instead of vector
int numEnemyCars = 0;
float enemySpawnTimer = 0.0f;
float enemySpawnDelay = 2.0f;
bool gameOver = false;
bool gameStarted = false;

// OpenGL objects
unsigned int shaderProgram;
unsigned int VAO, VBO;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 transform;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(aPos, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";

// Simple random number generator
unsigned int seed = 1;
int myRand() {
    seed = seed * 1103515245 + 12345;
    return (seed / 65536) % 32768;
}

// Create transformation matrix
void createTransformMatrix(float matrix[16], float x, float y, float scaleX, float scaleY) {
    // Identity matrix
    for (int i = 0; i < 16; i++) matrix[i] = 0;
    matrix[0] = scaleX; matrix[5] = scaleY; matrix[10] = 1.0f; matrix[15] = 1.0f;
    
    // Translation
    matrix[12] = x;
    matrix[13] = y;
}

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Car Lane Game", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    // build and compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data for a quad
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f,  // bottom right
         0.5f,  0.5f, 0.0f,  // top right
        -0.5f,  0.5f, 0.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Initialize enemy cars array
    for (int i = 0; i < 10; i++) {
        enemyCars[i].active = false;
    }

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        static float lastTime = 0.0f;
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processInput(window);

        // Game logic
        if (gameStarted && !gameOver) {
            // Spawn enemy cars
            enemySpawnTimer += deltaTime;
            if (enemySpawnTimer >= enemySpawnDelay) {
                // Find inactive slot
                for (int i = 0; i < 10; i++) {
                    if (!enemyCars[i].active) {
                        enemyCars[i].x = lanePositions[myRand() % 3];
                        enemyCars[i].y = 1.2f;
                        enemyCars[i].speed = 0.5f + (myRand() % 50) / 100.0f;
                        enemyCars[i].active = true;
                        break;
                    }
                }
                enemySpawnTimer = 0.0f;
                enemySpawnDelay = 1.5f + (myRand() % 100) / 100.0f;
            }

            // Update enemy cars
            for (int i = 0; i < 10; i++) {
                if (enemyCars[i].active) {
                    enemyCars[i].y -= enemyCars[i].speed * deltaTime;
                    if (enemyCars[i].y < -1.2f) {
                        enemyCars[i].active = false;
                    }
                }
            }

            // Check collisions
            float playerX = lanePositions[playerLane];
            for (int i = 0; i < 10; i++) {
                if (enemyCars[i].active && checkCollision(playerX, playerY, carWidth, carHeight, 
                                                enemyCars[i].x, enemyCars[i].y, carWidth, carHeight)) {
                    gameOver = true;
                    break;
                }
            }
        }

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Draw road lanes (white lines)
        int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        int transformLocation = glGetUniformLocation(shaderProgram, "transform");
        
        glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f); // White
        
        // Lane dividers
        for (int i = 0; i < 2; i++) {
            float matrix[16];
            createTransformMatrix(matrix, -0.25f + i * 0.5f, 0.0f, 0.02f, 2.0f);
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, matrix);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        if (gameStarted) {
            // Draw player car (blue)
            glUniform4f(colorLocation, 0.0f, 0.5f, 1.0f, 1.0f);
            float playerMatrix[16];
            createTransformMatrix(playerMatrix, lanePositions[playerLane], playerY, carWidth, carHeight);
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, playerMatrix);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Draw enemy cars (red)
            glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
            for (int i = 0; i < 10; i++) {
                if (enemyCars[i].active) {
                    float enemyMatrix[16];
                    createTransformMatrix(enemyMatrix, enemyCars[i].x, enemyCars[i].y, carWidth, carHeight);
                    glUniformMatrix4fv(transformLocation, 1, GL_FALSE, enemyMatrix);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }
        }

        // Draw start/game over indicator (green square in corner)
        if (!gameStarted || gameOver) {
            glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f); // Green
            float indicatorMatrix[16];
            createTransformMatrix(indicatorMatrix, -0.8f, 0.8f, 0.1f, 0.1f);
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, indicatorMatrix);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    static bool spacePressed = false;
    static bool aPressed = false;
    static bool dPressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Start/Restart game
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
        if (!gameStarted || gameOver) {
            gameStarted = true;
            gameOver = false;
            playerLane = 1;
            // Clear all enemy cars
            for (int i = 0; i < 10; i++) {
                enemyCars[i].active = false;
            }
            enemySpawnTimer = 0.0f;
        }
        spacePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spacePressed = false;
    }

    // Lane changing
    if (gameStarted && !gameOver) {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !aPressed) {
            if (playerLane > 0) {
                playerLane--;
            }
            aPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
            aPressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !dPressed) {
            if (playerLane < 2) {
                playerLane++;
            }
            dPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
            dPressed = false;
        }
    }
}

bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (x1 - w1/2 < x2 + w2/2 && x1 + w1/2 > x2 - w2/2 &&
            y1 - h1/2 < y2 + h2/2 && y1 + h1/2 > y2 - h2/2);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}