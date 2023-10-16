#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

namespace mrfsat {
class OPBParser {
    /*
        BNF Grammar for OPB files
        <equations>    ::= <equation> | <equation> <equations>
        <equation>     ::= <terms> ">=" <integer> ";"
        <terms>        ::= <term> | <term> <terms>
        <term>         ::= <sign> <integer> "x" <integer>
        <sign>         ::= "+" | "-"
        <integer>      ::= <digit> | <digit> <integer>
        <digit>        ::= "0" | "1" | "2" | ... | "9"
        <comment>      ::= "*"
    */
    public:
        void parseFile(std::ifstream &file_name);
    private:
        void getEquations(std::string &line);
        void getEquation(std::string &line);
        void getTerms(std::string &line);
        void getTerm(std::string &line);
        void getSign(std::string &line);
        int getInteger(std::string &line);
        void getDigit(std::string &line);
};
}
