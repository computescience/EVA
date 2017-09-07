#ifndef FITTINGWINDOW_H
#define FITTINGWINDOW_H

#include "dataseriestable.h"
#include "impedance.h"
#include "plotdualgraph.h"

#include <QButtonGroup>
#include <QDialog>
#include <QList>
#include <QRadioButton>
#include <QTableView>


class FittingWindow : public QDialog
{
    Q_OBJECT
public:
    FittingWindow(QWidget* parent, DataSeriesTable* SeriesTable);
    
public slots:
    
    void dataSourceChanged();
    void fittingModeSelected(int buttonId);
    void expSeriesAvailable (bool Enabled); // controls whether fitting mode is enabled or not

private:
    /// Data

    // The fitted data is a copy of an imp series from the 
    // main data source. It needs to be re-copied every time
    // the fitting window opens.
    QList <impedance> fittedData;
    
    DataSeriesTable* seriesTable;

    /// Window component    
    QButtonGroup* fittingModeButtons; 
    QRadioButton* selectSimButton;
    QRadioButton* selectFitButton;
    QTableView*    seriesListView;
    PlotDualGraph* FittingGraph;

    void dataSelectionChanged(const QVector<impedance *>& newData);
    
    const int DEFAULT_DUAL_GRAPH_SIZE = 400;
    
private slots:
    void fittingDataSelected(QModelIndex index);
};

#endif // FITTINGWINDOW_H
