#ifndef GEN_IMG_HPP
#define GEN_IMG_HPP

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
