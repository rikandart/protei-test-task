#include "parser.h"

int Parser::_splitString(const std::string& str, const char del){
    std::string substr = str;
    bool havetoBreak = false;
    for(size_t pos = substr.find_first_of(del); !havetoBreak;
        substr = substr.substr(pos+1), pos = substr.find_first_of(del)){
        if(pos == std::string::npos){
            havetoBreak = true;
        }
        try{
            int number = std::stoi(substr);
            _result.push_back(number);
        } catch (const std::invalid_argument& e){
            continue;
        }
    }
    return _result.size();
}

bool Parser::parse(const std::string& data, const char del, std::string& output){
    int splitRes = _splitString(data, del);
    if(!splitRes){
        output = "No numbers specified";
        return false;
    }
    output.clear(); 
    int sum = 0;
    for(int i : _result){
        output += std::to_string(i) + " ";
        sum += i;
    }
    output += "\n" + std::to_string(sum);
    _result.clear();
    return true;
}