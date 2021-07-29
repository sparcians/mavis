#pragma once

#include "DecoderTypes.h"
#include "Field.h"
#include <exception>
#include <string>
#include <sstream>

namespace mavis {

/**
 * Decoder exceptions base class
 */
class BaseException : public std::exception
{
public:
    virtual const char *what() const noexcept override
    {
        return why_.c_str();
    }

protected:
    std::string why_;
};

/**
 * Exception thrown when JSON ISA file will not open
 */
class BadISAFile : public BaseException
{
public:
    explicit BadISAFile(const std::string &fname) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Cannot open JSON ISA file '" << fname << "'";
        why_ = ss.str();
    }
};

/**
 * Exception thrown when JSON uArch file will not open
 */
class BadAnnotationFile : public BaseException
{
public:
    explicit BadAnnotationFile(const std::string &fname) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Cannot open JSON uArch file '" << fname << "'";
        why_ = ss.str();
    }
};

/**
 * DTable build error: the JSON ISA file is missing a mnemonic for the instruction
 */
class BuildErrorMissingMnemonic : public BaseException
{
public:
    explicit BuildErrorMissingMnemonic(const std::string &filename) :
        BaseException()
    {
        std::stringstream ss;
        ss << "The JSON ISA file '" << filename << "'"
           << " is missing a mnemonic in one of the clauses";
        why_ = ss.str();
    }

    BuildErrorMissingMnemonic(const std::string &filename, const std::string &stencil_str) :
        BaseException()
    {
        std::stringstream ss;
        ss << "The entry with stencil '" << stencil_str << "'"
                                                           " is missing a mnemonic in JSON ISA file '" << filename
           << "'";
        why_ = ss.str();
    }
};

/**
 * DTable build error: the mnemonic's form is unknown
 */
class BuildErrorUnknownForm : public BaseException
{
public:
    BuildErrorUnknownForm(const std::string &filename, const std::string &mnemonic, const std::string &form_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Form '" << form_name << "'"
           << " for mnemonic '" << mnemonic << "'"
           << " in JSON ISA file '" << filename << "'"
           << " is unknown";
        why_ = ss.str();
    }

    explicit BuildErrorUnknownForm(const std::string& form_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Attempt to reference unknown form '" << form_name << "'";
        why_ = ss.str();
    }
};

/**
 * DTable build error: Occurs when building the TRIE branches (non-leaves)
 * and we're trying to add an instruction whose field is incompatible with
 * the field for the given branch node
 *
 * This can usually be resolved by introducing a new decode-only form for use by all
 * the affected instructions. For an example, see the compressed instruction forms C0 - C2
 */
class BuildErrorFieldsIncompatible : public BaseException
{
public:
    BuildErrorFieldsIncompatible(const std::string &mnemonic, const Field &existing_field,
                                 const Field &building_field) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "': "
           << "field '" << building_field.getName() << "' is incompatible with "
           << "field '" << existing_field.getName() << "' from a prior instruction";
        why_ = ss.str();
    }
};

/**
 * DTable build error: Occurs when we're trying to add an instruction to the TRIE, but there's
 * already a leaf node present for the same opcode point.
 *
 * This can usually be resolved by either making one of the conflicting instructions more specific
 * (e.g. adding missing fixed field definitions in the JSON), or introducing new decode-only forms
 */
class BuildErrorInstructionAlias : public BaseException
{
public:
    BuildErrorInstructionAlias(uint64_t istencil, const std::string &mnemonic, const std::string &node_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "ALREADY A LEAF for stencil: 0x" << std::hex << istencil
           << ", '" << mnemonic << "'"
           << " will alias to " << node_name;
        why_ = ss.str();
    }
};

/**
 * DTable build error: Occurs when we're trying to add an instruction to the TRIE, but the
 * instruction's stencil is the same as a prior instruction's stencil in the JSON and the
 * forms of the two are incompatible.
 *
 * This can usually be resolved by either making one of the conflicting instructions more specific
 * (e.g. adding missing fixed field definitions in the JSON), or introducing new decode-only forms
 */
class BuildErrorOpcodeConflict : public BaseException
{
public:
    BuildErrorOpcodeConflict(const std::string &mnemonic, uint64_t istencil) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "': "
           << "opcode 0x" << std::hex << istencil << " is the same as another instruction, with incompatible form";
        why_ = ss.str();
    }
};

/**
 * IFactory builder error: expansion named in JSON for an instruction has not yet been processed.
 *
 * This can be resolved by moving the instruction's JSON stanza below the stanza first referring
 * to the expansion factory (or instruction)
 */
