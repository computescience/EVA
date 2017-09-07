#ifndef PLOTDUALGRAPH_H
#define PLOTDUALGRAPH_H
#include "plotgraph.h"

class PlotDualGraph : public PlotGraph
{
public:
    PlotDualGraph(QList <impedance>* dataSource, 
                  int Width, int Height, 
                  QWidget* parent=0);
    
    void setZData(impedance::Column Col, 
                  impedance::DataMod Mod = impedance::None){
        dataCol[2] = Col;
        dataMod[2] = Mod;
    }
    
    void setDualPlot(bool dual=1){
        Axes[2]->setVisible(dual);
    }
    
public slots:
    void Refresh() override;
    
private:
    void updateSecondaryDataSeries();
    //bool displaySecondary;

    QList<QtCharts::QScatterSeries *> plotSecondaryExpSeries;
    QList<QtCharts::QLineSeries *>    plotSecondaryFitSeries;
    
};

#endif // PLOTDUALGRAPH_H
