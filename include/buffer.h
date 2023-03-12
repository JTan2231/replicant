#ifndef BUFFER
#define BUFFER

#include <cstdlib>
#include <memory>

template <typename T>
class Buffer {
    std::shared_ptr<T> value;

  public:
    Buffer() {
        value = std::shared_ptr<T>(new T);
    }

    T getValue() {
        return *value;
    }
    void setValue(T newValue) {
        *value = newValue;
    }
};

#endif
