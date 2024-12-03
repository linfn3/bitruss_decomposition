#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>

template <class T, typename size_t = unsigned int>
class MyVector {
  private:
    T *data;
    size_t _size;
    size_t _capacity;

  public:
    MyVector();

    MyVector(size_t capacity);

    ~MyVector();

    void push_back(T _element);

    void pop_back();

    T &operator[](size_t index);

    inline size_t size() { return _size; }

    inline size_t capacity() { return _capacity; }

    inline bool empty() { return _size == 0; }

    void release_space();

    inline T *get_data() { return data; };

    void swap(MyVector<T, size_t> &_target);
};

template <class T, typename size_t>
MyVector<T, size_t>::MyVector() {
    _capacity = 4;
    _size = 0;
    data = new T[_capacity];
}

template <class T, typename size_t>
MyVector<T, size_t>::MyVector(size_t capacity) {
    _capacity = capacity;
    _size = _capacity;
    data = new T[_capacity];
}

template <class T, typename size_t>
MyVector<T, size_t>::~MyVector() {
    release_space();
}

template <class T, typename size_t>
void MyVector<T, size_t>::push_back(T _element) {
    if (_size == _capacity) {
        _capacity *= 2;
        T *_data = new T[_capacity];
        std::memcpy(_data, data, sizeof(T) * _size);
        std::swap(data, _data);
        delete[] _data;
        _data = NULL;
    }
    data[_size++] = _element;
}

template <class T, typename size_t>
void MyVector<T, size_t>::pop_back() {
    _size--;
}

template <class T, typename size_t>
T &MyVector<T, size_t>::operator[](size_t index) {
    if (index >= _size) {
        std::cout << "trying to access " << index
                  << "-th item, while current size is " << _size << std::endl;
        exit(-1);
    }
    return data[index];
}

template <class T, typename size_t>
void MyVector<T, size_t>::release_space() {
    if (data == NULL) return;
    delete[] data;
    data = NULL;
}

template <class T, typename size_t>
void MyVector<T, size_t>::swap(MyVector<T, size_t> &_target) {
    std::swap(this->data, _target.data);
    std::swap(this->_capacity, _target._capacity);
    std::swap(this->_size, _target._size);
}

typedef MyVector<int, unsigned int> MyVectorInt;
typedef MyVector<long long, unsigned int> MyVectorLong;

#endif