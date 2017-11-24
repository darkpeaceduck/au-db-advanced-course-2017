#include <set>
#include <cassert>
#include "puniquenode.h"

PUniqueNode::PUniqueNode(PGetNextNode * child, LAbstractNode* p) : PGetNextNode(child, NULL, p) {
	pos = 0;
	Initialize();
}

PUniqueNode::~PUniqueNode() {
	delete left;
}

std::vector<std::vector<Value>> PUniqueNode::GetNext() {
	return data;
}

struct rex_compare {
    bool operator() (const std::vector<Value>& l, const std::vector<Value>& r) const {
    	assert (l.size() == r.size());
    	for (size_t i = 0; i < l.size(); i++) {
    		if (l[i] > r[i])
    			return true;
    		if (r[i] > l[i])
    			return false;
    	}
    	return false;
    }
};

void PUniqueNode::Initialize() {
	std::set<std::vector<Value>, rex_compare> has;
	auto table = ((PGetNextNode*) left)->GetNext();
	for (auto row : table) {
		if (!has.count(row)) {
			data.push_back(row);
			assert(row.size() == GetAttrNum());
			has.insert(row);
		}
	}
}



void PUniqueNode::Print(int indent) {
	std::cout << std::string(indent, ' ') ;
	std::cout << "Unique " << std::endl;
	left->Print(indent + 2);
}



