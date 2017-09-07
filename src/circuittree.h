#ifndef CIRCUITTREE_H
#define CIRCUITTREE_H
/*
#include <vector>
#include <string>
#include <list>

#include "circuitelement.h"


class CircuitNode{
public:
    
	enum ctNodeType {Serial,Parallel, Element,Blank

                     //Blank, Wire, UpLt, UpRt,LoLt, LoRt,ParaLt, ParaRt, RootLt, RootRt 
                    };
    CircuitNode (int ID=-1, ctNodeType NodeType=CircuitNode::Blank,
                 CircuitNode* Parent = NULL,
                 CircuitElement::CirElemType ElemType=CircuitElement::NULL_Elem): 
            Type (NodeType), parent(Parent),nodeElement(ElemType), nodeID (ID) {}
    ~CircuitNode(){}
    

    std::list<CircuitNode*>::iterator findChild (CircuitNode* Child);
    
    //Properties
	ctNodeType Type;
	std::list <CircuitNode*> childList;
	CircuitNode* parent;
    CircuitElement nodeElement; //Node must have "element" regardless of type
	unsigned nodeID;
};

class CircuitTree{
public:
	CircuitTree ();
    ~CircuitTree (){clear();}
    void changeElement (CircuitNode* Node, CircuitElement::CirElemType newElemType);
    CircuitNode* addSerial  (CircuitNode* Node, bool Before, CircuitElement::CirElemType Type);
    CircuitNode* addParallel(CircuitNode* Node, bool Before, CircuitElement::CirElemType Type);
    void removeNode(CircuitNode* Node, bool removeSingleBranch=1);

    void clear();
	std::list<CircuitNode*> elementList;
    CircuitNode* RootNode;
	unsigned countNodeID;
    enum graphBlock{Blank,};
};


class CircuitGraph{
public:
    CircuitGraph(CircuitTree *CirTree);


    
private:
    std::list<std::list<CircuitNode*> > NodeMap;
    
  
    CircuitTree* Tree;
    CircuitNode Blank;
    CircuitNode HWire;
    CircuitNode VWire;
    CircuitNode UpLt;
    CircuitNode UpRt;
    CircuitNode LoLt;
    CircuitNode LoRt;
    CircuitNode Begin;
    CircuitNode End;
};
*/
#endif
