#include <stdio.h>
//#include <string.h>
#include <assert.h>
#include <inttypes.h>

void normalize_path(char * path);

char *read_name(char *path) {
    while (*path && *path != '/') {
        ++path;
    }
    return path;
}

size_t strlen_(char *str) {
    char *begin = str;
    while (*(str++)) {}
    return (size_t) (str - begin - 1);
}

int strncmp_(char *str1, char *str2, int n) {
    for (int i = 0; i < n; ++i) {
        if (!*str1 || !*str2) { return 1; }
        if (*(str1++) != *(str2++)) { return 1; }
    }
    return 0;
}

int strcmp_(char *str1, char *str2) {
    if (strlen_(str1) != strlen_(str2)) {
        return 1;
    }
    return strncmp_(str1, str2, strlen_(str1));
}

void strncpy_(char *dest, char *source, int n) {
    for (int i = 0; i < n; ++i) {
        dest[i] = source[i];
    }
}

void strcpy_(char* dest, char* source){
    strncpy_(dest, source, strlen_(source) + 1);
}

void shift_path(char *path, int64_t shift) {
    if (shift < 0) {
        char *end = path + strlen_(path);
        while (end >= path) {
            *(end - shift) = *end;
            --end;
        }
    } else {
        do {
            *path = *(path + shift);
        } while (*(path++));
    }
}


size_t restore_coupler(char *path) {
    char *coupler_begin = path;
    while (*path == '/') {
        ++path;
    }
    size_t coupler = path - coupler_begin;
    if (coupler > 1) {
        shift_path(coupler_begin, coupler - 1);
    }
    return coupler > 0;
};

int normalize_relative_path(char *path) {
    if (!*path) {
        return 0;
    }
    path += restore_coupler(path);

    char *path_begin = path;
    path = read_name(path);
    path += restore_coupler(path);
    if (!strncmp_(path_begin, "..", 2)) {
        shift_path(path_begin, path - path_begin); // skip "../"
        return 1;
    }
    if (!strncmp_(path_begin, ".", 1)) {
        shift_path(path_begin, path - path_begin); // skip "./"
        return normalize_relative_path(path_begin);
    }
    if (normalize_relative_path(path)) {
        shift_path(path_begin, path - path_begin);
        return normalize_relative_path(path_begin);
    }
    return 0;
}


void normalize_path(char *path) {
    char *begin_path = path;
    while (normalize_relative_path(path)) {
        shift_path(path, -3);
        strncpy_(path, "../", 3);
        path += 3;
    }
    if (strlen_(begin_path) == 0) {
        strcpy_(begin_path, "./");
    }
}

//int test(const char *input_path_, char *correct_path_) {
//    int max_size = 100;
//    char input_path[max_size], correct_path[max_size];
//    strcpy_(input_path, input_path_);
//    strcpy_(correct_path, correct_path_);
//    printf("Input path: %s\n", input_path);
//    normalize_path(input_path);
//    printf("Normalized path: %s\n", input_path);
//    printf("Correct normalized path: %s\n", correct_path);
//    int is_passed = !strcmp_(correct_path, input_path);
//    char *test_result = is_passed ? "passed" : "failed";
//    printf("Test %s\n\n", test_result);
//    return is_passed;
//}
//
//int main() {
//    assert(test("abrakadabra///abc", "abrakadabra/abc"));
//    assert(test("/var/log/../lib/./ejexec", "/var/lib/ejexec"));
//    assert(test("a/////b", "a/b"));
//    assert(test("abc/bcd////../", "abc/"));
//    assert(test("abc/bcd////./", "abc/bcd/"));
//    assert(test("abc", "abc"));
//    assert(test("abc/", "abc/"));
//    assert(test("////abcfsafdsa/fsdaf/fsda/sd", "/abcfsafdsa/fsdaf/fsda/sd"));
//    assert(test("////abcfsafdsa/fsdaf/fsda/sd/", "/abcfsafdsa/fsdaf/fsda/sd/"));
//    assert(test("././././", "./"));
//    assert(test("../", "../"));
//    assert(test(".././", "../"));
//    assert(test("../abc", "../abc"));
//    assert(test("./", "./"));
//    assert(test("/./", "/"));
//    assert(test("//hello//world/", "/hello/world/"));
//    assert(test("//////", "/"));
//    assert(test("/", "/"));
//    assert(test("d", "d"));
//    assert(test("fsad/////", "fsad/"));
//    assert(test("fsad/././././", "fsad/"));
//    assert(test("a/../b/../c/", "c/"));
//    assert(test("a/././././././", "a/"));
//    assert(test("a/././../b/././././", "b/"));
//    assert(test("/a/b/c/../../../", "/"));
//    assert(test("///", "/"));
//    assert(test("/a/../", "/"));
//    assert(test("/b/b/b", "/b/b/b"));
//    assert(test("/b/b/b/./././././", "/b/b/b/"));
//    assert(test("a/../b", "b"));
//    assert(test("../../", "../../"));
//    assert(test("../.././.././", "../../../"));
//    assert(test("/././././", "/"));
//    return 0;
//}
