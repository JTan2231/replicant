#include <cmath>
#include <cstdlib>

int randBinary() {
    const int divisor = RAND_MAX / (2);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > 1);

    return retval;
}

double marsagliaPolar() {
    double U, V, S;
    do {
        U = (double)rand() / (double)(RAND_MAX);
        if (randBinary()) U *= -1;
        V = (double)rand() / (double)(RAND_MAX);
        if (randBinary()) V *= -1;
        S = U * U + V * V;
    } while (S >= 1);

    return U * sqrtf(-2 * logf(S) / S);
}
