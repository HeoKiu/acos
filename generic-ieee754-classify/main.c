#include <stdio.h>
#include <stdint.h>

typedef enum {
    PlusZero = 0x00,
    MinusZero = 0x01,
    PlusInf = 0xF0,
    MinusInf = 0xF1,
    PlusRegular = 0x10,
    MinusRegular = 0x11,
    PlusDenormal = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN = 0x30,
    QuietNaN = 0x31
} float_class_t;

// 1 - знаковый бит (S)
// 11 - экспонента (E)
// 52 - мантисса (M)

#define MANTISSA_SIZE 0x34
#define SIGN_SIZE 0x1
#define LAST_BIT 0x1
#define EXP_SIZE 0xb
#define MAX_EXPONENT 0x7FF

typedef union {
    double real_value;
    uint64_t uint_value;
} real_or_uint;

extern float_class_t classify(double *value_ptr) {
    real_or_uint u;
    u.real_value = *value_ptr;
    uint64_t b = u.uint_value;
    uint8_t S = b >> (MANTISSA_SIZE + EXP_SIZE);
    uint16_t E = (b << SIGN_SIZE) >> (MANTISSA_SIZE + SIGN_SIZE);
    uint64_t M = (b << (EXP_SIZE + SIGN_SIZE)) >> (EXP_SIZE + SIGN_SIZE);

    float_class_t result;

    if (!E && !M && !S) result = PlusZero;
    else if (!E && !M) result = MinusZero;
    else if (E == MAX_EXPONENT && !M && !S) result = PlusInf;
    else if (E == MAX_EXPONENT && !M) result = MinusInf;
    else if (E == MAX_EXPONENT && M && !((M >> (MANTISSA_SIZE - SIGN_SIZE)) & LAST_BIT)) result = SignalingNaN;
    else if (E == MAX_EXPONENT) result = QuietNaN;
    else if (!E && !S) result = PlusDenormal;
    else if (!E) result = MinusDenormal;
    else if (!S) result = PlusRegular;
    else result = MinusRegular;

    return result;
};
