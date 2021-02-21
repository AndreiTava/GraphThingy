#pragma once

#include <string>
#include <cmath>

class node
{
	/*Nested Classes */
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
	/*Static Helper Functions*/
private:
	static unsigned int findPair(std::string str, unsigned int pos, bool open);
	static void putParenth(std::string& str, unsigned int opnPos, unsigned int clsPos);
	static void processExpr(std::string& expr);
	static functions resolveSymbol(std::string fnc);
	/*Member Variables*/
private:
	options type = options::oprator;
	data field;
	node* left = nullptr;
	node* right = nullptr;
	/*Private Functions*/
private:
	void simplifyTree();
	void extract(std::string expr);
	/*Public Functions*/
public:
	node();
	node(std::string);
	node(const node& replacement);
	~node();
	float const computeTree(float parameter);
	void operator=(std::string expr);
	void operator=(const node& replacement);
	const node operator+(const node& rightSide);
};
