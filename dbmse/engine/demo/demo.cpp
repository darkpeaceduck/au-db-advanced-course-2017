// Toy DBMS, v0.4
// George Chernishev (c) 2016-2017, chernishev<at>google mail
// A task framework for undergraduate students at Saint-Petersburg Academic University, DBMS development course
// More details regarding the course can be found here: www.math.spbu.ru/user/chernishev/
// CHANGELOG:
// 0.4: no chance for efficiency competition, so, this year I reoriented task towards ideas:
//      1) tried to remove C code, now we are using lots of std:: stuff
//      2) implemented support for multiple attributes in the DBMS
//      3) code clean-up and restructurization
// 0.3: added:
//      1) support for restricting physical join node size
//      2) support for deduplication node, LUniqueNode
//      3) print methods for Predicate and BaseTable
//      updated:
//      1) new format for data files: third line is the sort status now
//      2) added projection code
//      3) contract contains print methods for physical and logical nodes
// 0.2: first public release

#include <stdio.h>
#include <typeinfo>
#include <iostream>
#include <vector>
#include <memory>
#include <tuple>
#include <cassert>
#include "../interface/interface.h"
#include "../interface/basics.h"
#include "pselectnode.h"
#include "pjoinnode.h"
#include "pprojectnode.h"
#include "pcrossproductnode.h"
#include "puniquenode.h"

PResultNode* QueryFactory(LAbstractNode* node);

static PGetNextNode* QueryFactoryIterible(LAbstractNode* node) {
	return dynamic_cast<PGetNextNode*> (QueryFactory(node));
}

static PSelectNode * buildSelect(LSelectNode* node) {
	if (node == NULL) return nullptr;
	return new PSelectNode(node, node->GetAllPredicates());
}

static PJoinNode * buildJoin(LJoinNode* node) {
	if (node == nullptr) return nullptr;
	PGetNextNode * l = QueryFactoryIterible(node->GetLeft());
	PGetNextNode * r = QueryFactoryIterible(node->GetRight());
	return new PJoinNode(l, r, node);
}

static PCrossProductNode * buildCrossProduct(LCrossProductNode* node) {
	if (node == nullptr) return nullptr;
	PGetNextNode * l = QueryFactoryIterible(node->GetLeft());
	PGetNextNode * r = QueryFactoryIterible(node->GetRight());
	return new PCrossProductNode(l, r, node);
}

static PProjectNode * buildProject(LProjectNode * node) {
	if (node == nullptr) return nullptr;
	PGetNextNode * child = QueryFactoryIterible(node->GetLeft());
	return new PProjectNode(child, node);
}

static PUniqueNode* buildUnique(LUniqueNode * node) {
	if (node == nullptr) return nullptr;
	PGetNextNode * child = QueryFactoryIterible(node->GetLeft());
	return new PUniqueNode(child, node);
}


#define GO(f, arg)\
	do { \
	  PResultNode* res = f (arg); \
		if (res != nullptr)  {\
			return res;\
		}\
	} while (0);

// Here be rewriter and optimizer
PResultNode* QueryFactory(LAbstractNode* node){
  GO (buildSelect, dynamic_cast<LSelectNode*>(node));
  GO (buildJoin, dynamic_cast<LJoinNode*>(node));
  GO (buildProject, dynamic_cast<LProjectNode*>(node));
  GO (buildCrossProduct, dynamic_cast<LCrossProductNode*>(node));
  GO (buildUnique, dynamic_cast<LUniqueNode*>(node));

  return nullptr;
}

#undef GO

void ExecuteQuery(PResultNode* query){
  std::tuple<ErrCode, std::vector<Value>> res;
  res = query->GetRecord();
  ErrCode ec = std::get<0>(res);
  std::vector<Value> vals = std::get<1>(res);
  while(ec == EC_OK){
    for (int i = 0; i < query->GetAttrNum(); i++){
      if(vals[i].vtype == VT_INT)
        std::cout << vals[i].vint << " ";
      else if(vals[i].vtype == VT_STRING)
        std::cout << vals[i].vstr << " ";
    }
    printf("\n");
    res = query->GetRecord();
    ec = std::get<0>(res);
    vals = std::get<1>(res);
  }
}

