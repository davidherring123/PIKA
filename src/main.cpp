#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Scene/Camera.h"
#include "Scene/GLSL.h"
#include "Scene/Program.h"
#include "Scene/MatrixStack.h"
#include "Scene/Shape.h"
#include "Robot/Leg.h"
#include "Robot/Robot.h"

#include "Scene/Heightmap.h"

using namespace std;
using namespace glm;

GLFWwindow *window;       // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from

int keyPresses[256] = {0}; // only for English keyboards!

shared_ptr<Program> prog;
shared_ptr<Camera> camera;

shared_ptr<Shape> bodyShape, legShape, plane;

shared_ptr<Robot> robot;
Catmull spline;
Keyframe k1, k2, k3, k4, k5;

vec3 robotPosition;
float robotRotation, robotMovespeed, robotTurnspeed;

shared_ptr<Heightmap> H0, H1, H2;

int HeightMapNumber = 0;
const int TOTALMAPS = 3;

static void error_callback(int error, const char *description)
{
  cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
  if (key == 'h' || key == 'H')
  {
    HeightMapNumber++;
    HeightMapNumber = HeightMapNumber % TOTALMAPS;

    switch (HeightMapNumber)
    {
    case 0:
    {
      robot->setHeightMap(H0);
      plane = H0->generatePlane(RESOURCE_DIR);
      plane->init();
      break;
    }
    case 1:
    {
      robot->setHeightMap(H1);
      plane = H1->generatePlane(RESOURCE_DIR);
      plane->init();
      break;
    }
    case 2:
    {
      robot->setHeightMap(H2);
      plane = H2->generatePlane(RESOURCE_DIR);
      plane->init();
      break;
    }
    }
  }
}

static void cursor_position_callback(GLFWwindow *window, double xmouse, double ymouse)
{
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  if (state == GLFW_PRESS)
  {
    camera->mouseMoved(xmouse, ymouse);
  }
}

