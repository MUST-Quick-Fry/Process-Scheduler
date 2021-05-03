#include "project.h"
#include <cctype>
#include <sstream>
#include <fstream>
#include <stdexcept>

namespace Utils{
    std::string readFile(const char* fileName){
    std::stringstream ss;
    std::ifstream file(fileName,std::ios::binary);
	ss<<file.rdbuf(); // read stream buffer
            
	if(ss.str().empty()){
            throw std::invalid_argument("File Error: Job description file cannot be found or an empty file!");
        }
        
        return ss.str();
    }

    bool isSpace(char ch){
        if(isspace(ch) && ch!='\n'){
            return true;
        }
        return false;
    }
      
    bool arg_check(char* str){
        try{
            std::string tmp(str);
            int a = std::stoi(tmp);
        }
        catch (std::exception& e){
            throw std::invalid_argument("Arguement Error: Argument is NaN or an overbound number!");
        }
        return true;
    }
    
}
