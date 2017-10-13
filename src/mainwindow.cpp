#include <QDockWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QMenubar>
#include <QMessageBox>
#include <QPalette>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStatusBar>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include "mainwindow.h"
#include "table_reader.h"
#include "impedance.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), dataList()      
{
    
    /// Initialize the impedance class
    impedance::initializeColumnTitle();
    impedance::initializeColorPalette();
    
    dataFilePath = "%USER%/Documents";
    
    /// Initialize Window
    setWindowTitle(tr("EVA - EIS Viewer and Analyzer"));
    
    /// Menus
    QMenu* menuFile = menuBar()->addMenu("File");
    QMenu* menuEdit = menuBar()->addMenu("Edit");
    QMenu* menuView = menuBar()->addMenu("View");
    QMenu* menuHelp = menuBar()->addMenu("Help");
    
    QAction* actImportData = new QAction(
            QIcon::fromTheme("document-open",QIcon(":/icons/open.png")),tr("Import"),this);
    QAction* actSaveData = new QAction (
            QIcon::fromTheme("document-save",QIcon(":/icons/save.png")),tr("Save data"),this);
    QAction* actClose = new QAction (
            QIcon::fromTheme("document-close", QIcon(":/icons/close.png")),tr("Close"), this);
    QAction* actCircuit = new QAction (QIcon(":/icons/RCCircuit.png"),tr("Sim / Fit"), this);
    QAction* actCopyData = new QAction (
            QIcon::fromTheme("document-copy",QIcon(":/icons/copy.png")),tr("Copy data"),this);
    QAction* actZoomAll = new QAction (
            QIcon::fromTheme("document-zoom",QIcon(":/icons/full.png")),tr("Zoom to all"),this);
    QAction* actAbout = new QAction (
            QIcon::fromTheme("document-about",QIcon(":/icons/question-mark.png")),tr("About"),this);

    menuFile -> addAction (actImportData);
    menuFile -> addAction (actSaveData);
    menuFile -> addAction (actClose);
    menuEdit -> addAction (actCopyData);
    menuView -> addAction (actZoomAll);
    menuHelp -> addAction (actAbout);
    
    connect(actImportData,QAction::triggered,this, &importData);
    connect(actCircuit, QAction::triggered, this, &StartFitting);
    
    /// Top tool bar
    tbarMainTop = addToolBar(tr("Main toolbar"));
    tbarMainTop->setMovable(0);
    tbarMainTop -> addAction (actImportData);
    tbarMainTop -> addAction (actCircuit);

    /// L0: Top level widget: central widget
    CentralArea = new QWidget (this);
    setCentralWidget(CentralArea);
    CentralArea->setLayout(new QHBoxLayout);
    
    /// + L1: Scroll area for data table
    DataListArea = new QScrollArea(this);
    DataListArea->setFixedWidth(DATA_TABLE_WIDTH);
    CentralArea->layout() ->addWidget(DataListArea);
    DataListArea->setFixedWidth(170);
    DataListArea->setWidgetResizable(1);
    
    dataTable = new DataSeriesTable(this);
    dataTableView = new QTableView(this);
    dataTableView->setModel(dataTable);    
    DataListArea ->setWidget(dataTableView);
    dataTableView->setColumnWidth(0,20);
    dataTableView->setColumnWidth(1,80);
    dataTableView->setColumnWidth(2,20);
    dataTableView->setColumnWidth(3,20);
    dataTableView->setSelectionMode(QTableView::SingleSelection);
    dataTableView->setSelectionBehavior(QTableView::SelectRows);
    updateDataListTable();
    connect (this, SIGNAL(dataSeriesChanged()), 
             dataTableView, SLOT(update()));
    
    /// + L1: Nyquist area
    QWidget* NyqstWidget = new QWidget (this);
    CentralArea->layout() ->addWidget(NyqstWidget);
    
    NyqstWidget->setLayout(new QVBoxLayout);
    NyqstWidget->layout()->setContentsMargins(0,0,0,0);
    
    /// ++ L2: Nyquist area details
    QToolBar* NyqstToolbar = new QToolBar(tr("Nyquist plot tools"),this);
    NyqstToolbar->setFixedHeight(PLOT_TOOLBAR_HEIGHT);
    NyqstWidget->layout() -> addWidget(NyqstToolbar);
    
    /// +++ L3: Nyquist toolbar
    QComboBox* selectNyquistType = new QComboBox;
    NyqstToolbar->addWidget(selectNyquistType);
    selectNyquistType->addItem("Impedance (Zr, -Zi)");
    selectNyquistType->addItem("Admittance (Yr, Yi)");
    selectNyquistType->addItem("Custom ...");
    selectNyquistType->setCurrentIndex(0);
    currentNyquistSelection = 0;
    connect (selectNyquistType, SIGNAL(activated(int)), 
             this, SLOT(NyquistSelectionActivated(int)));

    /// +++ L3: Nyquist plot
    plotNyqst = new PlotGraph (&dataList,PLOT_HEIGHT,PLOT_HEIGHT,this);
    NyqstWidget->layout() ->addWidget(plotNyqst);
    NyqstWidget->layout()->setAlignment(NyqstToolbar, Qt::AlignTop);
    plotNyqst->setSquareWidget(1);
    plotNyqst->setXData(impedance::Zreal, impedance::None);
    plotNyqst->setYData(impedance::Zimag, impedance::Neg);
    plotNyqst->setGraphName("Nyquist");
    connect (this,&MainWindow::dataSeriesChanged, plotNyqst, &PlotGraph::Refresh);
    
    /// + L1: Bode area
    QWidget* BodeWidget = new QWidget (this);
    CentralArea->layout() -> addWidget(BodeWidget);
    BodeWidget->setLayout(new QVBoxLayout);
    BodeWidget->layout()->setContentsMargins(0,0,0,0);
    
    /// ++ L2: Bode area details
    QToolBar* BodeToolbar = new QToolBar (tr ("Bode plot tools"), this);
    BodeWidget->layout()->addWidget(BodeToolbar);
    BodeToolbar->setFixedHeight(PLOT_TOOLBAR_HEIGHT);
    
    /// +++ L3: Bode toolbar
    QComboBox* selectBodeType = new QComboBox;
    BodeToolbar->addWidget(selectBodeType);
    selectBodeType->addItem("Impedance (Zabs, φ)");
    selectBodeType->addItem("Impedance (Zr, -Zi)");
    selectBodeType->addItem("Admittance (Yabs, ψ)");
    selectBodeType->addItem("Admittance (Yr, Yi)");
    selectBodeType->addItem("RC serial (Zr, C)");
    selectBodeType->addItem("Custom ...");
    selectBodeType->setCurrentIndex(0);
    currentBodeSelection = 0;
    connect (selectBodeType, SIGNAL(activated(int)), 
             this, SLOT(BodeSelectionActivated(int)));
    
    /// +++ L3: Bode plots
    plotBode1 = new PlotGraph(&dataList,BODE_WIDTH, PLOT_HEIGHT/2, this);
    plotBode2 = new PlotGraph(&dataList,BODE_WIDTH, PLOT_HEIGHT-PLOT_HEIGHT/2, this);
    plotBode1-> setMaximumWidth(BODE_WIDTH);
    plotBode2-> setMaximumWidth(BODE_WIDTH);
    BodeWidget->layout()->addWidget(plotBode1);
    BodeWidget->layout()->addWidget(plotBode2);
    BodeWidget->layout()->setAlignment(BodeToolbar, Qt::AlignTop);
  
    plotBode1->setXData(impedance::Fr,   impedance::Log);
    plotBode1->setYData(impedance::Zabs, impedance::Log);
    plotBode1->setGraphName("Bode1");
    
    plotBode2->setXData(impedance::Fr,  impedance::Log);
    plotBode2->setYData(impedance::Phi, impedance::Neg);
    plotBode2->setGraphName("Bode2");

    plotNyqst->Refresh();
    plotBode1->Refresh();
    plotBode2->Refresh();
    
    connect (this,&MainWindow::dataSeriesChanged, plotBode1, &PlotGraph::Refresh);
    connect (this,&MainWindow::dataSeriesChanged, plotBode2, &PlotGraph::Refresh);
    
    colorSequenceNumber=0;
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
 
    /// Initialize fitting GUI
    fittingWindow = new FittingWindow(this, dataTable);
    fittingWindow->setModal(1);
    
    statusBar()->showMessage(tr("Ready"),2000);
    
    /// Connect the signals and slots
    connect (this, &MainWindow::expSeriesEnabled, fittingWindow, &FittingWindow::expSeriesAvailable);
    showMaximized();
}

