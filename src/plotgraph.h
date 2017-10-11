#ifndef PLOTGRAH_H
#define PLOTGRAH_H

#include <QColor>
#include <QList>
#include <QVector>
#include <QMargins>
#include <QMessageBox>

#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include "impedance.h"


class PlotGraph : public QtCharts::QChartView
{

    
public:
    // Constructors
    explicit PlotGraph(QList <impedance>* dataSource, 
                       int Width, int Height, 
                       QWidget* parent=0);
    virtual ~PlotGraph() {} 
    // Virtual destructor not really implemented, do not delete!

    // Widget setting
    void setSquareWidget (bool square=1){
        SquareWidget = square;
    }
    
    // Setters
    
    void setXData (impedance::Column Col,
                   impedance::DataMod Mod=impedance::None) {
        dataCol[0] = Col;
        dataMod[0] = Mod;
    }
    
    void setYData (impedance::Column Col,
                   impedance::DataMod Mod=impedance::None) {
        dataCol[1] = Col;
        dataMod[1] = Mod;
    }
    
    void useSeriesColor (bool enable=1) {
        usingSeriesColor=enable;
    }
    
    void autoScaleAxis(int whichAxis); // whichAxis: X=0, Y=1;
    void manualScaleAxis (int whichAxis, double Min, double Max); 
        // This sets the axis to at least the range
        // bound by [Min, Max]

    // For debug
    void setGraphName (QString Name) {GraphName=Name;}
    void setSymbolSize(int value) {symbolSize = value;}
    void setSymbolLineWidth(double value) {symbolLineWidth = value;}
    
public slots:
    virtual void Refresh ();
    
protected:

    // Widget properties
    int heightForWidth(int w) const override {
        return SquareWidget ? w : -1;
    } 
    
    bool SquareWidget;
    bool usingSeriesColor;
    
    
    // Data source
    QList<impedance>* data_source_;    //List of data to plot

    // Axes properties
    QVector <QtCharts::QValueAxis*> Axes;
    QVector <impedance::Column>  dataCol;
    QVector <impedance::DataMod> dataMod;
    QVector <double> dataSeriesMin;
    QVector <double> dataSeriesMax;
    QVector <double> axisPlotMin;
    QVector <double> axisPlotMax;
    QVector <int> NofMajUnits;
    QVector <int> NofMinUnits;
    QString axisTitle(impedance::Column Col, impedance::DataMod Mod);
    
    // Plot series on the graph
    QList<QtCharts::QScatterSeries *> plotExpSeries;
    QList<QtCharts::QLineSeries *> plotFitSeries;
    
    // Graphic properties
    
    QVector <float> fracMargin;  //Fractions of margin {Left, Top, Right, Bottom}
    QMargins Mgn;
    
    int symbolSize;
    double symbolLineWidth;
    
    bool square_aspect_ratio_;
    
    /// Private Methods
    
    // Enforce the x-y proportionality: Direction:
    // 0 - Keep X and scale Y; 1 - Keep Y and scale X
    void xyProportionate(int Direction);
    
    // Plotting
    void updateAxis (int whichAxis); // To be called after axis settings
    void updateDataSeries(); // To be called when X/Y set is changed
                         // modifies series data
    
    void appendSeries(const impedance& newImp, 
                      int HAxis, 
                      int VAxis,
                      QtCharts::QScatterSeries::MarkerShape shape); // Add a series to the back
    
    /// Constants
    constexpr const static float DEFAULT_FRAC_MARGIN=0.15;
    static const int DEFAULT_PLOT_MARGIN = 10;
    static const int DEFAULT_W = 600;
    static const int DEFAULT_H = 600;

    const QColor DEFAULT_LINE_COL = QColor(0,0,0,255);
    const QtCharts::QScatterSeries::MarkerShape SERIES_SHAPE = 
            QtCharts::QScatterSeries::MarkerShapeCircle;
    // For debug purpose
    QString GraphName;
};

#endif // PLOTGRAPH_H
