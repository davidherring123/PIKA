#include "../IK/IKSystem.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Eigen/Dense>

#include "../IK/IKSystem.h"

#include "Leg.h"

using namespace std;

Leg::Leg(int totalLimbs)
{
  iks = make_shared<IKSystem>(totalLimbs);
  iks->solve();
}