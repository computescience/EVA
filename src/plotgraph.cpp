#include <QPainter>
#include <QPointF>
#include <QString>
#include <QFont>
#include <cmath>

#include "plotgraph.h"
#include "impedance.h"
#include <cstdio>

PlotGraph::PlotGraph(QList<impedance> *dataSource,int Width, int Height, QWidget *parent)
    : QChartView(new QtCharts::QChart(),parent),
      data_source_(dataSource),
      Axes(2),
      dataCol(2),
      dataMod(2),
      dataSeriesMin(2),
      dataSeriesMax(2),
      axisPlotMin(2),
      axisPlotMax(2),
      NofMajUnits(2),
      NofMinUnits(2),
      
      fracMargin(4,0.05),
      square_aspect_ratio_(0)      
{ 
    /// Set system properties
    setMinimumWidth (Width);
    setMinimumHeight(Height);
    SquareWidget = 0;
    Mgn.setLeft  (width() * fracMargin[0]);
    Mgn.setRight (width() * fracMargin[2]);
    Mgn.setTop   (height()* fracMargin[1]);
    Mgn.setBottom(height()* fracMargin[3]);
    chart()->setMargins(Mgn);
    setRenderHint(QPainter::Antialiasing);
    
    /// Initialize parameters
    
    Axes[0] = new QtCharts::QValueAxis();
    Axes[1] = new QtCharts::QValueAxis();

    chart()->addAxis(Axes[0],Qt::AlignBottom);
    chart()->addAxis(Axes[1],Qt::AlignLeft);

    chart()->legend()->setVisible(0);
    chart()->setBackgroundVisible(0);
    
    setFrameStyle(QFrame::Sunken|QFrame::Box);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(1);
    
    /// Default representation: logF vs logZabs;
    dataCol[0] = impedance::Fr;
    dataMod[0] = impedance::Log;
    dataCol[1] = impedance::Zabs;
    dataMod[1] = impedance::Log;
    manualScaleAxis(0, 0, 1);
    manualScaleAxis(1, 0, 1);
    updateAxis(0);
    updateAxis(1);
    
    chart()->show();
}

