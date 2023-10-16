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
