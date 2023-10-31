// 设计一个存放int类型的可变长数组Vector，支持以下操作：
// 1. 默认初始化为空数组
// 2. Vector整体拷贝
// 3. push_back，末尾新增一个元素
// 4. pop_back，移除末尾元素

#include <iostream>
#include <algorithm>

template <typename T>
class Vector
{
private:
    T *array;        // 存储元素的数组
    size_t size;     // 当前元素的个数
    size_t capacity; // 数组容量
public:
    Vector() : array(nullptr), size(0), capacity(0)
    {
    }

    // 拷贝构造函数
    Vector(const Vector &other) : size(other.size), capacity(other.capacity)
    {
        array = new T[capacity];
        std::copy(other.array, other.array + size, array);
    }

    ~Vector()
    {
        delete[] array;
    }

    // 末尾添加一个元素
    void push_back(T value)
    {
        if (size == capacity)
        {
            if (capacity == 0)
            {
                reserve(1);
            }
            else
            {
                reserve(capacity * 2);
            }
        }
        array[size++] = value;
    }

    // 移除末尾元素
    void pop_back()
    {
        if (size > 0)
            size--;
    }

    // 获取当前元素
    size_t getSize() const
    {
        return size;
    }
    // 获取当前容量
    size_t getCapacity() const
    {
        return capacity;
    }

    T &operator[](size_t pos)
    {
        if (pos >= size)
        {
            throw std::out_of_range("Invalid index");
        }
        return array[pos];
    }

private:
    void reserve(size_t newCpacity)
    {
        if (newCpacity > capacity)
        {
            int *newArray = new int[newCpacity];
            std::copy(array, array + size, newArray);
            delete[] array;
            array = newArray;
            capacity = newCpacity;
        }
    }
};

int main()
{
    Vector<int> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    std::cout << vec.getSize() << std::endl;
    std::cout << vec.getCapacity() << std::endl;

    Vector<int> vec2 = vec;
    std::cout << vec2.getSize() << std::endl;
    std::cout << vec2[1] << std::endl;

    return 0;
}