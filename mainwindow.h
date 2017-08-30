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

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    /// Data
    // Main data storage
    QList<impedance> data_;

    DataSeriesTable* dataTable;
    QTableView* dataTableView;

    QScrollArea* DataListArea;
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
    
    static const int HUE_PROGRESSION = 63;
    
    QString dataFilePath;

private slots:
    void importData();
    void NyquistSelectionActivated(int index);
    void BodeSelectionActivated(int index);
    
};

#endif // MAINWINDOW_H
