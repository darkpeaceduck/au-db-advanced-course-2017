#ifndef DBMSE_ENGINE_DEMO_PCROSSPRODUCTNODE_H_
#define DBMSE_ENGINE_DEMO_PCROSSPRODUCTNODE_H_

#include <vector>

#include "../interface/basics.h"
#include "pgetnextnode.h"

class PCrossProductNode : public PGetNextNode{
  public:
		PCrossProductNode(PGetNextNode* left, PGetNextNode* right, LAbstractNode* p);
    ~PCrossProductNode();
    std::vector<std::vector<Value>> GetNext() override;
    void Initialize() override;
    void Print(int indent) override;
};


#endif
