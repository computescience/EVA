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
    int rowCount(const QModelIndex &) const override {return simSeries.size() + 1;}
    int columnCount(const QModelIndex &) const override {return 4;}
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    int nofExp () const {return expSeries.size();}
    int nofTotal () const {return simSeries.size();}
    impedance* getExp(int row) const {return expSeries.at(row);}
    impedance* getSim(int row) const {return simSeries.at(row);}
    
public slots:
    void addDataSeries(impedance* newDataSeries);
    void removeDataSeries(impedance* whichDataSeries);
    void attachFittedData(impedance* expData, impedance* fittedData);
    void rowEntryModified(int row) {emit dataChanged(index(row,0),index(row,3));}
    
private:
    QList<impedance> dataList; // All the data series
    
    impedance* pendingData; // The data series to be added
    
    // Individually maintain two lists of pointers to series
    // simSeries contains fitted data which is positioned in
    // alignment with the corresponding exp data series in
    // expSeries. Positions where there is no corresponding
    // fitted data will contain a NULL pointer
    QList <impedance*> expSeries;
    QList <impedance*> simSeries; 
};

#endif // DATASERIESTABLE_H
