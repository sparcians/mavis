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

/**
 * FormWrapper interface
 */
class FormWrapperIF
{
public:
    typedef const std::vector<Field> OpcodeFieldsType;

    virtual std::string getName() const = 0;

    virtual const Field& getField(const std::string& fname) const = 0;

    virtual uint32_t getFieldIndex(const std::string& fname) const = 0;

    virtual OpcodeFieldsType& getOpcodeFields() const = 0;

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

/**
 * Form Wrapper to provide polymorphic interfaces
 */
template<typename FormType>
class FormWrapper : public FormWrapperIF
{
public:
    std::string getName() const override
    {
        return FormType::getName();
    }

    const Field& getField(const std::string& fname) const override
    {
        return FormType::getField(fname);
    }

    uint32_t getFieldIndex(const std::string& fname) const override
    {
        return FormType::getFieldIndex(fname);
    }

    OpcodeFieldsType& getOpcodeFields() const override
    {
        return FormType::getOpcodeFields();
    }
};

} // namespace mavis
