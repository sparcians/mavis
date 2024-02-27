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

/*
class FormDefault
{
    enum idType : uint32_t
    {
        __N = 0
    };
    static inline const char * name {"?"};
    static inline const std::array<const Field, idType::__N> fields {};
    static inline const std::map<std::string, const Field &> fmap {};
    static inline const std::map<std::string, idType> imap {};
    static inline FieldsType opcode_fields {};
    static inline ImmediateType immediate_type = ImmediateType::NONE;
};
*/

class FormBase
{
public:
    virtual std::string getName() const = 0;
    virtual const Field &getField(const uint32_t fid) const = 0;
    virtual const Field &getField(const std::string &fname) const = 0;
    virtual uint32_t getFieldIndex(const std::string &fname) const = 0;
    virtual const FieldsType& getFields() const = 0;
    virtual const FieldsType& getOpcodeFields() const = 0;
    virtual ImmediateType getImmediateType() const = 0;
    virtual void print(std::ostream &os) const = 0;

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
 * Form Base to provide implementation of common methods
 */
template<class FormType>
class Form : public FormBase
{
public:
    using idType = typename FormType::idType;

    std::string getName() const override
    {
        return FormType::name;
    }

    const Field &getField(const uint32_t fid) const override
    {
        return FormType::fields[fid];
    }

    const Field &getField(const std::string &fname) const override
    {
        const auto itr = FormType::fmap.find(fname);
        if (itr == FormType::fmap.end()) {
            throw BuildErrorUnknownFormField(FormType::name, fname);
        }
        return itr->second;
    }

    uint32_t getFieldIndex(const std::string &fname) const override
    {
        const auto itr = FormType::imap.find(fname);
        if (itr == FormType::imap.end()) {
            throw BuildErrorUnknownFormField(FormType::name, fname);
        }
        return itr->second;
    }

    const FieldsType& getFields() const override
    {
        return FormType::fields;
    }

    const FieldsType& getOpcodeFields() const override
    {
        return FormType::opcode_fields;
    }

    ImmediateType getImmediateType() const override
    {
        return FormType::immediate_type;
    }

    void print(std::ostream &os) const override
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of " << FormType::name << ":" << std::endl;
        for (const auto &f : FormType::fields) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream& os, const FormBase& form)
{
    form.print(os);
    return os;
}

} // namespace mavis
