#ifndef SCALAR
#define SCALAR

#include <cstdlib>
#include <iostream>

#include "buffer.h"

template <typename T> class Buffer;

template <typename T> class Scalar {
  protected:
    Buffer<T> *value;

  public:
    Scalar() { value = new Buffer<T>(); }

    Scalar(T v) {
        value = new Buffer<T>();
        value->setValue(v);
    }

    void setValue(T newValue) { value->setValue(newValue); }
    void operator=(T newValue) { value->setValue(newValue); }

    friend std::ostream &operator<<(std::ostream &os, const Scalar &s) {
        os << s.getValue();
        return os;
    }
};

#endif
