#ifndef OCRTYPE_H_
#define OCRTYPE_H_

#include <string>
#include <map>

namespace ocr
{
	// possible document type
    enum class document_type
    {
        card,
        pdf_diana,
        pdf_uni,
        unknown
    };

    // map the document type to argument string input
    static const std::map <std::string, document_type> type_map {
        {"card", document_type::card},
        {"pdf_diana", document_type::pdf_diana},
        {"pdf_uni", document_type::pdf_uni},
        {"unknown", document_type::unknown}};

    // error status
    enum class status
    {
        ocr_success,
        ocr_fail,
        no_error,
        unknown_doc_type,
        file_not_found,
        non_valid_image,
        unknown
    };

    static const std::map <status, std::string> status_map {
        {status::ocr_success,"ocr_success"},
        {status::ocr_fail,"ocr_fail"},
        {status::no_error,"no_error"},
        {status::unknown_doc_type,"unknown_doc_type"},
        {status::file_not_found,"file_not_found"},
        {status::unknown,"unknown"}
    };

}

#endif // OCRTYPE_H_
