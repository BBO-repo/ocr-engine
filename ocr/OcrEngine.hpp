#ifndef OCRENGINE_H_
#define OCRENGINE_H_

#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <list>

#include <leptonica/allheaders.h>

#include <tesseract/baseapi.h>
#include <tesseract/resultiterator.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace ocr
{
	/**
	 * @brief open an image with opencv and display it if wanted
	 * 
	 * @param image_path the path to the image to display
	 * @param img opencv matrix containing the image
	 * @param display boolean if set to true display the image in a window else do not display it
	 */
	void open_image(const std::string & image_path, cv::Mat &img, bool display = false);

	/**
	 * @brief compute text orientation of an image using tesseract
	 * 
	 * @param image_path the path to the image
	 */
	void compute_orientation(const std::string & image_path);

}

#endif // OCRENGINE_H_
