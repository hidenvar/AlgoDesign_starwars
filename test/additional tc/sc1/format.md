iss >> name >> country >> lat >> lon >> type >> sp;

if (type == CityType::NORMAL):
    nothing more
if (type == CityType::BASE):
    iss >> missileTypesCount;
    iss >> thisMissileType; iss >> thisMissileCount;
if (type == CityType::TARGET):
    iss >> defenseLevel;

