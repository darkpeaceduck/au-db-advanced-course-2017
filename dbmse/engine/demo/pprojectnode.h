#ifndef DBMSE_ENGINE_DEMO_PPROJECTNODE_H_
#define DBMSE_ENGINE_DEMO_PPROJECTNODE_H_

#include "pgetnextnode.h"

class PProjectNode : public PGetNextNode{
  public:
    PProjectNode(PGetNextNode* child, LAbstractNode* p);
    ~PProjectNode();
    std::vector<std::vector<Value>> GetNext() override;
    void Initialize() override;
    void Print(int indent) override;
};

#endif