void PlotGraph::autoScaleAxis(int whichAxis){
    
    impedance::Column  Col = dataCol[whichAxis];
    impedance::DataMod Mod = dataMod[whichAxis];
    
    //int TargetAxis = whichAxis?2:0;
    
    if (data_source_->empty()) {
        dataSeriesMin[whichAxis] = 0;
        dataSeriesMax[whichAxis] = 1;

        manualScaleAxis(whichAxis, 0, 1);
        return;
    }

    dataSeriesMin[whichAxis] = dataSeriesMax[whichAxis]=0;
    
    for (QList <impedance>::iterator Data = data_source_->begin();
            Data!=data_source_->end(); Data++){ //Iterate through the data sets
        
        if (!Data->isVisible()) continue;  //If the data set is invisible, skip
        
        double currentMax=0; // Max / min of current data set
        double currentMin=0;
        
        switch (Mod){
        
        // How it Works
        /*  currMax = data_series->PosMax(dataX);  -- set the value to the positive maximum first */
        /*  if (!currMax)  -- if the positive maximum is zero - all the data are non-positive */
        /*      currMax = data_series->HasZero(dataX) ? 0 : (-data_series->NegMin(dataX)); */
        /*      -- then set it to either 0 or the highest of negative numer*/
        
        case impedance::None:
            currentMax = Data->PosMax(Col); //set the value to the positive maximum first
            //if there is no positive value, set max to 0 or NegMin
            if (!currentMax) currentMax = Data->HasZero(Col) ? 0 : (-Data->NegMin(Col));
            currentMin = - Data->NegMax(Col);
            if (!currentMin) currentMin = Data->HasZero(Col) ? 0 : ( Data->PosMin(Col));
            break;
            
        case impedance::Neg:
            currentMax = Data-> NegMax(Col); //same as above, except reversal of sign
            if (!currentMax) currentMax = Data->HasZero(Col) ? 0 : (-Data->PosMin(Col));
            currentMin = -Data-> PosMax(Col);
            if (!currentMin) currentMin = Data->HasZero(Col) ? 0 : ( Data->NegMin(Col));
            break;
            
        case impedance::Abs: 
        case impedance::SqrtAbs:
            currentMax = Data->PosMax(Col)>Data->NegMax(Col) ? Data->PosMax(Col) : Data->NegMax(Col);
            if (Data->HasZero(Col)) currentMin = 0;
            else if (!Data->PosMin(Col)) currentMin = Data->NegMin(Col);  // No positive value
            else if (!Data->NegMin(Col)) currentMin = Data->PosMin(Col);  // No negative value
            else currentMin = Data->PosMin(Col)<Data->NegMin(Col) ? Data->PosMin(Col) : Data->NegMin(Col);
            if (Mod == impedance::SqrtAbs) {
                currentMax = sqrt(currentMax);
                currentMin = sqrt(currentMin);
            }
            break;
            
        case impedance::Sqrt:
            currentMax = sqrt(Data->PosMax(Col));
            currentMin = Data->HasZero(Col) ? 0 : sqrt(Data->PosMin(Col));
            break;
            
        case impedance::SqrtNeg:
            currentMax = sqrt(Data->NegMax(Col));
            currentMin = Data->HasZero(Col) ? 0 : sqrt(Data->NegMin(Col));
            break;
            
        case impedance::Log:
            currentMax = Data->PosMax(Col);
            currentMin = Data->PosMin(Col);
            if (!currentMax) currentMax = currentMin = 0; // No valid data in the series
            else {
                currentMax = log10(currentMax);
                currentMin = log10(currentMin);
            }
            break;
            
        case impedance::LogNeg:
            currentMax = Data->NegMax(Col);
            currentMin = Data->NegMin(Col);
            if (!currentMax) currentMax = currentMin = 0;
            else {
                currentMax = log10(currentMax);
                currentMin = log10(currentMin);
            }
            break;

        case impedance::LogAbs:
            currentMax = Data->PosMax(Col)>Data->NegMax(Col) ? Data->PosMax(Col) : Data->NegMax(Col);
            if (Data->PosMax(Col) * Data->NegMax(Col)) // There are positive and negative values
                currentMin = Data->PosMin(Col)<Data->NegMin(Col) ? Data->PosMin(Col) : Data->NegMin(Col);
            else        //If one of them is zero (all are positive or all are negative)
                currentMin = Data->PosMin(Col)>Data->NegMin(Col) ? Data->PosMin(Col) : Data->NegMin(Col);
            if (currentMax!=0){
                currentMax = log10(currentMax);
                currentMin = log10(currentMin);
            }
            break;
        }
        
        if (currentMax!=currentMin){

            if (dataSeriesMin[whichAxis]==dataSeriesMax[whichAxis]){//No range is set yet
                dataSeriesMin[whichAxis] = currentMin;
                dataSeriesMax[whichAxis] = currentMax; 
            }
            else { // Range was set in the last data set, then update as necessary
                if (dataSeriesMin[whichAxis]>currentMin) dataSeriesMin[whichAxis]=currentMin;
                if (dataSeriesMax[whichAxis]<currentMax) dataSeriesMax[whichAxis]=currentMax;
            }
        }
    }
    
    // If after all the datasets are iterated through and still min=max,
    // which can happen if all the numbers are the same (not necessarily 0)
    // then set the min and max at +/- 0.5 away from the center
    if (dataSeriesMin[whichAxis]==dataSeriesMax[whichAxis]) {
        
        dataSeriesMin[whichAxis] -= 0.5;
        dataSeriesMax[whichAxis] += 0.5; 
    }
    
    manualScaleAxis(whichAxis,dataSeriesMin[whichAxis], dataSeriesMax[whichAxis]);
}

void PlotGraph::xyProportionate(int Direction)
{
}

QString PlotGraph::axisTitle(impedance::Column Col, impedance::DataMod Mod)
{
    QString Title = impedance::columnTitle(Col);
    switch (Mod){
    case impedance::None: default:   return (Title);
    case impedance::Neg:     return QString("-%1").arg(Title);
    case impedance::Abs:     return QString("|%1|").arg(Title);
    case impedance::Log:     return QString("log(%1)").arg(Title);
    case impedance::LogNeg:  return QString("log(-%1)").arg(Title);
    case impedance::LogAbs:  return QString("log|%1|").arg(Title);
    case impedance::Sqrt:    return QString("sqrt(%1)").arg(Title);
    case impedance::SqrtNeg: return QString("sqrt(-%1)").arg(Title);
    case impedance::SqrtAbs: return QString("sqrt|%1|").arg(Title);    
    }
}

void PlotGraph::updateAxis(int whichAxis) {
    QtCharts::QValueAxis* Axis = Axes[whichAxis];
    Axis->setRange(axisPlotMin[whichAxis], axisPlotMax[whichAxis]);
    Axis->setTickCount(NofMajUnits[whichAxis]+1);
    Axis->setMinorTickCount(NofMinUnits[whichAxis]-1);
    
    ///Set axis label format
    double maxDisplayNum = std::abs(Axis->max()) > std::abs (Axis->min()) ? 
                           std::abs(Axis->max()) : std::abs (Axis->min());
    if (maxDisplayNum > 10000 || maxDisplayNum < 0.01) Axis->setLabelFormat("%.1E");
    else if (maxDisplayNum>=10) Axis->setLabelFormat("%i");
    else if (maxDisplayNum>=1)  Axis->setLabelFormat("%.1f");
    else Axis->setLabelFormat("%.3f");
    
    Axis->setTitleText(axisTitle(dataCol[whichAxis], dataMod[whichAxis]));
}

