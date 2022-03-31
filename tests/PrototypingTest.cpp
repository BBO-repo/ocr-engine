#include "catch.hpp"

#include "OcrEngine.hpp"
#include "Helper.hpp"


cv::Mat src_gray;
int thresh = 0;
cv::RNG rng(12345);

void thresh_callback_contour(int, void *)
{
    auto compareContourAreas = [](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
    {
        //double i = fabs(contourArea(cv::Mat(contour1)));
        //double j = fabs(contourArea(cv::Mat(contour2)));
        int i = (boundingRect(contour1)).area();
        int j = (boundingRect(contour2)).area();
        return (i > j);
    };

    using namespace cv;
    using namespace std;

    Mat canny_output;
    Canny(src_gray, canny_output, thresh, thresh * 2);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(canny_output, contours, hierarchy, RETR_EXTERNAL/*RETR_TREE*/, CHAIN_APPROX_SIMPLE);
    std::sort(contours.begin(), contours.end(), compareContourAreas);
    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
    for (size_t i = 0; i < 1/*contours.size()*/; ++i)
    {
        Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
        drawContours(drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
    }
    imshow("Contours", drawing);
}

// Test can compute orientation
TEST_CASE("test-can-find-contours", "[opencv]")
{

    using namespace cv;
    using namespace std;

    std::string image_path{"/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff"};
    //std::string image_path{"/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-2.tiff"};
    cv::Mat src = cv::imread(image_path);

    cv::Mat img_bw;

    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    // cv::threshold(src_gray, img_bw, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    blur(src_gray, src_gray, Size(3, 3));
    const char *source_window = "Source";
    namedWindow(source_window);
    imshow(source_window, src);
    const int max_thresh = 255;
    createTrackbar("Canny thresh:", source_window, &thresh, max_thresh, thresh_callback_contour);
    thresh_callback_contour(0, 0);
    waitKey();
}

void thresh_callback_area(int, void* )
{
    using namespace cv;
    using namespace std;

    Mat canny_output;
    Canny( src_gray, canny_output, thresh, thresh*2 );
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_EXTERNAL/*RETR_TREE*/, CHAIN_APPROX_SIMPLE );
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( contours[i], contours_poly[i], 3, false/*true*/ );
        boundRect[i] = boundingRect( contours_poly[i] );
    }
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours_poly, (int)i, color );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2 );
    }
    imshow( "Contours", drawing );
}

// Test can compute orientation
TEST_CASE("test-can-find-largest-contours", "[.][opencv]")
{

    using namespace cv;
    using namespace std;

    std::string image_path{"/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff"};
    //std::string image_path{"/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-2.tiff"};
    cv::Mat src = cv::imread(image_path);

    cv::Mat img_bw;

    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    // cv::threshold(src_gray, img_bw, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    blur(src_gray, src_gray, Size(3, 3));
    const char *source_window = "Source";
    namedWindow(source_window);
    imshow(source_window, src);
    const int max_thresh = 255;
    createTrackbar("Canny thresh:", source_window, &thresh, max_thresh, thresh_callback_area);
    thresh_callback_area(0, 0);
    waitKey();
}

// Test can compute orientation
TEST_CASE("test-corners-perspective", "[.][opencv]")
{

    using namespace cv;
    using namespace std;

    auto compareContourAreas = [](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
    {
        //double i = fabs(contourArea(cv::Mat(contour1)));
        //double j = fabs(contourArea(cv::Mat(contour2)));
        int i = (boundingRect(contour1)).area();
        int j = (boundingRect(contour2)).area();
        return (i > j);
    };

    //std::string image_path{"/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff"};
    std::string image_path{"/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-2.tiff"};
    cv::Mat src = cv::imread(image_path);
    cv::Mat src_g;
    cvtColor(src, src_g, COLOR_BGR2GRAY);
    blur(src_g, src_g, Size(3, 3));
    imshow("source", src);

    Mat canny_output;
    Canny( src_g, canny_output, thresh, thresh*2 );
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_EXTERNAL/*RETR_TREE*/, CHAIN_APPROX_SIMPLE );
    std::sort(contours.begin(), contours.end(), compareContourAreas);

    size_t n_largest_contours = 1;//contours.size()
    vector<vector<Point> > contours_poly( n_largest_contours );
    vector<Rect> boundRect( n_largest_contours );
    for( size_t i = 0; i < n_largest_contours; i++ )
    {
        approxPolyDP( contours[i], contours_poly[i], 3, false/*true*/ );
        boundRect[i] = boundingRect( contours_poly[i] );
    }
    //Mat drawing = Mat::zeros(src_g.size(), CV_8UC3);
    Mat drawing = src.clone();
    for( size_t i = 0; i < n_largest_contours; i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours_poly, (int)i, color );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2 );
    }
    imshow( "Contours", drawing );

    // Crop image
    Mat imCrop;
    cvtColor(src, imCrop, COLOR_BGR2GRAY);
    imCrop = imCrop(boundRect[0]);
    imshow( "Cropped", imCrop );

    waitKey();


}