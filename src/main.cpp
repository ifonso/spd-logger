#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> messages {"Hello", "World!"};

    for (const auto& word : messages) {
        std::cout << word << " ";
    }
    std::cout << std::endl;

    return 0;
}
