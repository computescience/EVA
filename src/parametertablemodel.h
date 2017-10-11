#ifndef PARAMETERTABLEMODEL_H
#define PARAMETERTABLEMODEL_H
#include <QAbstractTableModel>
#include <QVector>
#include "circuitmodel.h"

class ParameterTableModel : public QAbstractTableModel
{
public:
    ParameterTableModel(CircuitModel* model, QObject *parent=NULL);
    
public slots:
    int rowCount(const QModelIndex &parent = QModelIndex()) const{return RowCount;}
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {return 6;}
    
    QVariant headerData(int section, Qt::Orientation orientation, int role) const{
        if (role == Qt::DisplayRole && orientation==Qt::Horizontal){
            switch(section){
            case 0: return (QString("Fix"));
            case 3: return (tr("Value"));
            case 4: return (tr("Min"));
            case 5: return (tr("Max"));
            }
        }
        return QVariant();
    }
    
    QVariant data(const QModelIndex &index, int role) const override;
    
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
        beginInsertRows(parent, row, row+count-1);
        RowCount+=count;
        endInsertRows();
        return 1;
    }
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
        beginRemoveRows(parent,row,row+count-1);
        RowCount-=count;
        endRemoveRows();
        return 1;
    }
    void refresh();
    
    /// Edit data
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;    
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    CircuitModel* circuitModel;
    int RowCount; // independently maintain a row count
    
    int findElement(int row, bool findPar = 0) const {
        // Given a row number in the parameter table
        // return the element number (if findPar is F)
        // or parameter number (if findPar is T)
        
        int iElem=0;
        while (row>=circuitModel->accessElemRO(iElem).varCount()){
            row -= circuitModel->accessElemRO(iElem).varCount();
            iElem++;
        }
        return findPar ? row : iElem;
    }
    
    QString validateInputValue(double val, double min, double max, int whichWasChanged);
};

#endif // PARAMETERTABLEMODEL_H
