#include <iostream>
#include <cstdlib>

int main() {
    std::string name;

    std::cout << "Enter C++ file name (without .cpp): ";
    std::cin >> name;

    std::string inputFile = name + ".cpp";
    std::string outputFile = name + ".exe";

    std::string command = "g++ " + inputFile + " -o " + outputFile;

    int result = system(command.c_str());

    if (result == 0) {
        std::cout << "Compilation successful: " << outputFile << "\n";
    } else {
        std::cout << "Compilation failed.\n";
    }

    return 0;
}