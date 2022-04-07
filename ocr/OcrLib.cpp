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

	void compute_text_orientation(const std::string &image_path, tesseract::Orientation & text_orientation, float & deskew_angle)
	{

		cv::Mat imgGray, imgColor;
		imgColor = cv::imread(image_path, cv::IMREAD_COLOR);
		
		tesseract::TessBaseAPI api;
		api.Init(NULL, "fra");
		// api.SetVariable("debug_file", "tesseract.log");

		// Get OSD - old example code
		tesseract::WritingDirection direction;
		tesseract::TextlineOrder order;

		//float deskew_angle;
		api.SetPageSegMode(tesseract::PSM_AUTO_OSD);
		cv::cvtColor(imgColor, imgGray, cv::COLOR_BGR2GRAY);
		api.SetImage(imgGray.data, imgGray.cols, imgGray.rows, 1, imgGray.step);
		api.Recognize(0);

		tesseract::PageIterator *it = api.AnalyseLayout();
		it->Orientation(&text_orientation, &direction, &order, &deskew_angle);
		printf("Old Method\nOrientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\nDeskew angle: %.4f\n",text_orientation, direction, order, deskew_angle);

		// Get OSD - new code
		//int orient_deg;
		//float orient_conf;
		//const char *script_name;
		//float script_conf;
		//api.DetectOrientationScript(&orient_deg, &orient_conf, &script_name, &script_conf);
		//printf("New method\n Orientation in degrees: %d\n Orientation confidence: %.2f\nScript: %s\n Script confidence: %.2f\n",orient_deg, orient_conf,script_name, script_conf);

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

	void ocr_insurance_card(const std::string & document_path, ocr::status & status, std::string & ocr_data)
	{
		// 1. load the image
		// 2. determine text orientation and rotate image accordingly
		// 2. try raw Tesseract allowing only digit
		// 3. if failing:
		//    a. estimate orientation and reorientate
		//    b. try to segment to retrieve card area in image and crop
		//    c. apply Tesseract in cropped reoriented image

		// regex pattern for insurance card number
		std::regex card_number_pattern{"^807\\d{17}$"};
		
		status = status::unknown;

		// 1. load the image
		cv::Mat imgColor, imgGray;
		imgColor = cv::imread(document_path, cv::IMREAD_COLOR);
		cv::cvtColor(imgColor, imgGray, cv::COLOR_BGR2GRAY);
		if(imgColor.empty()) { status = status::non_valid_image; return; }
		
		// 1. determine text orientation and rotate image accordingly
		tesseract::Orientation text_orientation{};
		float deskew_angle{};
		compute_text_orientation(document_path, text_orientation, deskew_angle);

		if(text_orientation == tesseract::Orientation::ORIENTATION_PAGE_RIGHT)
		{
			cv::rotate(imgGray, imgGray, cv::ROTATE_90_COUNTERCLOCKWISE);
		}
		else if(text_orientation == tesseract::Orientation::ORIENTATION_PAGE_LEFT)
		{
			cv::rotate(imgGray, imgGray, cv::ROTATE_90_CLOCKWISE);
		}
		else if(text_orientation == tesseract::Orientation::ORIENTATION_PAGE_DOWN)
		{
			cv::rotate(imgGray, imgGray, cv::ROTATE_180);
		}
		// cv::imshow("flipped ROTATE_90_CLOCKWISE", flipped);
		// cv::waitKey(0)

		// 2. try raw Tesseract allowing only digit
		tesseract::TessBaseAPI api;
		api.Init(NULL, "fra"); // initialize tesseract with French language 

		// Retrieve text
		api.SetPageSegMode(tesseract::PSM_SPARSE_TEXT);
		api.SetVariable("tessedit_char_whitelist", "01234567890");
		//api.SetVariable("tessedit_char_whitelist", "01234567890"/*".,01234567890"*/);
		//api.SetVariable("tessedit_char_blacklist", "!?@#$%&*()<>_-+=/:;'\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		//api.SetVariable("classify_bln_numeric_mode", "1");

		// add image to Tesseract engine	
		api.SetImage(imgGray.data, imgGray.cols, imgGray.rows, 1, imgGray.step);
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
				if(std::regex_match(word, card_number_pattern))
				{
					status = status::ocr_success;
					ocr_data = word;
					break;
				}
				//printf("word: '%s';  \tconf: %.2f;\n", word, conf);
				//int x1, y1, x2, y2;
				//ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				//cv::rectangle(imgGray, cv::Point2i(x1, y1), cv::Point2i(x2, y2), cv::Scalar(0, 255, 0), 3);
				//cv::imshow("ocr", imgGray);
				//cv::waitKey(0);
				//printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n", word, conf, x1, y1, x2, y2);
				delete[] word;
			} while (ri->Next(level));
		}
		// std::cout << api2->GetTSVText(0) << std::endl;

		// Release memory
		api.End();
	}


} // ocr
