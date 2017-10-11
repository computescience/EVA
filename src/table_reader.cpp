/** File name: table_reader.cpp
 ** Last modified: Thu Aug  4 16:55:32 2016
 ** Created by AQN
 ** License: GNU GPLv3
 */

#include "table_reader.h"
#include <sstream>
#include <fstream>

table_reader::table_reader(int Sep, bool MultiAsOne): 
    table_(0),table_int_(0),table_long_(0),table_float_(0),table_double_(0),
    rows_(0),cols_(0)
{
    status_ = 0;
    sep_ = 0;
    multiple_as_one_ = MultiAsOne;
}

table_reader::~table_reader(){}

int table_reader::open(const std::string &FileName, bool Header, 
                       unsigned Skip, unsigned Lines,  
                       bool SkipEmpty){
    status_=0; //Reset the flags
    
    std::ifstream file_in (FileName.c_str());
    if (file_in.fail()) {
        status_ |= FileError;
        return FileError;
    } 
    
    std::string LineBuffer;
    while (Skip-->0){
        std::getline (file_in, LineBuffer);
        if (file_in.eof()) return status_ |= LineError;
    }
    
    /** Read the first line **/
    std::streampos StartPos = file_in.tellg();
    
    std::getline (file_in,LineBuffer);
    if (LineBuffer.size() && LineBuffer[LineBuffer.size()-1] == '\r'){
            LineBuffer.erase(LineBuffer.size()-1); //Erase the '\r' character
    }
    if (!LineBuffer.size()) return status_ |= LineError;
    if (Header) StartPos = file_in.tellg();
    
    /** Process the first line for the number of columns**/
    std::vector <int> index_sep;     
    for (size_t iChar = 0; iChar < LineBuffer.size(); iChar++){
        
        separator CurrentCharType = None;
        if (LineBuffer[iChar] == '\t') CurrentCharType = Tab;
        else if (LineBuffer[iChar] == ',') CurrentCharType = Comma;
        else if (LineBuffer[iChar] == ' ') CurrentCharType = Space;
        
        if (CurrentCharType & sep_) {
            index_sep.push_back(iChar);
            if (multiple_as_one_){
                while (iChar<LineBuffer.size()-1 && (LineBuffer[iChar+1]==LineBuffer[iChar])){
                    LineBuffer.erase(iChar+1,1);
                }
            }
        }
    }
    
    if (!index_sep .size()) return status_ |= LineError;
    cols_ = index_sep.size()+1;
    
    for (size_t iSeg = 0; iSeg<cols_;iSeg++){
        header_.push_back(std::string("Column") + char('0'+iSeg));
    }
    
    if (Header){
        header_[0]= LineBuffer.substr(0, index_sep[0]);
        for (size_t iSeg = 1; iSeg<index_sep.size();iSeg++){
            header_[iSeg]=std::string(LineBuffer, index_sep[iSeg-1]+1, 
                              index_sep[iSeg]-index_sep[iSeg-1]-1);
        }
        
        header_[cols_-1]= LineBuffer.substr(index_sep.back()+1);
    }
    
    /** Read the main data**/
    file_in.seekg(StartPos);
    for (size_t i=0; i<cols_; i++) table_.push_back(std::vector<std::string> (0));
    do{
        getline(file_in, LineBuffer);
        Lines--;
        
        if (LineBuffer.size() && LineBuffer[LineBuffer.size()-1] == '\r'){
                LineBuffer.erase(LineBuffer.size()-1); //Erase the '\r' character
        } 
        
        if (!LineBuffer.size()){
            if (!SkipEmpty) status_ |= LineError;
            continue;
        }
        
        std::vector <int> index_sep_data;  // index_sep_data <= index_sep
        for (size_t iChar = 0; iChar < LineBuffer.size(); iChar++){

            separator CurrentCharType = None;
            if (LineBuffer[iChar] == '\t') CurrentCharType = Tab;
            else if (LineBuffer[iChar] == ',') CurrentCharType = Comma;
            else if (LineBuffer[iChar] == ' ') CurrentCharType = Space;

            if (CurrentCharType & sep_) {
                index_sep_data.push_back(iChar);
                if (multiple_as_one_){
                    while (iChar<LineBuffer.size()-1 && (LineBuffer[iChar+1]==LineBuffer[iChar])){
                        LineBuffer.erase(iChar+1,1);
                    }
                }
            }
        }
        
        if (index_sep_data.size()!=cols_-1) {
            //The number of separators can be smaller OR larger than cols_-1, 
            //both of which cases give LineError
            status_ |= LineError; 
            if (index_sep_data.size()>cols_-1) {
                LineBuffer.erase(index_sep_data[cols_]);
                index_sep_data.resize(cols_-1); 
                //If there are more columns than titles, then the data row is truncated to 
                //the number of columns    
            }
        }
        
        table_[0].push_back (index_sep_data.size() ? LineBuffer.substr(0, index_sep_data[0]) : LineBuffer);
        //If no separator is found, insert the entire LineBuffer
        
        for (size_t col=1; col<cols_; col++) {
            if (col < index_sep_data.size()) {//Inset according to separator
                table_[col].push_back(LineBuffer.substr(
                    index_sep_data[col-1]+1, index_sep_data[col]-index_sep_data[col-1]-1));
            }
            
            else if (col==index_sep_data.size()){//Insert the rest of line buffer
                table_[col].push_back(LineBuffer.substr(index_sep_data.back()+1)); 
            }
            else {//if col>index_sep_data.size()
                table_[col].push_back(std::string());  //empty string
            }
        }
        
    }while (!file_in.eof() && Lines);
    
    rows_ = table_.size()?table_[0].size() : 0;
    table_int_.resize(cols_);
    table_long_.resize(cols_);
    table_float_.resize(cols_);
    table_double_.resize(cols_);
    
    converted_int_.assign(cols_, 0);
    converted_long_.assign(cols_, 0);
    converted_float_.assign(cols_, 0);
    converted_double_.assign(cols_, 0);
    
    return status_;
}


const std::vector<int> table_reader::NULL_I = std::vector<int>(0);
const std::vector<long> table_reader::NULL_L = std::vector<long>(0);
const std::vector<float> table_reader::NULL_F = std::vector<float>(0);
const std::vector<double> table_reader::NULL_D = std::vector<double>(0);

const std::vector <int>& table_reader::i (size_t col){
    if (col>=cols_) return NULL_I;
    if (!(converted_int_ [col])) {
		convert <int> (col, table_int_[col], NA);
		converted_int_ [col]=1;
	}
    return table_int_[col];
}

const std::vector <long>& table_reader::l (size_t col){
    if (col>=cols_) return NULL_L;
    if (!(converted_long_ [col])) {
		convert <long> (col, table_long_[col], NA);
		converted_long_ [col]=1;
	}
    return table_long_[col];
}

const std::vector <float>& table_reader::f (size_t col){
    if (col>=cols_) return NULL_F;
    if (!(converted_float_ [col])) {
		convert <float> (col, table_float_[col], NA);
		converted_float_ [col]=1;
	}
    return table_float_[col];
}

const std::vector <double>& table_reader::d (size_t col){
    if (col>=cols_) return NULL_D;
    if (!(converted_double_ [col])) {
		convert <double> (col, table_double_[col], NA);
		converted_double_ [col]=1;
	}
    return table_double_[col];
}
