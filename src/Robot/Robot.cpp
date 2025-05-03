#include "Robot.h"

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
}

void Robot::init(const shared_ptr<Program> _prog, const shared_ptr<Shape> _bodyShape, const std::shared_ptr<Shape> _legShape)
{
    prog = _prog;
    bodyShape = _bodyShape;
    legShape = _legShape;

    for (int i = 0; i < 4; i++)
    {
        shared_ptr<Leg> l = make_shared<Leg>(limbLength);
        legs.push_back(l);
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

    int legIndex = 0;

    // for (int i = -1; i < 2; i += 2)
    // {
    //     for (int j = -1; j < 2; j += 2)
    //     {
    //         MV->pushMatrix();
    //         shared_ptr<Leg> l = legs.at(legIndex);
    //         l->setStart(position + vec3(i * bodyScale.x, 0, j * bodyScale.z));
    //         l->setTarget(position + vec3(i * bodyScale.x * 1.1, -position.y * .9, j * bodyScale.z * 1.1));
    //         l->setScale(vec3(0.45));
    //         l->draw(prog, MV, legShape);
    //         MV->popMatrix();
    //     }
    // }

    // Front Left Leg (-Z, +X)

    MV->pushMatrix();
    shared_ptr<Leg> l = legs.at(legIndex);
    l->setStart(position + (bodyScale.x, 0, bodyScale.z));
    l->setTarget(position + vec3(0, -position.y, 0));
    l->setScale(vec3(0.45));
    l->draw(prog, MV, legShape);
    MV->popMatrix();
}