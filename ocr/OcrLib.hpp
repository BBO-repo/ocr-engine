#ifndef OCRLIB_H_
#define OCRLIB_H_

#include <iostream>
#include <string>
#include <regex>
#include <numbers>

#include <leptonica/allheaders.h>

#include <tesseract/baseapi.h>
#include <tesseract/resultiterator.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

#include <easylogging++.h>

#include "OcrTypes.hpp"

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
	 * @param text_orientation the estimated orientation
	 * @param deskew_angle the estimated deskew_angle
	 */
	void compute_text_orientation(const std::string & image_path, tesseract::Orientation & text_orientation, float & deskew_angle);

	/**
	 * @brief compute text orientation of an image using tesseract
	 * 
	 * @param image_path the opencv mat image
	 * @param text_orientation the estimated orientation
	 * @param deskew_angle the estimated deskew_angle in radian
	 */
	void compute_text_orientation(const cv::Mat & image, tesseract::Orientation & text_orientation, float & deskew_angle);

	/**
	 * @brief change orientation of image for having a horizontal lfet to right text orientation 
	 * 
	 * @param image the opencv mat image
	 * @param text_orientation the estimated orientation
	 */
	void rectify_orientation (cv::Mat & image, tesseract::Orientation & text_orientation);

	/**
	 * @brief deskew an image
	 * 
	 * @param image the opencv mat image
	 * @param angle the deskew angle in radian
	 */
	void deskew_image(cv::Mat & image, double angle);

	/**
	 * @brief Get the largest contour rectangle from the image in file
	 * 
	 * @param image_path the path to the image
	 * @return cv::Rect the rectangle containing the largest contour
	 */
	cv::Rect get_largest_contour_rect(const std::string & image_path);
	
	/**
	 * @brief Get the largest contour rectangle of an image
	 * 
	 * @param image the image input
	 * @return cv::Rect the rectangle containing the largest contour
	 */
	cv::Rect get_largest_contour_rect(const cv::Mat & image);


	void rectify_orientation (cv::Mat & imgGray, tesseract::Orientation & text_orientation, float & deskew_angle);

	/**
	 * @brief Retrieve the patient insurance number form a insurance card picture  
	 * 
	 * @param document_path the path to image  
	 * @param status processing status
	 * @param message result of ocr processing
	 * @param display_internals display intermediary result in an opencv window
	 */
	void ocr_insurance_card(const std::string & document_path, ocr::status & status, std::string & ocr_data, bool display_internals = false);

}

#endif // OCRENGINE_H_
