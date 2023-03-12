#ifndef SCALAR
#define SCALAR

#include <cstdlib>
#include <iostream>
#include <memory>

#include "buffer.h"

template <typename T>
class Buffer;

template <typename T>
class Scalar {
  protected:
    std::shared_ptr<Buffer<T>> value;

  public:
    Scalar() {
        value = std::shared_ptr<Buffer<T>>(new Buffer<T>());
    }

    Scalar(T v) {
        value = std::shared_ptr<Buffer<T>>(new Buffer<T>());
        value->setValue(v);
    }

    void setValue(T newValue) {
        value->setValue(newValue);
    }
    void operator=(T newValue) {
        value->setValue(newValue);
    }

    friend std::ostream &operator<<(std::ostream &os, const Scalar &s) {
        os << s.getValue();
        return os;
    }
};

#endif
