#ifndef GEN_W_MUPDF_HPP
#define GEN_W_MUPDF_HPP
/**
 * Get pages
 **/
int mupdf_get_total_pages(const char* fname);
/**
 * generate page image
 **/

int mupdf_gen_page(const char* name_pdf,int page_number,float zoom,int sf);
#endif
