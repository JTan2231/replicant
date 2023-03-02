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

    cout << v.compute() << endl;

    return 0;
}
