#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>


const int MAX_BYTE_VALUE = 0xff;

const int HEADER_FIELD_SIZE = 0x2;
const int FILE_SIZE_FIELD_SIZE = 0x4;
const int PIXELS_ARRAY_POINTER_OFFSET = 0x0A;
const int PIXELS_ARRAY_POINTER_SIZE = 0x4;
const int IMAGE_WIDTH_OFFSET = 0x12;
const int IMAGE_WIDTH_SIZE = 0x4;
const int IMAGE_HEIGHT_SIZE = 0x4;

const int BYTES_PER_PIXEL = 0x4;
const uint32_t FULL_MASK = 0xffffffff;

typedef uint8_t byte_t;

void copy_bytes(int N, FILE *source, FILE *target) {
    char *buffer = malloc(N);
    fread(buffer, 1, N, source);
    fwrite(buffer, 1, N, target);
}

void skip_bytes(int N, FILE *source) {
    fseek(source, N, SEEK_CUR);
}

int read_value(int bytes_count, FILE *file) {
    int value;
    fread(&value, 1, bytes_count, file);
    return value;
}

void write_value(int bytes_count, int value, FILE *file) {
    fwrite(&value, 1, bytes_count, file);
}

int copy_value(int bytes_count, FILE *source, FILE *target) {
    int value = read_value(bytes_count, source);
    write_value(bytes_count, value, target);
    return value;
}

#pragma pack(push, 1)
typedef struct {
    byte_t a;
    byte_t b;
    byte_t g;
    byte_t r;

} Pixel;
#pragma pack(pop)

char *pixel_fields;

void combine_pixels(Pixel *p1, Pixel *p2, Pixel *res) {

    float transparency = (float) p2->a / (float) MAX_BYTE_VALUE;

    __m128i mask = _mm_set1_epi32(FULL_MASK);
    __m128i xmm0 = _mm_set_epi32(p1->b, p1->g, p1->r, 0);
    __m128i xmm1 = _mm_set_epi32(p2->b, p2->g, p2->r, 0);
    xmm1 = _mm_sub_epi32(xmm1, xmm0);

    __m128 xmm3 = _mm_cvtepi32_ps(xmm1);
    __m128 xmm2 = _mm_set_ps1(transparency);
    xmm3 = _mm_mul_ps(xmm3, xmm2);
    xmm1 = _mm_cvtps_epi32(xmm3);
    xmm0 = _mm_add_epi32(xmm0, xmm1);

    _mm_maskmoveu_si128(xmm0, mask, (char *) pixel_fields);
    res->b = pixel_fields[12];
    res->g = pixel_fields[8];
    res->r = pixel_fields[4];

//    res->r = p1->r + (byte_t) ((float) ((int16_t) p2->r - p1->r) * transparency);
//    res->g = p1->g + (byte_t) ((float) ((int16_t) p2->g - p1->g) * transparency);
//    res->b = p1->b + (byte_t) ((float) ((int16_t) p2->b - p1->b) * transparency);

    res->a = p1->a;
    if (p2->a > res->a) {
        res->a = p2->a;
    }
}

void copy_pixels(int pixels_count, FILE *source1, FILE *source2, FILE *result) {
    pixel_fields = (char *) malloc(sizeof(int) * BYTES_PER_PIXEL);
    Pixel pixel1, pixel2;
    Pixel result_pixel;
    for (int i = 0; i < pixels_count; ++i) {
        fread(&pixel1, 1, BYTES_PER_PIXEL, source1);
        fread(&pixel2, 1, BYTES_PER_PIXEL, source2);

        combine_pixels(&pixel1, &pixel2, &result_pixel);

        fwrite(&result_pixel, 1, BYTES_PER_PIXEL, result);
    }
    free(pixel_fields);
}


void overlap_image(FILE *source1, FILE *source2, FILE *result) {
    int offset = 0;
    copy_bytes(HEADER_FIELD_SIZE, source1, result);
    offset += HEADER_FIELD_SIZE;

    // Read file_size
    int file_size = copy_value(FILE_SIZE_FIELD_SIZE, source1, result);
    offset += FILE_SIZE_FIELD_SIZE;

    copy_bytes(PIXELS_ARRAY_POINTER_OFFSET - offset, source1, result);
    offset = PIXELS_ARRAY_POINTER_OFFSET;
    int pixels_array_offset = copy_value(PIXELS_ARRAY_POINTER_SIZE, source1, result);
    offset += PIXELS_ARRAY_POINTER_SIZE;
    copy_bytes(IMAGE_WIDTH_OFFSET - offset, source1, result);
    offset = IMAGE_WIDTH_OFFSET;

    // Read WIDTH and HEIGHT
    int32_t width = copy_value(IMAGE_WIDTH_SIZE, source1, result);
    int32_t height = copy_value(IMAGE_HEIGHT_SIZE, source1, result);
    offset += IMAGE_WIDTH_SIZE + IMAGE_HEIGHT_SIZE;

    int pixels_count = width * height;

    copy_bytes(pixels_array_offset - offset, source1, result);
    offset = pixels_array_offset;

    skip_bytes(offset, source2);

    // Begin of pixel array section in source1 and source2

    copy_pixels(pixels_count, source1, source2, result);

    // Copy staff after pixels array

    offset += pixels_count * BYTES_PER_PIXEL;
    copy_bytes(file_size - offset, source1, result);
}

int main(int argc, char **args) {

    FILE *source1 = fopen(args[1], "r");
    FILE *source2 = fopen(args[2], "r");
    FILE *result = fopen(args[3], "w");

    overlap_image(source1, source2, result);

    fclose(source1);
    fclose(source2);
    fclose(result);


    return 0;
}
