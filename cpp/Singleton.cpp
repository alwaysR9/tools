#include <iostream>

template<typename T>
class Singleton {
private:
    Singleton() {};
public:
    static T* get() {
        if (!m_p)
            m_p = new T();
        return m_p;
    }
public:
    static T* m_p;
};

class Mat {
public:
    Mat() {
        m = 1;
    }
    ~Mat() {}

    inline int val() {
        return m;
    }
private:
    int m;
};

// init static var
template<typename T>
T* Singleton<T>::m_p = NULL;

int main() {

    Mat* mat1 = Singleton<Mat>::get();
    std::cout << mat1 << std::endl;

    Mat* mat2 = Singleton<Mat>::get();
    std::cout << mat2 << std::endl;

    return 0;
}
