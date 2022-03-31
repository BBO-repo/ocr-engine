#include "catch.hpp"

#include "OcrEngine.hpp"
#include "Helper.hpp"

// Test can open image
TEST_CASE("test-can-open-image", "[opencv]")
{
    cv::Mat img;
    ocr::open_image("/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff", img);
    REQUIRE(!img.empty());
    // ocr::open_image("/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff", img, true);
}