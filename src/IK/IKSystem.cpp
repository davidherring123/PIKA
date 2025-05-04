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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Eigen/Dense>

#include "Link.h"
#include "ObjectiveNLinkIK.h"
#include "OptimizerBFGS.h"

#include "IKSystem.h"

using namespace std;
using namespace Eigen;

IKSystem::IKSystem(int _nLinks)
{

  nLinks = _nLinks;
  weights << 1e3, 1e1, 1e0;

  Matrix4d E(Matrix4d::Identity());

  links.clear();

  for (int i = 0; i < nLinks; ++i)
  {
    auto link = make_shared<Link>();
    links.push_back(link);
    link->setAngle(-M_PI / 4.0f);

    link->setPosition((i == 0 ? 0.0 : 1.0), 0.0);
    E(0, 3) = 0.5;
    link->setMeshMatrix(E);
    if (i == 0)
    {
      links[i]->setAngle(0);
    }
    if (i > 0)
    {
      links[i - 1]->addChild(links[i]);
    }
  }
}

vector<shared_ptr<Link>> IKSystem::solve(Eigen::Vector2d target)
{
  VectorXd x(nLinks);
  for (int i = 0; i < nLinks; ++i)
  {
    x(i) = links[i]->getAngle();
  }

  auto objective = make_shared<ObjectiveNLinkIK>();
  objective->setPTarget(target);
  objective->setWTarget(weights(0));
  MatrixXd wReg = MatrixXd::Identity(nLinks, nLinks);
  wReg(0, 0) = weights(1);
  objective->setWRegularizer(wReg);

  OptimizerBFGS optimizer;
  optimizer.setIterMax(150);
  optimizer.setTol(1e-3);
  optimizer.setIterMaxLS(20);
  optimizer.setAlphaInit(1.0);
  optimizer.setGamma(0.5);

  VectorXd theta = VectorXd(nLinks);
  for (int i = 0; i < nLinks; i++)
  {
    theta(i) = links[i]->getAngle();
    if (theta(i) > M_PI - 0.1f)
    {
      theta(i) = M_PI - 0.1f;
    }
  }

  VectorXd angle = optimizer.optimize(objective, theta);

  for (int i = 0; i < nLinks; i++)
  {
    double val = angle(i);
    while (val > 2 * M_PI)
      val -= 2 * M_PI;
    while (val < 2 * -M_PI)
      val += 2 * M_PI;

    x(i) = val;
  }

  for (int i = 0; i < nLinks; ++i)
  {
    double xi = x(i);
    links[i]->setAngle(xi);
  }

  return links;
}

void IKSystem::draw(const std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Shape> shape) const
{
  links.front()->draw(prog, MV, shape);
}