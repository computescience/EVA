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
    
    void autoScaleAxis(int whichAxis); // whichAxis: X=0, Y=1;
    void manualScaleAxis (int whichAxis, double Min, double Max); 
        // This sets the axis to at least the range
        // bound by [Min, Max]

    // For debug
    void setGraphName (QString Name) {GraphName=Name;}
    
public slots:
    void Refresh ();
    
private:

    // Widget properties
    int heightForWidth(int w) const override {
        return SquareWidget ? w : -1;
    } 
    
    bool SquareWidget;
    
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
    
    bool square_aspect_ratio_;
    bool X_reverse_;
    bool Y_reverse_;
    
    /// Private Methods
    
    // Enforce the x-y proportionality: Direction:
    // 0 - Keep X and scale Y; 1 - Keep Y and scale X
    void xyProportionate(int Direction);
    
    // Plotting
    void updateAxis (int whichAxis); // To be called after axis settings
    void updateDataSeries(); // To be called when X/Y set is changed
                         // modifies series data
    
    void appendSeries(const impedance& newImp, 
                      const QtCharts::QValueAxis* XAxis, 
                      const QtCharts::QValueAxis* YAxis); // Add a series to the back
    
    /// Constants
    constexpr const static float DEFAULT_FRAC_MARGIN=0.15;
    static const int DEFAULT_PLOT_MARGIN = 10;
    static const int DEFAULT_W = 600;
    static const int DEFAULT_H = 600;
    static const int DEFAULT_PT_SIZE = 5; //Default size of data point
    const QColor DEFAULT_LINE_COL = QColor(0,0,0,255);
    
    // For debug purpose
    QString GraphName;
};

#endif // PLOTGRAPH_H