class BuildErrorUnknownExpansion : public BaseException
{
public:
    BuildErrorUnknownExpansion(const std::string &mnemonic, const std::string &factory_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "': "
           << " Expansion factory '" << factory_name << "' "
           << "not yet declared. Move this instruction after the named factory in the JSON file";
        why_ = ss.str();
    }
};

/**
 * InstMetaData build error: the given type for the instruction was not found by the InstMetaData
 * object.
 *
 * This can happen from a mis-spelling of the type in the JSON, or from a need to update the
 * InstMetaData's list of supported types
 */
class BuildErrorUnknownType : public BaseException
{
public:
    BuildErrorUnknownType(const std::string &mnemonic, const std::string &type_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "': "
           << " type '" << type_name << "' "
           << "is not known to the decoder (InstMetaData object)";
        why_ = ss.str();
    }
};

/**
 * InstMetaData build error: the given data size for the instruction was invalid
 *
 * This can happen from a mis-spelling of the type in the JSON, or from a need to update the
 * InstMetaData's list of supported types
 */
class BuildErrorInvalidDataSize : public BaseException
{
public:
    BuildErrorInvalidDataSize(const std::string &mnemonic, int32_t data) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "': "
           << " data size '" << data << "' "
           << "must be a positive power-of-2 or zero (InstMetaData object)";
        why_ = ss.str();
    }
};

/**
 * Build error: the given field is not part of the given form
 */
class BuildErrorUnknownFormField : public BaseException
{
public:
    BuildErrorUnknownFormField(const std::string &form_name, const std::string &field_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Field '" << field_name << "': "
           << " in form '" << form_name << "' "
           << "is not known to the decoder (Form<> object)";
        why_ = ss.str();
    }
};

/**
 * \brief annotation build error: the annotation corresponding to this instruction has already
 * been registered with the instruction's factory (named by the factory clause in the ISA JSON
 * file).
 *
 * This could happen if the annotation JSON file has duplicate entries.
 */
class AnnotationNotUniqueInFile : public BaseException
{
public:
    AnnotationNotUniqueInFile(const std::string &mnemonic, const std::string &file_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "': "
           << " has duplicate annotation in file '" << file_name << "'."
           << " Check the annotation JSON files for correctness";
        why_ = ss.str();
    }
};

/**
 * \brief build error: the mnemonic corresponding to this instruction has already
 * been registered with one of the decoder's internal registries
 *
 * This could happen if the ISA JSON file has duplicate entries.
 */
class BuildErrorDuplicateMnemonic : public BaseException
{
public:
    explicit BuildErrorDuplicateMnemonic(const std::string &mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "': "
           << " has duplicate registry entry."
           << " Check the ISA and annotation JSON files for correctness";
        why_ = ss.str();
    }
};

/**
 * \brief annotation build error: the annotation corresponding to this instruction has already
 * been registered with the instruction's factory (named by the factory clause in the ISA JSON
 * file).
 *
 * This could happen if the annotation JSON file has duplicate entries.
 */
class BuildErrorDuplicateAnnotation : public BaseException
{
public:
    BuildErrorDuplicateAnnotation(const std::string &mnemonic, const std::string &factory_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "': "
           << " has duplicate annotation in factory '" << factory_name << "'."
           << " Check the ISA and annotation JSON files for correctness";
        why_ = ss.str();
    }
};

/**
 * \brief annotation build error: This may not really be an error, if the user has not provided
 * a matching annotation for this instruction. We raise the exception, and allow the user to
 * decide whether to ignore it
 */
class BuildErrorMissingAnnotation : public BaseException
{
public:
    BuildErrorMissingAnnotation(const std::string &mnemonic, const std::string &factory_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "'"
           << " has no matching annotation for factory named '" << factory_name << "'."
           << " Check the ISA and annotation JSON files for correctness";
        why_ = ss.str();
    }

    BuildErrorMissingAnnotation(const std::string &mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction or factory named '" << mnemonic << "'"
           << " has no matching annotation. Check the ISA and annotation JSON files for correctness";
        why_ = ss.str();
    }
};

/**
 * \brief Overlay instruction build error: The base instruction named as part of the
 * overlay specification must already be defined in the ISA JSON file.
 *
 * It could be that the base instruction is defined after this overlay definition
 * in the JSON file. If so, it needs to be moved up before the first reference
 */
class BuildErrorOverlayBaseNotFound : public BaseException
{
public:
    BuildErrorOverlayBaseNotFound(const std::string &olay_mnemonic, const std::string &base_mnemonic,
                                  const std::string &file_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Overlay instruction '" << olay_mnemonic << "' "
           << " in JSON ISA file '" << file_name << "':"
           << " The overlay's base instruction '" << base_mnemonic << "'"
           << " is not found. Check the ISA and annotation JSON files for correctness";
        why_ = ss.str();
    }
};

