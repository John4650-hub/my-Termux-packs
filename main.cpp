#include <iostream>
#include <cstdlib>
#include "fpdfview.h"
#include "fpdf_text.h"

int main(){
  FPDF_InitLibrary();
  FPDF_DOCUMENT document= FPDF_LoadDocument("foo.pdf",NULL);
  if(!document){
    std::cout << "Failed to load PDF\n";
    return 1;
  }
  int page_number = 0;
  FPDF_PAGE page = FPDF_LoadPage(document,page_number);
  if(!page){
    std::cout<< "Failed to load page\n";
    return 1;
  }
  FPDF_TEXTPAGE text_page  = FPDFText_LoadPage(page);
  int text_length = FPDFText_CountChars(text_page);
  char* buffer = (char*)malloc(text_length + 1);
  for(int i=0; i<text_length;i++)
    buffer[i] = FPDFText_GetUnicode(text_page, i);
  buffer[text_length] = '\0';
  std::cout<<buffer<<"\n";

  free(buffer);
  FPDFText_ClosePage(text_page);
  FPDF_ClosePage(page);
  FPDF_CloseDocument(document);
  FPDF_DestroyLibrary();
  return 0;
}
