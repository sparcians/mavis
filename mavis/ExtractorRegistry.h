#pragma once

#include <string>

#include "Extractor.h"

namespace mavis
{
    /**
     * ExtractorRegistry
     */
    class ExtractorRegistry
    {
      public:
        static const ExtractorIF::PtrType & getExtractor(const std::string & fname);
    };

} // namespace mavis
