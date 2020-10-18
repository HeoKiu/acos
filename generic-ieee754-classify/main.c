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

    if (!E && !M && !S) return PlusZero;
    if (!E && !M) return MinusZero;
    if (E == MAX_EXPONENT && !M && !S) return PlusInf;
    if (E == MAX_EXPONENT && !M) return MinusInf;
    if (E == MAX_EXPONENT && M && !((M >> (MANTISSA_SIZE - SIGN_SIZE)) & LAST_BIT)) return SignalingNaN;
    if (E == MAX_EXPONENT) return QuietNaN;
    if (!E && !S) return PlusDenormal;
    if (!E) return MinusDenormal;
    if (!S) return PlusRegular;
    return MinusRegular;
};
