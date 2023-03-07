#include <cstdlib>
#include <ctime>
#include <iostream>

#include "generation.h"
#include "gradient.h"
#include "mat_operations.h"
#include "matrix.h"

using namespace std;

template <typename T> void printVar(Variable<T> *v) {
    cout << v->getValue() << endl;
}

int main() {
    srand(time(0));

    Tape<float> t;

    Variable<float> s1(5, &t);
    Variable<float> pi(3.14, &t);

    auto v = s1 - pi;
    auto v2 = pi + pi;
    auto v3 = v / v2;
    auto v4 = v * v3;

    // NOTE: cannot do below because temporary objects are a headache.
    // Variable<float> v4 = v * v2 * v3 * s1 * pi;

    t.compute(&v4);
    printVar(&v);
    printVar(&v2);
    printVar(&v3);
    printVar(&v4);

    cout << "gradient dv4/dv2 == " << t.gradient(v4, v2) << endl;

    return 0;
}
