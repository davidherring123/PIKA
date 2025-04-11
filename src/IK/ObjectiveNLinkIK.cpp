#include "ObjectiveNLinkIK.h"
#include <cmath>
#include <iostream>

#include "Helper.h"

using namespace Eigen;
using namespace std;

Vector3d rn = Vector3d(1, 0, 1);

ObjectiveNLinkIK::ObjectiveNLinkIK(double a, double b)
{
  this->a = a;
  this->b = b;
}

ObjectiveNLinkIK::~ObjectiveNLinkIK()
{
}

VectorXd ObjectiveNLinkIK::getDeltaP(const VectorXd &x) const
{
  Matrix3d rootT = getT(0, 0);
  Matrix3d T = getT(1, 0);

  if (x.size() < 2)
  {
    Vector3d pos = rootT * getR(x(0)) * rn;
    return pos.head<2>() - pTar;
  }

  Matrix3d transform = rootT * getR(x(0));
  for (int i = 1; i < x.size(); i++)
  {
    transform = transform * T * getR(x(i));
  }

  Vector3d pos = transform * rn;
  return pos.head<2>() - pTar;
}

double ObjectiveNLinkIK::evalObjective(const VectorXd &x) const
{
  VectorXd deltaP = getDeltaP(x);
  double resultLeft = 0.5 * wTar * deltaP.transpose() * deltaP;
  double resultRight = 0.5 * x.transpose() * wReg * x;
  return resultLeft + resultRight;
}

MatrixXd ObjectiveNLinkIK::getP1(const VectorXd &x) const
{
  Matrix3d rootT = getT(0, 0);
  Matrix3d T = getT(1, 0);
  MatrixXd S(2, x.size());
  for (int i = 0; i < x.size(); i++)
  {
    Matrix3d transformLeft;
    if (i == 0)
    {
      transformLeft = rootT;
    }
    else
    {
      transformLeft = rootT;
      for (int j = 0; j < i; j++)
      {
        transformLeft = transformLeft * getR(x(j)) * T;
      }
    }
    Matrix3d transformRight = Matrix3d::Identity();
    for (int j = i + 1; j < x.size(); j++)
    {
      transformRight = transformRight * T * getR(x(j));
    }
    S.col(i) = (transformLeft * getR1(x(i)) * transformRight * rn).head<2>();
  }
  return S;
}

double ObjectiveNLinkIK::evalObjective(const VectorXd &x, VectorXd &g) const
{
  VectorXd deltaP = getDeltaP(x);
  MatrixXd sens = getP1(x);

  g = wTar * (deltaP.transpose() * sens).transpose() + wReg * x;
  return 0;
}

double ObjectiveNLinkIK::evalObjective(const VectorXd &x, VectorXd &g, MatrixXd &H) const
{
  return 0;
}

void ObjectiveNLinkIK::setPTarget(VectorXd pTar)
{
  this->pTar = pTar;
}

void ObjectiveNLinkIK::setWTarget(double wTar)
{
  this->wTar = wTar;
}

void ObjectiveNLinkIK::setWRegularizer(MatrixXd wReg)
{
  this->wReg = wReg;
}
