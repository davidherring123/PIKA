#pragma once
#ifndef Robot_H
#define Robot_H

#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Scene/Shape.h"
#include "../Scene/MatrixStack.h"
#include "../Scene/Program.h"

#include "Leg.h"

class Robot
{
public:
    Robot();
    void init(const std::shared_ptr<Program> _prog, const std::shared_ptr<Shape> _bodyShape, const std::shared_ptr<Shape> _legShape);
    void draw(const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> MV);

private:
    std::vector<std::shared_ptr<Leg>> legs;
    std::shared_ptr<Program> prog;
    std::shared_ptr<Shape> bodyShape;
    std::shared_ptr<Shape> legShape;

    int limbLength;

    glm::vec3 position;
};

#endif
