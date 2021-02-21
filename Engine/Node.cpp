
#include "Node.h"


unsigned int node::findPair(std::string str, unsigned int pos, bool open)
{
	unsigned int count = 0;
	if (open)
	{
		for (unsigned int i = pos; i < str.length(); i++)
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
	}
	else
	{
		for (unsigned int i = pos; i > 0; i--)
		{
			switch (str[i])
			{
			case '(':
			{
				count--;
				break;
			}
			case ')':
			{
				count++;
				break;
			}

			default:
				break;
			}
			if (count == 0)
				return i;
		}
	}
	return 0;
}

void node::putParenth(std::string& str, unsigned int opnPos, unsigned int clsPos)
{
	if (opnPos < clsPos)
	{
		str.insert(opnPos, 1, '(');
		str.insert(clsPos + 1, 1, ')');
	}
}

void node::processExpr(std::string& expr)
{
	/*
	KNOWN PROBLEMS / TODO (*,**,***, difficulty to fix),(+,++,+++, necessity of feature):


	(***),(+++) DOES NOT PROPERLY CHECK FOR SYNTAX ERRORS
	(*),  (++)  DOES NOT REMOVE UNNECESSARY BRACKETS ( ((x)) )
	(***),(+)   DOES NOT SUPPORT IMPLICIT FUNCTION CHAINING (sincosx - >sin(cos(x)))
	*/

	unsigned int Pos = 0;

	Pos = expr.find(' ');  //removing spaces
	while (Pos != std::string::npos)
	{
		expr.erase(Pos, 1);
		Pos = expr.find(' ');
	}
	Pos = expr.find('-');  //expanding binary '-'

	while (Pos != std::string::npos)
	{
		if (Pos > 0 && std::string("0123456789x)").find(expr[Pos - 1]) != std::string::npos)
		{
			expr.insert(Pos, 1, '+');
			Pos++;
		}
		Pos = expr.find('-', ++Pos);
	}

	Pos = expr.find_first_not_of("-()^+*/.0123456789x");  //enclosing function arguments

	while (Pos != std::string::npos)
	{
		unsigned int argStart = 0;
		unsigned int argEnd = 0;

		argStart = expr.find_first_of("(-0123456789x", Pos + 1);
		if (expr[argStart] == '(')
		{
			argEnd = findPair(expr, argStart, true);
		}
		else
		{
			argEnd = expr.find_first_of("(^*/+x", argStart);
			putParenth(expr, argStart, argEnd);
			argEnd++;

		}
		if (Pos == 0 || expr[Pos - 1] != '(' || expr[argEnd + 1] != ')')
		{
			putParenth(expr, Pos, argEnd + 1);
			argStart++;
		}

		Pos = expr.find_first_not_of("-()^+*/.0123456789x", argStart);

	}

	Pos = expr.find('x');  //enclosing variables

	while (Pos != std::string::npos)
	{
		if (Pos == expr.length() - 1 || Pos == 0 || expr[Pos - 1] != '(' || expr[Pos + 1] != ')')
		{
			putParenth(expr, Pos, Pos + 1);
			Pos++;
		}
		Pos = expr.find('x', ++Pos);
	}

	Pos = expr.find_first_of("0123456789");  //enclosing constants

	while (Pos != std::string::npos)
	{
		unsigned int nrEnd = expr.find_first_of("^*/+()", Pos);
		if (nrEnd == std::string::npos)
		{
			nrEnd = expr.length();
		}
		if (nrEnd == expr.length() || Pos == 0 || expr[Pos - 1] != '(' || expr[nrEnd] != ')')
		{
			putParenth(expr, Pos, nrEnd);
			nrEnd++;
		}
		Pos = expr.find_first_of("0123456789", ++nrEnd);
	}

	Pos = expr.rfind('^');  //enclosing '^'

	while (Pos != std::string::npos)
	{
		unsigned int leftBound = findPair(expr, Pos - 1, false);
		unsigned int rightOpen = expr.find('(', Pos);
		unsigned int rightBound = findPair(expr, rightOpen, true);
		if (leftBound == 0 || rightBound == expr.length() - 1 || expr[leftBound - 1] != '(' || expr[rightBound + 1] != ')')
		{
			putParenth(expr, leftBound, rightBound + 1);
		}
		Pos = expr.rfind('^', leftBound);
	}

	Pos = expr.find('-');  // enclosing '-'
	while (Pos != std::string::npos)
	{
		unsigned int rightBound = findPair(expr, Pos + 1, true);
		if (Pos == 0 || rightBound == expr.length() - 1 || expr[Pos - 1] != '(' || expr[rightBound + 1] != ')')
		{
			putParenth(expr, Pos, rightBound + 1);
			rightBound++;
		}
		Pos = expr.find('-', ++rightBound);
	}

	Pos = expr.find(')');    //expanding implicit multiplication

	while (Pos != std::string::npos)
	{
		if (expr[Pos + 1] == '(')
		{
			expr.insert(++Pos, 1, '*');
		}
		Pos = expr.find(')', ++Pos);
	}

	/*Enclosing operators*/


	Pos = expr.find_first_of("*/");  //enclosing '*' and '/'

	while (Pos != std::string::npos)
	{
		unsigned int leftBound = findPair(expr, Pos - 1, false);
		unsigned int rightBound = findPair(expr, Pos + 1, true);
		if (leftBound == 0 || rightBound == expr.length() - 1 || expr[leftBound - 1] != '(' || expr[rightBound + 1] != ')')
		{

			putParenth(expr, leftBound, rightBound + 1);
			rightBound++;
		}
		Pos = expr.find_first_of("*/", ++rightBound);
	}

	Pos = expr.find('+');  //enclosing '+'

	while (Pos != std::string::npos)
	{
		unsigned int leftBound = findPair(expr, Pos - 1, false);
		unsigned int rightBound = findPair(expr, Pos + 1, true);
		if (leftBound == 0 || rightBound == expr.length() - 1 || expr[leftBound - 1] != '(' || expr[rightBound + 1] != ')')
		{
			putParenth(expr, leftBound, rightBound + 1);
			rightBound++;
		}
		Pos = expr.find('+', ++rightBound);
	}

	/*erasing bounding parentheses*/
	expr.erase(0, 1);
	expr.erase(expr.length() - 1, 1);

};

