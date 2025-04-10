#ifndef GEN_IMG_HPP
#define GEN_IMG_HPP
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

#endif
