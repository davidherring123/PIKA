#pragma once
#ifndef Keyframe_H
#define Keyframe_H

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Keyframe
{
public:
    Keyframe();
    Keyframe(glm::vec3 _pos);
    Keyframe(glm::vec3 _pos, glm::quat _rot);

    glm::vec3 getPos();
    glm::quat getRot();

    void setPos(glm::vec3 q);
    void setRot(glm::quat q);

private:
    glm::vec3 pos;
    glm::quat rot;
};

#endif
