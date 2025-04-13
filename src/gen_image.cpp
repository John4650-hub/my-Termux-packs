#include "gen_image.hpp"
#include <iostream>
#include <sstream>
#include <string.h>
#include <cstdlib>
#include "fpdfview.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
void SaveBitmapAsPNG(FPDF_BITMAP bitmap, const char* filename, float scale_factor) {
    // Get bitmap details
    int width = FPDFBitmap_GetWidth(bitmap);
    int height = FPDFBitmap_GetHeight(bitmap);
    unsigned char* buffer = (unsigned char*)FPDFBitmap_GetBuffer(bitmap);

    cv::Mat image;
    cv::Mat mat(height,width,CV_8UC4,buffer);
    //convert to RGBA
    cv::cvtColor(mat, mat, cv::COLOR_BGRA2RGBA);
    image=mat;
    //scale factor
    int new_width=static_cast<int>(image.cols*scale_factor);
    int new_height = static_cast<int>(image.rows * scale_factor);
    cv::Mat resized_image;
    cv::resize(image,resized_image,cv::Size(new_width,new_height),0,0,cv::INTER_CUBIC);
    cv::imwrite(filename,resized_image);
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

    int width = (int)FPDF_GetPageWidth(page);
    int height = (int)FPDF_GetPageHeight(page);
    FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 0);
    FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF); // White background
    FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, 0);

    // Save bitmap
    std::ostringstream oss;
    oss<<"/storage/emulated/0/.Apps/ReadEra/images/page"<<page_number<<".png";
    std::string out_name_str=oss.str();
    const char* output_page_name = out_name_str.c_str();
    SaveBitmapAsPNG(bitmap, output_page_name,scale_factor);

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
