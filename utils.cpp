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
            throw std::invalid_argument("Job description file is empty!");
        }
        return ss.str();
    }

    bool isSpace(char ch){
        if(isspace(ch) && ch!='\n'){
            return true;
        }
        return false;
    }
}