MainWindow::~MainWindow()
{
}

void MainWindow::showNyquistSelectionDialog()
{
    
}

void MainWindow::showBodeSelectionDialog()
{
    
}


void MainWindow::importData(){
    
#define DEBBUG_SIMPLIFY_FILENAME
    
#ifdef DEBBUG_SIMPLIFY_FILENAME
    dataFilePath = QString("E:/Documents/programming/Qt/EVA/Test");
#endif

    QString importFileName = QFileDialog::getOpenFileName(
                this,"Import",dataFilePath,
                tr("Text files (*.txt);; All files (*)"));

    if (importFileName.isNull()) return;
    
    table_reader importData;
    if (importData.open(importFileName.toStdString())){ //Error upon opening
        QMessageBox::warning(this,tr("File error"),
                             QString("Failed to open: \n")+importFileName);
        return;
    }
    
    /// File open successful, but will report any error found during reading
    if (importData.errorStatus() & table_reader::LineError){
        QMessageBox::warning(this,tr("Warning"),tr("Error found in reading a a line"));
    }
    
    /// Extract the data file name
    
    const char DIRECTORY_SEP = '/'; // Specific to operating system!!
    
    QString dataName = importFileName.section(QChar(DIRECTORY_SEP),-1,-1);
    if (dataName.contains('.') && dataName.lastIndexOf('.') != 0){
        dataName = dataName.remove(dataName.lastIndexOf('.'),dataName.size());
    }    
    
    impedance newData (importData.d(0),importData.d(1), importData.d(2));
    newData.setAutoColor();
    newData.rename(dataName);
    
    dataList.push_back(newData);
    dataTable->addDataSeries(&(dataList.back()));
    updateDataListTable();
    
    emit expSeriesEnabled(1);
    emit dataSeriesChanged();
}

