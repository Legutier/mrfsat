#include "opbparser.hpp"

namespace mrfsat {

void OPBParser::parseFile(std::ifstream &file_name) {
    std::string line_stream;
    while (std::getline(file_name, line_stream)) {
        line_stream.erase(std::remove(line_stream.begin(), line_stream.end(), ' '), line_stream.end());
        if(line_stream[0] == '*') {
            continue;
        }
        getEquations(line_stream);
    }
}

void OPBParser::getEquations(std::string &line) {
    //<equations> ::= <equation> | <equation> <equations>
    std::cout << "getEquations" << std::endl;
    getEquation(line);
}

void OPBParser::getEquation(std::string &line) {
    //<equation> ::= <terms> ">=" <integer> ";"
    std::cout << "getEquation" << std::endl;
    getTerms(line);
    if (line[0] == '>') {
        line.erase(0, 2);
    }
    getInteger(line);
    if (line[0] == ';') {
        line.erase(0, 1);
    }
}
}
