#include <math.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "generation.h"
#include "gradient.h"

using namespace std;

template <typename T>
void printVar(Variable<T> *v) {
    cout << v->getValue() << endl;
}

void basicGradientTest();
void selfAssignmentTest();

int main() {
    srand(time(0));

    basicGradientTest();
    selfAssignmentTest();

    return 0;
}

void basicGradientTest() {
    TapePtr<double> t(new Tape<double>());

    Variable<double> s1(5, t);
    Variable<double> pi(3.14, t);

    const double power = 2.364;

    auto v = s1 - pi;
    auto v2 = pi + pi;
    auto v3 = v / v2;
    auto v4 = v * v3;
    auto v5 = v4 ^ v3;

    // NOTE: cannot do below because temporary objects are a headache.
    // Variable<float> v4 = v * v2 * v3 * s1 * pi;

    t->compute(&v5);
    cout << "v == ";
    printVar(&v);
    cout << "v2 == ";
    printVar(&v2);
    cout << "v3 == ";
    printVar(&v3);
    cout << "v4 == ";
    printVar(&v4);
    cout << "v5 == ";
    printVar(&v5);

    const double f = v.getValue() * v3.getValue();
    const double f1 = v.getValue();
    const double g = v3.getValue();
    const double g1 = 1;

    cout << "gradient dv5/dv3:" << endl;
    cout << "  - prediction == " << t->gradient(v5, v3) << endl;
    cout << "  - actual     == " << pow(f, g) * (f1 * g / f + g1 * log(f))
         << endl;
}

void selfAssignmentTest() {
    TapePtr<double> t(new Tape<double>());

    Variable<double> v(1, t);
    for (int i = 0; i < 10; i++) {
        v = v * 2.;
    }

    t->compute(&v);
    cout << "v^10 == ";
    printVar(&v);
}
