#include "cpu_test.h"
#include <filesystem>

int main(int argc, char *argv[])
{
    CPUTest test(argv[1]);

    if (test.RunTest() == TR_OK) {
        printf("test %s is passed\n", std::filesystem::path(argv[1]).filename().c_str());
    } else {
        printf("test %s is failed\n", std::filesystem::path(argv[1]).filename().c_str());
        return -1;
    }
    return 0;
}