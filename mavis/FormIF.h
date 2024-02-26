#pragma once

#include <vector>
#include "Field.h"

namespace mavis {

// Type of immediate declared as part of the encoding
enum class ImmediateType : uint32_t
{
    NONE,           // No immediate present in encoding
    UNSIGNED,       // UNSIGNED immediate (UIMM) in encoding
    SIGNED          // SIGNED immediate (SIMM) in encoding
};

typedef const std::vector<Field> FieldsType;

class FormBase {};

/**
 * Form Base to provide implementation of common methods
 */
template<class FormType>
class Form : public FormBase
{
public:
    using idType = typename FormType::idType;

    static std::string getName()
    {
        return FormType::name_;
    }

    static const Field &getField(const uint32_t fid)
    {
        return FormType::fields_[fid];
    }

    static const Field &getField(const std::string &fname)
    {
        const auto itr = FormType::fmap_.find(fname);
        if (itr == FormType::fmap_.end()) {
            throw BuildErrorUnknownFormField(FormType::name_, fname);
        }
        return itr->second;
    }

    static uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = FormType::imap_.find(fname);
        if (itr == FormType::imap_.end()) {
            throw BuildErrorUnknownFormField(FormType::name_, fname);
        }
        return itr->second;
    }

    static const FieldsType& getFields()
    {
        return FormType::fields_;
    }

    static const FieldsType& getOpcodeFields()
    {
        return FormType::opcode_fields_;
    }

    static ImmediateType getImmediateType()
    {
        return FormType::getImmediateType();
    }

    static void print(std::ostream &os)
    {
        FormType::print(os);
    }

    // TODO: Deprecate all uses of fixed_field_set! It's DANGEROUS
    // The problem with fixed_field_set is that it is generated from the DECODE form, using the
    // enum definition in that form's class. When we're extracting, we may use an "xform" which likely
    // has an incompatible field enum (where field id enum values are different between the form and xform)
    uint64_t getFieldIDSet(const std::vector<std::string>& flist) const
    {
        uint64_t fset = 0;
        for (const auto& f : flist) {
            fset |= (1u << getFieldIndex(f));
        }
        return fset;
    }
};

} // namespace mavis
