#define GRT_DEBUG
#include "Buffer.h"
#include "Log/Log.h"
#include <iostream>
int main(void) {
    grt::log::Log lll;
    lll.start();
    std::string teee = "566";
    lll << teee << "\n";
    while(1);
}