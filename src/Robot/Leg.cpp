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
#include "../Curves/Catmull.h"
#include "../Curves/Keyframe.h"

#include "Leg.h"

using namespace std;
using namespace glm;

Leg::Leg(int _totalLimbs, vec3 _startPosition, vec3 _facingDir)
{
  iks = make_shared<IKSystem>(_totalLimbs);
  startPosition = _startPosition;
  facingDir = _facingDir;
  resetDistance = 0.5;
  legScale = vec3(1);
  curve = make_shared<Catmull>();
  currentState = LegState::Idle;
  t = 0;
  neighborIndices = {};
}

void Leg::draw(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, const shared_ptr<Shape> shape)
{
  MV->pushMatrix();
  if (currentState == LegState::Idle)
  {
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

    vec4 rotatedTarget = rotate(mat4(1.0f), -theta, vec3(0, 1, 0)) * scale(mat4(1.0f), 1.0f / legScale) * vec4(target - startPosition, 1);

    Eigen::Vector2d point;
    point << rotatedTarget.x, rotatedTarget.y;

    MV->translate(startPosition);
    MV->rotate(theta, vec3(0, 1, 0));
    MV->scale(legScale);
    iks->solve(point);
    iks->draw(prog, MV, shape);
  }
  else if (currentState == LegState::Adjusting)
  {
    float tNorm = fmod(t, 1.0f);
    float sNorm = tNorm;
    float s = curve->getALTable().back().second * sNorm;
    float u = curve->s2u(s);
    targetLocal = curve->getInterpolatedPosition(2, u);
    // curve->drawSpline();

    t += 0.04f;
    if (t > 1)
    {
      stopAdjusting();
    }

    vec3 targetWorld = targetLocal + startPosition;

    vec3 planeDir = normalize(targetWorld - startPosition);
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

    vec4 rotatedTarget = rotate(mat4(1.0f), -theta, vec3(0, 1, 0)) * scale(mat4(1.0f), 1.0f / legScale) * vec4(targetWorld - startPosition, 1);

    Eigen::Vector2d point;
    point << rotatedTarget.x, rotatedTarget.y;

    MV->translate(startPosition);
    MV->rotate(theta, vec3(0, 1, 0));
    MV->scale(legScale);
    iks->solve(point);
    iks->draw(prog, MV, shape);
  }
  MV->popMatrix();
}

vec3 Leg::getResetTargetPosition()
{
  vec3 pos = vec3(startPosition.x, 0, startPosition.z);
  pos += resetDistance * facingDir;
  return pos;
}

void Leg::startAdjusting()
{
  currentState = LegState::Adjusting;
  targetLocal = target - startPosition;

  t = 0;
  generateCurve();
}

void Leg::stopAdjusting()
{
  currentState = LegState::Idle;
  target = targetLocal + startPosition;
}

void Leg::generateCurve()
{
  curve->clearALTable();
  vec3 curveStart = targetLocal;
  vec3 curveEnd = getResetTargetPosition() - startPosition;

  vec3 halfway = (curveEnd - curveStart) * 0.5f + curveStart;
  halfway.y = curveStart.y / 2.0f; // Control the height

  curve->addKeyframe(Keyframe(curveStart));
  curve->addKeyframe(Keyframe(curveStart));
  curve->addKeyframe(Keyframe(halfway));
  curve->addKeyframe(Keyframe(curveEnd));
  curve->addKeyframe(Keyframe(curveEnd));
  curve->buildALTable();
}
