#include <Eigen/Dense>

using namespace Eigen;

Matrix3d getT(double tx, double ty)
{
  Matrix3d T;
  T << 1, 0, tx,
      0, 1, ty,
      0, 0, 1;
  return T;
}

Matrix3d getR(double theta)
{
  Matrix3d R;
  R << cos(theta), -sin(theta), 0,
      sin(theta), cos(theta), 0,
      0, 0, 1;
  return R;
}

Matrix3d getR1(double theta)
{
  Matrix3d R1;
  R1 << -sin(theta), -cos(theta), 0,
      cos(theta), -sin(theta), 0,
      0, 0, 0;
  return R1;
}

Matrix3d getR2(double theta)
{
  Matrix3d R2;
  R2 << -cos(theta), sin(theta), 0,
      -sin(theta), -cos(theta), 0,
      0, 0, 0;
  return R2;
}