#ifndef DATASERIESTABLE_H
#define DATASERIESTABLE_H

#include <QAbstractTableModel>
#include <QList>
#include "impedance.h"

/* This is the table model for the list of data series
 * Two QList<impedance*> are maintained, with exp series
 * in the beginning (exp rows), attached by fitted series
 * if available. Following are the data from simulation
 * (sim-only rows).
 * Where data series is missing for a row, ie. sim in not-
 * yet-fitted exp rows, or exp data in sim-only rows, then
 * the corresponding pointer is NULL. This way, both lists
 * maintain the same length.
 */
class DataSeriesTable : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    explicit DataSeriesTable(QObject *parent);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    int nofExp () const {return nofExpSeries;}
    int nofTotal () const {return expSeries.size();}
    impedance* getExp(int row) const {return expSeries.at(row);}
    impedance* getSim(int row) const {return simSeries.at(row);}
    
public slots:
    void addDataSeries(impedance* newDataSeries);
    void removeDataSeries(impedance* whichDataSeries);
    void attachFittedData(impedance* expData, impedance* fittedData);
    
    
private:
    impedance* pendingData; // The data series to be added
    
    // individually maintain a table of pointers to series
    QList <impedance*> expSeries;
    QList <impedance*> simSeries; 
    int nofExpSeries; // Keeping a record of number of exp rows
};

#endif // DATASERIESTABLE_H
