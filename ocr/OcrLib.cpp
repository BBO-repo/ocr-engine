#include "OcrLib.hpp"

INITIALIZE_EASYLOGGINGPP

namespace ocr
{
	void open_image(const std::string &image_path, cv::Mat &img, bool display)
	{
		img = cv::imread(image_path, cv::IMREAD_COLOR);
		if (display)
		{
			cv::imshow("Display window", img);
			cv::waitKey(0); // Wait for a keystroke in the window
		}
	}

	void compute_orientation(const std::string &image_path)
	{

		cv::Mat imgGray, imgColor;
		imgColor = cv::imread(image_path, cv::IMREAD_COLOR);
		cv::cvtColor(imgColor, imgGray, cv::COLOR_BGR2GRAY);

		// cv::imshow("color", imgColor);
		// cv::imshow("gray", imgGray);
		// cv::waitKey(0);

		tesseract::TessBaseAPI api;
		api.Init(NULL, "fra");
		// api.SetVariable("debug_file", "tesseract.log");

		// Get OSD - old example code
		tesseract::Orientation orientation;
		tesseract::WritingDirection direction;
		tesseract::TextlineOrder order;
		float deskew_angle;
		api.SetPageSegMode(tesseract::PSM_AUTO_OSD);
		api.SetImage(imgGray.data, imgGray.cols, imgGray.rows, 1, imgGray.step);
		api.Recognize(0);
		tesseract::PageIterator *it = api.AnalyseLayout();
		it->Orientation(&orientation, &direction, &order, &deskew_angle);
		printf("Old Method\nOrientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n"
			   "Deskew angle: %.4f\n",
			   orientation, direction, order, deskew_angle);

		// Get OSD - new code
		int orient_deg;
		float orient_conf;
		const char *script_name;
		float script_conf;
		api.DetectOrientationScript(&orient_deg, &orient_conf, &script_name, &script_conf);
		printf("New method\n Orientation in degrees: %d\n Orientation confidence: %.2f\n"
			   " Script: %s\n Script confidence: %.2f\n",
			   orient_deg, orient_conf,
			   script_name, script_conf);

		cv::Mat rotGray, rotColor;
		cv::rotate(imgGray, rotGray, cv::ROTATE_90_COUNTERCLOCKWISE);
		cv::rotate(imgColor, rotColor, cv::ROTATE_90_COUNTERCLOCKWISE);
		// cv::imshow("flipped ROTATE_90_CLOCKWISE", flipped);
		// cv::waitKey(0);

		// Retrieve text
		api.SetPageSegMode(tesseract::PSM_SPARSE_TEXT);
		api.SetVariable("tessedit_char_whitelist", ".,01234567890");
		// api.SetVariable("tessedit_char_blacklist", "!?@#$%&*()<>_-+=/:;'\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		// api.setVariable("classify_bln_numeric_mode", "1");
		api.SetImage(rotGray.data, rotGray.cols, rotGray.rows, 1, rotGray.step);
		api.Recognize(0);

		tesseract::ResultIterator *ri = api.GetIterator();
		// // tesseract::PageIteratorLevel level = tesseract::RIL_TEXTLINE;
		tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
		if (ri != 0)
		{
			do
			{
				const char *word = ri->GetUTF8Text(level);
				float conf = ri->Confidence(level);
				int x1, y1, x2, y2;
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);

				printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n", word, conf, x1, y1, x2, y2);
				cv::rectangle(rotColor, cv::Point2i(x1, y1), cv::Point2i(x2, y2), cv::Scalar(0, 255, 0), 3);
				cv::imshow("ocr", rotColor);

				delete[] word;

			} while (ri->Next(level));
		}
		cv::waitKey(0);
		// // std::cout << api2->GetTSVText(0) << std::endl;

		// Release memory
		api.End();
	}

	cv::Rect get_largest_contour_rect(const std::string &image_path)
	{
		return get_largest_contour_rect(cv::imread(image_path, cv::IMREAD_COLOR));
	}

	cv::Rect get_largest_contour_rect(const cv::Mat &image)
	{
		// 1. convert to gray scale
		cv::Mat image_gray;
		cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);
		//cv::imshow("source", image);
		//cv::imshow("gray", image_gray);

		// 2. find contours
		cv::Mat canny_output;
		double thresh = 0;
		cv::blur(image_gray, image_gray, cv::Size(3, 3)); // blur image for metter contours detection
		cv::Canny(image_gray, canny_output, thresh, thresh * 2);
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(canny_output, contours, hierarchy, cv::RETR_EXTERNAL /*RETR_TREE*/, cv::CHAIN_APPROX_SIMPLE);
		
		// 3. sort contours
		// lambda to compare contour respectively to their area - used to sort contour from bigger area to lower area
		auto is_contour_bigger = [](std::vector<cv::Point> l_contour, std::vector<cv::Point> r_contour)
		{
			int l_area = (boundingRect(l_contour)).area(); // fabs(contourArea(cv::Mat(contour1)));
			int r_area = (boundingRect(r_contour)).area(); // fabs(contourArea(cv::Mat(contour2)));
			return (l_area > r_area);
		};
		std::sort(contours.begin(), contours.end(), is_contour_bigger);

		// 4. only keep the largest contour 
		size_t n_largest_contours = 1; // contours.size()
		std::vector<std::vector<cv::Point>> contours_poly(n_largest_contours);
		std::vector<cv::Rect> boundRect(n_largest_contours);
		for (size_t i = 0; i < n_largest_contours; i++)
		{
			approxPolyDP(contours[i], contours_poly[i], 3, false /*true*/);
			boundRect[i] = boundingRect(contours_poly[i]);
		}

		return boundRect[0];		
	}

} // ocr
