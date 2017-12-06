#pragma once
#include <string>
//store the uniform for updateing

class Uniform3fv
{
public:
	Uniform3fv() {
		name = "";
		value = nullptr;
	}

	Uniform3fv(std::string name, float* value) {
		this->name = name;
		this->value = value;
	}
	
	string const GetName() {
		return name;
	}

	float* const GetValue() {
		return value;
	}

protected:
	std::string name;
	float* value;
};

class Uniformf
{
public:
	Uniformf() {
		name = "";
		value = 0;
		valuePtr = nullptr;
	}

	Uniformf(std::string name, float value) {
		this->name = name;
		this->value = value;
		this->valuePtr = nullptr;
	}

	Uniformf(std::string name, float* vp) {
		this->name = name;
		this->value = 0;
		this->valuePtr = vp;
	}

	string const GetName() {
		return name;
	}

	float const GetValue() {
		if (valuePtr)
			return *valuePtr;
		else
			return value;
	}
protected:
	std::string name;
	float value;
	float* valuePtr;
};

class Uniformi
{
public:
	Uniformi() {
		name = "";
		value = 0;
		valuePtr = nullptr;
	}

	Uniformi(std::string name, int value) {
		this->name = name;
		this->value = value;
		this->valuePtr = nullptr;
	}

	Uniformi(std::string name, int* vp) {
		this->name = name;
		this->value = 0;
		this->valuePtr = vp;
	}

	string const GetName() {
		return name;
	}
	
	int const GetValue() {
		if (valuePtr)
			return *valuePtr;
		else
			return value;
	}
protected:
	std::string name;
	int value;
	int* valuePtr;
};