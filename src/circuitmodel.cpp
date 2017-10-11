#include "circuitmodel.h"
#include <QDir>

#include <QMessageBox>

QString popOperator(QVector<QString>& operStack,
                    CircuitModel::CircuitNode& outputManager){
    
    if (outputManager.child.size()<2) {
        return QString ("Error: syntax error - insufficient operand");
    }
    
    CircuitModel::CircuitNode* Node1 = outputManager.child.at(outputManager.child.size()-2);
    CircuitModel::CircuitNode* Node2 = outputManager.child.back();
    
    CircuitModel::CircuitNode::NodeType OperatorType = 
            operStack.back()==QString("+")? CircuitModel::CircuitNode::SerialJoint
                                          : CircuitModel::CircuitNode::ParallelJoint;
            

    if (Node1->Type==OperatorType && Node2->Type==OperatorType){
        // Transfer all children under Node 2 to Node 1
        for (int i=0; i<Node2->child.size();i++){ 
            Node1->child.push_back(Node2->child.at(i));
            Node2->child.at(i)->parent=Node1;
        }
        Node2->child.clear();
        
        outputManager.child.pop_back(); // remove Node2 from manager's list
        delete Node2;
    }
    else if (Node1->Type==OperatorType){ 
        // Transfer Node2 to child of Node1
        Node1->child.push_back(Node2);
        Node2->parent = Node1;
        outputManager.child.pop_back();
    } else if (Node2->Type==OperatorType){
        // Transfer Node1 to child of Node2
        Node2->child.push_front(Node1);
        Node1->parent = Node2;
        outputManager.child.removeOne(Node1);
    } else {
        CircuitModel::CircuitNode* newNode = new CircuitModel::CircuitNode (OperatorType);
        newNode->child.push_back(Node1);
        newNode->child.push_back(Node2);
        Node1->parent = newNode;
        Node2->parent = newNode;
        newNode->parent = &outputManager;
        outputManager.child.pop_back();
        outputManager.child.pop_back();
        outputManager.child.push_back(newNode);
    }
    
    operStack.pop_back();
    return QString();
}

CircuitModel::CircuitModel():
    rootNode(NULL)
{
    
}

CircuitModel::CircuitNode* CircuitModel::addSerial(const CircuitElement &elem, 
                                 CircuitModel::CircuitNode *targetNode, 
                                 bool insertBefore)
{
    /// Checklist: 
    /// 1. Create parallel / serial nodes as necessary
    /// 2. Fix parent's children list
    /// 3. Fix targetNode's parent
    /// 4. Fix newNode's parent
    
    CircuitNode* newElemNode = new CircuitNode(elem);
    
    if (targetNode == NULL){
        rootNode = newElemNode;
        return newElemNode;
    }

    if (targetNode->Type == CircuitNode::Element){
        if (targetNode->parent == NULL) { // Top level
            CircuitNode* newSerial = new CircuitNode(CircuitNode::SerialJoint);// 1
            rootNode = newSerial;
            newSerial->child.push_back(targetNode); //2
            if (insertBefore) newSerial -> child.push_front(newElemNode);
            else newSerial -> child.push_back(newElemNode); //2
            targetNode->parent = newSerial; //3
            newElemNode->parent = newSerial; //4
        }
        else if (targetNode->parent->Type == CircuitNode::SerialJoint){
            int indexOfTargetNode = targetNode->parent->child.indexOf(targetNode);
            targetNode->parent->child.insert(
                        insertBefore ? indexOfTargetNode : indexOfTargetNode+1,
                        newElemNode); // 2
            newElemNode->parent = targetNode->parent; //4
        }
        else if (targetNode->parent->Type == CircuitNode::ParallelJoint){
            CircuitNode* newSerial = new CircuitNode(CircuitNode::SerialJoint);
            newSerial->parent = targetNode->parent;
            newSerial->child.push_back(targetNode);
            if (insertBefore) newSerial->child.push_front(newElemNode);
            else newSerial->child.push_back(newElemNode); // 1
            int indexOfTargetNode = targetNode->parent->child.indexOf(targetNode);
            targetNode->parent->child [indexOfTargetNode] = newSerial; //2
            targetNode->parent = newSerial;  // 3
            newElemNode->parent = newSerial; // 4
        }
    }       
    elementList.push_back(newElemNode);
    return newElemNode;
}

