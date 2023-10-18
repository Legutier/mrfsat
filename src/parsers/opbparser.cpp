/*
    MRFSAT - Copyright (C) 2023  Lukas Esteban Gutierrez Lisboa

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "opbparser.hpp"


namespace mrfsat {

void OPBParser::parseFile(std::ifstream &file_name) {
    std::string line_stream;
    while (std::getline(file_name, line_stream)) {
        //<equations>  ::= <equation> | <equation> <equations>
        line_stream.erase(std::remove(line_stream.begin(), line_stream.end(), ' '), line_stream.end());
        if(line_stream[0] == '*') {
            continue;
        }
        stop = -1;
        getEquation(line_stream);
    }
}

void OPBParser::getEquation(std::string &line) {
    //<equation> ::= <terms> <comparator> <integer> ";"
    getTerms(line);
    getComparator(line);
    std::cout << " | ";
    int rhc = getInteger(line);
    std::cout << rhc << std::endl;
    if (line[++stop] != ';') throw std::invalid_argument("Syntax error: expected ;"); 
}

int OPBParser::getComparator(std::string &line) {
    //<comparator>   ::= "=" | ">="
    if (line[++stop] == '=') {
        return 0;
    } else if (line[stop] == '>' && line[stop + 1] == '=') {
        stop++;
        return 0;
    } else {
        stop--;
        throw std::invalid_argument("No support for non-linear constraints"); 
    }
}

int OPBParser::getTerms(std::string &line) {
    //<terms> ::= <term> | <term> <terms>
    int term = 0;
    while (term == 0) {
        term = getTerm(line);
    }
    return 0;
}

int OPBParser::getTerm(std::string &line) {
    //<term> ::= <sign> <integer> "x" <integer>
    try {
        int sign = getSign(line);
        int coefficient = getInteger(line);
        std::cout << ((sign > 0)? '+': '-') << coefficient; 
        if (line[++stop] != 'x') throw std::invalid_argument("Syntax error: Term must have variable x.");
        int variable = getInteger(line);
        std::cout << "x" << variable ;
        return 0;
    } catch (std::out_of_range const& ex) {
        return 1;
    }
}

int OPBParser::getSign(std::string &line) {
    //<sign> ::= "+" | "-"
    int sign = 1;
    if (line[++stop] == '-'){
        sign = -1;
    } else if (line[stop] == '+'){
        sign = 1;
    } else {
        stop --;
        throw std::out_of_range("No support for non-linear constraints. Expected +/-");
    }
    return sign;
}

int OPBParser::getInteger(std::string &line) {
    //<integer> ::= <digit> | <digit> <integer>
    int digit = 0;
    int number = 0;
    while (digit != -10) {
        digit = getDigit(line);
        if (digit != -10) {
            number = number * 10 + digit;
        }
    }
    return number;
}

int OPBParser::getDigit(std::string &line) { 
    //<digit> ::= "0" | "1" | "2" | ... | "9"
    std::set<char> digits = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    if (digits.find(line[++stop]) != digits.end()) {
        return (int)(line[stop] - '0');
    }
    stop--;
    return -10;
}

}
