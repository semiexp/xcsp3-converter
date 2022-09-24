#include <iostream>
#include <map>

#include <XCSP3Tree.h>
#include "Converter.h"

int main(int argc, char** argv) {
    auto converted = ConvertXCSP3Instance(argv[1]);
    if (converted.size() > 0 && converted.back() == '\n') converted.pop_back();
    std::cout << converted << std::endl;
    return 0;
}
