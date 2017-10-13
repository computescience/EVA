#include "fittingwindow.h"

#include <QComboBox>
#include <QDir>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
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

    QGridLayout* fittingOptionsLayout = new QGridLayout();
    QVBoxLayout* circuitEditorLayout  = new QVBoxLayout;
    QWidget* paramEditPane      = new QWidget(this);
    QWidget* fittingGraphPane   = new QWidget(this);
    
    setLayout(mainLayout);
    mainLayout->addLayout(fittingOptionsLayout,0,0,1,1);
    mainLayout->addLayout(circuitEditorLayout ,0,1,1,1);
    mainLayout->addWidget(paramEditPane       ,1,0,1,1);
    mainLayout->addWidget(fittingGraphPane    ,1,1,1,1);
    
    /// + L1 Fitting options pane
    QGroupBox* fittingModeGroupBox = new QGroupBox(this);
    seriesListView = new QTableView (this);
    
    fittingOptionsLayout -> addWidget(fittingModeGroupBox,0,0,1,1);
    fittingOptionsLayout -> addWidget(seriesListView,     0,1,1,1);
    QPushButton* startSimFitButton = new QPushButton(tr("Start"));
    connect(startSimFitButton, SIGNAL(clicked()), 
            this, SLOT(startSimFitButtonPushed()));
    
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
    seriesListView->setMinimumHeight(180);
    seriesListView->horizontalHeader()->setVisible(0);
    seriesListView->setModel(seriesTable); // Link to the same series table as MainWindow
    seriesListView->setColumnHidden(0,1);
    seriesListView->setColumnHidden(2,1);
    seriesListView->setColumnHidden(3,1);
    seriesListView->setColumnWidth(1,200);
    seriesListView->setSelectionMode(QTableView::SingleSelection);
    seriesListView->setSelectionBehavior(QTableView::SelectRows);
    
    /// + L1 Circuit pane
    QScrollArea* circuitDiagramFrame = new QScrollArea(this);
    circuitEditorLayout->addWidget(circuitDiagramFrame);
    
    /// + L2 CircuitDiagramFrame
    circuitDiagramFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    circuitDiagram = new CircuitDiagram (&circuitModel, this);
    circuitDiagramFrame-> setWidget(circuitDiagram);
   
    circuitDiagram->resize(100,100);
    
    /// ++ L2 Circuit expression editor
    QHBoxLayout* circuitExpressionEditLayout = new QHBoxLayout;
    circuitEditorLayout->addLayout(circuitExpressionEditLayout);
    circuitExpressionEdit = new QLineEdit();
    circuitExpressionEditLayout-> addWidget(circuitExpressionEdit);
    QPushButton* circuitExpressionConfirm = new QPushButton ("Accept", this);
    circuitExpressionEditLayout->addWidget(circuitExpressionConfirm);
    connect (circuitExpressionConfirm,SIGNAL(clicked(bool)), this,SLOT(circuitExpressionAccepted()));
    
    /// + L1 ParameterPane
    parameterTableView = new QTableView(this);
    mainLayout->addWidget(parameterTableView, 1,0,1,1);
    parameterTableView->setMinimumWidth(DEFAULT_LEFT_WIDTH);
    
    parameterTableModel= new ParameterTableModel(&circuitModel, this);
    parameterTableView->setModel(parameterTableModel);
    parameterTableView->setColumnWidth(0,25);
    parameterTableView->setColumnWidth(1,40);
    parameterTableView->setColumnWidth(2,40);
    parameterTableView->setColumnWidth(3,60);
    parameterTableView->setColumnWidth(4,60);
    parameterTableView->setColumnWidth(5,60);
    
    /// + L1 Fitting Graph
    FittingGraph = new PlotDualGraph(&fittedData, 
                                     DEFAULT_DUAL_GRAPH_SIZE, 
                                     DEFAULT_DUAL_GRAPH_SIZE, 
                                     this);
    FittingGraph->setSquareWidget(1);
    fittingGraphPane->setLayout(new QHBoxLayout);
    fittingGraphPane->layout()->addWidget(FittingGraph);
    FittingGraph->useSeriesColor(0);
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
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
    
    // In case fitting mode is changed from program and not user input,
    // then the button selection is manually changed
    if (buttonId != fittingModeButtons->checkedId()) {
        fittingModeButtons->button(buttonId)->setChecked(1);
    }
    
    const int rowHeight = 16;
    for (int row=0; row<seriesTable->nofExp(); row++){
        seriesListView->setRowHidden(row, selectSim);
        // If selectSim is true, then exp rows hidden, else show
        seriesListView->setRowHeight(row, rowHeight);
    }
    for (int row=seriesTable->nofExp(); row<=seriesTable->nofTotal(); row++){
        seriesListView->setRowHidden(row, !selectSim);
        seriesListView->setRowHeight(row, rowHeight);
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

bool FittingWindow::simulate(impedance& result, const impedance *freqSource=NULL)
{
    bool normal = 1;
    result.resize(freqSource==NULL ? defaultFreq.size() : freqSource->size());
    for (int i=0; i<result.size(); i++){
        double freq = freqSource==NULL ? defaultFreq.at(i) 
                                       : freqSource->get(i,impedance::Fr);
        result.setF(i,freq);
        std::complex<double> Z = circuitModel.evaluate(freq);
        if (! (std::isfinite(Z.real())) || ! (std::isfinite(Z.imag()))){
            normal = 0;
            result.setR(i, 0);
            result.setI(i, 0);
        }
        else {
            result.setR(i, Z.real());
            result.setI(i, Z.imag());
        }
    }
    normal &= result.validate();
    if 
}

bool FittingWindow::setDefaultFreq(double maxF, double minF, int ptPerDec)
{
    if (minF>=maxF) return 0;
    if (minF<0) return 0;
    
    defaultFreq.clear();
    double factor = pow(10, 1.0/ptPerDec);
    for (double freq=maxF; freq>minF*(1-1e-10); freq/=factor){
        defaultFreq.push_back(freq);
    }
    return 1;
}

void FittingWindow::fittingDataSelected(QModelIndex index)
{
    int row = index.row();
    if (row<0) return;
    
    seriesListView->selectRow(row);
    
    QVector <impedance*> currentSelection;

    if (row != seriesTable->nofTotal())  {
        currentSelection.push_back(seriesTable->getExp(row));
        currentSelection.push_back(seriesTable->getSim(row));
    }
    dataSelectionChanged(currentSelection);
}

void FittingWindow::circuitExpressionAccepted()
{
    QString errorMsg = 
            circuitModel.parseExpression(circuitExpressionEdit->text());
    if (errorMsg.size()){
        QMessageBox::warning(this,tr("Error"),errorMsg);
    }
    circuitExpressionEdit->setText(circuitModel.toMathExpression());
    circuitDiagram->updateDiagram();
    circuitDiagram->update();
    parameterTableModel->refresh();
}

void FittingWindow::startSimFitButtonPushed()
{
    int currentSelection = seriesListView->selectedIndexes().front().row();
    if (fittingModeButtons->checkedId() == 0) {// under fitting mode
        
    }
    else { // under simulation mode
        impedance temporarySimData;
        bool simSuccess = simulate(temporarySimData,NULL);
        if (!simSuccess) {
            QMessageBox::warning(this, tr("Math error"), tr("Check parameters"));
            return;
        }
        if (currentSelection==seriesTable->nofTotal()){
            temporarySimData.setAutoColor();
        }
        else temporarySimData.setColor(
                    seriesTable->getSim(currentSelection)->color());
        *(seriesTable->getSim(currentSelection)) = temporarySimData;
        
    }
}

