#include "gen_image.hpp"
#include <iostream>
#include <sstream>
#include <string.h>
#include <cstdlib>
#include "fpdfview.h"
#include <png.h>

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
//get bitmap details
    unsigned char* buffer = (unsigned char*)FPDFBitmap_GetBuffer(bitmap);

    SaveBitmapAsPNG(buffer,output_page_name,width,height);

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
