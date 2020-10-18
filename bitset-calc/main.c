#include <stdio.h>
#include <stdint.h>

typedef uint64_t bunch_t;

#define BUNCH_SIZE 62
#define SINGLE_BIT 1ULL
#define DIGITS_COUNT 10
#define UPPERCASE_ALPHABET_SIZE 26

bunch_t getCharMask(int c) {
    uint8_t shift;
    if (c >= '0' && c <= '9') {
        shift = c - '0';
    } else if (c >= 'A' && c <= 'Z') {
        shift = c - 'A' + DIGITS_COUNT;
    } else {
        shift = c - 'a' + (DIGITS_COUNT + UPPERCASE_ALPHABET_SIZE);
    }
    return (bunch_t) SINGLE_BIT << shift;
}

int getBunchCharByIndex(int i) {
    if (i < DIGITS_COUNT) return '0' + i;
    if (i < DIGITS_COUNT + UPPERCASE_ALPHABET_SIZE) return 'A' + i - DIGITS_COUNT;
    return 'a' + i - (DIGITS_COUNT + UPPERCASE_ALPHABET_SIZE);
}

int main() {
    bunch_t result_many = 0;
    bunch_t curr_many = 0;

    int sym;
    while ((sym = getchar()) != EOF) {
        if(sym == '\n') break;
        switch (sym) {
            case '|':
                result_many |= curr_many;
                curr_many = 0;
                break;
            case '&':
                result_many &= curr_many;
                curr_many = 0;
                break;
            case '^':
                result_many ^= curr_many;
                curr_many = 0;
                break;
            case '~':
                result_many = ~result_many;
                break;
            default:
                curr_many |= getCharMask(sym);
        }
    }

    for (int i = 0; i < BUNCH_SIZE; ++i) {
        if (result_many & (SINGLE_BIT << i)) {
            printf("%c", (char)getBunchCharByIndex(i));
        }
    }

    return 0;
}
