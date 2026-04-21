#pragma once

#include "Form.h"
#include <string>

namespace mavis
{

    /**
     * FormRegistry
     */
    class FormRegistry
    {
      public:
        static const FormBase::PtrType & findFormWrapper(const std::string & fname);
    };

} // namespace mavis
