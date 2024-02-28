#pragma once

#include "Form.h"
#include <string>

namespace mavis {

/**
 * FormRegistry
 */
class FormRegistry
{
public:
    static const FormBase* findFormWrapper(const std::string& fname);
    static const FormBase* getFormWrapper(const std::string& fname);

};

} // namespace mavis
