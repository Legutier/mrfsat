#include "filereader.hpp"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    mrfsat::FileReader reader;
    reader.parseFile(argv[1]);
    return 0;
}