/**
 * \brief Overlay annotation build error: This may not really be an error, if the user has not provided
 * a matching annotation for this instruction. We raise the exception, and allow the user to
 * decide whether to ignore it
 */
class BuildErrorOverlayMissingAnnotation : public BaseException
{
public:
    BuildErrorOverlayMissingAnnotation(const std::string &olay_mnemonic, const std::string &base_mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Overlay instruction '" << olay_mnemonic << "': "
           << " Cannot find an annotation corresponding to either this mnemonic,"
           << " or its base instruction '" << base_mnemonic << "'."
           << " Check the ISA and annotation JSON files for correctness";
        why_ = ss.str();
    }
};

/**
 * \brief Overlay build error: The overlay specification is missing a "base" specification.
 *
 * The ISA JSON file needs to be corrected
 */
class BuildErrorOverlayMissingBase : public BaseException
{
public:
    explicit BuildErrorOverlayMissingBase(const std::string &olay_mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Overlay instruction '" << olay_mnemonic << "': "
           << " is missing a base instruction specification."
           << " Check the ISA and annotation JSON files for correctness";
        why_ = ss.str();
    }
};

/**
 * \brief Overlay build error: The overlay specification is missing a "match" specification.
 *
 * The ISA JSON file needs to be corrected
 */
class BuildErrorOverlayMissingMatch : public BaseException
{
public:
    explicit BuildErrorOverlayMissingMatch(const std::string &olay_mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Overlay instruction '" << olay_mnemonic << "': "
           << " is missing a match specification."
           << " Check the ISA and annotation JSON files for correctness";
        why_ = ss.str();
    }
};

/**
 * @brief Malformed overlay match specification in the JSON ISA file
 */
class BuildErrorOverlayBadMatchSpec : BaseException
{
public:
    explicit BuildErrorOverlayBadMatchSpec(const std::string &olay_mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Overlay instruction '" << olay_mnemonic << "': "
           << " Match specification must have the form [mask_hex_string, val_hex_string].";
        why_ = ss.str();
    }
};

/**
 * Invalid EtractorIF::SpecialField id requested from extractor
 */
class InvalidExtractorSpecialFieldID : public BaseException
{
public:
    InvalidExtractorSpecialFieldID(const std::string &fname, Opcode icode) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Invalid extractor special field id '" << fname << "'"
           << " requested from opcode 0x" << std::hex << icode;
        why_ = ss.str();
    }

    InvalidExtractorSpecialFieldID(const std::string &mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Special fields are not valid for instruction '" << mnemonic << "'."
           << " This can happen if you're trying to get a special field from a direct extraction object";
        why_ = ss.str();
    }
};

/**
 * Unsupported EtractorIF::SpecialField id requested from extractor
 */
class UnsupportedExtractorSpecialFieldID : public BaseException
{
public:
    UnsupportedExtractorSpecialFieldID(const std::string &fname, Opcode icode) :
        BaseException(), sfid_name_(fname), opcode_(icode)
    {
        std::stringstream ss;
        ss << "Extractor special field id '" << fname << "'"
           << " is not supported by opcode 0x" << std::hex << icode;
        why_ = ss.str();
    }

    UnsupportedExtractorSpecialFieldID(const std::string &mnemonic, const UnsupportedExtractorSpecialFieldID& prior) :
        BaseException(), sfid_name_(prior.sfid_name_), opcode_(prior.opcode_)
    {
        std::stringstream ss;
        ss << "Extractor special field id '" << sfid_name_ << "'"
           << " for instruction '" << mnemonic << "'"
           << " (opcode 0x" << std::hex << opcode_ << ")"
           << " is not supported";
        why_ = ss.str();
    }

private:
    const std::string   sfid_name_;
    const Opcode        opcode_;
};

/**
 * Invalid register number for ExtractorDirectInfo
 */
class InvalidRegisterNumber : public BaseException
{
public:
    InvalidRegisterNumber(const std::string &mnemonic, uint32_t regnum) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Invalid register number '" << regnum << "'"
           << " for ExtractorDirectInfo instruction '" << mnemonic << "'";
        why_ = ss.str();
    }
};

/**
 * Builder UID Registry: Initialization list UID is invalid
 */
class BuilderInstIDListInvalidUID : public BaseException
{
public:
    BuilderInstIDListInvalidUID(const std::string &mnemonic, InstructionUniqueID uid) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Invalid instruction ID '" << uid << "'"
           << " for instruction '" << mnemonic << "'"
           << " in InstIDList provided to builder";
        why_ = ss.str();
    }
};

/**
 * Builder UID Registry: Initialization list UID is not unique
 */
