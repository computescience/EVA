#ifndef IMPEDANCE_H
#define IMPEDANCE_H

#include <QVector>
#include <QString>
#include <QColor>
#include <cmath>

#include <vector>

const double PI = 3.1415926535898;

class impedance
{
public:

    //impedance(int dataSize); //Create an uninitialized array
    impedance (const std::vector<double>& Freq,
               const std::vector<double>& Zr,
               const std::vector<double>& Zi);
    //must ensure externally that Zreal and Zimag has no smaller dimension than Freq
    
    ~impedance(){}
    
    /// Public Types 
    enum Column {Fr, Zreal, Zimag, Zabs, Phi, Yreal, Yimag, Yabs, Psi, Capacitance,
                 NOF_TYPE};

    // The last element NOF_TYPE records the number of columns available. It should not
    // be used as an enumerated value
    
    enum DataMod {None=0, Neg=1, Abs=2,
                  Log =4, LogNeg=5, LogAbs=6, 
                  Sqrt=8, SqrtNeg=9, SqrtAbs=10};
    
    /// Constants

    const static int NOF_MOD = 3;
    const static int NOF_EXTREME = 5; //zero, pos_min, pos_max, neg_min,neg_max
    
    // Using a bracket operator to directly extract the top-level data (type of data)
    inline const QVector<QVector <double> >& operator [] (Column Col) const{return data_[Col];}
    
    // Value extractor
    // Use Num to extract the number, use Mod to specify which modification is applied. 
    // Excep is used to record exceptions: log of negative or 0, or sqrt negative
    inline double get (int Num, Column Col, DataMod Mod=None, bool* Excep=NULL) const {
        switch (Mod) {
        case None: 
            return data_[Col][0][Num];
        case Neg : 
            return -data_[Col][0][Num];
        case Abs : 
            return std::abs(data_[Col][0][Num]); 
        case Log : 
            if (data_[Col][0][Num]<=0 && Excep!=NULL) *Excep=1;
            return data_[Col][1][Num];
        case LogNeg:
            if (data_[Col][0][Num]>=0 && Excep!=NULL) *Excep=1;
            return data_[Col][1][Num];
        case LogAbs:
            if (data_[Col][0][Num]==0 && Excep!=NULL) *Excep=1;
            return data_[Col][1][Num];
        case Sqrt:
            if (data_[Col][0][Num]< 0 && Excep!=NULL) *Excep=1;
            return data_[Col][2][Num];
        case SqrtNeg:
            if (data_[Col][0][Num]> 0 && Excep!=NULL) *Excep=1;
            return data_[Col][2][Num];
        case SqrtAbs:
            return data_[Col][2][Num];
        }
        return 0;
    }
    
    
    /// Methods
    void resize(int size);
    inline int size() const {return size_;}
    void rename(QString newName) {name_ = newName;}
    QString dataName () const {return name_;}
    
    inline void setF(int i, double freq) {if (i<size_) data_[Fr   ][0][i] = freq;}
    inline void setR(int i, double Zre ) {if (i<size_) data_[Zreal][0][i] = Zre;}
    inline void setI(int i, double Zim ) {if (i<size_) data_[Zimag][0][i] = Zim;}
    
    /* Tell the extremes of each type of data */
    
    inline double HasZero (Column dataCol) const {return extremes_[dataCol][0];}
    inline double PosMin  (Column dataCol) const {return extremes_[dataCol][1];}
    inline double PosMax  (Column dataCol) const {return extremes_[dataCol][2];}
    inline double NegMin  (Column dataCol) const {return extremes_[dataCol][3];}
    inline double NegMax  (Column dataCol) const {return extremes_[dataCol][4];}
    
    /** Set and retrieve information related to data set**/
    inline bool isExp() const {return isExp_;}
    
    inline bool isVisible() const {return visible_;}
    inline void setVisible(bool Visible) {visible_=Visible;}
    
    void setColor (const QColor& color) {color_ = color;}
    const QColor& color() const {return color_;}
    
    /// Dealing with the column title
    /// Titles must be initialized before any instance is created
    /// Initialization is called in MainWindow
    static QString columnTitle (Column Col) {return col_title[Col];}
    static void initializeColumnTitle ();
    
private:
    int size_;
    
    QVector <QVector<QVector<double> > > data_;  
    //A 3-D array to store the data:
    //Top dimension: type of data: Fr, Zr, Zi etc;
    //Mid dimension: modification of data: vector (3): None, Log, Sqrt;
    //Bottom dimension: data series;
    
    QVector <QVector<double> > extremes_; 
    //A 2-D array to store the extremes of the data;
    //Top dimension: type of data;
    //Bottom dimension: {zero, pos_min, pos_max, neg_min, neg_max};
    //The "zero" entry in the bottom dimensin allows only a boolean (0/1)
    //"zero" means this column contains 0

    static QVector <QString> col_title;
    
    /** Properties **/
    bool isExp_;   // Wheter the data come from experimental (as opposed to fitted/simulated)
    QString name_; // Stores the data name
    bool visible_; // Whether the data is visible
    QColor color_; // Assign a color representing this series
    
    int validate_();

};

#endif // IMPEDANCE_H
