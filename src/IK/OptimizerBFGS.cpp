#include "OptimizerBFGS.h"
#include "Objective.h"
#include <iostream>

using namespace std;
using namespace Eigen;

OptimizerBFGS::OptimizerBFGS()
{
}

OptimizerBFGS::~OptimizerBFGS()
{
}

VectorXd OptimizerBFGS::optimize(const shared_ptr<Objective> objective, const VectorXd &xInit)
{
  double a0 = alphaInit;
  MatrixXd A = MatrixXd::Identity(xInit.size(), xInit.size());
  VectorXd x = xInit;
  VectorXd x0;
  VectorXd g;
  VectorXd g0;

  MatrixXd I = MatrixXd::Identity(xInit.size(), xInit.size());

  for (iter = 0; iter < iterMax; iter++)
  {
    double f = objective->evalObjective(x);
    objective->evalObjective(x, g);

    if (iter > 0)
    {
      VectorXd s = x - x0;
      VectorXd y = g - g0;
      double rho = 1.0 / (y.transpose() * s);
      A = ((I - rho * s * y.transpose()) * A * (I - rho * y * s.transpose())) + rho * s * s.transpose();
    }

    VectorXd p = -A * g;

    double a = a0;

    VectorXd deltaX;
    for (iterLS = 0; iterLS < iterMaxLS; iterLS++)
    {
      deltaX = a * p;
      double f1 = objective->evalObjective(x + deltaX);
      if (f1 < f)
        break;
      a = a * gamma;
    }
    deltaX = a * p;
    x0 = x;
    g0 = g;

    x += deltaX;

    if (g.norm() < tol)
      break;
  }
  return x;
}
