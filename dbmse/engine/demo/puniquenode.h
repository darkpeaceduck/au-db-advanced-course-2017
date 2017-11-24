#ifndef DBMSE_ENGINE_DEMO_PUNIQUENODE_H_
#define DBMSE_ENGINE_DEMO_PUNIQUENODE_H_

#include <vector>
#include "../interface/interface.h"
#include "pgetnextnode.h"


class PUniqueNode : public PGetNextNode {
  public:
		PUniqueNode(PGetNextNode * child, LAbstractNode* p);
    ~PUniqueNode();
    std::vector<std::vector<Value>> GetNext() override;
    void Initialize() override;
    void Print(int indent) override;
};

#endif
