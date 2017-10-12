#ifndef CIRCUITMODEL_H
#define CIRCUITMODEL_H

#include "circuitelement.h"
#include <QList>
#include <QImage>

class CircuitModel
{
public:
    CircuitModel();
        
    struct CircuitNode{
        
        enum NodeType {Element, SerialJoint, ParallelJoint};
        CircuitNode(const CircuitElement& Elem):Type(Element), elem(Elem), parent(NULL){}
        CircuitNode(NodeType T):Type(T), parent(NULL){} //For use as serial or parallel joint
        ~CircuitNode(){for (int i=0; i<child.size(); i++) delete child[i];}
        NodeType Type;
        CircuitElement elem;
        CircuitNode* parent;
        QList<CircuitNode*> child;
    };
    
    CircuitNode* addSerial(const CircuitElement& elem, 
                      CircuitNode* targetNode, 
                      bool insertBefore = 0);
    CircuitNode* addParallel (const CircuitElement& elem, 
                      CircuitNode* targetNode, 
                      bool insertBefore = 0);
    
    void removeNode (CircuitNode* targetNode);
    
    QString parseExpression(QString expression);
    QList <CircuitElement*> toRPN() const;
    QString toMathExpression (const CircuitNode* atNode = NULL,
                              bool includeElementID = 1) const;
    
    int elemCount () const {return elementList.size();}
    
    // Read-only and non-read-only
    const CircuitElement& accessElemRO(int n) const {return elementList.at(n)->elem;}
    CircuitElement& accessElem(int n) {return elementList[n]->elem;}
    
    /// For diagram
    struct DiagramBlock{
        enum BlockType {Null,Wire, Element, MidVertical,
                        TopBranchBeg, TopBranchEnd,
                        MidBranchBeg, MidBranchEnd,
                        BotBranchBeg, BotBranchEnd};
        BlockType block;
        CircuitNode* elem;
        DiagramBlock():                   block(Null),    elem(NULL)  {}
        DiagramBlock(CircuitNode* Elem) : block(Element), elem(Elem)  {}
        DiagramBlock(BlockType Con):      block (Con),    elem(NULL)  {}
    };
    
    QVector <QVector <DiagramBlock> > generateDiagram () const {
        return generateDiagramChunk(rootNode);
    }
    
    static void initializeElementLibrary();
    static const QImage& icon(QString IconName) {
        if (IconName.isEmpty()) return Icons.at(0);
        int index = IconNames.indexOf(IconName);
        return Icons.at (index==-1 ? 0 : index);
    }
    
    std::complex <double> evaluate (double freq);
private:
    
    QVector <QVector <DiagramBlock> > generateDiagramChunk(CircuitNode* Node) const;
    
    //CircuitElement parallel;
    //CircuitElement serial;
    CircuitNode* rootNode;
    QList <CircuitNode*> elementList; // Useful for locating a specific element node
    
    /// Static library of elements and icons
    static QVector <CircuitElement> BasicElems;
    static QVector <CircuitElement> CustomElems;
    
    static QVector <QString> ElementLibrary; // Collection of recognized element names
    static QVector <QString> IconNames;
    static QVector <QImage>  Icons;
    
    std::complex <double> evaluateNode (double freq, CircuitNode* node);
};

#endif // CIRCUITMODEL_H
