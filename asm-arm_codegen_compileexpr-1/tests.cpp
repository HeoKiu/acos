//
// Created by boris on 31.10.2020.
//

#include "tests.h"
// Hear must be tests, but now they are not

std::vector<StackOperand> get_test_stack() {
    return {
            StackOperand(CONSTANT, 3),
            StackOperand(CONSTANT, 4),
            StackOperand(CONSTANT, 2),
            StackOperand(MATH_OPERATOR, "+"),
            StackOperand(MATH_OPERATOR, "*"),
            StackOperand(CONSTANT, 1),
            StackOperand(MATH_OPERATOR, "-"),
    };
}
