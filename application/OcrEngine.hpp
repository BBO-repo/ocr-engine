#ifndef OCRENGINE_H_
#define OCRENGINE_H_

#include <map>
#include <string>
#include <iostream>

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

    // https://stackoverflow.com/questions/11459294/is-there-a-way-of-implementing-the-strategy-pattern-using-variadic-templates
    // http://www.lucadavidian.com/2019/05/24/strategy-desing-pattern-and-variadic-class-templates/
    struct StratA
    {
        enum gender
        {
            male,
            female,
            unknown
        };
        double price(std::string name, int age, gender g) const
        {
            return 42;
        }
    };

    struct StratB
    {
        double price(int age, int volume, double historic_rate) const
        {
            return (age * age * historic_rate) / volume;
        }
    };

    // strategy pattern implemented with C++ template
    template <typename OcrStrategy>
    struct OcrEngine
    {
        template <typename... Args>
        void ocrize(std::string logmessage, Args... args) // involving pricing
        {
            std::cout << logmessage << ": " << OcrStrategy().price(std::forward<Args>(args)...) << '\n';
        }
    };

}

#endif // OCRENGINE_H_