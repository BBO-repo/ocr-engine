#include "catch.hpp"

#include "OcrEngine.hpp"
#include "Helper.hpp"

// Test can compute orientation
TEST_CASE("test-can-compute-orientation", "[ocr-engine]")
{
    // ocr::compute_orientation("/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff");
    ocr::compute_orientation("/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-2.tiff");
}

// Can compute largest contour
TEST_CASE("test-can-largest-contour", "[ocr-engine]")
{
    // ocr::compute_orientation("/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff");
    std::string image_path{"/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-2.tiff"};
    cv::Rect get_largest_contour_rect(image_path);

}