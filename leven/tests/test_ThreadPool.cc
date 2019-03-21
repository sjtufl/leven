//
// Created by fl on 1/26/19.
//
#include <iostream>

#include <leven/util/ThreadPool.h>


using namespace leven;

int main()
{
    ThreadPool threadPool(3, 12, nullptr);
    int i = 1;
    while (i < 100) {
        threadPool.runTask([i](){std::cout << "currently [" << i << "]" << std::endl;});
        i += 1;
    }
}
