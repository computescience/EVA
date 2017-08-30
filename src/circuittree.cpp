#include "circuittree.h"
#include <cstdio>

typedef std::list<CircuitNode*>::iterator listIndex;
CirElem::CirElem(CirElemType ceType):
elem_type_ (ceType)
{
    int parSize;
    
    switch (ceType){
    case NULL_Elem: 
        parSize=0; break;
    case R: case C: case L: case W:
        parSize=1; break;
    case Q: case FLWs: case FLWo:
        parSize=2; break;
    case dL:
        parSize=4;break;
    }
    
    ParValList.resize(parSize);
    dV.resize(parSize);
    dV2.resize(parSize);
    
    switch (ceType) { 
    //Initialize the parameters for different types of circuit lements
    case NULL_Elem: break;
    case R:
        ParValList[0] = DEFAULT_R; break;
    case C:
        ParValList[0] = DEFAULT_C; break;
    case L:
        ParValList[0] = DEFAULT_L; break;
    case W:
        ParValList[0] = DEFAULT_W; break;
    case Q:
        ParValList[0] = DEFAULT_Q; break;
        ParValList[1] = DEFAULT_QN; break;
    case FLWs: case FLWo:
        ParValList[0] = DEFAULT_Wr; break;
        ParValList[1] = DEFAULT_Wt; break;
    case dL:
        ParValList[0] = DEFAULT_dLR1; break;
        ParValList[1] = DEFAULT_dLR2; break;
        ParValList[2] = DEFAULT_dLQ;  break;
        ParValList[3] = DEFAULT_dLQN; break;
    }
}

CircuitTree::CircuitTree(){
	countNodeID = 0;
}

void CircuitTree::changeElement(CircuitNode *Node, CirElem::CirElemType newElemType)
{
    if (Node==NULL || Node->Type!=CircuitNode::Element) return;
    Node->nodeElement = CirElem(newElemType);
}

void CircuitTree::clear(){
    removeNode (RootNode);
}

listIndex CircuitNode::findChild(CircuitNode *Child){
    listIndex Target;
    for (Target=childList.begin(); Target!=childList.end(); Target++){
        if (*Target == Child) return Target;
    }
    return Target;
}


CircuitNode* CircuitTree::addSerial(CircuitNode *Node, 
                                   bool Before,
                                   CirElem::CirElemType Type){

    CircuitNode* newNode = new CircuitNode(countNodeID++, CircuitNode::Element,NULL, Type);
    elementList.push_back(newNode);
    
    if (Node==NULL) {
        RootNode = newNode;
        return newNode;
    }
    
    
    /** Checklist: 
     *  1. Create parallel / serial nodes as necessary
     *  2. Fix parent's children list
     *  3. Fix Node's dependence
     *  4. Fix newNode's dependence **/
    switch (Node->Type) {
    default: break;
    case CircuitNode::Serial:
        Node->childList.push_back(Node); //2,3
        if (Before) Node->childList.push_front(newNode); //2,3
        else Node->childList.push_back(newNode);
        newNode->parent = Node;  //4
        break;
    case CircuitNode::Parallel: 
    case CircuitNode::Element: 
        if (Node->parent == NULL){ //Node is a top-level node
            CircuitNode* newSerial = new CircuitNode(countNodeID++, CircuitNode::Serial,NULL);//1
            RootNode = newSerial;           
            newSerial->childList.push_back(Node); //2
            if (Before) newSerial->childList.push_front(newNode);//2
            else newSerial->childList.push_back(newNode);
            Node->parent = newSerial; //3
            newNode->parent = newSerial; //4
        }
        
        else if (Node->parent->Type == CircuitNode::Serial){
            //No need to create additional Serial, just add a branch
            listIndex NodeIndex = Node->parent->findChild(Node); 
            if (!Before) NodeIndex++; //2
            Node->parent->childList.insert(NodeIndex,newNode);//3
            newNode->parent = Node->parent;//4
        }
        
        else { // Node->parent is a parallel node
            CircuitNode* newSerial = new CircuitNode(countNodeID++, CircuitNode::Serial,Node->parent);//1
            *(Node->parent->findChild(Node)) = newSerial; //2
            newSerial->childList.push_back(Node);
            if (Before) newSerial->childList.push_front(newNode);
            else newSerial->childList.push_back(newNode);
            Node->parent = newSerial; //3
            newNode->parent = newSerial; //4
        }      
        break;
    }   
	return newNode;
}

