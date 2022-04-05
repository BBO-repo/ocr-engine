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
    std::map<std::string, document_type> type_map{
        {"card", document_type::card},
        {"pdf_diana", document_type::pdf_diana},
        {"pdf_uni", document_type::pdf_uni},
        {"unknown", document_type::unknown}};

    // error status
    enum class status
    {
        ocr_success,
        no_error,
        unknown_doc_type,
        file_not_found
    };

}

#endif // OCRTYPE_H_
