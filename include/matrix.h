#ifndef MATRIX
#define MATRIX
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "generation.h"

using namespace std;

template <typename T>
class Scalar {
  protected:
    T *value = nullptr;

  public:
    Scalar() {
        value = (T *)malloc(sizeof(T));
        *value = T{};
    }

    Scalar(T v) {
        value = (T *)malloc(sizeof(T));
        *value = v;
    }

    void setValue(T newValue) {
        *value = newValue;
    }

    void operator=(T newValue) {
        *value = newValue;
    }

    Scalar &operator+=(const Scalar &s) {
        *value += *s.value;
        return *this;
    }

    friend ostream &operator<<(ostream &os, const Scalar &s) {
        os << *s.value;
        return os;
    }
};

template <typename T>
class Matrix {
    Scalar<T> **data;

  public:
    const int rows;
    const int cols;

    Matrix(int r, int c);

    Scalar<T> &operator()(int r, int c);
    friend ostream &operator<<(ostream &os, const Matrix &m) {
        for (int r = 0; r < m.rows; r++) {
            for (int c = 0; c < m.cols; c++) {
                os << m.data[r][c] << ", ";
            }

            os << endl;
        }

        return os;
    }
};

template <typename T>
Matrix<T>::Matrix(int r, int c) : rows(r), cols(c) {
    data = (Scalar<T> **)calloc(r, sizeof(T *));
    for (int i = 0; i < r; i++) {
        data[i] = (Scalar<T> *)calloc(c, sizeof(T));
    }
}

template <typename T>
Scalar<T> &Matrix<T>::operator()(int r, int c) {
    assert(r < rows);
    assert(c < cols);

    return data[r][c];
}

template <typename T>
Matrix<T> ones(int r, int c) {
    Matrix<T> m(r, c);

    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            m(i, j) = 1;
        }
    }

    return m;
}

// initialize matrix with normally distributed values
template <typename T>
Matrix<T> normalInit(int r, int c) {
    Matrix<T> m(r, c);

    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            m(i, j) = marsagliaPolar();
        }
    }

    return m;
}

#endif
