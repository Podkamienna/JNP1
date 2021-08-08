#ifndef __FIBO_H__
#define __FIBO_H__

#include <string>
#include <ostream>
#include <vector>
#include <cassert>
#include <boost/operators.hpp>

namespace {
    using fibonacci_number_type = unsigned long long;
}

class Fibo :
        boost::bitwise<Fibo>,
        boost::addable<Fibo>,
        boost::totally_ordered<Fibo>,
        boost::left_shiftable<Fibo, size_t> {
public:
    // Constructors
    Fibo() noexcept;

    explicit Fibo(std::string &text);

    explicit Fibo(const char *text);

    Fibo(const Fibo &that) = default;

    Fibo(Fibo &&that) noexcept = default;

    template<typename T, std::enable_if_t<
            std::is_integral<T>::value && !std::is_same<T, bool>::value && !std::is_same<T, char>::value, int> = 0>
    Fibo(T number) {
        assert(number >= 0);

        if (number == 0) {
            representation.push_back(false);

            return;
        }

        if (number == 1) {
            representation.push_back(true);

            return;
        }

        size_t biggestIndex = biggestSmallerFibbonacciNumberIndex(number);

        for (size_t i = 0; i <= biggestIndex; i++) {
            representation.push_back(false);
        }

        representation[biggestIndex] = true;

        T tmpNumber = number - getFibonacciNumber(biggestIndex);

        size_t i = biggestIndex - 1;

        while (tmpNumber > 0) {
            if (getFibonacciNumber(i) <= (fibonacci_number_type) tmpNumber) {
                tmpNumber -= getFibonacciNumber(i);
                representation[i] = true;
            }
            i--;
        }
    }

    //destructor
    ~Fibo() = default;

    //assignment operators
    Fibo &operator=(const Fibo &rhs) = default;

    Fibo &operator=(Fibo &&rhs) noexcept = default;

    //arithmetic operator
    Fibo &operator+=(const Fibo &rhs);

    //bitwise operators
    Fibo &operator&=(const Fibo &rhs);

    Fibo &operator|=(const Fibo &rhs);

    Fibo &operator^=(const Fibo &rhs);

    Fibo &operator<<=(size_t number);

    //comparison operators
    friend bool operator<(const Fibo &lhs, const Fibo &rhs);

    friend bool operator==(const Fibo &lhs, const Fibo &rhs);

    //output operator
    friend std::ostream &operator<<(std::ostream &os, const Fibo &obj);

    size_t length() const;

private:
    std::vector<bool> representation;

    Fibo(const char *text, size_t length);

    void partiallyNormalize(size_t begin);

    void normalize();

    void deleteLeadingZeros();

    static size_t biggestSmallerFibbonacciNumberIndex(fibonacci_number_type number);

    static fibonacci_number_type getFibonacciNumber(size_t index);

    static bool checkStr(const char *text, size_t length);
};

const Fibo &Zero();

const Fibo &One();

#endif // __FIBO_H__