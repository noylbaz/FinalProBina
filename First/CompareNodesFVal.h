#pragma once
#include "Node.h"
class CompareNodesFVal
{
public:
	CompareNodesFVal();
	~CompareNodesFVal();
	bool operator () (Node* pn1, Node* pn2)
	{
		return pn1->getFVal() > pn2->getFVal();
	}
};