pair<vec3, float> moveRobot()
{
  vec3 moveInput(0.0f);
  float yRotation = 0.0f;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    moveInput.z -= 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    moveInput.z += 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    moveInput.x += 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    moveInput.x -= 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
  {
    yRotation += 1.0;
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
  {
    yRotation -= 1.0;
  }

  if (length(moveInput) > 0.0f)
  {
    moveInput = normalize(moveInput);
  }

  return pair<vec3, float>(moveInput, yRotation);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
  // Get the current mouse position.
  double xmouse, ymouse;
  glfwGetCursorPos(window, &xmouse, &ymouse);
  // Get current window size.
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  if (action == GLFW_PRESS)
  {
    bool shift = mods & GLFW_MOD_SHIFT;
    bool ctrl = mods & GLFW_MOD_CONTROL;
    bool alt = mods & GLFW_MOD_ALT;
    camera->mouseClicked(xmouse, ymouse, shift, ctrl, alt);
  }
}

static void init()
{
  GLSL::checkVersion();

  // Set background color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  // Enable z-buffer test
  glEnable(GL_DEPTH_TEST);

  keyPresses[(unsigned)'c'] = 1;

  prog = make_shared<Program>();
  prog->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
  prog->setVerbose(true);
  prog->init();
  prog->addUniform("P");
  prog->addUniform("MV");
  prog->addUniform("lightPos");
  prog->addUniform("ka");
  prog->addUniform("kd");
  prog->addUniform("ks");
  prog->addUniform("s");
  prog->addAttribute("aPos");
  prog->addAttribute("aNor");
  prog->setVerbose(false);

  camera = make_shared<Camera>();

  H0 = make_shared<Heightmap>(0, 0, 0, 0);
  H1 = make_shared<Heightmap>(0, 0, 1, 0.25);
  H2 = make_shared<Heightmap>(0, 0, 2, 0.5);
  plane = H0->generatePlane(RESOURCE_DIR);
  plane->init();

  bodyShape = make_shared<Shape>();
  bodyShape->loadMesh(RESOURCE_DIR + "body.obj");
  bodyShape->init();

  legShape = make_shared<Shape>();
  legShape->loadMesh(RESOURCE_DIR + "leg.obj");
  legShape->init();

  robotPosition = vec3(0, 0, 0);
  robotRotation = 0.0f;
  robotMovespeed = 0.01f;
  robotTurnspeed = 0.0f;

  robot = make_shared<Robot>();
  robot->setHeightMap(H0);
  robot->init(prog, bodyShape, legShape);

  // Initialize time.
  glfwSetTime(0.0);

  // If there were any OpenGL errors, this will print something.
  // You can intersperse this line in your code to find the exact location
  // of your OpenGL error.
  GLSL::checkError(GET_FILE_LINE);
}

void render()
{
  // Update time.
  double t = glfwGetTime();

  // Get current frame buffer size.
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  // Use the window size for camera.
  glfwGetWindowSize(window, &width, &height);
  camera->setAspect((float)width / (float)height);

  // Clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (keyPresses[(unsigned)'c'] % 2)
  {
    glEnable(GL_CULL_FACE);
  }
  else
  {
    glDisable(GL_CULL_FACE);
  }
  if (keyPresses[(unsigned)'z'] % 2)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  auto P = make_shared<MatrixStack>();
  auto MV = make_shared<MatrixStack>();

  // Apply camera transforms
  P->pushMatrix();
  camera->applyProjectionMatrix(P);
  MV->pushMatrix();
  camera->applyViewMatrix(MV);

  // Setup the projection matrix
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadMatrixf(glm::value_ptr(P->topMatrix()));

  // Setup the modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadMatrixf(glm::value_ptr(MV->topMatrix()));

  pair<vec3, float> robotMovement = moveRobot();

  // Movement

  robotRotation += robotMovement.second * robotTurnspeed;
  float sinYaw = sin(robotRotation);
  float cosYaw = cos(robotRotation);
  vec3 rotatedMoveInput;
  rotatedMoveInput.x = -(robotMovement.first.x * cosYaw - robotMovement.first.z * sinYaw);
  rotatedMoveInput.z = robotMovement.first.x * sinYaw + robotMovement.first.z * cosYaw;
  rotatedMoveInput.y = 0.0f;

  prog->bind();

  MV->pushMatrix();

  glUniform3f(prog->getUniform("kd"), 1.0f, 1.0f, 1.0f);
  glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

  plane->draw(prog);

  robot->move(rotatedMoveInput * robotMovespeed);

  robot->draw(P, MV);

  MV->popMatrix();

  prog->unbind();

  // Pop modelview matrix
  glPopMatrix();

  // Pop projection matrix
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // Pop stacks
  MV->popMatrix();
  P->popMatrix();

  GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
  RESOURCE_DIR = "../resources/";

  // Set error callback.
  glfwSetErrorCallback(error_callback);
  // Initialize the library.
  if (!glfwInit())
  {
    return -1;
  }
  // Create a windowed mode window and its OpenGL context.
  window = glfwCreateWindow(640, 480, "PIKA", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }
  // Make the window's context current.
  glfwMakeContextCurrent(window);
  // Initialize GLEW.
  glewExperimental = true;
  if (glewInit() != GLEW_OK)
  {
    cerr << "Failed to initialize GLEW" << endl;
    return -1;
  }
  glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
  cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
  cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
  // Set vsync.
  glfwSwapInterval(1);
  // Set keyboard callback.
  glfwSetKeyCallback(window, key_callback);
  // Set char callback.
  glfwSetCharCallback(window, char_callback);
  // Set cursor position callback.
  glfwSetCursorPosCallback(window, cursor_position_callback);
  // Set mouse button callback.
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  // Initialize scene.
  init();
  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(window))
  {
    if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED))
    {
      // Render scene.
      render();
      // Swap front and back buffers.
      glfwSwapBuffers(window);
    }
    // Poll for and process events.
    glfwPollEvents();
  }
  // Quit program.
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
