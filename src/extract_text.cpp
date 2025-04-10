#include <string.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "extract_text.hpp"

cv::Mat getRegionOfInterest(const cv::Mat& image, int x, int y, int width, int height) {
    // Crop the region of interest (ROI)
    cv::Rect roi(x, y, width, height);
    return image(roi);
}

cv::Mat convert_ROI_grayscale(){
    // Convert the cropped image to grayscale for better OCR results
    cv::Mat croppedImage = getRegionOfInterest();
    cv::Mat grayImage;
    cv::cvtColor(croppedImage, grayImage, cv::COLOR_BGR2GRAY);
    return grayImage;
}

std::string extractText(){
    // Initialize Tesseract API
    tesseract::TessBaseAPI ocr;
    if (ocr.Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY)) {
        std::cerr << "Could not initialize Tesseract." << std::endl;
        return "";
    }
    cv::Mat grayImage = convert_ROI_grayscale();
    // Set the image to Tesseract
    ocr.SetImage(grayImage.data, grayImage.cols, grayImage.rows, grayImage.channels(), grayImage.step);

    // Perform OCR
    char* text = ocr.GetUTF8Text();
    std::string extractedText(text);
    ocr.End();
    delete[] text;

    return extractedText;
}
