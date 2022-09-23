#include <iostream>
#include <map>

#include <XCSP3Tree.h>

int main() {
    XCSP3Core::Tree tree("eq(x,add(y,z,3))");
    std::map<std::string, int> assignment{
        {"x", 8},
        {"y", 2},
        {"z", 3},
    };
    std::cout << tree.evaluate(assignment) << std::endl;
    return 0;
}
