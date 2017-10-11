#include "parametertablemodel.h"
#include <QBrush>
#include <QMessageBox>

ParameterTableModel::ParameterTableModel(CircuitModel *model, QObject *parent)
    : QAbstractTableModel(parent),
      circuitModel (model)
{
    RowCount = 0;
}

QVariant ParameterTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int iPar = findElement(row, 1);
    int iElem = findElement(row, 0);
    
    const CircuitElement& element = circuitModel->accessElemRO(iElem);
    
    switch (role){
    case Qt::CheckStateRole:
        if (index.column()==0) return element.varFixed(iPar) ? 
                    Qt::Checked : Qt::Unchecked;
        break;
    case Qt::DisplayRole:
        switch(index.column()){
        case 1: if (!iPar) return element.fullName(); break;
            // Display element name only on the first line
        case 2: return element.varName(iPar); break;
        case 3: return element.var(iPar); break;
        case 4: if (!circuitModel->accessElemRO(iElem).varFixed(iPar)) 
                return element.min(iPar); break;
        case 5: if (!circuitModel->accessElemRO(iElem).varFixed(iPar)) 
                return element.max(iPar); break;
        } 
        break;
    case Qt::ForegroundRole:
        if (element.varFixed(iPar) && 
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
    case Qt::FontRole:
        if (index.column()==1) return QFont(QString("Arial"),-1,75);
        break;
    }
    return QVariant();
}

void ParameterTableModel::refresh()
{
    if (RowCount>0) removeRows(0, RowCount);
    
    int newRowCount=0;
    for (int i=0; i<circuitModel->elemCount(); i++) {
        newRowCount += circuitModel->accessElemRO(i).varCount();
    }
    
    if (newRowCount>0) insertRows(0, newRowCount);
}

bool ParameterTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int iElem = findElement(index.row(),0);
    int iPar  = findElement(index.row(),1);
    
    if (role==Qt::CheckStateRole && index.column()==0){
        circuitModel->accessElem(iElem).fixVariable(iPar, value.toInt());
        /// If the parameter was previously fixed and is now relaxed,
        /// value may have been set outside the range of [Min, Max]
        /// So extra step is needed to bring it back regardless
        if (value.toInt() == 0){
            if (circuitModel->accessElemRO(iElem).var(iPar) < 
                    circuitModel->accessElemRO(iElem).min(iPar)){
                circuitModel->accessElem(iElem).set(iPar,
                                            circuitModel->accessElemRO(iElem).min(iPar));
            }
            else if (circuitModel->accessElemRO(iElem).var(iPar) > 
                   circuitModel->accessElemRO(iElem).max(iPar)){
                circuitModel->accessElem(iElem).set(iPar,
                                            circuitModel->accessElemRO(iElem).max(iPar));
            }
        }
        dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount()-1));
        return 1;
    }
    else if (role == Qt::EditRole && index.column()>=3){
        if (value.toString().isEmpty()) return 0;
        bool parseOK;
        double newValue =value.toDouble(&parseOK);
        if (!parseOK) {
            QMessageBox::warning(NULL, tr("Invalid input"),
                                 tr("Please input a number"));
            return 0;
        }
        
        double origVal = circuitModel->accessElemRO(iElem).var(iPar);
        double origMin = circuitModel->accessElemRO(iElem).min(iPar);
        double origMax = circuitModel->accessElemRO(iElem).max(iPar);
        
        QString validationState;
        if (! circuitModel->accessElemRO(iElem).varFixed(iPar)) {
            switch (index.column()){
            case 3: validationState = validateInputValue(newValue, origMin, origMax, 0); break;
            case 4: validationState = validateInputValue(origVal, newValue, origMax, 1); break;
            case 5: validationState = validateInputValue(origVal, origMin, newValue, 2); break;
            }
        }
        
        if (validationState.isEmpty()) {
            switch (index.column()){
            case 3: circuitModel->accessElem(iElem).set(iPar, newValue); break;
            case 4: circuitModel->accessElem(iElem).setMin(iPar, newValue); break;
            case 5: circuitModel->accessElem(iElem).setMax(iPar, newValue); break;
            }
            dataChanged(index,index);
            return 1;
        }
        else {
            QMessageBox::warning(NULL, tr("Invalid input"),validationState);
            return 0;
        }
        /// ParameterTableModel is not a QWidget, so error message will
        /// be displayed by QMessageBox from the FittingWindow.
    }
    return 0;
}

Qt::ItemFlags ParameterTableModel::flags(const QModelIndex &index) const
{
    switch (index.column()){
    case 0: return Qt::ItemIsUserCheckable|Qt::ItemIsEnabled; break;
    case 1: 
    case 2: return Qt::ItemIsEnabled; break;
    case 3: return Qt::ItemIsEditable|Qt::ItemIsEnabled; break;
    case 4: 
    case 5: 
        {
            int iElem = findElement(index.row(), 0);
            int iPar = findElement(index.row(), 1);
            if (!(circuitModel->accessElemRO(iElem).varFixed(iPar))){
                return Qt::ItemIsEditable|Qt::ItemIsEnabled;
            }
        } break;
    }
    return Qt::NoItemFlags;
}

QString ParameterTableModel::validateInputValue(double val, double min, double max, int whichWasChanged)
{
    if (min>=max) return whichWasChanged==1 
            ? tr ("\"Minimum\" must be smaller than the Maximum")
            : tr ("\"Maximum\" must be bigger than the Minimum");
    if (val<min) return whichWasChanged==0
            ? tr ("\"Value\" must be no smaller than the Minimum")
            : tr ("\"Minimum\" must be no bigger than the Value");
    if (val>max) return whichWasChanged==0
            ? tr ("\"Value\" must be no bigger than the Maximum")
            : tr ("\"Maximum\" must be no smaller than the Value");
    if (min*max<=0) return 
            tr("Do not set the range [Min, Max] across 0,") + "\n"+
            tr("as this will likely cause fitting failure.") + "\n" +
            tr("Hint: use a very small value like 1e-20 instead of 0.");
    return QString();
}
