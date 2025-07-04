#pragma once
#include <string>
#include "missle_type.hpp"

class Missle {
  public:
  private:
      std::string name;
      int overalDistance;
      int uncontrolledDistance;
      int destruction;
      int stealth;
      MissleType type;
};
