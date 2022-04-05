#ifndef OCRENGINE_H_
#define OCRENGINE_H_

#include "OcrTypes.hpp"

namespace ocr
{
    // strategy pattern implemented with C++ template (a non-type parameter version)
    template <document_type>
    struct Strategy;

    template <>
    struct Strategy<document_type::card>
    {
        double concreteOcrize(std::string, int b) const
        {
            return b;
        }
    };

    template <>
    struct Strategy<document_type::pdf_diana>
    {
        double concreteOcrize(int a, int b, double c) const
        {
            return (a * b * b) / c;
        }
    };

    template <document_type DT>
    struct OcrEngine
    {
        template <typename... Args>
        void ocrize(std::string message, Args... args)
        {
            Strategy<DT>().concreteOcrize(std::forward<Args>(args)...);
        }
    };

}

#endif // OCRENGINE_H_