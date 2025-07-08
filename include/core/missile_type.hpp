#pragma once

enum class MissileType {
  A1,
  A2,
  A3,
  B1,
  B2,
  C1,
  C2,
  D1
};

std::istream& operator>>(std::istream& is, MissileType& type);