#include <cstdlib>
#include <ctime>
#include <iostream>

#include "generation.h"
#include "mat_operations.h"
#include "matrix.h"

using namespace std;

int main() {
    srand(time(0));

    Matrix<float> m1 = normalInit<float>(2, 7);
    Matrix<float> m2 = normalInit<float>(7, 4);

    cout << "m1: " << endl << m1 << endl << endl;
    cout << "m2: " << endl << m2 << endl << endl;

    cout << "m1 * m2: " << endl << m1 * m2 << endl;
    return 0;
}
