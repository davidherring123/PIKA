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
}

Heightmap::Heightmap(float _seed, float _maxHeight, float _minHeight)
{
  seed = _seed;
  maxHeight = _maxHeight;
  minHeight = _minHeight;
}

float Heightmap::getHeight(float x, float y)
{
  float perlinValue = perlin(vec2(x, y) + vec2(seed, seed));

  float normalizedZ = (perlinValue + 1) / 2.0f;
  float z = minHeight + (normalizedZ * (maxHeight - minHeight));

  return z;
}

shared_ptr<Shape> Heightmap::generatePlane(string RESOURCE_DIR)
{
  vector<vec3> vertices;
  vector<vec3> normals;
  vector<int> faces;

  int rows = 100;
  int cols = 100;
  float stepSize = 1.0f / (float)(rows - 1);

  // Generate vertices and normals
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      float x = -0.5f + j * stepSize;
      float z = -0.5f + i * stepSize;

      vertices.push_back(vec3(x, getHeight(x, z), z)); // Position of the vertex
      normals.push_back(vec3(0.0f, 1.0f, 0.0f));       // Normals pointing upwards
    }
  }

  // Generate faces (two triangles for each square in the grid)
  for (int i = 0; i < rows - 1; i++)
  {
    for (int j = 0; j < cols - 1; j++)
    {
      // Indices of the four corners of the square
      int topLeft = i * cols + j;
      int topRight = topLeft + 1;
      int bottomLeft = (i + 1) * cols + j;
      int bottomRight = bottomLeft + 1;

      // First triangle (top-left, top-right, bottom-left)
      faces.push_back(topLeft);
      faces.push_back(topRight);
      faces.push_back(bottomLeft);

      // Second triangle (bottom-left, top-right, bottom-right)
      faces.push_back(bottomLeft);
      faces.push_back(topRight);
      faces.push_back(bottomRight);
    }
  }

  ofstream file(RESOURCE_DIR + "plane.obj", ios::out | ios::trunc);

  if (!file.is_open())
  {
    cerr << "Error: Could not open file for writing." << endl;
  }

  for (const auto &vertex : vertices)
  {
    file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
  }

  for (const auto &normal : normals)
  {
    file << "vn " << normal.x << " " << normal.y << " " << normal.z << "\n";
  }

  for (size_t i = 0; i < faces.size(); i += 3)
  {
    file << "f "
         << faces[i] + 1 << "//" << faces[i] + 1 << " "
         << faces[i + 1] + 1 << "//" << faces[i + 1] + 1 << " "
         << faces[i + 2] + 1 << "//" << faces[i + 2] + 1 << "\n";
  }

  file.close();

  shared_ptr<Shape> plane = make_shared<Shape>();

  plane->loadMesh(RESOURCE_DIR + "plane.obj");
  return plane;
}