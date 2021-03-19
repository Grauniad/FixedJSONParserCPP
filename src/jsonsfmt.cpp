#include <iostream>
#include <SimpleJSONFmt.h>


int main(int argc, char** argv) {
    FmtJSON::Fmt(std::cin, std::cout);

    std::cout.flush();

    return 0;
}