class BuilderInstIDListUIDNotUnique : public BaseException
{
public:
    BuilderInstIDListUIDNotUnique(const std::string &mnemonic, InstructionUniqueID uid) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction ID '" << uid << "'"
           << " for instruction '" << mnemonic << "'"
           << " is already used in InstIDList provided to builder";
        why_ = ss.str();
    }
};

/**
 * Builder UID Registry: Initialization list instruction already registered
 */
class BuilderInstIDListAlreadyRegistered : public BaseException
{
public:
    BuilderInstIDListAlreadyRegistered(const std::string &mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Instruction '" << mnemonic << "'"
           << " in InstIDList provided to builder has already been registered";
        why_ = ss.str();
    }
};

/**
 * Invalid mnemonic for ExtractorDirectInfo (the user-supplied mnemonic does not match any
 * factory known to the IFactoryBuilder)
 */
class UnknownMnemonic : public BaseException
{
public:
    explicit UnknownMnemonic(const std::string &mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Unknown mnemonic '" << mnemonic << "'"
           << " in ExtractorDirectInfo";
        why_ = ss.str();
    }
};

/**
 * Invalid pseudo instruction mnemonic for ExtractorDirectInfo (the user-supplied mnemonic does not match any
 * factory known to the PseudoBuilder)
 */
class UnknownPseudoMnemonic : public BaseException
{
public:
    explicit UnknownPseudoMnemonic(const std::string &mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Unknown pseudo instruction mnemonic '" << mnemonic << "'"
           << " in ExtractorDirectInfo";
        why_ = ss.str();
    }
};

/**
 * Invalid pseudo instruction UID for PseudoBuilder (the user-supplied UID does not match any
 * factory known to the PseudoBuilder)
 */
class UnknownPseudoUID : public BaseException
{
public:
    explicit UnknownPseudoUID(const InstructionUniqueID uid) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Unknown pseudo instruction UID '" << uid << "'"
           << " in PseudoBuilder";
        why_ = ss.str();
    }
};

/**
 * @brief DTableLookupError (from IFactorySpecialCaseComposite lookup failure)
 *
 * We can get into this situation for complex fixed/ignore combinations in the instruction's JSON ISA spec.
 * I've seen it for mv (a special case of addi with an immediate of 0). The problem arose because the form
 * used for decode (not the extraction xform) had two fields to cover the immediate -- and if either of
 * these fields are non-zero, the instruction needed to decode to addi. Only when BOTH fields are 0
 * do we decode as a mv. The "either-or" concept wasn't representable in Mavis.
 *
 * Short story, if you get here, you'll need to think about handling the instruction using an overlay
 */
class DTableLookupError : public BaseException
{
public:
    explicit DTableLookupError(const Opcode opcode, const std::string& closest_mnemonic) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Cannot decode opcode 0x" << std::hex << opcode
           << " due to DTable lookup error (malformed TRIE). Default/closest mnemonic is '" << closest_mnemonic << "'";
        why_ = ss.str();
    }
};

/**
 * Exception thrown when decoder cannot decode the opcode
 */
class UnknownOpcode : public BaseException
{
public:
    explicit UnknownOpcode(uint64_t opcode) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Cannot decode opcode 0x" << std::hex << opcode;
        why_ = ss.str();
    }
};

/**
 * Exception thrown when decoder trying to decode an "illegal" op
 * (This is mainly for compressed instructions which have reserved immediate or register field values)
 */
class IllegalOpcode : public BaseException
{
public:
    explicit IllegalOpcode(const std::string& mnemonic, uint64_t opcode) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Opcode 0x" << std::hex << opcode
           << " is illegal for instruction '" << mnemonic << "'";
        why_ = ss.str();
    }
};

/**
 * Exception thrown when user attempts to register an already existing mavis context
 */
class ContextAlreadyExists : public BaseException
{
public:
    explicit ContextAlreadyExists(const std::string& name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Mavis context '" << name << "'"
           << " already exists";
        why_ = ss.str();
    }
};

/**
 * Exception thrown when user attempts to register an already existing mavis context
 */
class UnknownContext : public BaseException
{
public:
    explicit UnknownContext(const std::string& name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "Mavis context '" << name << "'"
           << " is not known";
        why_ = ss.str();
    }
};

/**
 * Exception thrown when user attempts to access an invalid fieldID from an OperandInfo object
 */
class OperandInfoInvalidFieldID : public BaseException
{
public:
    explicit OperandInfoInvalidFieldID(const std::string& fid_name) :
        BaseException()
    {
        std::stringstream ss;
        ss << "OperandInfo field ID '" << fid_name << "'"
           << " is invalid";
        why_ = ss.str();
    }
};

} // namespace mavis

