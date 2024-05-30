#include "Polynomial.h"

int main() {
    Polynom pol1("12x^2yz+12+11x");
    Polynom pol2("0");
    Polynom pol3("-12x^2yz-12-11x");
    Polynom pol4("1");
    Polynom pol5("x^5");
    Polynom pol6 = pol1 - pol1;
    cout << pol1 << endl;
    return 0;
}