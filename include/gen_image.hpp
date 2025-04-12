#ifndef GEN_IMG_HPP
#define GEN_IMG_HPP
#include "fpdfview.h"

/**
 * saves the bitmap image to png 
 * using opencv which scales it as required
 *
 **/
void SaveBitmapAsPNG(FPDF_BITMAP bitmap, const char* filename, float scale_n);

/**
 * Generates bitmap image using pdfmium
 * and calls SaveBitmapAsPNG()to save it
 **/
void gen_page_image(const char* file_name,int page_number,float scale_factor);

/**
 * get the total pages of the pdf
 **/
int get_total_pages(const char* pdf_file_name);

/**
 * return pdf document
 **/
FPDF_DOCUMENT getPDF_Doc(const char* pdf_file_name);
#endif
