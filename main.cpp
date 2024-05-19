#include <iostream>
#include "lib/ComputerClubInfo.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "urequired parameters were not entered" << std::endl;
        return 1;
    }
    ComputerClubInfo info;
    try {
        info.Parse(argv[1]);
        info.Analysis();
        info.Print();
    }
    catch (parce_error ex) {
        std::cerr << "Произошла ошибка при чтении файла в строке: \n";
        std::cout << ex.what();
    }
    catch (std::runtime_error ex) {
        std::cerr << ex.what();
    }

    return 0;
}
