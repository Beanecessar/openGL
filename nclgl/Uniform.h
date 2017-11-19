#pragma once
#include <string>

class Uniformfv
{
public:
	Uniformfv() {
		name = "";
		value = nullptr;
	}
	Uniformfv(std::string name, float* value) {
		this->name = name;
		this->value = value;
	}
	std::string name;
	float* value;
};

class Uniformi
{
public:
	Uniformi() {
		name = "";
		value = 0;
	}
	Uniformi(std::string name, int value) {
		this->name = name;
		this->value = value;
	}
	std::string name;
	int value;
};