class mytest {
public:
	virtual LAbstractNode* buildLTree() = 0;
	virtual std::string testName() = 0;
	void run () {
		std::string name = testName();

		std::cout << std::endl << "[TEST START] << " << name << std::endl;
		std::cout << "build ltree... ";
		LAbstractNode* ltree = buildLTree();
		assert(ltree != nullptr);
		std::cout << "finished" << std::endl;
		std::cout << "build ptree... ";
		PResultNode* ptree = QueryFactory(ltree);
		std::cout << "finished" << std::endl;
		assert(ptree != nullptr);
		std::cout << std::endl; ptree->Print(0); std::cout << std::endl << "=====================" << std::endl;
		std::cout << std::endl; ExecuteQuery(ptree); std::cout << std::endl;

		std::cout << std::endl << "[TEST END] " << std::endl;

		delete ltree;
		delete ptree;
	}
};

#define T(test) std::make_shared<test>()

class testSelest1 : public mytest {
public:
	LAbstractNode* buildLTree() override {
		BaseTable bt1 = BaseTable("table1");
		Predicate p1 (PT_GREATERTHAN, VT_INT, 0, 1, "");
		Predicate p2 (PT_GREATERTHAN, VT_STRING, 1, -1, "czzzzzzzzzzzzz");
		return new LSelectNode (bt1, {p1, p2});
	}
	std::string testName() override {
		return "select1 ";
	}
};

class testSelest2 : public mytest {
public:
	LAbstractNode* buildLTree() override {
		BaseTable bt1 = BaseTable("table1");
		Predicate p1 (PT_EQUALS, VT_INT, 3, 2, "");
		Predicate p2 (PT_EQUALS, VT_STRING, 1, -1, "dos");
		return new LSelectNode (bt1, {p1, p2});
	}
	std::string testName() override {
		return "select2 ";
	}
};

class testSelest3 : public mytest {
public:
	LAbstractNode* buildLTree() override {
		BaseTable bt2 = BaseTable("table2");
		return new LSelectNode (bt2, {});
	}
	std::string testName() override {
		return "select2 ";
	}
};

class testProj : public mytest {
public:
	LAbstractNode* buildLTree() override {
		LAbstractNode* tree = T(testSelest1)->buildLTree();
		return new LProjectNode  (tree, {"table1.id", "table1.frequency", "table1.description"});
	}
	std::string testName() override {
		return "proj ";
	}
};


class testJoin : public mytest {
public:
	LAbstractNode* buildLTree() override {
		LAbstractNode* ltree = T(testProj)->buildLTree();
		return new LJoinNode (ltree, T(testSelest3)->buildLTree(), "table1.id", "table2.id2", 666);
	}
	std::string testName() override {
		return "join ";
	}
};

class testProj2 : public mytest {
public:
	LAbstractNode* buildLTree() override {
		LAbstractNode* ltree = T(testJoin)->buildLTree();
		return new LProjectNode  (ltree, {"table2.id2", "table1.description", "table2.type2"});
	}
	std::string testName() override {
		return "proj2 ";
	}
};

class testCross : public mytest {
public:
	LAbstractNode* buildLTree() override {
		LAbstractNode* ltree = T(testSelest1)->buildLTree();
		LAbstractNode* rtree = T(testSelest3)->buildLTree();
		LCrossProductNode * cross1 = new LCrossProductNode(ltree, rtree);
		return cross1;
	}
	std::string testName() override {
		return "cross ";
	}
};

class testUnique : public mytest {
public:
	LAbstractNode* buildLTree() override {
		LAbstractNode* ltree = T(testCross)->buildLTree();
		LProjectNode *proj = new LProjectNode(ltree, {"table1.id" , "table1.description"});
		return new LUniqueNode(proj);
	}
	std::string testName() override {
		return "unique ";
	}
};




int main(){

#define TT(test, enable) std::make_pair(T(test), enable)


	std::vector<std::pair<std::shared_ptr<mytest>, bool> > tests {
		TT(testSelest1, 1),
		TT(testSelest2, 1),
		TT(testSelest3, 1),
		TT(testProj, 1),
		TT(testJoin, 1),
		TT(testProj2, 1),
		TT(testCross, 1),
		TT(testUnique, 1)
	};
	for (auto &test :tests) {
		if (test.second)
			test.first->run();
	}
}
