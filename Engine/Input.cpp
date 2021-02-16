
#include "Input.h"

unsigned int findClosed(std::string str, unsigned int openPos)
{
	unsigned int count = 0;
	for (unsigned int i = openPos; i < str.length(); i++)
	{
		switch (str[i])
		{
		case '(':
		{
			count++;
			break;
		}
		case ')':
		{
			count--;
			break;
		}

		default:
			break;
		}
		if (count == 0)
			return i;
	}
	return 0;
}

functions resolveFunction(std::string fnc)
{
	if (fnc == "abs")
		return functions::abs;
	if (fnc == "sin")
		return functions::sin;
	if (fnc == "cos")
		return functions::cos;
	if (fnc == "tan")
		return functions::tan;
	if (fnc == "sqrt")
		return functions::sqrt;
	if (fnc == "log")
		return functions::log;
	if (fnc == "exp")
		return functions::exp;
	if (fnc == "ceil")
		return functions::ceil;
	if (fnc == "floor")
		return functions::floor;
	if (fnc == "trunc")
		return functions::trunc;
	if (fnc == "round")
		return functions::round;
	if (fnc == "cbrt")
		return functions::cbrt;
	if (fnc == "asin")
		return functions::asin;
	if (fnc == "acos")
		return functions::acos;
	if (fnc == "atan")
		return functions::atan;
	if (fnc == "sinh")
		return functions::sinh;
	if (fnc == "cosh")
		return functions::cosh;
	if (fnc == "tanh")
		return functions::tanh;
	if (fnc == "asinh")
		return functions::asinh;
	if (fnc == "acosh")
		return functions::acosh;
	if (fnc == "atanh")
		return functions::atanh;
	if (fnc == "erf")
		return functions::erf;
	if (fnc == "erfc")
		return functions::erfc;
	if (fnc == "tgamma")
		return functions::tgamma;
	return functions::invalid;

}

std::string processExpr(std::string expr)
{
	std::string newstr;
	unsigned int Pos = expr.find_first_of("0123456789");
	while (Pos != std::string::npos)
	{
		Pos = expr.find_first_of("0123456789", Pos);
	}
	return newstr;
};

node* extract(std::string expr)
{
	node* elem = new node;
	unsigned int Pos = expr.find("(");
	if (Pos != std::string::npos)
	{
		unsigned int closed = findClosed(expr, Pos);
		if (closed < expr.length() - 1)
		{
			elem->type = options::oprator;
			elem->field.op = expr[closed + 1];

			std::string left = expr.substr(Pos + 1, closed - Pos - 1);

			Pos = closed + 2;
			closed = findClosed(expr, Pos);
			std::string right = expr.substr(Pos + 1, closed - Pos - 1);

			elem->left = extract(left);
			elem->right = extract(right);

		}
		else
		{
			elem->type = options::function;
			elem->field.func = expr.substr(0, Pos);

			std::string right = expr.substr(Pos + 1, closed - Pos - 1);
			elem->right = extract(right);

		}
	}
	else
	{
		if (expr == "x")
		{
			elem->type = options::parameter;
		}
		else
		{
			elem->type = options::constant;
			elem->field.nr = stof(expr);
		}
	}
	return elem;
}

float const node::computeTree(float parameter)
{
	switch (type)
	{
	case options::constant:
	{
		return field.nr;
	}
	case options::parameter:
	{
		return parameter;
	}
	case options::oprator:
	{
		switch (field.op)
		{
		case '^':
		{
			return pow(left->computeTree(parameter), right->computeTree(parameter));
		}
		case '*':
		{
			return left->computeTree(parameter) * right->computeTree(parameter);
		}
		case '/':
		{
			return left->computeTree(parameter) / right->computeTree(parameter);
		}
		case '+':
		{
			return left->computeTree(parameter) + right->computeTree(parameter);
		}
		default:
		{
			return 0;
		}
		}
	}
	case options::function:
	{
		switch (resolveFunction(field.func))
		{
		case functions::invalid:
		{
			return 0;
		}
		case functions::abs:
		{
			return abs(right->computeTree(parameter));
		}
		case functions::sin:
		{
			return sin(right->computeTree(parameter));
		}
		case functions::cos:
		{
			return cos(right->computeTree(parameter));
		}
		case functions::tan:
		{
			return tan(right->computeTree(parameter));
		}
		case functions::sqrt:
		{
			return sqrt(right->computeTree(parameter));
		}
		case functions::log:
		{
			return log(right->computeTree(parameter));
		}
		case functions::exp:
		{
			return exp(right->computeTree(parameter));
		}
		case functions::ceil:
		{
			return ceil(right->computeTree(parameter));
		}
		case functions::floor:
		{
			return floor(right->computeTree(parameter));
		}
		case functions::trunc:
		{
			return trunc(right->computeTree(parameter));
		}
		case functions::round:
		{
			return round(right->computeTree(parameter));
		}
		case functions::cbrt:
		{
			return cbrt(right->computeTree(parameter));
		}
		case functions::asin:
		{
			return asin(right->computeTree(parameter));
		}
		case functions::acos:
		{
			return acos(right->computeTree(parameter));
		}
		case functions::atan:
		{
			return atan(right->computeTree(parameter));
		}
		case functions::sinh:
		{
			return sinh(right->computeTree(parameter));
		}
		case functions::cosh:
		{
			return cosh(right->computeTree(parameter));
		}
		case functions::tanh:
		{
			return tanh(right->computeTree(parameter));
		}
		case functions::asinh:
		{
			return asinh(right->computeTree(parameter));
		}
		case functions::acosh:
		{
			return acosh(right->computeTree(parameter));
		}
		case functions::atanh:
		{
			return atanh(right->computeTree(parameter));
		}
		case functions::erf:
		{
			return erf(right->computeTree(parameter));
		}
		case functions::erfc:
		{
			return erfc(right->computeTree(parameter));
		}
		case functions::tgamma:
		{
			return tgamma(right->computeTree(parameter));
		}
		}

	}
	default:
	{
		return 0;
	}
	}
}