void MainWindow::NyquistSelectionActivated(int index)
{
    const int customIndex = 2;
    
    if (index == currentNyquistSelection){
        if (index == customIndex) showNyquistSelectionDialog();
        else return;
    }
    switch (index){
    case 0:
        plotNyqst->setXData(impedance::Zreal,impedance::None);
        plotNyqst->setYData(impedance::Zimag,impedance::Neg);
        break;
    case 1:
        plotNyqst->setXData(impedance::Yreal,impedance::None);
        plotNyqst->setYData(impedance::Yimag,impedance::None);
        break;
    case customIndex:
        showNyquistSelectionDialog();
        break;
    }
    currentNyquistSelection = index;
    plotNyqst->Refresh();
    
}

void MainWindow::BodeSelectionActivated(int index)
{
    const int customIndex = 5;
    
    if (index == currentBodeSelection){
        if (index == customIndex) showBodeSelectionDialog();
        else return;
    }
    switch (index){
    case 0:
        plotBode1->setYData(impedance::Zabs,  impedance::Log);
        plotBode2->setYData(impedance::Phi,   impedance::Neg);
        break;
    case 1:
        plotBode1->setYData(impedance::Zreal, impedance::LogAbs);
        plotBode2->setYData(impedance::Zimag, impedance::LogAbs);
        break;
    case 2:
        plotBode1->setYData(impedance::Yabs,  impedance::Log);
        plotBode2->setYData(impedance::Psi,   impedance::None);
        break;
    case 3:
        plotBode1->setYData(impedance::Yreal, impedance::LogAbs);
        plotBode2->setYData(impedance::Yimag, impedance::LogAbs);
        break;
    case 4:
        plotBode1->setYData(impedance::Zreal, impedance::LogAbs);
        plotBode2->setYData(impedance::Capacitance,impedance::LogAbs);
        break;
    case customIndex:
        showBodeSelectionDialog();
        break;
    }
    
    currentBodeSelection = index;
    plotBode1->Refresh();    
    plotBode2->Refresh();
    
}

void MainWindow::StartFitting()
{
    fittingWindow->fittingModeSelected(1);
    fittingWindow->show();
}

void MainWindow::updateDataListTable()
{
    for (int i = 0; i<dataTable->nofTotal(); i++){
        dataTableView->setRowHidden(i, 0);
    }
    dataTableView->setRowHidden(dataTable->nofTotal(),1);
}
