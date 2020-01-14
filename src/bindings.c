/*
 * Use this file for building any C-layer functionality that we want to keep out of Reason
 */

#include <stdlib.h>
#include <stdio.h>

#include "sk_canvas.h"
#include "sk_data.h"
#include "sk_image.h"
#include "sk_paint.h"
#include "sk_surface.h"
#include "sk_types.h"
#include "sk_typeface.h"

#include <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define Val_none Val_int(0)

static value
Val_some( value v )
{   
    CAMLparam1( v );
    CAMLlocal1( some );
    some = caml_alloc(1, 0);
    Store_field( some, 0, v );
    CAMLreturn( some );
}

sk_color_t resk_color_set_argb(value vAlpha, value vRed, value vGreen, value vBlue)
{
    int alpha = Int_val(vAlpha);
    int red = Int_val(vRed);
    int blue = Int_val(vBlue);
    int green = Int_val(vGreen);
    return Val_int(sk_color_set_argb(alpha, red, green, blue));
}

void test_typeface() {
    printf("Hello, world!\n");
  sk_typeface_t *typeface = sk_typeface_create_from_file("/Users/bryphe/reason-skia/example/Orbitron-Medium.ttf", 0); 
  int unitsPerEm = sk_typeface_get_units_per_em(typeface);
    printf("Hello, again: %d\n", unitsPerEm);
  //let maybeTypeface = Typeface.makeFromFile(filePath, 0);
}

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define INIT_STRUCT(TYPENAME) static struct custom_operations CONCAT(TYPENAME, __custom_ops)= { \
    identifier: #TYPENAME, \
    finalize: custom_finalize_default, \
    compare: custom_compare_default, \
    hash: custom_hash_default, \
    serialize: custom_serialize_default, \
    deserialize: custom_deserialize_default \
};

#define INIT_POINTER(TYPENAME, FINALIZER) void FINALIZER(value v); \
typedef struct CONCAT(__resk_internal__, TYPENAME) { \
    TYPENAME *v;   \
} CONCAT(TYPENAME, __wrapped); \
static struct custom_operations CONCAT(TYPENAME, __ptr_custom_ops)= { \
    identifier: #TYPENAME, \
    finalize: FINALIZER, \
    compare: custom_compare_default, \
    hash: custom_hash_default, \
    serialize: custom_serialize_default, \
    deserialize: custom_deserialize_default \ 
};

#define ALLOC_STRUCT(TYPENAME, INSTANCE, OUTNAME) OUTNAME = caml_alloc_custom(&CONCAT(TYPENAME, __custom_ops), sizeof(TYPENAME), 0, 1); \
    memcpy(Data_custom_val(OUTNAME), &INSTANCE, sizeof(TYPENAME));

#define ALLOC_POINTER(TYPENAME, INSTANCE, OUTNAME) OUTNAME = caml_alloc_custom(&CONCAT(TYPENAME, __ptr_custom_ops), sizeof(CONCAT(TYPENAME, __wrapped)), 0, 1); \
        CONCAT(TYPENAME, __wrapped) __internal_wrapper; \
        __internal_wrapper.v = INSTANCE; \
        memcpy(Data_custom_val(OUTNAME), &__internal_wrapper, sizeof(CONCAT(TYPENAME, __wrapped)));
    
#define STRUCT_VAL(TYPENAME, VALUE) (TYPENAME*)Data_custom_val(VALUE)

#define POINTER_VAL(TYPENAME, VALUE) (TYPENAME*)((CONCAT(TYPENAME, __wrapped)*)(void *)Data_custom_val(VALUE))->v

INIT_STRUCT(sk_imageinfo_t)
INIT_STRUCT(sk_rect_t)

INIT_POINTER(sk_typeface_t, resk_finalize_sk_typeface);
INIT_POINTER(sk_surface_t, resk_finalize_sk_surface);
INIT_POINTER(sk_paint_t, resk_finalize_sk_paint);

void resk_finalize_sk_typeface(value vTypeface) {
    sk_typeface_t *typeface = POINTER_VAL(sk_typeface_t, vTypeface);
    printf("Finalizing typeface: %d\n", typeface);
    sk_typeface_unref(typeface);
    printf("Typeface finalized!\n");
};

void resk_finalize_sk_surface(value vSurface) {
    sk_surface_t *surface = POINTER_VAL(sk_surface_t, vSurface);
    printf("Finalizing surface: %d\n", surface);
    sk_surface_unref(surface);
    printf("Surface finalized!\n");
};

void resk_finalize_sk_paint(value vPaint) {
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    printf("Finalizing paint: %d\n", paint);
    sk_paint_delete(paint);
    printf("Paint finalized!\n");
};

CAMLprim value resk_imageinfo_make(value vWidth, value vHeight) {
    CAMLparam2(vWidth, vHeight);
    CAMLlocal1(v);

    int width = Int_val(vWidth);
    int height = Int_val(vHeight);

    sk_imageinfo_t imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = RGBA_8888_SK_COLORTYPE;
    imageInfo.alphaType = PREMUL_SK_ALPHATYPE;
    imageInfo.colorspace = NULL;

    ALLOC_STRUCT(sk_imageinfo_t, imageInfo, v);
    CAMLreturn(v);
};

CAMLprim value resk_rect_make(value vLeft, value vTop, value vRight, value vBot) {
    CAMLparam4(vLeft, vTop, vRight, vBot);
    CAMLlocal1(v);
    
    float left = Double_val(vLeft);
    float top = Double_val(vTop);
    float right = Double_val(vRight);
    float bot = Double_val(vBot);

    sk_rect_t rect;
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bot;

    ALLOC_STRUCT(sk_rect_t, rect, v);
    CAMLreturn(v);
}

