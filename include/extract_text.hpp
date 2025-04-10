#ifndef EXTRACT_TEXT_HPP
#define EXTRACT_TEXT_HPP

#include <string.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include <iostream>


/**
 * get text selection by passing the dimesion
 * and position of the selection box
 **/
void getSelectedText(const char* image_name,float x, float y, int width,int height);

/**
 * Extract the selected text from the
 * grayImage
 **/
std::string extractText(const cv::Mat& grayImage);

/**
 * covert the Region Of Interest to
 * grayscale to ease the ocr process
 **/
cv::Mat convert_ROI_grayscale(const cv::Mat& croppedImage);

/**
 * crop out the image to get the
 * Region Of Interest
 **/
cv::Mat getRegionOfInterest(const cv::Mat& image, int x, int y, int width, int height);

#endif
