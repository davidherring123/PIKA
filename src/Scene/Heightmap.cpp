#include "Heightmap.h"

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
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shape.h"

using namespace std;
using namespace glm;

Heightmap::Heightmap()
{
  seed = 0;
  maxHeight = 1;
  minHeight = 0;
  width = 10;
  length = 10;
}

Heightmap::Heightmap(float _seed, float _maxHeight, float _minHeight, float _scale)
{
  seed = _seed;
  maxHeight = _maxHeight;
  minHeight = _minHeight;
  width = 100;
  length = 100;
  scale = _scale;
}

float Heightmap::getHeight(float x, float z)
{
  float perlinValue = perlin(vec2(x * scale, z * scale) + vec2(seed, seed));

  float normalizedZ = (perlinValue + 1) / 2.0f;
  float y = minHeight + (normalizedZ * (maxHeight - minHeight));

  return y;
}

shared_ptr<Shape> Heightmap::generatePlane(string RESOURCE_DIR)
{
  int rows = 100;
  int cols = 100;
  float stepSizeX = width / (float)(cols - 1);
  float stepSizeZ = length / (float)(rows - 1);

  vector<vec3> vertices;
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      float x = -(width / 2.0f) + j * stepSizeX;
      float z = -(length / 2.0f) + i * stepSizeZ;

      vertices.push_back(vec3(x, getHeight(x, z), z));
    }
  }

  vector<int> faces;
  for (int i = 0; i < rows - 1; i++)
  {
    for (int j = 0; j < cols - 1; j++)
    {
      int topLeft = i * cols + j;
      int topRight = topLeft + 1;
      int bottomLeft = (i + 1) * cols + j;
      int bottomRight = bottomLeft + 1;

      faces.push_back(topLeft);
      faces.push_back(bottomLeft);
      faces.push_back(topRight);

      faces.push_back(topRight);
      faces.push_back(bottomLeft);
      faces.push_back(bottomRight);
    }
  }

  vector<vec3> normals(vertices.size(), vec3(0.0f));
  for (int i = 0; i < faces.size(); i += 3)
  {
    int v1 = faces[i];
    int v2 = faces[i + 1];
    int v3 = faces[i + 2];

    vec3 v1v2 = vertices[v2] - vertices[v1];
    vec3 v1v3 = vertices[v3] - vertices[v1];
    vec3 faceNormal = normalize(cross(v1v2, v1v3));

    normals[v1] += faceNormal;
    normals[v2] += faceNormal;
    normals[v3] += faceNormal;
  }

  for (vec3 normal : normals)
  {
    normal = normalize(normal);
  }

  ofstream file(RESOURCE_DIR + "plane.obj");

  for (vec3 vertex : vertices)
  {
    file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
  }

  for (vec3 normal : normals)
  {
    file << "vn " << normal.x << " " << normal.y << " " << normal.z << "\n";
  }

  for (int i = 0; i < faces.size(); i += 3)
  {
    file << "f ";
    file << faces[i] + 1 << "//" << faces[i] + 1 << " ";
    file << faces[i + 1] + 1 << "//" << faces[i + 1] + 1 << " ";
    file << faces[i + 2] + 1 << "//" << faces[i + 2] + 1 << "\n";
  }

  file.close();
  shared_ptr<Shape> plane = make_shared<Shape>();
  plane->loadMesh(RESOURCE_DIR + "plane.obj");
  return plane;
}