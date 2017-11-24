#include <iterator>
#include <cassert>
#include "pprojectnode.h"

PProjectNode::PProjectNode(PGetNextNode* child, LAbstractNode* p) : PGetNextNode(child, NULL, p){
	 pos = 0;
	 data.clear();
   Initialize();
}


PProjectNode::~PProjectNode() {
    delete left;
}

void PProjectNode::Initialize() {
    PGetNextNode* child = (PGetNextNode*) left;

    std::vector<std::vector<Value>> child_vals = child->GetNext();
    LAbstractNode* child_prot = child->prototype;

    for(auto &row :  child_vals) {
        std::vector<Value> put;
        for (auto &colnames : prototype->fieldNames) {
        	for (int j = 0; j < child_prot->fieldNames.size(); j++) {
        		if (colnames == child_prot->fieldNames[j]) {
        			put.push_back(row[j]);
        		}
        	}
        }
        assert (put.size() == GetAttrNum());
        data.push_back(put);
    }
}

std::vector<std::vector<Value> > PProjectNode::GetNext() {
		return data;
}


void PProjectNode::Print(int indent) {
    std::cout << std::string(indent, ' ') ;
    std::cout << "proj onto colums : " ;
    for (auto &col : (prototype)->fieldNames) {
    	std::cout << "[ ";
    	std::copy(col.begin(), col.end(), std::ostream_iterator<std::string>(std::cout, " | "));
    	std::cout << "] ";
    }
    std::cout << std::endl;
    left->Print(indent + 2);
}
