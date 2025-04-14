#include "gen_image.hpp"
#include <iostream>
#include <sstream>
#include <string.h>
#include <cstdlib>
#include "fpdfview.h"
#include "fpdf_edit.h"
#include <png.h>

void SaveBitmapAsPNG(FPDF_BITMAP bitmap, const char* filename) {
    // Open the file for writing
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        return;
    }

    // Initialize libpng structures
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr, "Failed to create PNG write struct\n");
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "Failed to create PNG info struct\n");
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return;
    }

    // Set up error handling
    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Error during PNG creation\n");
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    // Set the file output
    png_init_io(png, fp);

    // Get bitmap details
    int width = FPDFBitmap_GetWidth(bitmap);
    int height = FPDFBitmap_GetHeight(bitmap);
    int stride = FPDFBitmap_GetStride(bitmap);
    unsigned char* buffer = (unsigned char*)FPDFBitmap_GetBuffer(bitmap);

    // Set PNG metadata
    png_set_IHDR(png, info, width, height, 8 /* bit depth */, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Write header
    png_write_info(png, info);

    // Write the image row by row
    for (int y = 0; y < height; ++y) {
        png_write_row(png, buffer + (y * stride));
    }

    // Finish writing
    png_write_end(png, NULL);

    // Cleanup
    png_destroy_write_struct(&png, &info);
    fclose(fp);

    printf("Saved PNG file: %s\n", filename);
}

FPDF_DOCUMENT getPDF_Doc(const char* pdf_file_name){
  return FPDF_LoadDocument(pdf_file_name, NULL);
}

void gen_page_image(const char* file_name,int page_number,float scale_factor){
    FPDF_InitLibrary();
    FPDF_DOCUMENT document = getPDF_Doc(file_name);
    if (!document) {
        std::cout << "Failed to load PDF\n";
        FPDF_DestroyLibrary();
        return;
    }

    FPDF_PAGE page = FPDF_LoadPage(document, page_number);
    if (!page) {
        std::cout << "Failed to load page\n";
        FPDF_CloseDocument(document);
        FPDF_DestroyLibrary();
        return;
    }
    FPDF_BYTESTRING blend_mode = "Saturation";
    FPDFPageObj_setBlendMode(page,blend_mode);

    int width = (int)FPDF_GetPageWidth(page);
    int height = (int)FPDF_GetPageHeight(page);
    int renderflags = FPDF_PRINTING | FPDF_LCD_TEXT | FPDF_RENDER_NO_SMOOTHIMAGE | FPDF_RENDER_NO_SMOOTHPATH;
    FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 0);
    FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF); // White background
    FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, 0);

    // Save bitmap
    std::ostringstream oss;
    oss<<"/storage/emulated/0/.Apps/ReadEra/images/page"<<page_number<<".png";
    std::string out_name_str=oss.str();
    const char* output_page_name = out_name_str.c_str();
    SaveBitmapAsPNG(bitmap,output_page_name);

    // Cleanup
    FPDFBitmap_Destroy(bitmap);
    FPDF_ClosePage(page);
    FPDF_CloseDocument(document);
    FPDF_DestroyLibrary();
}

int get_total_pages(const char* file_name){
  FPDF_InitLibrary();
  FPDF_DOCUMENT doc = getPDF_Doc(file_name);
  if (!doc){
    std::cerr<<"Invalid pdf failed to open\n";
    FPDF_DestroyLibrary();
    return -1;
  }
  int pages= FPDF_GetPageCount(doc);
  FPDF_CloseDocument(doc);
  FPDF_DestroyLibrary();
return pages;
}
