#include <iostream>
#include <cstdlib>
#include "fpdfview.h"
#include <opencv2/opencv.hpp>

void SaveBitmapAsPNG(FPDF_BITMAP bitmap, const char* filename) {
    // Get bitmap details
    int width = FPDFBitmap_GetWidth(bitmap);
    int height = FPDFBitmap_GetHeight(bitmap);
    unsigned char* buffer = (unsigned char*)FPDFBitmap_GetBuffer(bitmap);

    cv::Mat image;
    cv::Mat mat(height,width,CV_8UC4,buffer);
    //convert to RGBA
    cv::cvtColor(mat, mat, cv::COLOR_BGRA2RGBA);
    image=mat;
    //scaling factor
    double scaling_factor=2.0;
    int new_width=static_cast<int>(image.cols*scaling_factor);
    int new_height = static_cast<int>(image.rows * scaling_factor);
    cv::Mat resized_image;
    cv::resize(image,resized_image,cv::Size(new_width,new_height),0,0,cv::INTER_CUBIC);
    cv::imwrite(filename,image);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <integer>" << std::endl;
        return 1;
    }

    int page_number = std::atoi(argv[1]);
    FPDF_InitLibrary();

    FPDF_DOCUMENT document = FPDF_LoadDocument("foo.pdf", NULL);
    if (!document) {
        std::cout << "Failed to load PDF\n";
        FPDF_DestroyLibrary();
        return 1;
    }

    FPDF_PAGE page = FPDF_LoadPage(document, page_number);
    if (!page) {
        std::cout << "Failed to load page\n";
        FPDF_CloseDocument(document);
        FPDF_DestroyLibrary();
        return 1;
    }

    int width = (int)FPDF_GetPageWidth(page);
    int height = (int)FPDF_GetPageHeight(page);
    FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 0);
    FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF); // White background
    FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, 0);

    // Save bitmap
    SaveBitmapAsPNG(bitmap, "foo.png");

    // Cleanup
    FPDFBitmap_Destroy(bitmap);
    FPDF_ClosePage(page);
    FPDF_CloseDocument(document);
    FPDF_DestroyLibrary();

    return 0;
}
