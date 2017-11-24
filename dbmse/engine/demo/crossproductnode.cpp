#include "../interface/interface.h"
#include "pcrossproductnode.h"
#include <cassert>

#define MERGE_VEC(A, B) (A).insert(std::end(A), std::begin(B), std::end(B))
#define MERGE(VEC)\
	do {\
		MERGE_VEC(VEC, left->VEC);\
		MERGE_VEC(VEC, right->VEC);\
	} while(0);


LCrossProductNode::LCrossProductNode(LAbstractNode* left,
        LAbstractNode* right) : LAbstractNode(left, right) {

	MERGE(fieldNames);
	MERGE(fieldTypes);
	MERGE(fieldOrders);
}

#undef MERGE

LCrossProductNode::~LCrossProductNode() {
	delete left;
	delete rigth;
}

PCrossProductNode::PCrossProductNode(PGetNextNode* left,
		PGetNextNode* right, LAbstractNode* p) : PGetNextNode(left, right, p) {
	pos = 0;
	Initialize();
}

PCrossProductNode::~PCrossProductNode() {
	delete left;
	delete right;
}

void PCrossProductNode::Initialize() {

	auto lres = ((PGetNextNode*)left) ->GetNext();
	auto rres = ((PGetNextNode*)right) ->GetNext();

	for (auto &lrow : lres) {
		for (auto &rrow : rres) {
			data.push_back(std::vector<Value>());
			MERGE_VEC(data.back(), lrow);
			MERGE_VEC(data.back(), rrow);
			assert(data.back().size() == GetAttrNum());
		}
	}
}

std::vector<std::vector<Value> > PCrossProductNode::GetNext() {
	return data;
}

void PCrossProductNode::Print(int indent) {
	for (int i = 0; i < indent; i++){
	    std::cout << " ";
	}
	std::cout << "CROSSPRODUCT: fields num = " << GetAttrNum() << std::endl;
	left->Print(indent + 2);
	right->Print(indent + 2);
}
