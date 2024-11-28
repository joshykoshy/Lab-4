#include <iostream>
#include <iomanip>  /* I/O manipulation utilities */

/* Print floating-point number with specified formatting */
void print_real(float number, int fieldspace, int precision) {
    std::cout << std::fixed << std::setprecision(precision) << std::setw(fieldspace) << number << std::endl;
}

/* Scale and update two values using a template function */
template <typename T>
void update_scale(T &m1, T &m2, T m3 = 10) {
    T original_m1 = m1;
    T original_m2 = m2;
    m1 = (original_m1 + original_m2) * m3;
    m2 = (original_m1 - original_m2) * m3;
}

int main() {
    /* Initialize variables */
    float a, b;

    // Take user input for 'a' and 'b'
    std::cout << "Enter the first number (a): ";
    std::cin >> a;

    std::cout << "Enter the second number (b): ";
    std::cin >> b;

    std::cout << "Initial values:" << std::endl;
    print_real(a, 10, 2);
    print_real(b, 10, 2);

    /* Transform values */
    update_scale(a, b);

    std::cout << "Updated values:" << std::endl;
    print_real(a, 10, 2);
    print_real(b, 10, 2);

    return 0;
}
