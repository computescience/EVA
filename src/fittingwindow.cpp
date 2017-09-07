#include "fittingwindow.h"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>

#include <QVBoxLayout>
#include <QFrame>


FittingWindow::FittingWindow(QWidget *parent,
                             DataSeriesTable *SeriesTable):
    QDialog (parent,Qt::Dialog),
    seriesTable (SeriesTable)
    
{
    QGridLayout* mainLayout = new QGridLayout;
    /** Contents
        Fitting option pane (TL)
            * Sim / fit / KK etc.
            * Data set select
            * Sim button
        Circuit pane    (TR)
            * Text editor
            * Favorite circuit chooser
            * Graphical circuit editor
        Parameter editing pane (BL)
            * Parameter table
        Fitting graph (BR)
    **/

    QVBoxLayout* fittingOptionsLayout = new QVBoxLayout();
    QVBoxLayout* circuitEditorLayout  = new QVBoxLayout;
    QWidget* paramEditPane      = new QWidget(this);
    QWidget* fittingGraphPane   = new QWidget(this);
    
    setLayout(mainLayout);
    mainLayout->addLayout(fittingOptionsLayout,0,0,1,1);
    mainLayout->addLayout(circuitEditorLayout ,0,1,1,1);
    mainLayout->addWidget(paramEditPane     ,1,0,1,1);
    mainLayout->addWidget(fittingGraphPane  ,1,1,1,1);
    
    /// + L1 Fitting options pane
    QGroupBox* fittingModeGroupBox = new QGroupBox(this);
    seriesListView = new QTableView (this);
    
    fittingOptionsLayout -> addWidget(fittingModeGroupBox);
    fittingOptionsLayout -> addWidget(seriesListView);
    
    /// ++ L2 Fitting mode buttons
    fittingModeGroupBox->setLayout(new QHBoxLayout);
    selectSimButton = new QRadioButton(tr("Simulate"),this);
    selectFitButton = new QRadioButton(tr("Fit"),this);
    fittingModeGroupBox->layout()->addWidget(selectSimButton);
    fittingModeGroupBox->layout()->addWidget(selectFitButton);
    fittingModeButtons = new QButtonGroup ();
    fittingModeButtons->addButton(selectFitButton, 0);
    fittingModeButtons->addButton(selectSimButton, 1);
    selectFitButton->setEnabled(0);
    selectSimButton->setChecked(1);

    /// ++ L2 List of fit data series
    //seriesTable = new DataSeriesTable(this);
    seriesListView->setModel(seriesTable);
    seriesListView->setColumnHidden(0,1);
    seriesListView->setColumnHidden(2,1);
    seriesListView->setColumnHidden(3,1);
    seriesListView->setColumnWidth(1,120);
    seriesListView->setSelectionMode(QTableView::SingleSelection);
    seriesListView->setSelectionBehavior(QTableView::SelectRows);
    
    /// + L1 Circuit pane
    QFrame* circuitDiagramFrame = new QFrame(this);
    circuitEditorLayout->addWidget(circuitDiagramFrame);
    
    /// + L2 CircuitDiagramFrame
    circuitDiagramFrame->setBackgroundRole(QPalette::Background);
    circuitDiagramFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    
    /// ++ L2 Circuit expression editor
    QHBoxLayout* circuitExpressionEditLayout = new QHBoxLayout;
    circuitEditorLayout->addLayout(circuitExpressionEditLayout);
    QLineEdit* circuitExpressionEdit = new QLineEdit();
    circuitExpressionEditLayout-> addWidget(circuitExpressionEdit);
    QPushButton* circuitExpressionConfirm = new QPushButton ("OK", this);
    circuitExpressionEditLayout->addWidget(circuitExpressionConfirm);
    
    /// + L1 ParameterPane
    
    /// + L1 Fitting Graph
    FittingGraph = new PlotDualGraph(&fittedData, DEFAULT_DUAL_GRAPH_SIZE, DEFAULT_DUAL_GRAPH_SIZE, this);
    fittingGraphPane->setLayout(new QHBoxLayout);
    fittingGraphPane->layout()->addWidget(FittingGraph);
    
    /// Connect signal/slot
    connect (fittingModeButtons, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
             this, &FittingWindow::fittingModeSelected);
    connect (seriesListView, SIGNAL(clicked(QModelIndex)),this,SLOT(fittingDataSelected(QModelIndex)));
}

void FittingWindow::dataSelectionChanged(const QVector<impedance*>& newData)
{
    fittedData.clear();
    for (int i=0;i<newData.size(); i++){
        if (newData[i]!=NULL) fittedData.push_back(*(newData.at(i)));
    }
    FittingGraph->Refresh();
}

void FittingWindow::dataSourceChanged()
{
    FittingGraph -> update();
}

void FittingWindow::fittingModeSelected(int buttonId)
{
    
    bool selectSim = buttonId;
    for (int row=0; row<seriesTable->nofExp(); row++){
        seriesListView->setRowHidden(row,selectSim);
        // If selectSim is true, then exp rows hidden, else show
    }
    for (int row=seriesTable->nofExp(); row<seriesTable->nofTotal(); row++){
        seriesListView->setRowHidden(row, !selectSim);
    }

    // Set Section to the last exp / sim
    fittingDataSelected(
            seriesTable->index(selectSim ? (seriesTable->nofTotal())       // "new" fit row
                                         : (seriesTable->nofExp()-1), 0)); // last exp row
}

void FittingWindow::expSeriesAvailable(bool Enabled)
{
    selectFitButton->setEnabled(Enabled);
}

void FittingWindow::fittingDataSelected(QModelIndex index)
{
    int row = index.row();
    if (row<0) return;
    QVector <impedance*> currentSelection;

    if (row != seriesTable->nofTotal())  {
        currentSelection.push_back(seriesTable->getExp(row));
        currentSelection.push_back(seriesTable->getSim(row));
    }
    dataSelectionChanged(currentSelection);
}

