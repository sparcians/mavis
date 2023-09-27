#pragma once

#include "FormIF.h"
#include <string>

namespace mavis {

/**
 * FormRegistry
 */
class FormRegistry
{
public:
    static const FormWrapperIF* findFormWrapper(const std::string& fname);
    static const FormWrapperIF* getFormWrapper(const std::string& fname);

};

} // namespace mavis
