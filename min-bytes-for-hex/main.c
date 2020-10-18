#include <stdio.h>
#include <string.h>

int main(int argc, char **args) {

    for (int i = 1; i < argc; ++i) {
        int firstDigit;
        for (firstDigit = 2; args[i][firstDigit] == '0'; ++firstDigit) {}
        int numRecordSize = strlen(args[i]) - firstDigit;
        int numBytes = (numRecordSize + 1) / 2;
        if (numBytes < 1) numBytes = 1;
        printf("%d ", numBytes);
    }

    return 0;
}
