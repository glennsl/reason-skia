/*
 * Use this file for building any C-layer functionality that we want to keep out of Reason
 */

#include "c_stubs.h"

#include <stdlib.h>
#include <stdio.h>


sk_color_t reason_skia_stub_sk_color_set_argb(int alpha, int red, int green, int blue)
{
    return sk_color_set_argb(alpha, red, green, blue);
}

uint32_t reason_skia_stub_sk_color_get_a(sk_color_t color)
{
    return sk_color_get_a(color);
}

uint32_t reason_skia_stub_sk_color_get_r(sk_color_t color)
{
    return sk_color_get_r(color);
}

uint32_t reason_skia_stub_sk_color_get_g(sk_color_t color)
{
    return sk_color_get_g(color);
}

uint32_t reason_skia_stub_sk_color_get_b(sk_color_t color)
{
    return sk_color_get_b(color);
}

void test_typeface() {
    printf("Hello, world!\n");
  sk_typeface_t *typeface = sk_typeface_create_from_file("/Users/bryphe/reason-skia/example/Orbitron-Medium.ttf", 0); 
  int unitsPerEm = sk_typeface_get_units_per_em(typeface);
    printf("Hello, again: %d\n", unitsPerEm);
  //let maybeTypeface = Typeface.makeFromFile(filePath, 0);
}

void test_api() {
    printf("Creating image info...\n");
    sk_imageinfo_t* imageInfo= (sk_imageinfo_t *)malloc(sizeof(sk_imageinfo_t));
    imageInfo->height = 256l;
    imageInfo->width = 256l;
    imageInfo->colorType = RGBA_8888_SK_COLORTYPE;
    imageInfo->alphaType = PREMUL_SK_ALPHATYPE;
    imageInfo->colorspace = NULL;
    printf("ImageInfo created\n");

    sk_surface_t *surface = sk_surface_new_raster(
        imageInfo,
        0,
        NULL);
    printf("Surface created: %d\n", surface);

    sk_canvas_t *canvas = sk_surface_get_canvas(surface);
    printf("Canvas created: %d\n", canvas);

    sk_image_t *image = sk_surface_new_image_snapshot(surface);
    printf("Created image snapshot: %d\n", image);

    sk_data_t *data = sk_image_encode(image);
    printf("Encoded image\n");

    char* dataString = (char *)sk_data_get_data(data);
    size_t dataSize = sk_data_get_size(data);
    printf("Datastring: %d (size: %d)\n", dataString, dataSize);
    FILE* fp = fopen("test_out.png", "w");
    printf("Opened file...\n");
    for (int i = 0; i < dataSize; i++) {
        fputc(dataString[i], fp);
    }
    //fprintf(fp, "%s", dataString);
    fclose(fp);

};
