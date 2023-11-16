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

#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <set>
#include "graph.hpp"

namespace mrfsat {
class OPBParser {
    /*
        BNF Grammar for OPB files
        <equations>    ::= <equation> | <equation> <equations>
        <equation>     ::= <terms> <comparator> <integer> ";"
        <comparator>   ::= "=" | ">="
        <terms>        ::= <term> | <term> <terms>
        <term>         ::= <sign> <integer> "x" <integer>
        <sign>         ::= "+" | "-"
        <integer>      ::= <digit> | <digit> <integer>
        <digit>        ::= "0" | "1" | "2" | ... | "9"
        <comment>      ::= "*"
    */
    public:
        OPBParser(Graph& g) : graph(g) {
            line_number = 1;
            max_variable_id = 0;
        }
        void parseFile(std::ifstream &file_name);
    private:
        // element parsers
        void getEquation(std::string &line);
        int getTerms(std::string &line);
        int getTerm(std::string &line);
        int getSign(std::string &line);
        int getInteger(std::string &line);
        int getComparator(std::string &line);
        int getDigit(std::string &line);
        // parsing helpers
        int stop;
        int line_number;
        // graph
        Graph& graph;
        int max_variable_id;
};
}