CircuitModel::CircuitNode *CircuitModel::addParallel(const CircuitElement &elem, CircuitModel::CircuitNode *targetNode, bool insertBefore)
{
    /// Checklist: 
    /// 1. Create parallel / serial nodes as necessary
    /// 2. Fix parent's children list
    /// 3. Fix targetNode's parent
    /// 4. Fix newNode's parent
    
    CircuitNode* newElemNode = new CircuitNode (elem);
    
    if (targetNode == NULL){
        rootNode = newElemNode;
        return newElemNode;
    }

    if (targetNode->Type == CircuitNode::Element){
        if (targetNode->parent == NULL) { // Top level
            CircuitNode* newParallel = new CircuitNode(CircuitNode::ParallelJoint);// 1
            rootNode = newParallel;
            newParallel->child.push_back(targetNode); //2
            if (insertBefore) newParallel -> child.push_front(newElemNode);
            else newParallel -> child.push_back(newElemNode); //2
            targetNode->parent = newParallel; //3
            newElemNode->parent = newParallel; //4
        }
        else if (targetNode->parent->Type == CircuitNode::ParallelJoint){
            int indexOfTargetNode = targetNode->parent->child.indexOf(targetNode);
            targetNode->parent->child.insert(
                        insertBefore ? indexOfTargetNode : indexOfTargetNode+1,
                        newElemNode);
            newElemNode->parent = targetNode->parent; //4
        }
        else if (targetNode->parent->Type == CircuitNode::SerialJoint){
            CircuitNode* newParallel = new CircuitNode(CircuitNode::ParallelJoint);
            newParallel->parent = targetNode->parent;
            newParallel->child.push_back(targetNode);
            if (insertBefore) newParallel->child.push_front(newElemNode);
            else newParallel->child.push_back(newElemNode); // 1
            int indexOfTargetNode = targetNode->parent->child.indexOf(targetNode);
            targetNode->parent->child [indexOfTargetNode] = newParallel; //2
            targetNode->parent = newParallel;  // 3
            newElemNode->parent = newParallel; // 4
        }
    }       
    elementList.push_back(newElemNode);
    return newElemNode;
}

void CircuitModel::removeNode(CircuitModel::CircuitNode *targetNode)
{
    if (targetNode -> Type==CircuitNode::Element){
        if (targetNode->parent == NULL){ // Top level element
            rootNode = NULL;
        } 
        else if (targetNode->parent->child.size()==2) {
            CircuitNode* grandParent = targetNode->parent->parent;
            if (grandParent ==NULL) { // targetNode within the top-level structure
                targetNode->parent->child.removeOne(targetNode); // Remove parent's child targetNode
                rootNode = targetNode->parent->child.first();    // Set the top level node to the other child
                rootNode->parent = NULL;  // Set the other child's parent to NULL
                targetNode->parent->child.clear(); // Remove parent's other child to prevent deletion
                delete targetNode->parent;  // Now delete the parent joint
            }
            else { // targetNode's parent joint is under another parent (grandParent)
                targetNode->parent->child.removeOne(targetNode); // Remove parent's child
                int indexOfParent = grandParent->child.indexOf(targetNode->parent); 
                grandParent->child[indexOfParent] = targetNode->parent->child.first();
                // Replace grandParent's child to targetNode's sibling
                grandParent->child[indexOfParent]->parent = grandParent; // Set sibling's parent to grandParent
                targetNode->parent->child.clear(); // Remove parent's other child
                delete targetNode->parent;  // Delete the parent joint
            }
        }
        else { // at least 3 children in the parent joint, no need to remove parent joint
            targetNode->parent->child.removeOne(targetNode);
        }
        
        delete targetNode; 
    }
    elementList.removeOne(targetNode);
    delete targetNode;
}

