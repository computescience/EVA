#include "parametertablemodel.h"
#include <QBrush>

ParameterTableModel::ParameterTableModel(CircuitModel *model, QObject *parent)
    : QAbstractTableModel(parent),
      circuitModel (model)
{
    RowCount = 0;
}

QVariant ParameterTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int var = row;
    int elemIndex = 0;
    
    while (var >= circuitModel->getElemInfo(elemIndex).varCount()){
        var-= circuitModel->getElemInfo(elemIndex).varCount();
        elemIndex++;
    }
    const CircuitElement& element = circuitModel->getElemInfo(elemIndex);
    
    switch (role){
    case Qt::CheckStateRole:
        if (index.column()==0) return element.variableFixed(var);
        break;
    case Qt::DisplayRole:
        switch(index.column()){
        case 1: return element.fullName(); break;
        case 2: return element.varName(var); break;
        case 3: return element.var(var); break;
        case 4: return element.min(var); break;
        case 5: return element.max(var); break;
        } 
        break;
    case Qt::ForegroundRole:
        if (element.variableFixed(var) && 
                (index.column()==4||index.column()==5)){
            return QBrush(QColor(80, 80, 80));
        }
        break;
    case Qt::BackgroundRole:
        if (index.column()==1||index.column()==2){
            return QBrush(QColor(200, 200, 200));
        }
        if (index.column()==4||index.column()==5){
            return QBrush(QColor(169, 214, 214));
        }
        break;
    }
    return QVariant();
}

void ParameterTableModel::refresh()
{
    if (RowCount>0) removeRows(0, RowCount);
    
    int newRowCount=0;
    for (int i=0; i<circuitModel->elemCount(); i++) {
        newRowCount += circuitModel->getElemInfo(i).varCount();
    }
    
    if (newRowCount>0) insertRows(0, newRowCount);
}

