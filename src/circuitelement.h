#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QFile>
#include <QString>
#include <QVector>
#include <QImage>

#include <complex>

#include <cmath>

struct token {
    enum tokenType{Null=1024, Numeric, Complex, Variable, Function, Operator};
    // Null for passing error message or simply setu
    
    enum operType {OpAdd, OpMin, OpMul, OpDiv, OpPow, LBra, RBra};
    //Means operations: add, minus, muliply, divide, power to, left, right bracket
    
    constexpr static const double Pi = 3.14159265359;

    static const char* const stdFun [12]; // standard functions

    static int isStdFunc (const QString& s){
        for (int i=0; i<12; i++) {
            if (s == QString(stdFun[i])) return i;
        }
        return -1;
    }
    
    token() {Type=Null;}
    token(double  V) : v(V) {Type=Numeric;}
    token(QString S) : s(S) {Type=Variable;}
    token(int     F) : f(F) {Type=Function;}
    token(std::complex<double> Z)   : z(Z) {Type=Complex;}
    token(operType O, int Prec, bool RightAss=0) : 
        o(O), opPrec(Prec), rightAss(RightAss){Type = Operator;}
      
    
    tokenType Type;
    double       v;  // value
    QString      s;  // string
    operType     o;  // operator
    int          f;  // function
    std::complex<double> z;  // complex representation    
    
    int opPrec;      // precedence of operator
    bool rightAss;   // right associativity of operator
    
}; 

class CircuitElement
{    
public:

    CircuitElement() {}
    CircuitElement(QFile* fromXML);
   
    /// Setters and getters

    QString symbol() const {return Symbol;}
    
    void setID (QString id) {ID = id;}
    QString id () const {return ID;}
    
    QString fullName(QString sep=".") const {
        return Symbol+sep+ID;
    }
    
    int varCount () const {return VarVal.size();}
    
    void fixVariable (int v, bool fixed=1) {VarFixed[v]=fixed;}
    bool varFixed (int v) const {return VarFixed.at(v);}
    
    QString varName(int v) const {return VarNames.at(v);}
    
    void set (int v, double val) {VarVal [v] = val;}
    double var (int v) const {return VarVal.at(v);}
    
    void setMin (int v, double min) {VarMin [v] = min;}
    double min (int v) const {return VarMin.at(v);}
    
    void setMax (int v, double max) {VarMax [v] = max;}
    double max (int v) const {return VarMax.at(v);}
    
    QString iconName () const {return IconName;}
    
    bool importSucceeded() const {return ImportSuccess;}
    QString importError()const {
        return ImportSuccess ? QString() :importErrorMsg;
    }
    
    QString toRPN() const;
    
private:
    
    /// Identity
    QString ElemType; // Such as "resistor", "capacitor"
    QString Symbol;
    QString ID;       // Such as "ct", "dl"
    QString Ref;      // Provide a reference for this type
    QString Doi;
    QString IconName;
    QString Description;
    
    /// Store the expressions (as vector of tokens)
    QVector <token> formula; // tokens in Reverse-Polish Notation
    QVector <QVector <token> > d;  // 1st-order derivatives
    QVector <QVector <token> > d2; // 2nd-order derivatives
    
    /// Content
    QVector <QString> VarNames; // Variable names
    QVector <double>  VarVal  ; // Variable values
    QVector <double>  VarMin  ; // Minima
    QVector <double>  VarMax  ; // Maxima
    QVector <bool>    VarFixed; 
    
    /// For importing from XML
    QString importErrorMsg;
    bool ImportSuccess;

};

#endif // CIRCUITELEMENT_H