QString CircuitModel::parseExpression(QString expression)
{
    /// Remove blanks
    for(int i=0; i<expression.size();){
        if (expression.at(i)==QChar(' ') || expression.at(i)==QChar('\t')){
            expression.remove(i,1);
            continue;
        }
        i++;
    }
    if (expression.isEmpty()) return QString("Error: no expression entered");
    
    QVector <QString> TokenList;
    int iChar = 0;
    char ch;
    while (iChar<expression.size()){
        ch = expression.at(iChar).toLatin1();
        
        if (ch=='+'||ch=='*'||ch=='('||ch==')'){
            TokenList.push_back(QString(ch));
            iChar++;
        }
        else{
            TokenList.push_back(QString());
            while (!(ch=='+'||ch=='*'||ch=='('||ch==')')){
                TokenList.back().push_back(ch);
                iChar++;
                if (iChar == expression.size()) break;
                ch = expression.at(iChar).toLatin1();
            }
        }
    }
    
    /// Modified shunting yard

    QVector <QString> OperatorStack;
    CircuitNode outputManager (CircuitNode::SerialJoint);
    // To take temporary ownership of the nodes in the output tree which 
    // are not yet attached to a parent junction
    
    QList <CircuitNode*> newElemList;
    
    
    for (int iToken = 0; iToken<TokenList.size(); iToken++){
        const QString& Token = TokenList.at(iToken);
        if (Token==QString("(")||Token==QString("*")){
            OperatorStack.push_back(Token);
        }
        else if (Token==QString("+")){
            while (OperatorStack.size() && OperatorStack.back()==QString("*")){

                QString Error = popOperator(OperatorStack, outputManager);
                if (!Error.isEmpty()) return Error;
            }
            OperatorStack.push_back(Token);
        }
        else if (Token==QString(")")){
            while (OperatorStack.size() && OperatorStack.back()!=QString("(")){
                QString Error = popOperator(OperatorStack, outputManager);
                if (!Error.isEmpty()) return Error;
            }
            if (!OperatorStack.size()) {
                return QString("Error: unpaired bracket \'(\'");
            }
            OperatorStack.pop_back();
        }
        else { // Token is an element
            int delimiterPos = Token.indexOf(QString("."));
            QString ElemSymbol = Token.left (delimiterPos);
            QString ElemId = delimiterPos==-1 ? QString() : Token.mid (delimiterPos+1);
            
            // Check if the elment symbol is recognized in library
            int libraryIndex = ElementLibrary.indexOf(ElemSymbol);
            if (libraryIndex == -1){
                return QString("Error: unrecognized element symbol: %1").arg(ElemSymbol);
            }
            
            // Check for duplicate id
            if (!ElemId.isEmpty()){
                for (int i=0; i<newElemList.size(); i++){
                    if (ElemSymbol == newElemList.at(i)->elem.symbol()
                            && ElemId == newElemList.at(i)->elem.id()){
                        return QString ("Error: duplicate elements: %1").arg(Token);
                    }
                }
            }
            
            CircuitNode* newElemNode = new CircuitNode(
                        libraryIndex>=BasicElems.size() ? 
                            CustomElems.at(libraryIndex-BasicElems.size()):
                            BasicElems.at(libraryIndex)
                        );
            newElemNode->elem.setID (ElemId);
            newElemNode->parent = &outputManager;
            outputManager.child.push_back(newElemNode);
            
            // Retain the original element if available
            for (int i=0; i<elementList.size(); i++){
                if (newElemNode->elem.symbol() == elementList.at(i)->elem.symbol() &&
                        newElemNode->elem.id() == elementList.at(i)->elem.id()){
                    newElemNode->elem = elementList.at(i)->elem; // copy over the parameters too
                    break;
                }
            }
            newElemList.push_back(newElemNode);           
        }
        
    }
    
    while (OperatorStack.size()){
        if (OperatorStack.back()==QString("(")){
            return QString("Error: unpaired bracket \')\'");
        }
        popOperator(OperatorStack,outputManager);
    }
    
    if (outputManager.child.size()!=1) return QString("Error: missing operator");
    
    /// Successfully constructed the new tree
    
    /// Rename any missing identifiers
    for (int i=0; i<newElemList.size(); i++){
        if (newElemList.at(i)->elem.id().isEmpty()){
            int autoIndex = 1;
            bool clashWithPrevious;
            do{
                newElemList[i]->elem.setID(QString::number(autoIndex++));
                clashWithPrevious = 0;
                for (int j=0; j<newElemList.size(); j++){
                    if (newElemList.at(i)->elem.symbol() == newElemList.at(j)->elem.symbol()
                            && newElemList.at(i)->elem.id() == newElemList.at(j)->elem.id()
                            && i!=j) {
                        clashWithPrevious = 1;
                        break;
                    }
                }
            } while (clashWithPrevious);
        }
    }
    
    elementList = newElemList; // Copy over the new list of elements
        
    delete rootNode;
    rootNode = outputManager.child.at(0);
    rootNode->parent = NULL;
    outputManager.child.clear();
    
    return QString();
}

