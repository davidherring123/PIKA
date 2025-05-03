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
#include "../Scene/Shape.h"
#include "../Scene/MatrixStack.h"
#include "../Scene/Program.h"

#include "Leg.h"

using namespace std;
using namespace glm;

Leg::Leg(int totalLimbs)
{
    iks = make_shared<IKSystem>(totalLimbs);
    startPosition = vec3(0, 0, 0);
    target = vec3(totalLimbs, 0, 0);
    s = vec3(0.25);
}

void Leg::draw(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, const shared_ptr<Shape> shape) const
{
    vec3 planeDir = normalize(target - startPosition);
    vec3 initialDir = normalize(vec3(target.x, target.y, 0) - vec3(startPosition.x, startPosition.y, 0));
    vec3 downdir = vec3(0, -1, 0);

    vec3 c1 = cross(planeDir, downdir);
    vec3 c2 = cross(initialDir, downdir);

    float theta;

    if (length(c1) == 0.0f || length(c2) == 0.0f)
    {
        theta = 0.0f;
    }
    else
    {
        theta = acos(dot(c1, c2) / (length(c1) * length(c2)));
    }

    vec4 rotatedTarget = rotate(mat4(1.0f), theta, vec3(0, 1, 0)) * glm::scale(mat4(1.0f), 1.0f / s) * vec4(target - startPosition, 1);

    Eigen::Vector2d point;
    point << rotatedTarget.x, rotatedTarget.y;

    MV->translate(startPosition);
    MV->rotate(theta, vec3(0, 1, 0));
    MV->scale(s);
    iks->solve(point);
    iks->draw(prog, MV, shape);
}
