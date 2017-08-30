/** File name: table_reader.h
 ** Last modified: Thu Aug  4 16:55:32 2016
 */

#ifndef TABLE_READER_H 
#define TABLE_READER_H

#include <vector>
#include <string>
#include <sstream>

class table_reader {
	
public:
	table_reader(int Sep = Tab|Comma, bool MultiAsOne=0);
	~table_reader();
	
	int open (const std::string& FileName, 
              bool Header=0, unsigned Skip=0, unsigned Lines=-1, bool SkipEmpty = 1);
    
    /** Enums **/
	enum separator{None=0,Tab = 0x1, Comma = 0x1, Space = 0x4};
	enum error {FileError = 0x1, LineError = 0x2, ParseError = 0x4};
    
    /** Get a column of values in strings **/
    const std::vector<std::string>& operator [] (size_t n) const {return table_[n];} 
    
    /** Extract data **/
    const std::vector <int>& i (size_t col);
    const std::vector <long>& l (size_t col);
    const std::vector <float>& f (size_t col);
    const std::vector <double>& d (size_t col);
    
    void setMultipleAsOne() {multiple_as_one_ = 1;}
    bool MultipleAsOne () const {return multiple_as_one_;}
    
    inline int errorStatus () const {return status_;}
    inline void resetError() {status_=0;}
    
    inline const std::vector <std::string>& header () const {return header_;}
    
    /** Tell the dimensions of the table**/
    inline int rows () const {return rows_;}
    inline int cols () const {return cols_;}
    
private:
	std::vector <std::vector<std::string> > table_;
	std::vector <std::vector<int> > table_int_;
    std::vector <std::vector<long> > table_long_;
	std::vector <std::vector<float> > table_float_;
	std::vector <std::vector<double> > table_double_;
    
    size_t rows_;
    size_t cols_;
	
    bool multiple_as_one_;
	int status_;
    int sep_;
    
    std::vector <std::string> header_;
    std::vector <bool> converted_int_;
    std::vector <bool> converted_long_;
    std::vector <bool> converted_float_;
    std::vector <bool> converted_double_;
    
    /** Returned value for out-of-bound columns**/
    static const std::vector<int> NULL_I;
    static const std::vector<long> NULL_L;
    static const std::vector<float> NULL_F;
    static const std::vector<double> NULL_D;
    /** Default for empty values **/
    const static int NA = 0;
	
    /** Methods **/
    template <typename T> 
    void convert(size_t col, std::vector <T>& values, T MISSING_VAL) {
        //col is assumed safe
        values.assign(rows_, MISSING_VAL);
        for (int row = 0; row<rows_; row++){
            std::stringstream s (table_ [col] [row]);
            s >> values [row];
            if (s.fail()) status_ |= ParseError;
        }
    }
};
#endif
