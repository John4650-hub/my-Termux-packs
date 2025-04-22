#include <mupdf/fitz.h>
#include "save_to_png.hpp"
#include "gen_w_mupdf.hpp"
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
    std::cerr <<"can't create mupdf context\n";
    return EXIT_FAILURE;
  }
  try{
    fz_register_document_handlers(ctx);
  }
  catch (const std::runtime_error &err)
  {
    std::cerr << err.what() << "\n";
		fz_drop_context(ctx);
		return EXIT_FAILURE;
  }
  try{
		doc = fz_open_document(ctx, pdf_file_name);
  }
	catch (const std::runtime_error &err)
	{
		
    std::cerr << err.what() << "\n";
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}
  try{
		page_count = fz_count_pages(ctx, doc);
  }
catch (const std::runtime_error &err)
	{
		std::cerr << err.what() << "\n";
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
  return page_count;
}

int mupdf_gen_page(const char* name_pdf,int page_number,float factor){
  int page_count;
  fz_context *ctx;
  fz_document *doc;
  fz_pixmap *pix;
  fz_page* page;
  fz_matrix scale_matrix;
  fz_device* dev;

  ctx =fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
  if(!ctx){
    std::cerr << "couldn't create mupdf context"<< "\n";
    return EXIT_FAILURE;
  }
  try{
    fz_register_document_handlers(ctx);
  }
  catch (const std::runtime_error &err)
  {
    std::cerr << err.what() << "\n";
		fz_drop_context(ctx);
		return EXIT_FAILURE;
  }
  try{
		doc = fz_open_document(ctx, name_pdf);
  }
	
catch (const std::runtime_error &err)
	{
    std::cerr<<err.what()<<"\n";
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}
  try{
    page=fz_load_page(ctx,doc,page_number);
  }
  catch (const std::runtime_error &err){
    std::cerr<<err.what()<<"\n";
    fz_drop_page(ctx,page);
    return EXIT_FAILURE;
  }
  fz_colorspace* cs = fz_device_rgb(ctx);
  fz_rect page_bounds = fz_bound_page(ctx,page);
  float w = page_bounds.x1 - page_bounds.x0;
  float h = page_bounds.y1 - page_bounds.y0;
  float nw_A4 = 595.28f,nh_A4=842.0f;
  std::cout<<"width: "<<w<<"\n";
  std::cout<<"height: "<<h<<"\n";
  std::cout<<"ratio of width to height: "<<w/h<<"\n";
  float scale = fz_min(w/nw_A4,h/nh_A4);
  if(scale<1.0f){
    scale*=2.0;
  }
  nw_A4=w*scale*scale;
  nh_A4=h*scale*scale;
  std::cout<<"scale of image: "<<scale<<"\n";
  scale_matrix=fz_scale(scale,scale);
 // float translated_width = nw_A4-(nw_A4*0.521f*scale);
 // float translated_height = nh_A4 - ((nh_A4/1.95f)*scale);
 // fz_matrix translation_matrix = fz_translate(translated_width,translated_height);
 // fz_matrix final_matrix = fz_concat(scale_matrix,translation_matrix);
  try{
		pix = fz_new_pixmap_with_bbox(ctx, cs,fz_make_irect(0,0,nw_A4,nh_A4),NULL,1);
    fz_clear_pixmap_with_value(ctx,pix,0xfaebd7);
  }
	catch (const std::runtime_error &err)
	{
    std::cerr << err.what() << "\n";
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return EXIT_FAILURE;
}
  
  dev = fz_new_draw_device(ctx,scale_matrix,pix);
  fz_run_page(ctx,page,dev,scale_matrix,NULL);
  std::ostringstream oss;
  oss<<"/storage/emulated/0/.Apps/ReadEra/images/page"<<page_number<<".png";
  std::string out_name_str=oss.str();
  const char* output_page_name = out_name_str.c_str();
  fz_save_pixmap_as_png(ctx, pix, output_page_name);
  fz_drop_pixmap(ctx, pix);
  fz_close_device(ctx,dev);
  fz_drop_device(ctx,dev);
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
	return EXIT_SUCCESS;
}
