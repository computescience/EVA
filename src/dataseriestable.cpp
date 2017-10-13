#include "dataseriestable.h"

#include <QFont>

DataSeriesTable::DataSeriesTable(QObject* parent):
    QAbstractTableModel(parent)
  
{
}

QVariant DataSeriesTable::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    
    switch (role) {
    
    case Qt::DecorationRole:
        if (index.column()==0 && index.row()!=simSeries.size()){
            // The total rows is simSeries.size()+1
            impedance* dataSet = (row < nofExp() ? 
                                  expSeries.at(row):simSeries.at(row);
            return dataSet->color();
        } 
        break;
        
    case Qt::CheckStateRole:
        if (row == nofTotal()) return QVariant();
        if (index.column() == 2 && row<nofExp()){
            return expSeries.at(row)->isVisible() ? Qt::Checked : Qt::Unchecked;
        }
        if (index.column() == 3 && simSeries.at(row)!=NULL){
            return simSeries.at(row)->isVisible() ? Qt::Checked : Qt::Unchecked;
        }
        break;
        
    case Qt::DisplayRole:
        if (index.column()== 1){
            if (row == nofTotal()) return QString("<new>");
            return ((row < nofExp()) ? expSeries.at(row):simSeries.at(row)) -> dataName();
        }
        break;
    case Qt::FontRole:
        if (index.row()==nofTotal()) return QVariant();
        if (index.column()==1){
            QFont font (QString("calibri"),8);
            if (row>=nofExp()) font.setItalic(1); // Simulated data serieses
            else if (simSeries.at(row) != NULL) font.setBold(1); // Exp with Sim attached
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
    int insertAtRow = pendingData->isExp() ? nofExp() : nofTotal();
    insertRows(insertAtRow, 1);   
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

