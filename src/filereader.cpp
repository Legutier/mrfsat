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

#include "filereader.hpp"
#include "parsers/opbparser.hpp"


namespace mrfsat {

std::string FileReader::getFileExtension(const std::string &file_name) {
    size_t pos = file_name.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return file_name.substr(pos + 1);
}

void FileReader::parseFile(std::string file_name) {
    if (getFileExtension(file_name) == "opb") {
        parseOPBFile(file_name);
    } else {
        std::cout << "Error: File extension not supported." << std::endl;
        return;
    }
    return;
}

void FileReader::parseOPBFile(std::string file_name) {
    std::string line_stream;
    std::ifstream file_stream(file_name);
    if (!file_stream.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }
    OPBParser parser;
    parser.parseFile(file_stream);
    return;
}
}
