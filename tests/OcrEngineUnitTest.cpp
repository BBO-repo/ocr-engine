#include "catch.hpp"

#include "OcrLib.hpp"
#include "Helper.hpp"

// Test can compute orientation
TEST_CASE("test-compute-orientation", "[ocr-engine]")
{
    tesseract::Orientation text_orientation{};
    float deskew_angle{};
    // ocr::compute_text_orientation("/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff", text_orientation, deskew_angle);
    ocr::compute_text_orientation("/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-2.tiff", text_orientation, deskew_angle);
}

// Can compute largest contour
TEST_CASE("test-largest-contour", "[ocr-engine]")
{
    // ocr::compute_orientation("/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-1.tiff");
    std::string image_path{"/workspaces/ocr-engine/tests/data/insurance_card/PastedGraphic-2.tiff"};
    cv::Rect rect;
    rect = ocr::get_largest_contour_rect(image_path);
    REQUIRE((rect.width > 0 && rect.height > 0));
}

// Can perform ocr insurance card dataset
TEST_CASE("test-ocr-insurance-card", "[ocr-engine]")
{
    // first dataset
    // std::string data_directory{"/workspaces/ocr-engine/tests/data/2022-04-01-testing-data/insurance-card/"};

    // std::map<std::string, std::string> image_insurance_number{
    //     {"PastedGraphic-1.tiff", "80756000620054622611"},
    //     //{"PastedGraphic-2.tiff", "80756009940015401778"}
    // };

    // ocr::status status{};
    // std::string ocr_data{};
    // std::string document_path{};
    // for (auto const &[key, val] : image_insurance_number)
    // {
    //     document_path = data_directory + key;
    //     ocr::ocr_insurance_card(document_path, status, ocr_data, true);
    //     REQUIRE(status == ocr::status::ocr_success);
    //     REQUIRE(val == ocr_data);
    // }

    // second dataset
    std::string data_directory{"/workspaces/ocr-engine/tests/data/2022-04-06-testing-data/insurance-card/"};

    std::map<std::string, std::string> image_insurance_number{
        {"carete gm.png", "80756014790032134492"},
        //{"carte assure sanacur.png", "80756015620245213019"},
        //{"carte atupri.png", ""},
        //{"carte concordia.png", ""},
        //{"carte egk.png", ""},
        //{"carte kpt.png", ""},
        //{"carte okk.png", ""},
        //{"carte rhenusana.png", ""},
        //{"carte sanacur.png", ""},
        //{"carte sanitas.png", ""},
        //{"carte swica.png", ""},
        //{"carte sympany.png", ""},  
        //{"carte visana.png", ""},  
    };

    ocr::status status{};
    std::string ocr_data{};
    std::string document_path{};
    for (auto const &[key, val] : image_insurance_number)
    {
        document_path = data_directory + key;
        ocr::ocr_insurance_card(document_path, status, ocr_data, true);//val == "80756015620245213019");
        //REQUIRE(val == ocr_data);
    }
}