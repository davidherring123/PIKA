#include "Robot.h"

#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Curves/Catmull.h"
#include "../Curves/Keyframe.h"

using namespace std;
using namespace glm;

Robot::Robot()
{
    position = vec3(0, 1, 0);
    limbLength = 3;
    bodyScale = vec3(.5, 0.15, 1);
    legScale = vec3(.45);
    maxLegDistance = legScale.y * limbLength;
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
            shared_ptr<Leg> l = make_shared<Leg>(limbLength, vec3(i * bodyScale.x / 2.0f, position.y, j * bodyScale.z / 2.0f), normalize(vec3(i, 0, j)));
            l->setTarget(l->getResetTargetPosition());
            l->setScale(legScale);
            legs.push_back(l);
        }
    }
}

void Robot::move(vec3 v)
{
    position += v;
    for (shared_ptr<Leg> l : legs)
    {
        l->setStart(l->getStart() + v);

        float d = distance(l->getStart(), l->getTarget());

        if (d > maxLegDistance)
        {
            l->setTarget(l->getResetTargetPosition());
        }
    }
}

void Robot::draw(const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> MV)
{
    vec3 bodyScale = vec3(.5, 0.15, 1);

    // Draw Body
    MV->pushMatrix();
    MV->translate(position);
    MV->scale(bodyScale);

    glUniform3f(prog->getUniform("kd"), 0.5f, 0.5f, 0.5f);
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
    bodyShape->draw(prog);
    MV->popMatrix();

    for (shared_ptr<Leg> l : legs)
    {
        l->draw(prog, MV, legShape);
    }
}