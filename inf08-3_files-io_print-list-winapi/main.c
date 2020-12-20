#include <stdio.h>
//#include <fcntl.h>
#include <stdint.h>
//#include <zconf.h>
#include <windows.h>
#include <fileapi.h>
#include <windef.h>
#include <winbase.h>


int write_bytes(HANDLE fd, char *buffer, int bytes_to_write) {
    for (size_t bytes_written = 0; bytes_written < bytes_to_write;) {
        DWORD currently_written;
        WriteFile(fd, buffer + bytes_written, bytes_to_write - bytes_written, &currently_written, NULL);
        if (currently_written == -1) {
            return -1;
        }
        bytes_written += currently_written;
    }
    return bytes_to_write;
}


typedef struct {
    int value;
    uint32_t next_pointer;
} Item;

Item getItem(HANDLE fd, uint32_t pos) {
    Item item;
    SetFilePointer(fd, pos, NULL, FILE_BEGIN);
    ReadFile(fd, &item, 8, NULL, NULL);
    return item;
}

char digits[20];
const int buffer_size = 1000;


uint32_t print(char *buffer, int pointer, int value) {
    if (value < 0) {
        buffer[pointer++] = '-';
        value *= -1;
    }
    int dig_pointer = 0;
    if (!value) {
        digits[dig_pointer++] = '0';
    }
    while (value) {
        digits[dig_pointer++] = '0' + value % 10;
        value /= 10;
    }
    for (int i = 0; i < dig_pointer; ++i) {
        buffer[pointer++] = digits[dig_pointer - 1 - i];
    }
    buffer[pointer++] = ' ';
    return pointer;
}

int main(int argc, char **argv) {
    char buffer[buffer_size];
    uint32_t buffer_pointer = 0;
    uint32_t next_pointer = 0;
    HANDLE file;
    file = CreateFile(
            argv[1],                         // имя файла
            FILE_ALL_ACCESS,                      // режим доступа
            FILE_SHARE_READ,                          // совместный доступ
            NULL, // SD (дескр. защиты)
            OPEN_ALWAYS,                // как действовать
            FILE_ATTRIBUTE_NORMAL,                 // атрибуты файла
            NULL                        // дескр.шаблона файла
    );
//    file = OpenFile(argv[1], NULL, OF_READ);
    if (file == INVALID_HANDLE_VALUE) {
        return 1;
    }
    DWORD off = SetFilePointer(file, 0, NULL, FILE_END);

    HANDLE stdout_ = GetStdHandle(STD_OUTPUT_HANDLE);

    if (off > 0) {
        Item item;
        do {
            item = getItem(file, next_pointer);
            next_pointer = item.next_pointer;
            buffer_pointer = print(buffer, buffer_pointer, item.value);
            if (buffer_pointer > buffer_size - 30) {
                write_bytes(stdout_, buffer, buffer_pointer);
                buffer_pointer = 0;
            }
        } while (next_pointer != 0);
        if (buffer_pointer > 0) {
            write_bytes(stdout_, buffer, buffer_pointer);
        }
    }
    CloseHandle(file);

    return 0;
}