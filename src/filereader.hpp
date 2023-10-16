#pragma once
#include <iostream>
#include <fstream>
#include <string>


namespace mrfsat {
class FileReader {
    public:
        void parseFile(std::string file_name);
    private:
        void parseOPBFile(std::string file_name);
        std::string getFileExtension(const std::string& filename);
};
}