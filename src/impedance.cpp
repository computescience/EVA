#include "impedance.h"
#include <cmath>

impedance::impedance(int dataSize):
    data_(  NOF_TYPE, QVector<QVector<double> > (
            NOF_MOD, QVector<double> (dataSize, 0))),
    extremes_(NOF_TYPE, QVector<double> (NOF_EXTREME,0))
{
    size = dataSize;
}

impedance::impedance(const std::vector<double> &Freq,
                     const std::vector<double> &Zr,
                     const std::vector<double> &Zi):
    //A three-layered initialization call
    data_(  NOF_TYPE, QVector<QVector<double> > (
            NOF_MOD, QVector<double> (Freq.size(), 0))),
    extremes_(NOF_TYPE, QVector<double> (NOF_EXTREME,0))

{
    size_=Freq.size();

    for (int iType =0; iType<NOF_TYPE; iType++) {
        data_[iType].reserve(NOF_MOD);
    }
    
    for (int iFr = 0; iFr<size_; iFr++){
        //Unsafe, must ensure externally that Zreal and Zimag has no smaller size than Freq
        data_[Fr][0][iFr]= Freq [iFr];
        data_[Zreal][0][iFr]= Zr[iFr];
        data_[Zimag][0][iFr]= Zi[iFr];
    }
    
    validate_();
    visible_=1;
    isExp_=1;
    
}

void impedance::resize(int size) {
    size_=size; 
    for (int iType = 0; iType<NOF_TYPE;iType++){
        for (int iMod = 0; iMod<NOF_MOD;iMod++){
            data_[iType][iMod].resize(size);
        }
    }
}

int impedance::validate_(){
    if (!size_) return 0;
    
    int errors = 0;
    for (int iFr=0; iFr<size_; iFr++){
        while ((data_[Fr][0][iFr]<=0 || //Screen out lines Freq[iFr]==0 or Zabs[abs]==0
                data_[Zreal][0][iFr]*data_[Zreal][0][iFr]+
                data_[Zimag][0][iFr]*data_[Zimag][0][iFr]==0)){
            errors++;
            data_[Fr][0].remove(iFr,1);
            data_[Zreal][0].remove(iFr,1);
            data_[Zimag][0].remove(iFr,1);
        }
    }
    
    size_ = data_[0][0].size();
    
    for (int iTy=0; iTy<3; iTy++){ 
        // Initialize the modified arrays of the first 3 cols
        // Use fill() will also initialize the value
        data_[iTy][1].fill(0, size_); 
        data_[iTy][2].fill(0, size_);
    }

    for (int iTy =3; iTy<NOF_TYPE; iTy++){ //Initialize the rest of the array
        data_[iTy][0].fill(0, size_);
        data_[iTy][1].fill(0, size_);
        data_[iTy][2].fill(0, size_);
    }
    
    /// Update the derived data 
    for (int iFr =0; iFr<size_; iFr++){
        
        double Zsqr = data_[Zreal][0][iFr]*data_[Zreal][0][iFr] 
                    + data_[Zimag][0][iFr]*data_[Zimag][0][iFr];
        
        data_[Zabs ][0][iFr] = sqrt(Zsqr);
        data_[Phi  ][0][iFr] = atan2(data_[Zimag][0][iFr], data_[Zreal][0][iFr]) / PI * 180;
        data_[Yreal][0][iFr] =  data_[Zreal][0][iFr] / Zsqr;
        data_[Yimag][0][iFr] = -data_[Zimag][0][iFr] / Zsqr;
        data_[Yabs ][0][iFr] = sqrt(data_[Yreal][0][iFr]*data_[Yreal][0][iFr] + 
                                    data_[Yimag][0][iFr]*data_[Yimag][0][iFr]);
        data_[Psi  ][0][iFr] = atan2(data_[Yimag][0][iFr], data_[Yreal][0][iFr]) / PI * 180;
        data_[Capacitance][0][iFr] = (data_[Zimag][0][iFr] == 0) ? 0 :
                                     -1/(data_[Fr][0][iFr]*2*PI*data_[Zimag][0][iFr]);
    }
    
    /// Update the modifiers
    for (int iTy=0; iTy<NOF_TYPE;iTy++){
        for (int iFr=0; iFr<size_;iFr++){
            data_[iTy][1][iFr] = (data_[iTy][0][iFr]==0) ? 0 : log10(std::abs(data_[iTy][0][iFr]));
            data_[iTy][2][iFr] = sqrt (std::abs(data_[iTy][0][iFr]));
        }
    }
    
    /// Update the extremes
    for (int iTy=0; iTy<NOF_TYPE;iTy++){ 
        extremes_[iTy].fill(0,NOF_EXTREME); //Initialize with 0
        
        for (int iFr=0; iFr<size_;iFr++){
            
            double& value = data_[iTy][0][iFr];
            double& pos_min= extremes_[iTy][1];
            double& pos_max= extremes_[iTy][2];
            double& neg_min= extremes_[iTy][3];
            double& neg_max= extremes_[iTy][4];
            
            if (value>0){
                if (value>pos_max) pos_max=value;//pos_max
                
                if (pos_min==0) pos_min=value;
                else if (value<pos_min) pos_min=value;//pos_min
                
            }
            else if (value<0){
                if ((-value)>neg_max) neg_max = -value;
                
                if (neg_min==0) neg_min = -value;
                else if ((-value)<neg_min) neg_min = -value;
            }
            
            else{
                extremes_[iTy][0]=1;
            }    
        }
    }
    
    return errors;
    
}

// Column title initialized here
QVector <QString> impedance::col_title (impedance::NOF_TYPE);

void impedance::initializeColumnTitle(){
    //col_title.resize(impedance::NOF_TYPE);
    col_title[Fr]       = QString ("Freq");
    col_title[Zreal]    = QString ("Zreal");
    col_title[Zimag]    = QString ("Zimag");
    col_title[Zabs]     = QString ("Zabs"); 
    col_title[Phi]      = QString ("φ(Z)°");
    col_title[Yreal]    = QString ("Yreal");
    col_title[Yimag]    = QString ("Yimag");
    col_title[Yabs]     = QString ("Yabs"); 
    col_title[Psi]      = QString ("ψ(Y)°");
    col_title[Capacitance]=QString("Cap");
}
