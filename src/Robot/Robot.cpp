#include "Robot.h"

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Curves/Catmull.h"
#include "../Curves/Keyframe.h"

using namespace std;
using namespace glm;

Robot::Robot(shared_ptr<Heightmap> _H)
{
  position = vec3(0, 1, 0);
  limbLength = 3;
  bodyScale = vec3(.5, 0.2, .5);
  legScale = vec3(.45);
  maxLegDistance = legScale.y * limbLength * .9;
  H = _H;
}

void Robot::init(const shared_ptr<Program> _prog, const shared_ptr<Shape> _bodyShape, const std::shared_ptr<Shape> _legShape)
{
  prog = _prog;
  bodyShape = _bodyShape;
  legShape = _legShape;

  for (int i = 1; i > -2; i -= 2)
  {
    for (int j = 1; j > -2; j -= 2)
    {
      shared_ptr<Leg> l = make_shared<Leg>(limbLength, vec3(i * bodyScale.x / 2.0f, position.y, j * bodyScale.z / 2.0f), normalize(vec3(i, 0, j)), H);
      l->setTarget(l->getResetTargetPosition());
      l->setScale(legScale);
      legs.push_back(l);
    }
  }

  // Set Neighbors
  legs.at(0)->neighborIndices.push_back(1);
  legs.at(0)->neighborIndices.push_back(2);
  legs.at(1)->neighborIndices.push_back(0);
  legs.at(1)->neighborIndices.push_back(3);
  legs.at(2)->neighborIndices.push_back(0);
  legs.at(2)->neighborIndices.push_back(3);
  legs.at(3)->neighborIndices.push_back(1);
  legs.at(3)->neighborIndices.push_back(2);
}

void Robot::move(vec3 v)
{
  position += v;
  float AverageTargetHeight = 0.0;
  float numLegs = 0;
  for (shared_ptr<Leg> l : legs)
  {
    if (l->currentState == LegState::Idle)
    {
      AverageTargetHeight += l->getTarget().y;
    }
    else if (l->currentState == LegState::Adjusting)
    {
      AverageTargetHeight += l->interpolatedHeight;
    }
    numLegs++;
  }

  position.y = AverageTargetHeight / numLegs + 0.8f;

  for (shared_ptr<Leg> l : legs)
  {
    vec3 newStart = l->getStart() + v;
    newStart.y = position.y;

    l->setStart(newStart);

    float d = distance(l->getStart(), l->getTarget());

    vec3 currDir = normalize(vec3(l->getTarget().x, 0, l->getTarget().z) - vec3(l->getStart().x, 0, l->getStart().z));

    float theta = acos(dot(currDir, l->facingDir) / (length(currDir) * length(l->facingDir)));

    // Make Leg isn't facing inside Robot
    bool invalidAngle = theta > radians(60.0f) || theta < radians(-60.0f);

    // Make sure neighbor legs aren't adjusting
    bool neighborsAdjusting = false;
    for (int i : l->neighborIndices)
    {
      if (legs.at(i)->currentState == LegState::Adjusting)
      {
        neighborsAdjusting = true;
      }
    }

    // if angle is bad or distance too great, adjust robot leg
    if ((invalidAngle || d > maxLegDistance) && l->currentState == LegState::Idle && !neighborsAdjusting)
    {
      l->startAdjusting();
    }
  }
}

void Robot::draw(const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> MV)
{
  // Draw Body
  MV->pushMatrix();
  MV->translate(position);
  MV->scale(bodyScale);

  glUniform3f(prog->getUniform("kd"), 0.5f, 0.5f, 0.5f);
  glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
  bodyShape->draw(prog);
  MV->popMatrix();

  // Draw Legs
  for (shared_ptr<Leg> l : legs)
  {
    l->draw(prog, MV, legShape);
  }
}