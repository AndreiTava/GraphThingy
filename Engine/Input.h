#pragma once

#include <string>
#include <cmath>


/*ToDo: convert all this to OOP*/

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
	std::string func = "\0";
	char op;
	float nr;
	data()
	{
		op = '\0';
		nr = 0;
	};
	~data() {};
};

struct node
{
	options type = options::oprator;
	data field;
	node* left = nullptr;           
	node* right = nullptr;
	float const computeTree(float parameter);
};
functions resolveFunction(std::string fnc);

unsigned int findClosed(std::string str, unsigned int openPos);

std::string processExpr(std::string expr);

node* extract(std::string expr);

