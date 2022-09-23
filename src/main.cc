#include <iostream>
#include <map>

#include <XCSP3Tree.h>
#include "TreeConverter.h"

int main() {
    XCSP3Core::Tree tree("eq(x,add(y,z,3))");
    std::map<std::string, std::tuple<std::string, Type>> vars{
        {"x", {"i0", Type::kInt}},
        {"y", {"i1", Type::kInt}},
        {"z", {"i2", Type::kInt}},
    };
    std::cout << std::get<0>(AsBool(ConvertTree(&tree, vars))) << std::endl;
    return 0;
}
