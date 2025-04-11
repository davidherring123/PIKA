#pragma once
#ifndef OBJECTIVE_NLINK_IK_H
#define OBJECTIVE_NLINK_IK_H

#include "Objective.h"

class ObjectiveNLinkIK : public Objective
{
public:
  ObjectiveNLinkIK(double a = 1.0, double b = 1.0);
  virtual ~ObjectiveNLinkIK();
  virtual double evalObjective(const Eigen::VectorXd &x) const;
  virtual double evalObjective(const Eigen::VectorXd &x, Eigen::VectorXd &g) const;
  virtual double evalObjective(const Eigen::VectorXd &x, Eigen::VectorXd &g, Eigen::MatrixXd &H) const;
  void setPTarget(Eigen::VectorXd pTar);
  void setWTarget(double wTar);
  void setWRegularizer(Eigen::MatrixXd wReg);

private:
  double a;
  double b;
  double wTar;
  Eigen::MatrixXd wReg;
  Eigen::VectorXd pTar;

  Eigen::VectorXd getDeltaP(const Eigen::VectorXd &x) const;
  Eigen::MatrixXd getP1(const Eigen::VectorXd &x) const;
};

#endif