void PlotGraph::updateDataSeries() {
    
    /// Remove all series completely;
    chart()->removeAllSeries();
    
    plotExpSeries.clear();
    plotFitSeries.clear();
    
    /// Now add series;
    for (QList<impedance>::iterator iDataSeries = data_source_->begin();
         iDataSeries!=data_source_->end(); iDataSeries++)
    {
        appendSeries(*iDataSeries, Axes[0], Axes[1]);
    }
}


void PlotGraph::appendSeries(const impedance& newImp, 
                             const QtCharts::QValueAxis* XAxis, 
                             const QtCharts::QValueAxis* YAxis){

    if (newImp.isExp()){
        QtCharts::QScatterSeries* newSeries = new QtCharts::QScatterSeries(this);
        plotExpSeries.push_back(newSeries);

        for (int iPt = 0; iPt< newImp.size(); iPt++){
            bool ExcepX=0;
            bool ExcepY=0;
            
            double X = newImp.get(iPt, dataCol[0], dataMod[0],&ExcepX);
            if (ExcepX) continue;
            
            double Y = newImp.get(iPt, dataCol[1], dataMod[1],&ExcepY);
            if (ExcepY) continue;
            
            newSeries->append(X,Y);
        }
        newSeries->setMarkerSize(DEFAULT_PT_SIZE);
        newSeries->setColor(newImp.color());
        newSeries->setPen(QPen(QBrush(QColor(Qt::black)),0.5));
       
        chart()->addSeries(newSeries);
        chart()->setAxisX(Axes[0],newSeries);
        chart()->setAxisY(Axes[1],newSeries);
    }
    else {
        
        QtCharts::QLineSeries* newSeries = new QtCharts::QLineSeries(this);
        plotFitSeries.push_back(newSeries);
        for (int iPt = 0; iPt< newImp.size(); iPt++){
            bool ExcepX=0;
            bool ExcepY=0;
            
            double X = newImp.get(iPt, dataCol[0], dataMod[0],&ExcepX);
            if (ExcepX) continue;
            
            double Y = newImp.get(iPt, dataCol[1], dataMod[1],&ExcepY);
            if (ExcepY) continue;
            
            newSeries->append(X,Y);
        }
        newSeries->setColor(newImp.color());
        
        chart()->addSeries(newSeries);
        chart()->setAxisX(Axes[0],newSeries);
        chart()->setAxisY(Axes[1],newSeries);
        
    }
}

void PlotGraph::Refresh()
{
    autoScaleAxis(0);
    autoScaleAxis(1);
    
    updateDataSeries();
    
    update();
}

void PlotGraph::manualScaleAxis(int whichAxis, double Min, double Max)
{
    // deny any case with min >= max;
    if (Min >= Max) return; 
    
    // Set the pointer to properties of X/Y axis
    double majUnit;
    double minUnit;
    
    // If the axis is in log scale, then apply fixed maj interval of 1;
    if (dataMod[whichAxis] & impedance::Log) {
        majUnit = 1;
        minUnit = 0.5;
        NofMinUnits[whichAxis] = 1;
    }
    
    else { // Deduce the major / minor intervals from the input Min / Max
        // Determine the span of the data, then extract the order and fractions
        double Span = std::abs (Max - Min);
        int orderSpan = floor (log10(Span));
        double fracSpan = Span / pow(10, orderSpan);
        
        double majFrac, minFrac; //Used to store the fraction for maj/min intervals
        if (fracSpan >= 4) {
            majFrac = 1, minFrac=0.2, NofMinUnits[whichAxis]=5;
        } 
        else if (fracSpan>=2) {
            majFrac = 0.5, minFrac=0.1, NofMinUnits[whichAxis]=5;
        }
        else {
            majFrac = 0.2, minFrac=0.1, NofMinUnits[whichAxis]=2;
        }
        majUnit = majFrac * pow(10, orderSpan);
        minUnit = minFrac * pow(10, orderSpan);        
    }
    
    int majorLo = floor (Min/(majUnit)); // Lowest major
    int majorHi = ceil  (Max/(majUnit)); // Highest major
    
    axisPlotMin[whichAxis] = majorLo * (majUnit);
    axisPlotMax[whichAxis] = majorHi * (majUnit);
    
    NofMajUnits[whichAxis] = majorHi - majorLo;  // Span of units

    updateAxis(whichAxis);

}
