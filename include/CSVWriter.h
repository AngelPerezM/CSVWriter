#ifndef CSVWRITER_H
#define CSVWRITER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <typeinfo>


class CSVWriter
{
    public:
        CSVWriter(){
            this->firstRow = true;
            this->seperator = ";";
            this->columnNum = -1;
            this->valueCount = 0;
        }

        CSVWriter(int numberOfColums){
            this->firstRow = true;
            this->seperator = ";";
            this->columnNum = numberOfColums;
            this->valueCount = 0;
        }

        CSVWriter(const std::string& seperator){
            this->firstRow = true;
            this->seperator = seperator;
            this->columnNum = -1;
            this->valueCount = 0;
        }

        CSVWriter(const std::string& seperator, int numberOfColums){
            this->firstRow = true;
            this->seperator = seperator;
            this->columnNum = numberOfColums;
            this->valueCount = 0;
        }

        CSVWriter& add(const char *str){
            return this->add(std::string(str));
        }

        CSVWriter& add(char *str){
            return this->add(std::string(str));
        }

        CSVWriter& add(std::string str){
            //if " character was found, escape it
            size_t position = str.find("\"",0);
            bool foundQuotationMarks = position != std::string::npos;
            while(position != std::string::npos){
                str.insert(position,"\"");
                position = str.find("\"",position + 2);
            }
            if(foundQuotationMarks){
                str = "\"" + str + "\"";
            }else if(str.find(this->seperator) != std::string::npos){
                //if seperator was found and string was not escapted before, surround string with "
                str = "\"" + str + "\"";
            }
            return this->add<std::string>(str);
        }

        template<typename T>
        CSVWriter& add(T str){
            if(this->columnNum > -1){
                //if autoNewRow is enabled, check if we need a line break
                if(this->valueCount == this->columnNum ){
                    this->newRow();
                }
            }
            if(valueCount > 0)
                this->ss << this->seperator;
            this->ss << str;
            this->valueCount++;

            return *this;
        }

        template<typename T>
        CSVWriter& operator<<(const T& t){
            return this->add(t);
        }

        void operator+=(CSVWriter &csv){
            this->ss << std::endl << csv;
        }

        std::string toString(){
            return ss.str();
        }

        friend std::ostream& operator<<(std::ostream& os, CSVWriter & csv){
            return os << csv.toString();
        }

        CSVWriter& newRow(){
            if(!this->firstRow || this->columnNum > -1){
                ss << std::endl;
            }else{
                //if the row is the first row, do not insert a new line
                this->firstRow = false;
            }
            valueCount = 0;
            return *this;
        }

        bool writeToFile(const std::string& filename){
            return writeToFile(filename,false);
        }

        bool writeToFile(const std::string& filename, bool append){
            std::ofstream file;
            bool appendNewLine = false;
	        bool success = false;
            if (append) {
                //check if last char of the file is newline
                std::ifstream fin;
                fin.open(filename);
                if (fin.is_open()) {
                    fin.seekg(-1, std::ios_base::end); //go to end of file
                    int lastChar = fin.peek();
                    if (lastChar != -1 && lastChar != '\n') //if file is not empry and last char is not new line char
                        appendNewLine = true;
                }
                file.open(filename.c_str(), std::ios::out | std::ios::app);
            } else {
                file.open(filename.c_str(), std::ios::out | std::ios::trunc);
            }
            if(!file.is_open())
                return false;
            if(append && appendNewLine)
                file << std::endl;
            file << this->toString();
            file.close();
            success = file.good();

            if (success) {
                this->ss.str("");
                this->firstRow = true;
            }
            return success;
        }

        void enableAutoNewRow(int numberOfColumns){
            this->columnNum = numberOfColumns;
        }

        void disableAutoNewRow(){
            this->columnNum = -1;
        }
    protected:
        std::string seperator;
        int columnNum;
        int valueCount;
        bool firstRow;
        std::stringstream ss;

};

#endif // CSVWRITER_H
