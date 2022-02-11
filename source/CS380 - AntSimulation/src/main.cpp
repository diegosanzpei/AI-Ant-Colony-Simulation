/**
* @file main.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "demo_simulation.hpp"

int main(int argc, const char* argv[])
{
    cs380::demo& simulation = cs380::demo::instance();
	simulation.create();
	while (simulation.update()){}
	simulation.destroy();
	return 0;
}