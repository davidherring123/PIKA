#include <iostream>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Keyframe.h"

using namespace glm;
using namespace std;

Keyframe::Keyframe()
{
  pos = vec3(0, 0, 0);
  rot = quat();
}

Keyframe::Keyframe(vec3 _pos)
{
  pos = _pos;
  rot = quat();
}

Keyframe::Keyframe(vec3 _pos, quat _rot)
{
  pos = _pos;
  rot = _rot;
}

vec3 Keyframe::getPos()
{
  return pos;
}

quat Keyframe::getRot()
{
  return rot;
}

void Keyframe::setPos(vec3 p)
{
  pos = p;
}

void Keyframe::setRot(quat q)
{
  rot = q;
}