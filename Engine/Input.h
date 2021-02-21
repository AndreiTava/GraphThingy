#pragma once

#include <string>
#include <cmath>

class node
{

private:
	enum class options
	{
		function,
		oprator,
		parameter,
		constant
	};
	enum class functions
	{
		invalid,
		minus,
		abs,
		ceil,
		floor,
		trunc,
		round,
		exp,
		log,
		sqrt,
		cbrt,
		sin,
		cos,
		tan,
		asin,
		acos,
		atan,
		sinh,
		cosh,
		tanh,
		asinh,
		acosh,
		atanh,
		erf,
		erfc,
		tgamma
	};
	union data
	{
		functions func = functions::invalid;
		char op;
		float nr;
		data()
		{
			op = '\0';
			nr = 0;
		};
		~data() {};
	};

private:
	options type = options::oprator;
	data field;
	node* left = nullptr;
	node* right = nullptr;

public:
	node(std::string);
	~node()
	{
		delete left;
		delete right;
	}
	static functions resolveSymbol(std::string fnc);
	float const computeTree(float parameter);
	void simplifyTree();
};

unsigned int findPair(std::string str, unsigned int pos, bool open);

void putParenth(std::string& str, unsigned int opnPos, unsigned int clsPos);

std::string processExpr(std::string expr);