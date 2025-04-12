#pragma once
#ifndef IKSystem_H
#define IKSystem_H

#include <memory>
#include <vector>

#include <Eigen/Dense>
#include "Link.h"

class IKSystem
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  IKSystem(int _nLinks);
  void init();
  std::vector<std::shared_ptr<Link>> solve(Eigen::Vector2d target);

private:
  int nLinks;
  Eigen::Vector3d weights;
  std::vector<std::shared_ptr<Link>> links;
};

#endif