CircuitNode* CircuitTree::addParallel(CircuitNode *Node, 
                                      bool Before, 
                                      CirElem::CirElemType Type){

    CircuitNode* newNode = new CircuitNode(countNodeID++, CircuitNode::Element,NULL, Type);
    elementList.push_back(newNode);
    
    if (Node==NULL) {
        RootNode = newNode;
        return newNode;
    }
    
    
    /** Checklist: 
     *  1. Create parallel / serial nodes as necessary
     *  2. Fix parent's children list
     *  3. Fix Node's dependence
     *  4. Fix newNode's dependence **/
    switch (Node->Type) {
    default: break;
    case CircuitNode::Parallel:
        Node->childList.push_back(Node); //2,3
        if (Before) Node->childList.push_front(newNode); //2,3
        else Node->childList.push_back(newNode);
        newNode->parent = Node;  //4
        break;
    case CircuitNode::Serial: 
    case CircuitNode::Element: 
        if (Node->parent == NULL){ //Node is a top-level node
            CircuitNode* newParallel = new CircuitNode(countNodeID++, CircuitNode::Parallel,NULL);//1
            RootNode = newParallel;           
            newParallel->childList.push_back(Node); //2
            if (Before) newParallel->childList.push_front(newNode);//2
            else newParallel->childList.push_back(newNode);
            Node->parent = newParallel; //3
            newNode->parent = newParallel; //4
        }
        
        else if (Node->parent->Type == CircuitNode::Parallel){
            //No need to create additional Serial, just add a branch
            listIndex NodeIndex = Node->parent->findChild(Node); 
            if (!Before) NodeIndex++; //2
            Node->parent->childList.insert(NodeIndex,newNode);//3
            newNode->parent = Node->parent;//4
        }
        
        else { // Node->parent is a parallel node
            CircuitNode* newParallel = new CircuitNode(countNodeID++, CircuitNode::Parallel,Node->parent);//1
            *(Node->parent->findChild(Node)) = newParallel; //2
            newParallel->childList.push_back(Node);
            if (Before) newParallel->childList.push_front(newNode);
            else newParallel->childList.push_back(newNode);
            Node->parent = newParallel; //3
            newNode->parent = newParallel; //4
        }      
        break;
    }   
	return newNode;
}

void CircuitTree::removeNode(CircuitNode *Node, bool removeSingleBranch){
    if (Node->Type==CircuitNode::Element){
        listIndex iElemList;
        for (iElemList=elementList.begin();
             iElemList!=elementList.end(); iElemList++){
            if (*iElemList == Node) {
                elementList.erase(iElemList); //de-register from elemList
                break;
            }
        }
        if (iElemList == elementList.end()) return;
    }
    else { //Node type is a Serial or Parallel
        while (!Node->childList.empty()) removeNode (Node->childList.front(),0);
    }
    
    if (Node->parent!=NULL){ //Non-root node
        listIndex whichChild = Node->parent->findChild(Node);
        Node->parent->childList.erase(whichChild);
    }
    else { //Top level node
        RootNode=NULL;
    }
	
	if (removeSingleBranch){//Remove the Serial/Parallel with only 1 branch
		if (Node->parent!=NULL && Node->parent->childList.size()==1){
			CircuitNode* grandParent = Node->parent->parent;
            CircuitNode* sibling = Node->parent->childList.front();
			delete Node->parent;
			sibling->parent = grandParent;
            
			if (grandParent!=NULL) {
				listIndex whichChild = grandParent->findChild(Node->parent);
				*whichChild = sibling;
			}
            else {
                RootNode=sibling;
            }
		}
	}
}

CircuitGraph::CircuitGraph(CircuitTree *CirTree):
    Tree (CirTree)
{
    NodeMap.push_back(std::list<CircuitNode*> (1,&Begin));
    NodeMap.front().push_back (&End);
    
}






