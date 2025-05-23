#pragma once
#ifndef HeightMap_H
#define HeightMap_H

#include <memory>
#include <vector>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shape.h"

class Heightmap
{
public:
  Heightmap();
  Heightmap(float _seed, float _maxHeight, float _minHeight, float _scale);

  float getHeight(float x, float z);
  std::shared_ptr<Shape> generatePlane(std::string RESOURCE_DIR);

private:
  float seed;
  float maxHeight;
  float minHeight;
  float width;
  float length;
  float scale;
};

#endif
