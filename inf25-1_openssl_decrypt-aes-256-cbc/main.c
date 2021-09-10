#include <stdio.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <string.h>

unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len) {
    int p_len = *len, f_len = 0;
    unsigned char *plaintext = malloc(p_len);

    EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex(e, plaintext + p_len, &f_len);

    *len = p_len + f_len;
    return plaintext;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Too few args\n");
        return 1;
    }
    unsigned char *password = argv[1];
    unsigned char buffer[1024 * 1024];
    int n;
    int bytes_read = 0;
    while((n = read(0, buffer + bytes_read, sizeof(buffer) - bytes_read)) > 0){
        bytes_read += n;
    }

    // Создание контекста
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char *salt = buffer + 8;

    unsigned char key[1024], iv[1024];
    // Генерация ключа и начального вектора из
    // пароля произвольной длины и 8-байтной соли
    EVP_BytesToKey(
            EVP_aes_256_cbc(),    // алгоритм шифрования
            EVP_sha256(),         // алгоритм хеширования пароля
            salt,                 // соль
            password, strlen(password), // пароль
            1,                    // количество итераций хеширования
            key,          // результат: ключ нужной длины
            iv            // результат: начальный вектор нужной длины
    );

    // Начальная стадия: инициализация
    EVP_DecryptInit(
            ctx,                  // контекст для хранения состояния
            EVP_aes_256_cbc(),    // алгоритм шифрования
            key,                  // ключ нужного размера
            iv                    // начальное значение нужного размера
    );
    int buffer_size = bytes_read - 16;
    unsigned char * result = aes_decrypt(ctx, buffer + 16, &buffer_size);
    for (int i = 0; i < buffer_size; ++i) {
        printf("%c", result[i]);
    }

    EVP_CIPHER_CTX_free(ctx);
    free(result);

    return 0;
}