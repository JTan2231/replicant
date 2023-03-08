#include <cstdlib>
#include <ctime>
#include <iostream>

#include "generation.h"
#include "gradient.h"

using namespace std;

template <typename T> void printVar(Variable<T> *v) {
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
    cout << "v == ";
    printVar(&v);
    cout << "v2 == ";
    printVar(&v2);
    cout << "v3 == ";
    printVar(&v3);
    cout << "v4 == ";
    printVar(&v4);

    cout << "gradient dv4/dv2 == " << t.gradient(v4, v2) << endl;
}

void selfAssignmentTest() {
    Tape<double> t;

    Variable<double> v(1, &t);
    for (int i = 0; i < 10; i++) {
        v = v * 2.;
    }

    t.compute(&v);
    cout << "v^10 == ";
    printVar(&v);
}
