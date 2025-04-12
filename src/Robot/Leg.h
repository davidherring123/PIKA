#pragma once
#ifndef LEG
#define LEG

class Leg
{
public:
  Leg();
  void setLinkLength();
  void setTotalLinks();

private:
  int totalLinks;
  int linkLength;

  Eigen::VectorXd
  getDeltaP(const Eigen::VectorXd &x) const;
  Eigen::MatrixXd getP1(const Eigen::VectorXd &x) const;
};

#endif
