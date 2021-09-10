#include <stdio.h>
#include <openssl/sha.h>
#include <unistd.h>

int main() {

    SHA512_CTX c;
    SHA512_Init(&c);

    unsigned char buffer[SHA512_DIGEST_LENGTH];
    ssize_t n;
    while((n = read(0, buffer, sizeof(buffer))) > 0){
        SHA512_Update(&c, buffer, n);
    }

    SHA512_Final(buffer, &c);
    printf("0x");
    for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
        printf("%02x", buffer[i]);
    }

    return 0;
}
