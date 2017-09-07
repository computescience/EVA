#include "dataseriestable.h"

#include <QFont>

DataSeriesTable::DataSeriesTable(QObject* parent):
    QAbstractTableModel(parent)
  
{
    nofExpSeries = 0;
}

int DataSeriesTable::rowCount(const QModelIndex &parent) const
{
    return simSeries.size();
}

int DataSeriesTable::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant DataSeriesTable::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    
    switch (role) {
    
    case Qt::DecorationRole:
        if (index.column()==0){
            impedance* dataSet = (expSeries.at(row)==NULL) ? 
                                  simSeries.at(row):expSeries.at(row);
            return dataSet->color();
        } 
        break;
        
    case Qt::CheckStateRole:
        if (index.column() == 2){
            if (row<expSeries.size())
            return expSeries.at(row)->isVisible() ? Qt::Checked : Qt::Unchecked;
        }
        else if (index.column() == 3 && simSeries.at(row)!=NULL){
            return simSeries.at(row)->isVisible() ? Qt::Checked : Qt::Unchecked;
        }
        break;
        
    case Qt::DisplayRole:
        if (index.column()== 1){
            impedance* dataSet = (row < expSeries.size()) ? 
                                  expSeries.at(row):simSeries.at(row);
            return dataSet->dataName();
        }
        break;
    case Qt::FontRole:
        if (index.column()==1){
            QFont font (QString("calibri"),8);
            if (row>=nofExpSeries) font.setItalic(1);
            else if (simSeries.at(row) != NULL) font.setBold(1);
        }
    }
    return QVariant();
}

QVariant DataSeriesTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal){
        if (role == Qt::DisplayRole){
            switch (section) {
            case 1: return tr("Series"); 
            case 2: return "E";
            case 3: return "S";
            }
        }
    }
    return QVariant();
}

bool DataSeriesTable::insertRows(int row, int count, const QModelIndex &parent)
{
    impedance* newRowExp = pendingData->isExp() ? pendingData : NULL;
    impedance* newRowSim = pendingData->isExp() ? NULL : pendingData;
    
    beginInsertRows(parent, row, row+count-1);
    for (int iRow = 0; iRow<count; iRow++){
        expSeries.insert(row, newRowExp);
        simSeries.insert(row, newRowSim);
    }
    endInsertRows();    
    return 1;
}

void DataSeriesTable::addDataSeries(impedance *newDataSeries)
{
    /// Put the new data series into buffer;
    pendingData = newDataSeries;
    // If the new series is exp, then insert at the end of exp list, otherwise append to the end;
    int insertAtRow = pendingData->isExp() ? nofExpSeries : expSeries.size();
    if (insertRows(insertAtRow, 1)) nofExpSeries++;    
}

void DataSeriesTable::removeDataSeries(impedance *whichDataSeries)
{
    
}

void DataSeriesTable::attachFittedData(impedance *expData, impedance *fittedData)
{
    int row=expSeries.indexOf(expData);
    simSeries[row] = fittedData;
    emit (dataChanged(index(row,2),index(row,2)));    
}

