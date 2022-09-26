#define GRT_DEBUG
#include "Buffer.h"
#include <iostream>
int main(void) {
    grt::Buffer temp;
    std::string data(1500 , 'c');
    temp.append(data.c_str() , 1500);
    //std::cout << temp.readableBytes();
    std::cout << temp.writableBytes();
    //std::vector<char> res = temp.returnBuffer();
}