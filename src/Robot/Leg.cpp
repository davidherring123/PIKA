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

Leg::Leg(int _totalLimbs, vec3 _startPosition, vec3 _facingDir)
{
    iks = make_shared<IKSystem>(_totalLimbs);
    startPosition = _startPosition;
    facingDir = _facingDir;
    resetDistance = 0.2;
    s = vec3(1);
}

void Leg::draw(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, const shared_ptr<Shape> shape) const
{
    MV->pushMatrix();
    vec3 planeDir = normalize(target - startPosition);
    vec3 initialDir = vec3(1, 0, 0);
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
        if (planeDir.z > 0)
        {
            theta = -theta;
        }
    }

    vec4 rotatedTarget = rotate(mat4(1.0f), -theta, vec3(0, 1, 0)) * glm::scale(mat4(1.0f), 1.0f / s) * vec4(target - startPosition, 1);

    Eigen::Vector2d point;
    point << rotatedTarget.x, rotatedTarget.y;

    MV->translate(startPosition);
    MV->rotate(theta, vec3(0, 1, 0));
    MV->scale(s);
    iks->solve(point);
    iks->draw(prog, MV, shape);
    MV->popMatrix();
}

vec3 Leg::getResetTargetPosition()
{
    vec3 pos = vec3(startPosition.x, 0, startPosition.z);
    pos += resetDistance * facingDir;
    return pos;
}