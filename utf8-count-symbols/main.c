#include <stdio.h>
#include <stdint.h>

#define BYTE 8ULL
#define FIRST_BIT 0x80UL
#define FIRST2_BITS 0xC0UL

uint8_t countLeadingOnes(uint8_t c) {
    for (uint8_t i = 0; i < 8U; ++i) {
        if (!(c & (1U << (7U - i)))) {
            return i;
        }
    }
    return 8U;
}

void printValues(int a, int b) {
    printf("%d %d", a, b);
}

int main() {

    int c;

    int asciiSymCount = 0;
    int otherSymCount = 0;
    int bytesToSym = 0;

    while ((c = getchar()) != EOF) {
        uint8_t leadingOnes = countLeadingOnes(c);
        if (leadingOnes > 4 || (uint8_t)(leadingOnes != 1U) ^ (uint8_t)(bytesToSym == 0)) {
            printValues(asciiSymCount, otherSymCount);
            return 1;
        }
        if (leadingOnes == 1U) {
            --bytesToSym;
        } else if (!leadingOnes) {
            ++asciiSymCount;
        } else {
            ++otherSymCount;
            bytesToSym = leadingOnes - 1;
        }
    }
    printValues(asciiSymCount, otherSymCount);

    if (bytesToSym) {
        return 1;
    }

    return 0;
}
