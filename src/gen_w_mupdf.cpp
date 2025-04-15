#include <mupdf/fitz.h>
#include "save_to_png.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <cstdlib>

int mupdf_get_total_pages(const char* pdf_file_name){
  fz_context* ctx;
  fz_document* doc;
  int page_count{};
  ctx =fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
  if(!ctx){
    fprintf(stderr,"can't create mupdf context");
    return EXIT_FAILURE;
  }
  fz_try(ctx)
    fz_register_document_handlers(ctx);
  fz_catch(ctx)
  {
    fz_report_error(ctx);
		fprintf(stderr, "cannot register document handlers\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
  }
  fz_try(ctx)
		doc = fz_open_document(ctx, pdf_file_name);
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		fprintf(stderr, "cannot open document\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}
  fz_try(ctx)
		page_count = fz_count_pages(ctx, doc);
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		fprintf(stderr, "cannot count number of pages\n");
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
  return page_count;
}

int mupdf_gen_page(const char* name_pdf,int page_number){
  float zoom=5.0f,rotate=0.0f;
  int width,height, page_count;
  fz_context *ctx;
  fz_document *doc;
  fz_pixmap *pix;
  fz_matrix ctm;

  ctx =fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
  if(!ctx){
    fprintf(stderr,"can't create mupdf context");
    return EXIT_FAILURE;
  }
  fz_try(ctx)
    fz_register_document_handlers(ctx);
  fz_catch(ctx)
  {
    fz_report_error(ctx);
		fprintf(stderr, "cannot register document handlers\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
  }
  fz_try(ctx)
		doc = fz_open_document(ctx, name_pdf);
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		fprintf(stderr, "cannot open document\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	/* Compute a transformation matrix for the zoom and rotation desired. */
	/* The default resolution without scaling is 72 dpi. */
	ctm = fz_scale(zoom / 100, zoom / 100);
	ctm = fz_pre_rotate(ctm, rotate);

  fz_try(ctx)
		pix = fz_new_pixmap_from_page_number(ctx, doc, page_number, ctm, fz_device_rgb(ctx), 0);
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		fprintf(stderr, "cannot render page\n");
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}
  unsigned char* data = pix->samples;
  width=pix->w;
  height=pix->h;
  std::ostringstream oss;
  oss<<"/storage/emulated/0/.Apps/ReadEra/images/page"<<page_number<<".png";
  std::string out_name_str=oss.str();
  const char* output_page_name = out_name_str.c_str();

 SaveBitmapAsPNG(data, output_page_name,width,height);
fz_drop_pixmap(ctx, pix);
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
	return EXIT_SUCCESS;

}
