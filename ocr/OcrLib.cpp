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

	void compute_text_orientation(const std::string &image_path, tesseract::Orientation &text_orientation, float &deskew_angle)
	{
		cv::Mat imgGray, imgColor;
		imgColor = cv::imread(image_path, cv::IMREAD_COLOR);
		compute_text_orientation(imgColor, text_orientation, deskew_angle);
	}

	void compute_text_orientation(const cv::Mat &image, tesseract::Orientation &text_orientation, float &deskew_angle)
	{
		cv::Mat imgGray;
		// if RGB image then convert to grayscale
		if (image.channels() == 3)
		{
			cv::cvtColor(image, imgGray, cv::COLOR_BGR2GRAY);
		}
		else
		{
			imgGray = image.clone();
		}

		tesseract::TessBaseAPI api;
		api.Init(NULL, "fra");
		// api.SetVariable("debug_file", "tesseract.log");

		// Get OSD - old method
		tesseract::WritingDirection direction;
		tesseract::TextlineOrder order;

		// float deskew_angle;
		api.SetPageSegMode(tesseract::PSM_AUTO_OSD);
		api.SetImage(imgGray.data, imgGray.cols, imgGray.rows, 1, imgGray.step);
		api.Recognize(0);

		tesseract::PageIterator *it = api.AnalyseLayout();
		it->Orientation(&text_orientation, &direction, &order, &deskew_angle);
		printf("Old Method\nOrientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\nDeskew angle: %.4f\n", text_orientation, direction, order, deskew_angle);

		// Get OSD - new method
		// int orient_deg;
		// float orient_conf;
		// const char *script_name;
		// float script_conf;
		// api.DetectOrientationScript(&orient_deg, &orient_conf, &script_name, &script_conf);
		// printf("New method\n Orientation in degrees: %d\n Orientation confidence: %.2f\nScript: %s\n Script confidence: %.2f\n",orient_deg, orient_conf,script_name, script_conf);

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
		// cv::imshow("source", image);
		// cv::imshow("gray", image_gray);

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

	void rectify_orientation(cv::Mat &image, tesseract::Orientation &text_orientation)
	{
		if (text_orientation == tesseract::Orientation::ORIENTATION_PAGE_RIGHT)
		{
			cv::rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
		}
		else if (text_orientation == tesseract::Orientation::ORIENTATION_PAGE_LEFT)
		{
			cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
		}
		else if (text_orientation == tesseract::Orientation::ORIENTATION_PAGE_DOWN)
		{
			cv::rotate(image, image, cv::ROTATE_180);
		}
	}

	void ocr_insurance_card(const std::string &document_path, ocr::status &status, std::string &ocr_data, bool display_internals)
	{
		// 1. load the image
		// 2. determine text orientation and rotate image accordingly
		// 3. deskew image if required
		// 4. try raw Tesseract allowing only digit
		// 5. if failing:
		//    a. estimate orientation and reorientate
		//    b. try to segment to retrieve card area in image and crop
		//    c. apply Tesseract in cropped reoriented image

		// regex pattern for insurance card number
		//std::regex card_number_pattern{"^807\\d{17}$"};
		std::regex card_number_pattern{"^807\\d{17}.*"};

		status = status::ocr_fail;
		ocr_data.clear();

		// 1. load the image in grayscale
		cv::Mat image, image_colored;
		image = cv::imread(document_path, cv::IMREAD_GRAYSCALE);
		image_colored = cv::imread(document_path, cv::IMREAD_COLOR);
		if (image.empty())
		{
			status = status::non_valid_image;
			return;
		}

		// 2. resize keeping ratio if to small resolution
		cv::Mat image_zoom, image_colored_zoom;
		if(image.size().height < 1000)
		{
			int width = image.size().width;
			int height = image.size().height;
			cv::Size size{int(1000*width/height),1000};
			cv::resize(image, image, size, 0, 0, cv::INTER_CUBIC);
			cv::resize(image_colored, image_colored, size, 0, 0, cv::INTER_CUBIC);
		}

		// 2. Binarisation
		//cv::imshow("before binarisation", image.clone());
		//cv::adaptiveThreshold(image,image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 13, 0);
		//cv::imshow("binarisation", image.clone());
		//cv::waitKey(0);

		// 3. Remove noise
		//cv::fastNlMeansDenoising(image, image, 15.0, 7, 21);
		//cv::imshow("denoise", image.clone());
		//cv::waitKey(0);

		// 4. Dilatation and Erosion
		//int morph_size = 1;
		//cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * morph_size + 1, 2 * morph_size + 1), cv::Point(morph_size, morph_size));
		//cv::morphologyEx(image, image, cv::MORPH_CLOSE, element, cv::Point(-1, -1), 2);
		//cv::imshow("morpho", image.clone());
		//cv::waitKey(0);

		// 2. determine text orientation and rotate image accordingly
		//tesseract::Orientation text_orientation{};
		//float deskew_angle{};
		//compute_text_orientation(image, text_orientation, deskew_angle);
		//rectify_orientation(image, text_orientation);
		
		// 2. deskew image if skew more that 5 degree
		//compute_text_orientation(imgGray, text_orientation, deskew_angle);
		//deskew_image(imgGray, deskew_angle);

		// 3. try raw Tesseract allowing only digit
		tesseract::TessBaseAPI api;
		api.Init(NULL, "fra"); // initialize tesseract with French language

		// Retrieve text
		api.SetPageSegMode(tesseract::PSM_SPARSE_TEXT);
		api.SetVariable("tessedit_char_whitelist", "01234567890");
		// api.SetVariable("classify_bln_numeric_mode", "1");
		// api.SetVariable("tessedit_char_blacklist", ".,!?@#$%&*()<>_-+=/:;'\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		// api.SetVariable("tessedit_char_whitelist", "01234567890"/*".,01234567890"*/);

		// add image to Tesseract engine
		api.SetImage(image.data, image.cols, image.rows, 1, image.step);
		api.Recognize(0);

		tesseract::ResultIterator *ri = api.GetIterator();
		// tesseract::PageIteratorLevel level = tesseract::RIL_TEXTLINE;
		tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
		if (ri != 0)
		{
			do
			{
				const char *word = ri->GetUTF8Text(level);
				float conf = ri->Confidence(level);
				if (std::regex_match(word, card_number_pattern))
				{
					status = status::ocr_success;
					ocr_data = word;
					ocr_data = ocr_data.substr(0,20);
					cv::destroyAllWindows();
					break;
				}

				if (display_internals)
				{
					printf("word: '%s';  \tconf: %.2f;\n", word, conf);
					int x1, y1, x2, y2;
					ri->BoundingBox(level, &x1, &y1, &x2, &y2);
					cv::rectangle(image_colored, cv::Point2i(x1, y1), cv::Point2i(x2, y2), cv::Scalar(0, 255, 0), 3);
					//cv::imshow("ocr", imgGray);
					//cv::waitKey(0);
					// printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n", word, conf, x1, y1, x2, y2);
				}

				delete[] word;
			} while (ri->Next(level));
		}
		cv::imshow("ocr", image_colored);
		cv::waitKey(0);
		// std::cout << api2->GetTSVText(0) << std::endl;

		// Release memory
		api.End();
	}

	void deskew_image(cv::Mat & image, double angle)
	{
		// convert angle in degree since Tesseract provide angle in radian but opencv use degree
		angle = angle*180/std::numbers::pi_v<float>;

		std::vector<cv::Point> points;
		for (cv::Mat_<uchar>::iterator it = image.begin<uchar>(); it != image.end<uchar>(); ++it)
		{
			if (*it) {
				points.push_back(it.pos());
			}	
		}

		cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));
		cv::Mat rot_mat = getRotationMatrix2D(box.center, angle, 1);
		cv::Mat rotated;
		cv::warpAffine(image, rotated, rot_mat, image.size(), cv::INTER_CUBIC);
		image = rotated.clone();
	}

} // ocr
