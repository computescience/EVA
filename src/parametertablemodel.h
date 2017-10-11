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
    int rowCount(const QModelIndex &) const override{return RowCount;}
    int columnCount(const QModelIndex &) const override {return 6;}
    
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

private:
    CircuitModel* circuitModel;
    int RowCount; // independently maintain a row count
};

#endif // PARAMETERTABLEMODEL_H
