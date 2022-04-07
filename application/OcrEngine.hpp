#ifndef OCRENGINE_H_
#define OCRENGINE_H_

#include "OcrTypes.hpp"

namespace ocr
{
    class Ocrizer
    {
    public:
        virtual void ocrize(const std::string &document_path, ocr::status &status, std::string &ocr_data) = 0;
    };

    // Implementation for insurance card ocr 
    class InsuranceCardOcrizer : public Ocrizer
    {
    public:
        virtual void ocrize(const std::string &document_path, ocr::status &status, std::string &ocr_data)
        {
            ocr_insurance_card(document_path, status, ocr_data);
        }
    };

    // Context
    class OcrEngine
    {
    private:
        Ocrizer *m_ocrizer;

    public:
        OcrEngine() {}
        void set_ocrizer(Ocrizer *ocrizer)
        {
            m_ocrizer = ocrizer;
        }
 
        void ocrize(const std::string &document_path, ocr::status &status, std::string &message) 
        {
            m_ocrizer->ocrize(document_path, status, message);
        }
    };

}

#endif // OCRENGINE_H_