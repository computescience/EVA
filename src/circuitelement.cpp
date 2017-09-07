#include "circuitelement.h"
/*

CircuitElement::CircuitElement(CirElemType ceType):
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

CircuitElement::CirElemType CircuitElement::Type() const{return elem_type_;}
*/
