#ifndef BUFFER
#define BUFFER

#include <cstdlib>

template <typename T> class Buffer {
    T *value;

  public:
    Buffer() { value = (T *)malloc(sizeof(T)); }

    T getValue() { return *value; }
    void setValue(T newValue) { *value = newValue; }
};

#endif
