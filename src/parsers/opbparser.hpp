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
