#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QComboBox>
#include <QList>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QTableView>

#include "plotgraph.h"
#include "impedance.h"
#include "dataseriestable.h"
#include "fittingwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    
    /// Data
    // Actual data storage
    

    /// Backstage components
    DataSeriesTable* dataTable;
    
    /// Widgets
    QToolBar* tbarMainTop;
    QWidget* CentralArea;
    
    // Data list area
    QScrollArea* DataListArea;
    QTableView* dataTableView;
    
    // Nyquist area
    
    PlotGraph* plotNyqst;
    PlotGraph* plotBode1;
    PlotGraph* plotBode2;
    
    int  currentNyquistSelection;
    int  currentBodeSelection;
    
    QStatusBar* mainStatusBar;
    
    QPushButton* openFittingButton;
    QWidget* circuitThumbnail;
    int colorSequenceNumber;
    
    /// Methods
    void showNyquistSelectionDialog();
    void showBodeSelectionDialog();
    
    /// Constants
    const int PLOT_TOOLBAR_HEIGHT = 24;
    const int DATA_TABLE_WIDTH = 150;
    const int SELECT_PLOT_WIDTH = 80;
    
    // For Nyq, this is both W and H, for Bode
    // For Bode, this is the sum of height of two plots
    const int PLOT_HEIGHT = 500;
    const int BODE_WIDTH = 400;
    
    QString dataFilePath;
    FittingWindow* fittingWindow;

private slots:
    void importData();
    void NyquistSelectionActivated(int index);
    void BodeSelectionActivated(int index);
    void StartFitting();
    void updateDataListTable();

signals:
    void dataSeriesChanged();
    void expSeriesEnabled(bool Enabled);
    
};

#endif // MAINWINDOW_H
