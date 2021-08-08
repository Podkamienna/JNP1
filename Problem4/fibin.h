#ifndef FIBIN_FIBIN_H
#define FIBIN_FIBIN_H

#include <iostream>
#include <cstdint>
#include <type_traits>

// Type for indices of fibonacci numbers.
using FibonacciIndex = uint64_t;
// Type for ids of variable names.
using VariableId = uint32_t;

// Base in which variable id is calculated.
static const VariableId BASE = 37;

static const VariableId INVALID_VARIABLE_SIGN = 0;
static const VariableId INVALID_VARIABLE_ID = 0;

namespace details {
// Function that maps bijectively signs "0"-"9" and "a"-"z"
// with convert("A") == convert("a") and analogically for other letters.
// If given sign different than "0"-"9", "a"-"z", "A"-"Z" makes the compilation fail.
    constexpr VariableId convert(const char sign) {
        if ('0' <= sign && sign <= '9') {
            return sign - '0' + 1; // "+ 1" to be able to distinguish "0" from "00".
        }

        if ('a' <= sign && sign <= 'z') {
            return sign - 'a' + 11;
        }

        if ('A' <= sign && sign <= 'Z') {
            return sign - 'A' + 11;
        }

        return INVALID_VARIABLE_SIGN;
    }

// Type for empty list.
    struct EmptyList {
    };

// Type for list.
    template<VariableId = 0, typename First = EmptyList, typename Rest = EmptyList>
    struct List {
        using head = First;
        using tail = Rest;
    };

// Searching list for a given variable.
    template<VariableId Variable, typename Environment>
    struct FindList {
        using result = typename FindList<Variable, typename Environment::tail>::result;
    };

// The search ends here if the variable was found.
    template<VariableId Variable, typename Value, typename Tail>
    struct FindList<Variable, List<Variable, Value, Tail>> {
        using result = Value;
    };

// The search generates compilation error if the element
// that was searched for is not in the list.
    template<VariableId Variable>
    struct FindList<Variable, EmptyList> {
    };

// Encapsulation for number values.
    template<typename ValueType, ValueType n>
    struct Value {
        static const ValueType numberValue = n;
    };

    template<bool value>
    struct Boolean {
    };

// Calculating fibonacci numbers.
    template<typename ValueType, FibonacciIndex N>
    struct CalculateFib {
        static const ValueType result = CalculateFib<ValueType, N - 1>::result +
                                        CalculateFib<ValueType, N - 2>::result;
    };

    template<typename ValueType>
    struct CalculateFib<ValueType, 1> {
        static const ValueType result = 1;
    };

    template<typename ValueType>
    struct CalculateFib<ValueType, 0> {
        static const ValueType result = 0;
    };
}

struct True {
    template<typename ValueType>
    using result = details::Boolean<true>;
};

struct False {
    template<typename ValueType>
    using result = details::Boolean<false>;
};

template<FibonacciIndex N>
struct Fib {
    template<typename ValueType>
    using result =
    details::Value<ValueType, details::CalculateFib<ValueType, N>::result>;
};

template<typename T>
struct Lit {
    template<typename ValueType, typename Environment>
    using result = typename T::template result<ValueType>;
};

constexpr VariableId Var(const char *name) {
    if (name == nullptr) {
        return INVALID_VARIABLE_ID;
    }

    if (name[0] == '\0') {
        return INVALID_VARIABLE_ID;
    }

    VariableId result = 0;
    VariableId multiplier = 1;

    for (int i = 0; name[i] != '\0'; i++) {
        if (i >= 6) {
            return INVALID_VARIABLE_ID;
        }

        VariableId converted = details::convert(name[i]);
        if (converted == INVALID_VARIABLE_SIGN) {
            return INVALID_VARIABLE_ID;
        }

        result += converted * multiplier;
        multiplier *= BASE;
    }

    return result;
}

template<typename First, typename... Rest>
struct Sum {
    template<typename ValueType, typename Environment>
    using result = details::Value<
            ValueType, static_cast<ValueType>(
                    First::template result<ValueType, Environment>::numberValue +
                    Sum<Rest...>::template result<ValueType, Environment>::numberValue
            )
    >;
};

template<typename First, typename Second> // Sum can't be called with less than 2 arguments.
struct Sum<First, Second> {
    template<typename ValueType, typename Environment>
    using result = details::Value<ValueType, First::template result<ValueType, Environment>::numberValue +
                                             Second::template result<ValueType, Environment>::numberValue>;
};

template<typename ToIncrement>
struct Inc1 {
    template<typename ValueType, typename Environment>
    using result = typename Sum<ToIncrement, Lit<Fib<1>>>::template result<ValueType, Environment>;
};

template<typename ToIncrement>
struct Inc10 {
    template<typename ValueType, typename Environment>
    using result = typename Sum<ToIncrement, Lit<Fib<10>>>::template result<ValueType, Environment>;
};

template<typename Left, typename Right>
struct Eq {
    template<typename ValueType, typename Environment>
    using result = details::Boolean<
            Left::template result<ValueType, Environment>::numberValue ==
            Right::template result<ValueType, Environment>::numberValue>;
};

template<VariableId Variable>
struct Ref {
    static_assert(Variable != INVALID_VARIABLE_ID);

    template<typename ValueType, typename Environment>
    using result = typename details::FindList<Variable, Environment>::result;
};

template<typename Condition, typename Then, typename Else>
struct If {
    template<typename ValueType, typename Environment>
    using result = typename If<typename Condition::template result<ValueType, Environment>,
            Then, Else>::template result<ValueType, Environment>;
};

template<typename Then, typename Else>
struct If<details::Boolean<true>, Then, Else> {
    template<typename ValueType, typename Environment>
    using result = typename Then::template result<ValueType, Environment>;
};

template<typename Then, typename Else>
struct If<details::Boolean<false>, Then, Else> {
    template<typename ValueType, typename Environment>
    using result = typename Else::template result<ValueType, Environment>;
};

template<VariableId Variable, typename Value, typename Expression>
struct Let {
    static_assert(Variable != INVALID_VARIABLE_ID);

    template<typename ValueType, typename Environment>
    using result = typename Expression::template result<ValueType, details::List<Variable,
            typename Value::template result<ValueType, Environment>, Environment>>;
};

template<VariableId Variable, typename Body>
struct Lambda {
    static_assert(Variable != INVALID_VARIABLE_ID);

    template<typename ValueType, typename Environment>
    struct result {
        template<typename Param>
        using invoke = typename Body::template result<ValueType, details::List<Variable, Param, Environment>>;
    };
};

template<typename Fun, typename Param>
struct Invoke {
    template<typename ValueType, typename Environment>
    using result = typename Fun::template result<ValueType,
            Environment>::template invoke<typename Param::template result<ValueType, Environment>>;
};

template<typename ValueType>
class Fibin {

public:
    template<typename Expression, typename X = ValueType, std::enable_if_t<std::is_integral<X>::value, int> = 0>
    static constexpr ValueType eval() {
        return Expression::template result<ValueType, details::EmptyList>::numberValue;
    }

    template<typename Expression, typename X = ValueType, std::enable_if_t<!std::is_integral<X>::value, int> = 0>
    static constexpr void eval() {
        std::cout << "Fibin doesn't support: " << typeid(ValueType).name() << std::endl;
    }
};

#endif //FIBIN_FIBIN_H
