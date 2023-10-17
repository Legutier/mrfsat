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
