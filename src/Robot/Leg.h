#pragma once
#ifndef Leg_H
#define Leg_H

#include <memory>

#include "../IK/IKSystem.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Scene/Shape.h"
#include "../Scene/MatrixStack.h"
#include "../Scene/Program.h"

class Leg
{
public:
    Leg(int totalLimbs);
    glm::vec3 getTarget() { return target; };
    void setTarget(glm::vec3 _target) { target = _target; };
    glm::vec3 getStart() { return startPosition; };
    void setStart(glm::vec3 _startPosition) { startPosition = _startPosition; };
    void draw(const std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Shape> shape) const;
    void setScale(glm::vec3 _scale) { s = _scale; };
    glm::vec3 getScale() { return s; };

private:
    std::shared_ptr<IKSystem> iks;
    glm::vec3 target;
    glm::vec3 startPosition;
    glm::vec3 s;
};

#endif