QString CircuitModel::toMathExpression(const CircuitNode *atNode, 
                                       bool includeElementID) const
{
    if (rootNode==NULL) return QString();
    const CircuitNode* Node = atNode==NULL ? rootNode : atNode;
    QString s;
    
    switch (Node->Type){
    case CircuitNode::Element:
        s = includeElementID ? Node->elem.symbol() + "." + Node->elem.id():
                               Node->elem.symbol();
        break;
    case CircuitNode::ParallelJoint:
        s = toMathExpression(Node->child.at(0),includeElementID); // recursion
        for (int i=1; i<Node->child.size(); i++){
            s = s + " * " + toMathExpression(Node->child.at(i),includeElementID);
        }
        break;
    case CircuitNode::SerialJoint:
        s = toMathExpression(Node->child.at(0),includeElementID); // recursion
        for (int i=1; i<Node->child.size(); i++){
            s = s + " + " + toMathExpression(Node->child.at(i),includeElementID);
        }
        if (Node->parent != NULL) s = QString("( %1 )").arg(s);
        break;
    }
    return s;
}



void CircuitModel::initializeElementLibrary()
{
    /// Initialize the elements
    QDir basicElementDir ("basicelem");
    QStringList basicElemFiles = basicElementDir.entryList(
                QStringList("*.xml"), QDir::Files);
    
    BasicElems.clear();
    for (int iFile=0; iFile<basicElemFiles.size(); iFile++){
        QFile xmlFile(basicElementDir.filePath(basicElemFiles.at(iFile)));
        if (!xmlFile.open(QFile::ReadOnly)) continue;
        CircuitElement readElem(&xmlFile);
        xmlFile.close();
        if (!readElem.importSucceeded()) {
            QMessageBox::warning(NULL,xmlFile.fileName(), readElem.importError());
            continue;
        }
        
        BasicElems.push_back(readElem);
        ElementLibrary.push_back(readElem.symbol());
    }
    
    QDir customElementDir ("customelem");
    QStringList customElemFiles = customElementDir.entryList(
                QStringList("*.xml"),QDir::Files);
    CustomElems.clear();    
    for (int iFile=0; iFile<customElemFiles.size(); iFile++){
        if (customElemFiles.at(iFile).startsWith(QString("_"))) continue;
        QFile xmlFile(customElementDir.filePath(customElemFiles.at(iFile)));
        if (!xmlFile.open(QFile::ReadOnly)) continue;
        CircuitElement readElem(&xmlFile);
        xmlFile.close();
        if (!readElem.importSucceeded()) continue;
        CustomElems.push_back(readElem);
        ElementLibrary.push_back(readElem.symbol());
    }
        
    /// Initialize the icons
    QDir iconDir ("./elementicons");
    
    // Icon #0 is reserved for unspecified.png
    IconNames.push_back("unspecified.png");
    Icons.push_back(QImage(iconDir.filePath(QString("unspecified.png"))));

    QStringList iconFiles = iconDir.entryList(QStringList("*.png"),QDir::Files);
    for (int iFile = 0; iFile<iconFiles.size(); iFile++){
        QImage currentIcon (iconDir.filePath(iconFiles.at(iFile)));
        if (!currentIcon.isNull()){
            IconNames.push_back(iconFiles.at(iFile));
            Icons.push_back(currentIcon);
        }
    }
    
}

