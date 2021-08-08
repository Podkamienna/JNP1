#include "fibo.h"

#include <string>
#include <ostream>
#include <vector>
#include <cstring>
#include <climits>
#include <cassert>

using std::string;
using std::ostream;
using std::vector;

namespace {
    using fibonacci_number_type = unsigned long long;
    const fibonacci_number_type MAX_FIBBONACCI_NUMBER_TYPE = ULLONG_MAX;

    //Fibonacci numbers from F_{2} = 1
    vector<fibonacci_number_type> fibonacciNumbers;

    size_t stringLength(const char *text) {
        assert(text != nullptr);
        return strlen(text);
    }

    void calculateFibonacciNumbers() {
        if (!fibonacciNumbers.empty()) {
            return;
        }

        fibonacciNumbers.push_back(1);
        fibonacciNumbers.push_back(2);

        int i = 2;

        while (MAX_FIBBONACCI_NUMBER_TYPE - fibonacciNumbers[i - 1] >= fibonacciNumbers[i - 2]) {
            fibonacciNumbers.push_back(fibonacciNumbers[i - 1] + fibonacciNumbers[i - 2]);
            i++;
        }
    }
}

//Finds and returns the index of the biggest Fibonacci number smaller than the given number
size_t Fibo::biggestSmallerFibbonacciNumberIndex(fibonacci_number_type number) {
    calculateFibonacciNumbers();

    size_t left = 0, right = fibonacciNumbers.size() - 1;

    while (left != right) {
        size_t index = (left + right + 1) / 2;

        if (fibonacciNumbers[index] > number) {
            right = index - 1;
        } else {
            left = index;
        }
    }

    return left;
}

fibonacci_number_type Fibo::getFibonacciNumber(size_t index) {
    if (index >= fibonacciNumbers.size()) {
        return 0;
    }

    return fibonacciNumbers[index];
}

//Checks if the given string is valid
bool Fibo::checkStr(const char *text, size_t length) {
    if (length == 0 || text == nullptr) {
        return false;
    }

    if (text[0] == '0' && length > 1) {
        return false;
    }

    for (size_t i = 0; i < length; i++) {
        if (!(text[i] == '0' || text[i] == '1')) {
            return false;
        }
    }

    return true;
}

//constructors
Fibo::Fibo(const char *text, size_t length) {
    assert(checkStr(text, length));

    for (size_t i = length; i-- > 0;) {
        representation.push_back(text[i] == '1');
    }

    normalize();
}

Fibo::Fibo() noexcept : representation{false} {}

Fibo::Fibo(string &text) : Fibo{text.c_str(), text.size()} {}

Fibo::Fibo(const char *text) : Fibo{text, stringLength(text)} {}

//arithemtic operator
Fibo &Fibo::operator+=(const Fibo &rhs) {
    if (representation.size() < rhs.length()) {
        representation.resize(rhs.length());
    }

    for (size_t j = 0; j < rhs.length(); j++) {
        if (rhs.representation[j]) {
            if (representation[j]) {
                bool done = false;

                if (j != 0) {
                    for (size_t i = j - 1; i + 2 > 1; i -= 2) {
                        representation[i] = true;

                        if (i > 0 && !representation[i - 1]) {
                            representation[i - 1] = true;
                            partiallyNormalize(i > 1 ? i - 2 : i - 1);
                            done = true;
                            break;
                        }
                    }
                }

                if (!done) {
                    normalize();

                    if (representation.size() == 1) {
                        representation.push_back(true);
                    }
                    representation[1] = representation[0];
                    representation[0] = !representation[0];

                    normalize();
                }

            } else {
                representation[j] = true;
                partiallyNormalize(j > 0 ? j - 1 : 0);
            }
        }
    }

    return *this;
}

//bitwise operators
Fibo &Fibo::operator&=(const Fibo &rhs) {
    if (representation.size() > rhs.representation.size()) {
        representation.resize(rhs.representation.size());
    }

    for (size_t i = 0; i < representation.size(); i++) {
        representation[i] = representation[i] & rhs.representation[i];
    }

    deleteLeadingZeros();

    return *this;
}

Fibo &Fibo::operator|=(const Fibo &rhs) {
    if (representation.size() < rhs.representation.size()) {
        representation.resize(rhs.representation.size(), false);
    }

    for (size_t i = 0; i < rhs.representation.size(); i++) {
        representation[i] = representation[i] | rhs.representation[i];
    }

    normalize();
    deleteLeadingZeros();

    return *this;
}

Fibo &Fibo::operator^=(const Fibo &rhs) {
    if (representation.size() < rhs.representation.size()) {
        representation.resize(rhs.representation.size());
    }

    for (size_t i = 0; i < rhs.representation.size(); i++) {
        representation[i] = representation[i] ^ rhs.representation[i];
    }

    normalize();
    deleteLeadingZeros();

    return *this;
}

Fibo &Fibo::operator<<=(size_t number) {
    size_t oldSize = representation.size();
    representation.resize(oldSize + number, false);

    for (size_t i = oldSize +number; i-->0;) {
        representation[i] = i >= number ? representation[i - number] : false;
        if (i > 0 && i + 1 == representation.size() && representation[i] == 0) {
            representation.pop_back();
        }
    }

    return *this;
}

//comparison operators
bool operator<(const Fibo &lhs, const Fibo &rhs) {
    if (lhs.representation.size() == rhs.representation.size()) {
        size_t i = rhs.representation.size() - 1;

        while (i > 0 && rhs.representation[i] == lhs.representation[i]) {
            i--;
        }

        return lhs.representation[i] < rhs.representation[i];
    }

    return lhs.representation.size() < rhs.representation.size();
}

bool operator==(const Fibo &lhs, const Fibo &rhs) {
    return lhs.representation == rhs.representation;
}

//output operator
ostream &operator<<(ostream &os, const Fibo &obj) {
    for (size_t i = obj.representation.size(); i > 0; i--) {
        os << obj.representation[i - 1];
    }

    return os;
}

size_t Fibo::length() const {
    return representation.size();
}

void Fibo::partiallyNormalize(unsigned long begin) {
    unsigned long end = representation.size();

    while (begin < end) {

        if (begin + 1 < end && representation[begin] && representation[begin + 1]) {
            unsigned long index = begin;

            while (index < end && representation[index]) {
                index++;
            }

            if (index < end) {
                representation[index] = true;
            } else {
                representation.push_back(true);
                end++;
            }

            if ((index - begin) % 2 == 1) {
                representation[begin++] = true;
                representation[begin++] = false;
            } else {
                representation[begin++] = false;
            }

            for (size_t i = begin; i < index; i += 2)
                representation[i] = false;

            begin = index;
        } else {
            begin++;
        }
    }
}

void Fibo::normalize() {
    partiallyNormalize(0);
}

void Fibo::deleteLeadingZeros() {
    size_t newSize = representation.size();

    while (newSize > 1 && representation[newSize - 1] == 0) {
        newSize--;
    }

    representation.resize(newSize);
}

const Fibo &Zero() {
    static const Fibo zero;

    return zero;
}

const Fibo &One() {
    static const Fibo one(1);

    return one;
}



