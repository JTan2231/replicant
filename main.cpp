#include <cstdlib>
#include <ctime>
#include <iostream>

#include "generation.h"
#include "gradient.h"
#include "mat_operations.h"
#include "matrix.h"

using namespace std;

int main() {
    srand(time(0));

    Tape<float> t;

    Variable<float> s1(5, &t);
    Variable<float> pi(3.14, &t);

    Variable<float> v = s1 * pi;

    t.compute(&v);

    cout << v.getValue() << endl;

    return 0;
}