node::functions node::resolveSymbol(std::string fnc)
{
	if (fnc == "-")
		return functions::minus;
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

void node::extract(std::string expr)
{
	unsigned int Pos = expr.find("(");
	if (Pos != std::string::npos)
	{
		unsigned int closed = findPair(expr, Pos, 1);
		if (closed < expr.length() - 1)
		{
			type = options::oprator;
			field.op = expr[closed + 1];

			std::string leftexpr = expr.substr(Pos + 1, closed - Pos - 1);

			Pos = closed + 2;
			closed = findPair(expr, Pos, 1);
			std::string rightexpr = expr.substr(Pos + 1, closed - Pos - 1);

			left = new node;
			left->extract(leftexpr);
			right = new node;
			right->extract(rightexpr);

		}
		else
		{
			type = options::function;
			field.func = resolveSymbol(expr.substr(0, Pos));

			std::string rightexpr = expr.substr(Pos + 1, closed - Pos - 1);
			right = new node;
			right->extract(rightexpr);

		}
	}
	else
	{
		if (expr == "x")
		{
			type = options::parameter;
		}
		else
		{
			type = options::constant;
			field.nr = stof(expr);
		}
	}
}

node::~node()
{
	delete left;
	left = nullptr;
	delete right;
	right = nullptr;
}

node::node()
{
	type = options::constant;
	field.nr = 0;
	left = nullptr;
	right = nullptr;
}

node::node(std::string expr)
{
	processExpr(expr);
	this->extract(expr);
	this->simplifyTree();
}

node::node(const node& replacement)
{
	delete left;
	left = nullptr;
	delete right;
	right = nullptr;
	type = replacement.type;
	field = replacement.field;
	if (replacement.type == options::oprator)
	{
		left = new node;
		*left = *replacement.left;
		right = new node;
		*right = *replacement.right;
	}
	else if (replacement.type == options::function)
	{
		right = new node;
		*right = *replacement.right;
	}
}

void node::simplifyTree()
{
	if (type == options::oprator)
	{
		left->simplifyTree();
		right->simplifyTree();
		if (left->type == options::constant && right->type == options::constant)
		{
			field.nr = this->computeTree(0);
			type = options::constant;
			delete left;
			left = nullptr;
			delete right;
			right = nullptr;
		}
	}
	else
	{
		if (type == options::function)
		{
			right->simplifyTree();
			if (right->type == options::constant)
			{
				field.nr = this->computeTree(0);
				type = options::constant;
				delete right;
				right = nullptr;
			}
		}
	}
}

void node::operator=(std::string expr)
{
	processExpr(expr);
	delete left;
	left = nullptr;
	delete right;
	right = nullptr;
	this->extract(expr);
	this->simplifyTree();
}

void node::operator=(const node& replacement)
{
	delete left;
	left = nullptr;
	delete right;
	right = nullptr;
	type = replacement.type;
	field = replacement.field;
	if (replacement.type == options::oprator)
	{
		left = new node;
		*left = *replacement.left;
		right = new node;
		*right = *replacement.right;
	}
	else if (replacement.type == options::function)
	{
		right = new node;
		*right = *replacement.right;
	}
}

const node node::operator+(const node& rightSide)
{
	node newNode;
	if (this->type == options::parameter)
	{
		newNode = rightSide;
	}
	else
	{
		newNode.type = this->type;
		newNode.field = this->field;
		if (this->type == options::oprator)
		{
			newNode.left = new node;
			*newNode.left = *this->left + rightSide;
			newNode.right = new node;
			*newNode.right = *this->right + rightSide;
		}
		else if (this->type == options::function)
		{
			newNode.right = new node;
			*newNode.right = *this->right + rightSide;
		}
	}
	return newNode;
	
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
		switch (field.func)
		{
		case functions::invalid:
		{
			return 0;
		}
		case functions::minus:
		{
			return -1 * right->computeTree(parameter);
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
