#include <stdio.h>
#include <stdlib.h>

void page_to_png(const char* name_pdf,int page_pdf){
  float zoom=5.0f,rotate=0.0f;
  int x,y, page_count,page_number;
  fz_context *ctx;
  fz_document *doc;
  fz_pixmap *pix;
  fz_matrix ctm;

  ctx =fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
  if(!ctx)
}