CAMLprim value resk_typeface_create_from_file(value vPath, value vIndex) {
    CAMLparam2(vPath, vIndex);
    CAMLlocal2(v, font);

    char *szPath = String_val(vPath);
    int index = Int_val(vIndex);

    sk_typeface_t *pTypeface = sk_typeface_create_from_file(szPath, index);

    if (!pTypeface) {
        v = Val_none;
    } else {
        ALLOC_POINTER(sk_typeface_t, pTypeface, font);
        v = Val_some(font);
    }
    
    CAMLreturn(v);

}

CAMLprim value resk_typeface_get_units_per_em(value vTypeface) {
    sk_typeface_t *typeface = POINTER_VAL(sk_typeface_t, vTypeface);
    int unitsPerEM = sk_typeface_get_units_per_em(typeface);

    return Val_int(unitsPerEM);
}

CAMLprim value resk_paint_make() {
    CAMLparam0();
    CAMLlocal1(v);

    ALLOC_POINTER(sk_paint_t, sk_paint_new(), v);

    CAMLreturn(v);
}

CAMLprim value resk_paint_set_color(value vPaint, value vColor) {
    sk_color_t color = (sk_color_t)Int_val(vColor);
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    
    sk_paint_set_color(paint, color);
    return Val_unit;
}

CAMLprim value resk_paint_set_antialias(value vPaint, value vAntialias) {
    int antialias = Bool_val(vAntialias);
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    sk_paint_set_antialias(paint, antialias);
    return Val_unit;
}

CAMLprim value resk_paint_set_lcd_render_text(value vPaint, value vLCD) {
    int lcd = Bool_val(vLCD);
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    sk_paint_set_lcd_render_text(paint, lcd);
    return Val_unit;
}

CAMLprim value resk_paint_set_subpixel_text(value vPaint, value vSubpixel) {
    int subpixel = Bool_val(vSubpixel);
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    sk_paint_set_subpixel_text(paint, subpixel);
    return Val_unit;
}

CAMLprim value resk_paint_set_text_size(value vPaint, value vTextSize) {
    float textSize = Double_val(vTextSize);
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    
    sk_paint_set_textsize(paint, textSize);
    return Val_unit;
}

CAMLprim value resk_paint_set_typeface(value vPaint, value vTypeface) {
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    sk_typeface_t *typeface = POINTER_VAL(sk_typeface_t, vTypeface);
    sk_paint_set_typeface(paint, typeface);
    return Val_unit;
}

CAMLprim value resk_surface_new_raster(value vImageInfo) {
    CAMLparam1(vImageInfo);
    CAMLlocal1(v);

    sk_imageinfo_t* imageInfo = STRUCT_VAL(sk_imageinfo_t, vImageInfo);
    
    sk_surfaceprops_t* surfaceProps = sk_surfaceprops_new(0,
        RGB_H_SK_PIXELGEOMETRY
        );

    sk_surface_t *surface = sk_surface_new_raster(
        imageInfo,
        0,
        surfaceProps);

    ALLOC_POINTER(sk_surface_t, surface, v);
    printf("Surface created: %d\n", surface);
    CAMLreturn(v);
}

CAMLprim value resk_surface_get_canvas(value vSurface) {
    CAMLparam1(vSurface);

    sk_surface_t *pSurface = POINTER_VAL(sk_surface_t, vSurface);
    sk_canvas_t *pCanvas = sk_surface_get_canvas(pSurface);
    printf("Canvas created: %d\n", pCanvas);
    CAMLreturn((value)pCanvas);
};

CAMLprim value resk_canvas_draw_paint(value vCanvas, value vPaint) {
    sk_canvas_t *canvas = (sk_canvas_t *)vCanvas;
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);

    sk_canvas_draw_paint(canvas, paint);
    return Val_unit;
}

CAMLprim value resk_canvas_draw_rect(value vCanvas, value vRect, value vPaint) {
    sk_canvas_t *canvas = (sk_canvas_t *)vCanvas;
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    sk_rect_t *rect = STRUCT_VAL(sk_rect_t, vRect);

    sk_canvas_draw_rect(canvas, rect, paint);
    return Val_unit;
}

CAMLprim value resk_canvas_draw_oval(value vCanvas, value vRect, value vPaint) {
    sk_canvas_t *canvas = (sk_canvas_t *)vCanvas;
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    sk_rect_t *rect = STRUCT_VAL(sk_rect_t, vRect);

    sk_canvas_draw_oval(canvas, rect, paint);
    return Val_unit;
}

CAMLprim value resk_canvas_draw_text(value vCanvas, value vStr, value vX, value vY, value vPaint) {
    sk_canvas_t *canvas = (sk_canvas_t *)vCanvas;
    sk_paint_t *paint = POINTER_VAL(sk_paint_t, vPaint);
    float x = Double_val(vX);
    float y = Double_val(vY);
    char *str = String_val(vStr);
    int length = strlen(str);

    sk_canvas_draw_text(canvas, str, length, x, y, paint);
    return Val_unit;
}

CAMLprim value test_write_surface(value vSurface) {
    CAMLparam1(vSurface);
    sk_surface_t *surface = POINTER_VAL(sk_surface_t, vSurface);
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
    
    CAMLreturn(Val_unit);
}
