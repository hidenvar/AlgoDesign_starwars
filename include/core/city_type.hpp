#pragma once

enum class CityType {
  BASE,
  NORMAL,
  TARGET
};

std::istream& operator>>(std::istream& is, CityType& type);