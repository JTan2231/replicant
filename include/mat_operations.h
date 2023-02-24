#ifndef MATOP
#define MATOP
#include <cassert>

#include "matrix.h"

// naive implementation
template <typename T>
Matrix<T> matmul(Matrix<T> m1, Matrix<T> m2) {
    assert(m1.cols == m2.rows);

    Matrix<T> m3(m1.rows, m2.cols);

    for (int i = 0; i < m3.rows; i++) {
        for (int j = 0; j < m3.cols; j++) {
            for (int k = 0; k < m2.cols; k++) {
                m3(i, j) += m1(i, k) * m2(k, j);
            }
        }
    }

    return m3;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& m1, const Matrix<T>& m2) {
    return matmul(m1, m2);
}

#endif
