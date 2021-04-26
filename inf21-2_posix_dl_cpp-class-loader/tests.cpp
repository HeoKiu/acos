#include <iostream>
#include "main.cpp"


class LikeA {
public:
    int var;

};


int main() {
    ClassLoader loader;
    auto class_ = loader.loadClass("A");
    if (!class_) {
        std::cout << "Error: " << '\n';
        return 0;
    } else {
        std::cout << "Success\n";
    }

    auto *orig = (Class<LikeA> *) class_;
    auto a_instance = orig->newInstance();
    std::cout << a_instance->var << std::endl;

    delete a_instance;
    return 0;
}