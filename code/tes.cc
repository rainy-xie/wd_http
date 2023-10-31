#include <iostream>

class A
{
public:
    virtual void func1(){};
};

int main()
{
    std::cout << sizeof(A);
}