#pragma once
#include <string>
#include <vector>
#include <iostream>

#define MAX_NUMBERS 100

class Parser {
    std::vector<int> _result;
    // splitting strings by delimeter
    // fills _result
    // returns number of splitted values
    int _splitString(const std::string& str, const char del);
public:
    Parser(){
        _result.reserve(MAX_NUMBERS);
    };
    bool parse(const std::string& data, const char del, std::string& output);
};