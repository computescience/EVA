#include "plotdualgraph.h"


PlotDualGraph::PlotDualGraph(QList<impedance> *dataSource, 
                             int Width, int Height, QWidget *parent):
    PlotGraph(dataSource, Width, Height, parent)
{
    Axes.resize(3);
    dataCol.resize(3);
    dataMod.resize(3);
    dataSeriesMin.resize(3);
    dataSeriesMax.resize(3);
    axisPlotMin.resize(3);
    axisPlotMax.resize(3);
    NofMajUnits.resize(3);
    NofMinUnits.resize(3);
    
    Axes[2] = new QtCharts::QValueAxis(this);
    Axes[2]->setGridLineVisible(0);
    chart()->addAxis(Axes[2], Qt::AlignRight);
    Axes[2]->setVisible(0);    
}

void PlotDualGraph::Refresh()
{
    autoScaleAxis(0);
    autoScaleAxis(1);
    if (Axes[2]->isVisible()) autoScaleAxis(2);
    
    updateDataSeries();
    if (Axes[2]->isVisible()) updateSecondaryDataSeries();
    
    update();
}

void PlotDualGraph::updateSecondaryDataSeries()
{
    /// Similar to updateDataSeries(), except that 
    /// it does not clear the data series on the chart 
    
    plotSecondaryExpSeries.clear();
    plotSecondaryFitSeries.clear();
    
    for (QList<impedance>::iterator iDataSeries = data_source_->begin();
         iDataSeries!=data_source_->end(); iDataSeries++)
    {
        appendSeries(*iDataSeries, 1, 2, QtCharts::QScatterSeries::MarkerShapeRectangle);
    }
}
