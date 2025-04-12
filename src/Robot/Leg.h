#pragma once
#ifndef Leg_H
#define Leg_H

#include <memory>

#include "../IK/IKSystem.h"

class Leg
{
public:
  Leg(int totalLimbs);

private:
  std::shared_ptr<IKSystem> iks;
};

#endif
