#include <iostream>
#include <algorithm>

class Vector
{
private:
    int *array;      // 存储元素的数组
    size_t size;     // 当前元素个数
    size_t capacity; // 数组的容量

public:
    Vector() : array(nullptr), size(0), capacity(0) {}

    // 拷贝构造函数
    Vector(const Vector &other) : size(other.size), capacity(other.capacity)
    {
        array = new int[capacity];
        std::copy(other.array, other.array + size, array);
    }

    ~Vector()
    {
        delete[] array;
    }

    // 在末尾新增一个元素
    void push_back(int value)
    {
        if (size == capacity)
        {
            if (capacity == 0)
                reserve(1);
            else
                reserve(capacity * 2);
        }
        array[size++] = value;
    }

    // 移除末尾元素
    void pop_back()
    {
        if (size > 0)
            size--;
    }

    // 获取当前元素个数
    size_t getSize() const
    {
        return size;
    }

private:
    // 调整数组容量
    void reserve(size_t newCapacity)
    {
        if (newCapacity > capacity)
        {
            int *newArray = new int[newCapacity];
            std::copy(array, array + size, newArray);
            delete[] array;
            array = newArray;
            capacity = newCapacity;
        }
    }
};

int main()
{
    Vector vec; // 默认初始化为空数组

    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    std::cout << "Size: " << vec.getSize() << std::endl; // 输出当前元素个数（3）

    vec.pop_back(); // 移除末尾元素

    Vector vec2 = vec; // 整体拷贝

    std::cout << "Size: " << vec2.getSize() << std::endl; // 输出vec2的元素个数（2）

    return 0;
}
