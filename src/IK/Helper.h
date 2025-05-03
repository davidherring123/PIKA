#ifndef HELPER_H
#define HELPER_H

#include <Eigen/Dense>

Eigen::Matrix3d getT(double tx, double ty);
Eigen::Matrix3d getR(double theta);
Eigen::Matrix3d getR1(double theta);
Eigen::Matrix3d getR2(double theta);
Eigen::Matrix3d getRy(double theta);
Eigen::Matrix3d getRy1(double theta);

#endif