QVector<QVector<CircuitModel::DiagramBlock> > CircuitModel::generateDiagramChunk(CircuitModel::CircuitNode *Node) const
/// This function works by constructing each child chunk and assemble them to form
/// the larger chunk unit. Function is called recursively for each children. If the
/// node hosts an element, then return the block representing the element
{
    QVector<QVector<CircuitModel::DiagramBlock> > assembledChunk;
    if (Node==NULL) return QVector<QVector<CircuitModel::DiagramBlock> >();
    if (Node->Type == CircuitNode::Element){
        assembledChunk = QVector<QVector<CircuitModel::DiagramBlock> >
            (1,QVector<CircuitModel::DiagramBlock> (1,DiagramBlock(Node)));
    }
    
    else if (Node->Type == CircuitNode::SerialJoint){
        QVector <QVector <QVector <DiagramBlock> > > listChildChunks(Node->child.size());
        
        int chunkRows = 0;  // max rows
        int chunkCols = 0;  // cumulative cols
        for (int iChild = 0; iChild<Node->child.size(); iChild++){
            listChildChunks[iChild] = generateDiagramChunk(Node->child.at(iChild));
            // Recursion into children
            
            if (chunkRows < listChildChunks.at(iChild).size()){
                chunkRows = listChildChunks.at(iChild).size();
                // Compare to expand rows
            }
            chunkCols += listChildChunks.at(iChild).at(0).size(); 
            // Accumulate child chunks for columns
        }
        // Found the minimum chunck to hold all the child chunks
        
        assembledChunk = QVector <QVector <DiagramBlock> >
                (chunkRows,QVector<DiagramBlock> (chunkCols)); // Resize assembledChunck
        
        for (int iChild=0, beginCol=0; iChild<Node->child.size(); iChild++){
            for (int row = 0; row < listChildChunks.at(iChild).size(); row++){
                for (int col = 0; col < listChildChunks.at(iChild).at(row).size(); col++){
                    assembledChunk[row][col+beginCol] = listChildChunks.at(iChild).at(row).at(col);
                }
            }
            beginCol += listChildChunks.at(iChild).at(0).size();
        }
    }
        
    else {// CircuitNode::ParallelJoint:
        QVector <QVector <QVector <DiagramBlock> > > listChildChunks(Node->child.size());
        
        int chunkRows = 0;  // max rows
        int chunkCols = 2;  // cumulative cols (parallel wiring included)
        for (int iChild = 0; iChild<Node->child.size(); iChild++){
            listChildChunks[iChild] = generateDiagramChunk(Node->child.at(iChild));
            // Recursion into children
            if (chunkCols < listChildChunks.at(iChild).at(0).size()+2){
                chunkCols = listChildChunks.at(iChild).at(0).size()+2;
                // Compare to expand columns
            }
            chunkRows += listChildChunks.at(iChild).size(); 
            // Accumulate child chunks for columns
        }
        
        assembledChunk = QVector <QVector <DiagramBlock> >
                (chunkRows,QVector<DiagramBlock> (chunkCols, DiagramBlock()));
        // Additional step to add the wiring for parallel junctions
        
        // Now copy the child chunks and add the connectors
        for (int iChild=0, beginRow=0; iChild<Node->child.size(); iChild++){
            for (int row = 0; row < listChildChunks.at(iChild).size(); row++){
                if (iChild==0) {
                    assembledChunk[beginRow][0].block = DiagramBlock::TopBranchBeg; // top-left
                    assembledChunk[beginRow][chunkCols-1].block = DiagramBlock::TopBranchEnd; // top-right
                }
                else if (iChild==Node->child.size()-1){
                    assembledChunk[beginRow][0].block = DiagramBlock::BotBranchBeg; // bottom-left
                    assembledChunk[beginRow][chunkCols-1].block = DiagramBlock::BotBranchEnd; // bottom-right
                }
                else if (iChild<Node->child.size()-1){
                    assembledChunk[beginRow][0].block = DiagramBlock::MidBranchBeg; // middle-left
                    assembledChunk[beginRow][chunkCols-1].block = DiagramBlock::MidBranchEnd; // middle-right
                    
                    for (int midRow=beginRow+1; midRow<beginRow+listChildChunks.at(iChild).size()-1; midRow++){
                        assembledChunk[midRow][0].block = DiagramBlock::MidVertical; 
                        assembledChunk[midRow][chunkCols-1].block = DiagramBlock::MidVertical; 
                    }
                }
                for (int col = 0; col < listChildChunks.at(iChild).at(row).size(); col++){
                    assembledChunk[row+beginRow][col+1] = listChildChunks.at(iChild).at(row).at(col);
                } // Copy the child blocks
            }
            
            for (int col=listChildChunks.at(iChild).at(0).size(); col<chunkCols-2;col++){
                assembledChunk[beginRow][col+1] = DiagramBlock::Wire;
                // After each child is copied, fill the rest of row with wire
            }
            
            beginRow += listChildChunks.at(iChild).size();
        }
    }
    return assembledChunk;
}

QVector <QString> CircuitModel::ElementLibrary = QVector<QString>();
QVector <QString> CircuitModel::IconNames      = QVector<QString>();
QVector <QImage>  CircuitModel::Icons          = QVector<QImage>();
QVector <CircuitElement> CircuitModel::BasicElems  = QVector <CircuitElement>();
QVector <CircuitElement> CircuitModel::CustomElems = QVector <CircuitElement>();
