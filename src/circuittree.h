#ifndef CIRCUITTREE_H
#define CIRCUITTREE_H

#include <vector>
#include <string>
#include <list>

class CirElem{
public:
	enum CirElemType {NULL_Elem, R, C, Q, L, W, FLWs, FLWo, dL};
	CirElem(CirElemType ceType=NULL_Elem); //Constructor

	std::string Name;
    inline CirElemType Type() const{return elem_type_;}
	/*
    inline double getV  (int i) const {return ParValList[i];}
    inline double getdV (int i) const {return dV[i];}
    inline double getdV2(int i) const {return dV2[i];}
    inline void setV(int i, double V) {ParValList[i]=V;}
    inline void setdV(int i, double dV) {dV[i]=dV;}
    inline void setdV2(int i, double dV2) {dV[i]=dV2;}*/

private:
	CirElemType elem_type_;    
    std::vector <double> ParValList;
	std::vector <double> dV; //To store the first-order derivative
	std::vector <double> dV2;  //To store the second-order derivative;
    constexpr const static double DEFAULT_R = 100 ;
    constexpr const static double DEFAULT_C = 1E-4 ;
    constexpr const static double DEFAULT_Q = 1E-4 ;
    constexpr const static double DEFAULT_QN= 1;
    constexpr const static double DEFAULT_L = 1;
    constexpr const static double DEFAULT_W = 100;
    constexpr const static double DEFAULT_Wr= 10;
    constexpr const static double DEFAULT_Wt= 10;
    constexpr const static double DEFAULT_dLR1= 1000;
    constexpr const static double DEFAULT_dLR2= 1000;
    constexpr const static double DEFAULT_dLQ = 1e-4;
    constexpr const static double DEFAULT_dLQN= 1;
};

class CircuitNode{
public:
	enum ctNodeType {Serial,Parallel, Element,Blank

                     //Blank, Wire, UpLt, UpRt,LoLt, LoRt,ParaLt, ParaRt, RootLt, RootRt 
                    };
    CircuitNode (int ID=-1, ctNodeType NodeType=CircuitNode::Blank,
                 CircuitNode* Parent = NULL,
                 CirElem::CirElemType ElemType=CirElem::NULL_Elem): 
            Type (NodeType), parent(Parent),nodeElement(ElemType), nodeID (ID) {}
    ~CircuitNode(){}
    

    std::list<CircuitNode*>::iterator findChild (CircuitNode* Child);
    
    //Properties
	ctNodeType Type;
	std::list <CircuitNode*> childList;
	CircuitNode* parent;
    CirElem nodeElement; //Node must have "element" regardless of type
	unsigned nodeID;
};

class CircuitTree{
public:
	CircuitTree ();
    ~CircuitTree (){clear();}
    void changeElement (CircuitNode* Node, CirElem::CirElemType newElemType);
    CircuitNode* addSerial  (CircuitNode* Node, bool Before, CirElem::CirElemType Type);
    CircuitNode* addParallel(CircuitNode* Node, bool Before, CirElem::CirElemType Type);
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
    
    /*Connector nodes*/ 
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

#endif
