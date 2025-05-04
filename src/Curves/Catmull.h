#pragma once
#ifndef Catmull_H
#define Catmull_H

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Keyframe.h"

class Catmull
{
public:
    Catmull();
    void addKeyframe(Keyframe kf);
    void drawSpline();
    glm::vec3 getInterpolatedPosition(int uMax, float u);
    glm::quat getInterpolatedRotation(int uMax, float u);
    void buildALTable();
    void clearALTable()
    {
        keyframes.clear();
        ALTable.clear();
    };
    std::vector<std::pair<float, float>> getALTable();
    float s2u(float s);

private:
    std::vector<Keyframe> keyframes;
    std::vector<std::pair<float, float>> ALTable;
    glm::mat4 B;
};

#endif
