#ifdef GEN_W_MUPDF_HPP
#define GEN_W_MUPDF_HPP
#include <mupdf/flitz.h>
/**
 * Get pages
 **/
int mupdf_get_total_pages(fz_context* ctx, fz_document* doc);
/**
 * generate page image
 **/

int mupdf_gen_page(const char* name_pdf,int page_number)
#endif
