#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <array>
#include <map>
#include <vector>
#include <memory>
#include "DecoderTypes.h"
#include "OpcodeInfo.h"
#include "Extractor.h"
#include "InstructionRegistry.hpp"
#include "Stash.hpp"
#include "Overlay.hpp"

namespace mavis {

/**
 * IFactoryIF<InstType, AnnotationType>: IFactory interface (Composite Pattern)
 */
template<typename InstType, typename AnnotationType>
class IFactoryIF
{
public:
    typedef typename std::shared_ptr<IFactoryIF> PtrType;

    struct IFactoryInfo
    {
        typedef std::shared_ptr<IFactoryInfo> PtrType;

        const OpcodeInfo::PtrType opinfo;
        const typename AnnotationType::PtrType uinfo;

        IFactoryInfo(const OpcodeInfo::PtrType& oi, const typename AnnotationType::PtrType& ui) :
            opinfo(oi), uinfo(ui)
        {}
    };

public:
    virtual ~IFactoryIF<InstType, AnnotationType>() = default;

    // virtual typename InstType::PtrType makeInst(Opcode icode) = 0;

    // virtual typename InstType::PtrType
    // makeInst(const std::string& mnemonic, Opcode icode, const ExtractorIF::PtrType& extractor) = 0;

    virtual typename IFactoryInfo::PtrType getInfo(Opcode icode) = 0;

    virtual typename IFactoryInfo::PtrType
    getInfo(const std::string& mnemonic, Opcode icode, const ExtractorIF::PtrType& extractor) = 0;

    virtual typename IFactoryIF<InstType, AnnotationType>::PtrType getNode(const Opcode istencil) = 0;

    virtual typename IFactoryIF<InstType, AnnotationType>::PtrType getDefault() const = 0;

    virtual std::string getName() const = 0;

    virtual const Field* getField() const = 0;

    virtual Opcode getStencil() const = 0;

    virtual void flushCaches() = 0;

    virtual void
    addIFactory(const Opcode istencil, const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) = 0;

    virtual void
    addIFactory(const std::string& mnemonic, const Opcode istencil,
                const typename IFactoryIF<InstType, AnnotationType>::PtrType& node,
                const ExtractorIF::PtrType& extractor) = 0;

    virtual void addDefaultIFactory(const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) = 0;

    virtual void print(std::ostream& os, const uint32_t level = 0) const = 0;
};

template<typename InstType, typename AnnotationType>
inline std::ostream& operator<<(std::ostream& os, const typename IFactoryIF<InstType, AnnotationType>::PtrType& ifact)
{
    ifact->print(os);
    return os;
}

/**
 * IFactorySpecialCaseComposite:
 */
template<typename InstType, typename AnnotationType>
class IFactorySpecialCaseComposite : public IFactoryIF<InstType, AnnotationType>
{
public:
    IFactorySpecialCaseComposite() = default;

    ~IFactorySpecialCaseComposite() = default;

    std::string getName() const override
    {
        return "<SpecialCases>";
    }

    /**
     * addSpecialCase: Add fixed field value constraints to an instruction
     * @param mnemonic
     * @param istencil
     * @param flist
     */
    /**
     * NOTE: Exclusions are implied by matching one of the special cases (fixed fields)
     * The general case is added as the default (no fixed fields).
     *
     * HOWEVER, If we ever need to implement explicit exclusions, I would suggest the following.
     *
     * 1. Add an "exclude" stanza to the JSON ISA file to provide a list of fields and
     *    the values of each field that should be excluded, e.g:
     *
     *         "exclude" : [ { "field": "rs2", "value": 0 }, ...]
     *
     * 2. Parse this in DTable::config() and pass a vector of exclusions "xlist" to addSpecialCase(), e.g:
     *
     *          struct FieldExclusion {
     *              std::string    name,
     *              uint64_t       val
     *          };
     *          std::vector<const FieldExclusion>  xlist;
     *
     * 3. Create a mask of the excluded fields (as is done for the fixed field list, flist), and
     *    a combined exclusion value (each of the field exclusion values shifted to lie under the
     *    corresponding field's mask). e.g:
     *
     *         for (const auto& fe : xlist) {
     *             xmask |= form->getField(fe.name).getShiftedMask();
     *             xval  |= form->getField(fe.name).getShiftedValue(fe.val);
     *         }
     *
     * 4. Add the exclusion mask and exclusion values (2 uint64's, xmask and xval) to the table_ entry.
     *    For nfixed, use the greater of the two field list sizes (maximum of flist.size() and xlist.size())
     *
     * 5. In getNode(), addIFactory(), and makeInst(), the matching criteria would now also include
     *    (istencil & entry.xmask) != entry.xval;
     *
     * 6. If we need to support multiple exclusion values for a given field, I suggest the following:
     *
     *         "exclude" : [ { "field": "rs2", "value": [0,7,...] }, ...]
     *
     *    i.e. exclude.value is a list of integers. If we can guarantee the values are all < 64 (0..63),
     *    then the values can be represented as a uint64_t bit vector.
     *
     *    a. Create the xmask and xval as before
     *    b. Matching (in getNode(), addIFactory(), and makeInst()) now involves turning the values under
     *       mask to a shifted bit value:
     *
     *         for (const auto& fe : xlist) {
     *             bval |= form->getField(fe.name).getShiftedBitVal(istencil);
     *         }
     *
     *     c. Matching criteria is (istencil & entry.mask & bval) == 0, i.e. no matching exclusion values
     *     d. NOTE: The bval for-loop needs to be efficient. Rather than looping on xlist, it would be
     *        better to construct a vector of field shift values from xlist (and store that in the entry
     *        for use during matching)
     */
    // TODO: Deprecate all uses of fixed_field_set! It's DANGEROUS
    // The problem with fixed_field_set is that it is generated from the DECODE form, using the
    // enum definition in that form's class. When we're extracting, we may use an "xform" which likely
    // has an incompatible field enum (where field id enum values are different between the form and xform)
    void addSpecialCase(const FormWrapperIF* form, const std::string& mnemonic, const Opcode istencil,
                        const std::vector<std::string>& flist)
    {
        Opcode mask = 0;        // Mask of fixed fields
        for (const auto& fname : flist) {
            // std::cout << FormType::getField(fname) << std::endl;
            // mask |= FormType::getField(fname).getShiftedMask();
            mask |= form->getField(fname).getShiftedMask();
        }

        // Bit set of fixed field id's
        const uint64_t field_set = form->getFieldIDSet(flist);

        // Insert to keep the vector sorted by decreasing nfixed number
        // A higher nfixed number indicates more fields are fixed and thus the match is more specific
        // The intended effect is to sort the table_ vector in decreasing order of specificity
        const uint32_t nfixed = flist.size();
        bool inserted = false;
        for (auto iter = table_.begin(); iter != table_.end(); ++iter) {
            if (iter->nfixed < nfixed) {
                table_.insert(iter, {mnemonic, mask, field_set, istencil & mask, nfixed, nullptr});
                inserted = true;
                break; // table_ vector will be reallocated after insert, iterator now invalid!
            }
        }
        if (!inserted) {
            table_.push_back(
                {mnemonic, mask, field_set, istencil & mask, static_cast<uint32_t>(flist.size()), nullptr});
        }
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getNode(const Opcode istencil) override
    {
        for (const auto& entry : table_) {
            // The first match will be the most specific match
            if ((istencil & entry.mask) == entry.value) {
                return entry.factory;
            }
        }
        return default_.factory;
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getDefault() const override
    {
        return default_.factory;
    }

    void addIFactory(const std::string& mnemonic, const Opcode istencil,
                     const typename IFactoryIF<InstType, AnnotationType>::PtrType& node,
                     const ExtractorIF::PtrType& extractor) override
    {
        assert(node != nullptr);
        assert(extractor != nullptr);

        for (auto& entry : table_) {
            // The first match will be the most specific match
            if ((istencil & entry.mask) == entry.value) {
                entry.factory = node;
                entry.extractor = extractor->specialCaseClone(entry.mask, entry.field_set);
                return;
            }
        }

        // If this is not true, then we've already assigned a default
        // This can happen if fixed fields are missing from the specification
        assert(default_.factory == nullptr);
        default_ = {mnemonic, 0, 0, istencil, 0, node, extractor};
    }

    void addDefaultIFactory(const std::string& mnemonic, const Opcode istencil,
                            const typename IFactoryIF<InstType, AnnotationType>::PtrType& node,
                            const ExtractorIF::PtrType& extractor)
    {
        assert(node != nullptr);
        assert(extractor != nullptr);

        // If this is not true, then we've already assigned a default
        // This can happen if fixed fields are missing from the specification
        assert(default_.factory == nullptr);
        default_ = {mnemonic, 0, 0, istencil, 0, node, extractor};
    }

#if 0
    virtual typename InstType::PtrType makeInst(Opcode icode) {
        for (const auto& entry : table_) {
            // The first match will be the most specific match
            if ((icode & entry.mask) == entry.value) {
                assert(entry.factory != nullptr);
                return entry.factory->makeInst(entry.mnemonic, icode, entry.extractor);
            }
        }

        if (default_.factory != nullptr) {
            return default_.factory->makeInst(default_.mnemonic, icode, default_.extractor);
        } else {
            // WARN ABOUT THIS!
            assert(false);
        }
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(Opcode icode) override
    {
        for (const auto& entry : table_) {
            // The first match will be the most specific match
            if ((icode & entry.mask) == entry.value) {
                assert(entry.factory != nullptr);
                // Check for illegal opcode
                if (entry.extractor->isIllop(icode)) {
                    throw IllegalOpcode(entry.mnemonic, icode);
                }
                return entry.factory->getInfo(entry.mnemonic, icode, entry.extractor);
            }
        }

        if (default_.factory != nullptr) {
            // Check for illegal opcode
            if (default_.extractor->isIllop(icode)) {
                throw IllegalOpcode(default_.mnemonic, icode);
            }
            return default_.factory->getInfo(default_.mnemonic, icode, default_.extractor);
        } else {
            // We can get here naturally for encodings that are not part of the TRIE
            //
            // WARN ABOUT THIS!
            // We can also get into this situation for complex fixed/ignore combinations in the
            // instruction's JSON ISA spec. I've seen it for mv (a special case of addi with
            // an immediate of 0). The problem arose because the form used for decode (not the extraction
            // xform) had two fields to cover the immediate -- and if either of these fields are
            // non-zero, the instruction needed to decode to addi. Only when BOTH fields are 0
            // do we decode as a mv. The "either-or" concept wasn't representable in Mavis.
            //
            // Short story, if you get here, you'll need to think about handling the instruction
            // using an overlay
            //
            // throw DTableLookupError(icode, default_.mnemonic);
            // Changing the exception to an UnknownOpcode, since it can occur naturally.
            throw UnknownOpcode(icode);
        }
    }

    void flushCaches() override
    {
        for (auto& entry : table_) {
            assert(entry.factory != nullptr);
            entry.factory->flushCaches();
        }

        if (default_.factory != nullptr) {
            default_.factory->flushCaches();
        }
    }

    void print(std::ostream& os, const uint32_t level = 0) const override
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "IFactorySpecialCaseComposite::Field " << std::endl;
        uint32_t i = 0;
        for (const auto& sce : table_) {
            assert(sce.factory != nullptr);
            for (uint32_t j = 0; j < level + 1; ++j) {
                os << "|\t";
            }
            os << "[" << std::hex << i << "]: '" << sce.mnemonic << "'"
               << ", mask=0x" << std::hex << sce.mask
               << ", field_set=0x" << sce.field_set
               << ", value=0x" << sce.value
               << ", nfixed=" << std::dec << sce.nfixed
               << ", extractor=" << sce.extractor
               << ", factory=";
            sce.factory->print(os, level + 1);
            ++i;
        }

        if (default_.factory != nullptr) {
            for (uint32_t j = 0; j < level + 1; ++j) {
                os << "|\t";
            }
            os << "[default]: '" << default_.mnemonic << "'"
               << ", value=0x" << std::hex << default_.value
               << ", extractor=" << default_.extractor
               << ", factory=";
            default_.factory->print(os, level + 1);
        }
        os.flags(os_state);
    }

private:
    struct SpecialCaseEntry
    {
        std::string mnemonic;
        Opcode mask = 0;
        uint64_t field_set = 0;
        uint64_t value = 0;
        uint32_t nfixed = 0;
        typename IFactoryIF<InstType, AnnotationType>::PtrType factory = nullptr;
        ExtractorIF::PtrType extractor = nullptr;
    };
    std::vector<SpecialCaseEntry> table_;
    SpecialCaseEntry default_;

    const Field* getField() const override
    {
        assert(false);
        return nullptr;
    }

    Opcode getStencil() const override
    { return 0; }

    void addIFactory(const Opcode, const typename IFactoryIF<InstType, AnnotationType>::PtrType&) override
    {
        assert(false);
    }

    void addDefaultIFactory(const typename IFactoryIF<InstType, AnnotationType>::PtrType&) override
    { assert(false); }

#if 0
    virtual typename InstType::PtrType
    makeInst(const std::string& mnemonic, Opcode icode, const ExtractorIF::PtrType& extractor) {
        assert(false);
        return nullptr;
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType
    getInfo(const std::string& mnemonic, Opcode, const ExtractorIF::PtrType&) override
    {
        assert(false);
        return nullptr;
    }
};

/**
 * IFactorySparseComposite: TODO
 */
template<typename InstType, typename AnnotationType>
class IFactorySparseComposite : public IFactoryIF<InstType, AnnotationType>
{
public:
    explicit IFactorySparseComposite(const Field& f) :
        field_(f.clone())
    {
        mask_ = field_->getShiftedMask();
        // TODO: replace with mavis_assert
        assert(mask_ != 0 && "field mask must be non-zero");
    }

    ~IFactorySparseComposite() = default;

    std::string getName() const override
    {
        return "<Disambiguator>";
    }

    const Field* getField() const override
    {
        return field_;
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getNode(const Opcode istencil) override
    {
        const auto itr = hash_.find(istencil);
        if (itr != hash_.end()) {
            return itr->second;
        } else {
            return nullptr;
        }
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getDefault() const override
    {
        return default_;
    }

    void addIFactory(const Opcode istencil, const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) override
    {
        const auto itr = hash_.find(istencil);
        assert(itr != hash_.end() && "cannot find stencil");
        hash_[istencil] = node;
    }

    void addDefaultIFactory(const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) override
    {
        assert(default_ == nullptr && "default node already has a value");
        default_ = node;
    }

#if 0
    virtual typename InstType::PtrType makeInst(Opcode icode) {
        auto itr = hash_.find(icode & mask_);
        if (itr != hash_.end()) {
            assert(itr->second != nullptr);
            return itr->second->makeInst(icode);
        } else if (default_ != nullptr) {
            return default_->makeInst(icode);
        } else {
            std::cout << "CANT FIND FACTORY FOR ICODE: 0x" << std::hex << icode << std::dec << std::endl;
            std::cout << "MASK = 0x" << std::hex << mask_ << std::dec << std::endl;
            std::cout << this;
            assert(false);
            return nullptr;
        }
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(Opcode icode) override
    {
        const auto itr = hash_.find(icode & mask_);
        if (itr != hash_.end()) {
            assert(itr->second != nullptr && "cannot find hash entry for opcode");
            return itr->second->getInfo(icode);
        } else if (default_ != nullptr) {
            return default_->getInfo(icode);
        } else {
            std::ios_base::fmtflags cout_state(std::cout.flags());
            std::cout << "CANT FIND FACTORY FOR ICODE: 0x" << std::hex << icode << std::endl;
            std::cout << "MASK = 0x" << std::hex << mask_ << std::endl;
            std::cout << this;
            std::cout.flags(cout_state);
            assert(false && "cannot find factory for opcode");
            return nullptr;
        }
    }

    void flushCaches() override
    {
        for (const auto&[key, ifact] : hash_) {
            if (ifact != nullptr) {
                ifact->flushCaches();
            }
        }

        if (default_ != nullptr) {
            default_->flushCaches();
        }
    }

    void print(std::ostream& os, const uint32_t level = 0) const override
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "IFactoryUniqueComposite::Field " << field_->getName() << std::endl;
        uint32_t i = 0;
        for (const auto&[key, ifact] : hash_) {
            if (ifact != nullptr) {
                for (uint32_t j = 0; j < level + 1; ++j) {
                    os << "|\t";
                }
                os << "[" << std::hex << i << "]: ";
                ifact->print(os, level + 1);
            }
        }

        if (default_ != nullptr) {
            for (uint32_t j = 0; j < level + 1; ++j) {
                os << "|\t";
            }
            os << "[default]: ";
            default_->print(os, level + 1);
        }
        os.flags(os_state);
    }

private:
    typename IFactoryIF<InstType, AnnotationType>::PtrType default_ = nullptr;
    std::map<Opcode, typename IFactoryIF<InstType, AnnotationType>::PtrType> hash_;
    Field* field_;
    Opcode mask_;

    Opcode getStencil() const override
    { return 0; };

    void addIFactory(const std::string&, const Opcode,
                     const typename IFactoryIF<InstType, AnnotationType>::PtrType&,
                     const ExtractorIF::PtrType&) override
    { assert(false); }

#if 0
    virtual typename InstType::PtrType
    makeInst(const std::string& mnemonic, Opcode icode, const ExtractorIF::PtrType& extractor) {
        assert(false);
        return nullptr;
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType
    getInfo(const std::string&, Opcode, const ExtractorIF::PtrType&) override
    {
        assert(false);
        return nullptr;
    }
};

/**
 * IFactoryDenseComposite: Composite for densely populated nodes
 */
template<typename InstType, typename AnnotationType>
class IFactoryDenseComposite : public IFactoryIF<InstType, AnnotationType>
{
public:
    explicit IFactoryDenseComposite(const Field& f) :
        field_(f.clone())
    {
        itable_.reset(new typename IFactoryIF<InstType, AnnotationType>::PtrType[field_->getSize()]);
        for (uint32_t i = 0; i < field_->getSize(); ++i) {
            itable_[i] = nullptr;
        }
    }

    ~IFactoryDenseComposite() = default;

    std::string getName() const override
    {
        return field_->getName();
    }

    const Field* getField() const override
    {
        return field_;
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getDefault() const override
    {
        return default_;
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getNode(const Opcode istencil) override
    {
        const uint32_t index = field_->extract(istencil);
        return itable_[index];
    }

    void addIFactory(const Opcode istencil, const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) override
    {
        const uint32_t index = field_->extract(istencil);
        itable_[index] = node;
    }

    void addDefaultIFactory(const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) override
    {
        assert((default_ == nullptr) && "default already assigned");
        default_ = node;
    }

#if 0
    virtual typename InstType::PtrType makeInst(Opcode icode) {
        uint32_t index = field_->extract(icode);
        if (itable_[index] != nullptr) {
            return itable_[index]->makeInst(icode);
        } else if (default_ != nullptr) {
            return default_->makeInst(icode);
        } else {
            return nullptr;
        }
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(Opcode icode) override
    {
        const uint32_t index = field_->extract(icode);
        if (itable_[index] != nullptr) {
            // Try the specific nodes first, if we fail to decode,
            // then try the default (if it exists)
            typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType iptr = nullptr;
            try {
                iptr = itable_[index]->getInfo(icode);
            } catch (const UnknownOpcode& ex) {
                if (default_ != nullptr) {
                    return default_->getInfo(icode);
                } else {
                    // rethrow
                    throw;
                }
            }
            return iptr;
            // return itable_[index]->getInfo(icode);
        } else if (default_ != nullptr) {
            return default_->getInfo(icode);
        } else {
            throw UnknownOpcode(icode);
        }
    }

    void flushCaches() override
    {
        for (uint32_t i = 0; i < field_->getSize(); ++i) {
            if (itable_[i] != nullptr) {
                itable_[i]->flushCaches();
            }
        }

        if (default_ != nullptr) {
            default_->flushCaches();
        }
    }

    void print(std::ostream& os, const uint32_t level = 0) const override
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "IFactoryDenseComposite::Field " << field_->getName() << std::endl;
        for (uint32_t i = 0; i < field_->getSize(); ++i) {
            if (itable_[i] != nullptr) {
                for (uint32_t j = 0; j < level + 1; ++j) {
                    os << "|\t";
                }
                os << "[" << std::hex << i << "]: ";
                itable_[i]->print(os, level + 1);
            }
        }

        if (default_ != nullptr) {
            for (uint32_t j = 0; j < level + 1; ++j) {
                os << "|\t";
            }
            os << "[default]: ";
            default_->print(os, level + 1);
        }
        os.flags(os_state);
    }

private:
    typename IFactoryIF<InstType, AnnotationType>::PtrType default_ = nullptr;
    Field* field_;
    std::unique_ptr<typename IFactoryIF<InstType, AnnotationType>::PtrType[]> itable_;

    Opcode getStencil() const override
    { return 0; };

    void addIFactory(const std::string&, const Opcode,
                const typename IFactoryIF<InstType, AnnotationType>::PtrType&,
                const ExtractorIF::PtrType&) override
    { assert(false); }

#if 0
    virtual typename InstType::PtrType
    makeInst(const std::string& mnemonic, Opcode icode, const ExtractorIF::PtrType& extractor) {
        assert(false);
        return nullptr;
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType
    getInfo(const std::string&, Opcode, const ExtractorIF::PtrType&) override
    {
        assert(false);
        return nullptr;
    }
};

/**
 * IFactorySelectorComposite: select branch given lambda
 */
template<typename InstType, typename AnnotationType>
class IFactorySelectorComposite : public IFactoryIF<InstType, AnnotationType>
{
public:
    typedef std::function<uint32_t(uint32_t)> SelectorType;

    IFactorySelectorComposite(const Field& f, const SelectorType& lambda, uint32_t tablesize) :
        field_(f.clone()), selector_(lambda), tsize_(tablesize)
    {
        // itable_ = new typename IFactoryIF<InstType, AnnotationType>::PtrType[tsize_];
        itable_.reset(new typename IFactoryIF<InstType, AnnotationType>::PtrType[tsize_]);
        for (uint32_t i = 0; i < tsize_; ++i) {
            itable_[i] = nullptr;
        }
    }

    ~IFactorySelectorComposite() = default;
    //{
        //delete[] itable_;
    //}

    std::string getName() const override
    {
        return field_->getName();
    }

    const Field* getField() const override
    {
        return field_;
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getDefault() const override
    {
        return default_;
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getNode(const Opcode istencil) override
    {
        uint32_t index = selector_(field_->extract(istencil));
        return itable_[index];
    }

    void addIFactory(const Opcode istencil, const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) override
    {
        uint32_t index = selector_(field_->extract(istencil));
        itable_[index] = node;
    }

    void addDefaultIFactory(const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) override
    {
        assert((default_ == nullptr) && "default already assigned");
        default_ = node;
    }

#if 0
    virtual typename InstType::PtrType makeInst(Opcode icode) {
        uint32_t index = selector_(field_->extract(icode));
        if (itable_[index] != nullptr) {
            return itable_[index]->makeInst(icode);
        } else if (default_ != nullptr) {
            return default_->makeInst(icode);
        } else {
            return nullptr;
        }
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(Opcode icode) override
    {
        const uint32_t index = selector_(field_->extract(icode));
        if (itable_[index] != nullptr) {
            return itable_[index]->getInfo(icode);
        } else if (default_ != nullptr) {
            return default_->getInfo(icode);
        } else {
            throw UnknownOpcode(icode);
            // return nullptr;
        }
    }

    void flushCaches() override
    {
        for (uint32_t i = 0; i < tsize_; ++i) {
            if (itable_[i] != nullptr) {
                itable_[i]->flushCaches();
            }
        }

        if (default_ != nullptr) {
            default_->flushCaches();
        }
    }

    void print(std::ostream& os, const uint32_t level = 0) const override
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "IFactorySelectorComposite::Field " << field_->getName() << std::endl;
        for (uint32_t i = 0; i < tsize_; ++i) {
            for (uint32_t j = 0; j < level + 1; ++j) {
                os << "|\t";
            }
            if (itable_[i] != nullptr) {
                os << "[" << std::hex << i << "]: ";
                itable_[i]->print(os, level + 1);
            }
        }

        if (default_ != nullptr) {
            for (uint32_t j = 0; j < level + 1; ++j) {
                os << "|\t";
            }
            os << "[default]: ";
            default_->print(os, level + 1);
        }
        os.flags(os_state);
    }

private:
    typename IFactoryIF<InstType, AnnotationType>::PtrType default_ = nullptr;
    Field* field_;
    SelectorType selector_;
    uint32_t tsize_;
    // typename IFactoryIF<InstType, AnnotationType>::PtrType* itable_;
    std::unique_ptr<typename IFactoryIF<InstType, AnnotationType>::PtrType[]> itable_;

    Opcode getStencil() const override
    { return 0; };

    void addIFactory(const std::string&, const Opcode,
                const typename IFactoryIF<InstType, AnnotationType>::PtrType&,
                const ExtractorIF::PtrType&) override
    { assert(false); }

#if 0
    virtual typename InstType::PtrType
    makeInst(const std::string& mnemonic, Opcode icode, const ExtractorIF::PtrType& extractor) {
        assert(false);
        return nullptr;
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType
    getInfo(const std::string&, Opcode, const ExtractorIF::PtrType&) override
    {
        assert(false);
        return nullptr;
    }
};

/**
 * IFactoryMatchListComposite: select branch given list of lambdas
 */
template<typename InstType, typename AnnotationType, uint32_t TableSize>
class IFactoryMatchListComposite : public IFactoryIF<InstType, AnnotationType>
{
public:
    typedef std::function<bool(uint32_t)> MatcherType;

    IFactoryMatchListComposite(const Field& f, std::initializer_list<MatcherType> lambdas) :
        field_(f.clone())
    {
        uint32_t i = 0;
        for (const auto& matcher : lambdas) {
            itable_[i] = {matcher, nullptr};
            ++i;
        }
    }

    ~IFactoryMatchListComposite() = default;

    std::string getName() const override
    {
        return field_->getName();
    }

    const Field* getField() const override
    {
        return field_.get();
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getDefault() const override
    {
        return default_;
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getNode(const Opcode istencil) override
    {
        for (const auto& me : itable_) {
            if (me.matcher(field_->extract(istencil))) {
                return me.factory;
            }
        }
        return nullptr;
    }

    void addIFactory(const Opcode istencil, const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) override
    {
        for (auto& me : itable_) {
            if (me.matcher(field_->extract(istencil))) {
                me.factory = node;
                return;
            }
        }
    }

    void addDefaultIFactory(const typename IFactoryIF<InstType, AnnotationType>::PtrType& node) override
    {
        assert((default_ == nullptr) && "default already assigned");
        default_ = node;
    }

#if 0
    virtual typename InstType::PtrType makeInst(Opcode icode) {
        for (auto me : itable_) {
            if (me.matcher(field_->extract(icode))) {
                if (me.factory != nullptr) {
                    return me.factory->makeInst(icode);
                } else if (default_ != nullptr) {
                    return default_->makeInst(icode);
                } else {
                    return nullptr;
                }
            }
        }
        return nullptr;
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(Opcode icode) override
    {
        for (auto& me : itable_) {
            if (me.matcher(field_->extract(icode))) {
                if (me.factory != nullptr) {
                    return me.factory->getInfo(icode);
                } else if (default_ != nullptr) {
                    return default_->getInfo(icode);
                } else {
                    throw UnknownOpcode(icode);
                    // return nullptr;
                }
            }
        }
        throw UnknownOpcode(icode);
        // return nullptr;
    }

    void flushCaches() override
    {
        for (const auto& me : itable_) {
            if (me.factory != nullptr) {
                me.factory->flushCaches();
            }
        }

        if (default_ != nullptr) {
            default_->flushCaches();
        }
    }

    void print(std::ostream& os, const uint32_t level = 0) const override
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "IFactoryMatchListComposite::Field " << field_->getName() << std::endl;
        uint32_t i = 0;
        for (const auto& me : itable_) {
            if (me.factory != nullptr) {
                for (uint32_t j = 0; j < level + 1; ++j) {
                    os << "|\t";
                }
                os << "[" << std::hex << i << "]: ";
                me.factory->print(os, level + 1);
            }
            ++i;
        }

        if (default_ != nullptr) {
            for (uint32_t j = 0; j < level + 1; ++j) {
                os << "|\t";
            }
            os << "[default]: ";
            default_->print(os, level + 1);
        }
        os.flags(os_state);
    }

private:
    typename IFactoryIF<InstType, AnnotationType>::PtrType default_ = nullptr;
    struct MatchEntry
    {
        MatcherType matcher;
        typename IFactoryIF<InstType, AnnotationType>::PtrType factory;
    };
    //Field* field_;
    std::unique_ptr<Field> field_;
    std::array<MatchEntry, TableSize> itable_;

    Opcode getStencil() const override
    { return 0; };

    void addIFactory(const std::string&, const Opcode,
                const typename IFactoryIF<InstType, AnnotationType>::PtrType&,
                const ExtractorIF::PtrType&) override
    { assert(false); }

#if 0
    virtual typename InstType::PtrType
    makeInst(const std::string& mnemonic, Opcode icode, const ExtractorIF::PtrType& extractor) {
        assert(false);
        return nullptr;
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType
    getInfo(const std::string&, Opcode, const ExtractorIF::PtrType&) override
    {
        assert(false);
        return nullptr;
    }
};

/**
 * IFactory: Leaf node of composite pattern
 * @tparam FormType
 */
template<typename InstType, typename AnnotationType>
class IFactory : public IFactoryIF<InstType, AnnotationType>
{
public:
    typedef std::shared_ptr<IFactory> PtrType;

private:
#if 0
    // Just a placeholder for the sake of prototyping
    template<uint32_t Size>
    class Cache {
    public:
        struct Line {
            uint32_t tag = 0;
            DecodedInstructionInfo::PtrType dii;
            typename AnnotationType::PtrType anno;
        };

    private:
        std::array<Line, Size> table_;
        DecodedInstructionInfo::PtrType not_found_;

    public:
        const Line* lookup(const ExtractorIF::PtrType& extractor, const Opcode icode) const {
            uint32_t hash = extractor->getIFCIndex(icode) % Size;
            // TODO: eliminate getIFCTag(), just use the icode
            if ((table_[hash].dii != nullptr) && (table_[hash].tag == extractor->getIFCTag(icode))) {
                return &table_[hash];
            } else {
                return nullptr;
            }
        }

        template<typename ...Types>
        const Line* allocate(const std::string& mnemonic,
                             const InstructionUniqueID inst_uid,
                             const ExtractorIF::PtrType& extractor,
                             const InstMetaData::PtrType& einfo,
                             const Opcode icode,
                             const typename AnnotationType::PtrType& anno,
                             Types... args) {
            uint32_t hash = extractor->getIFCIndex(icode) % Size;

            table_[hash].tag = extractor->getIFCTag(icode);
            table_[hash].dii.reset(new DecodedInstructionInfo(mnemonic, inst_uid, extractor, einfo, icode, args...));
            table_[hash].anno = anno;
            return &table_[hash];
        }
    };

    typedef Cache<7>    ExtractionCacheType;
#endif

    struct StashEntry
    {
        Opcode tag = 0;
        DecodedInstructionInfo::PtrType dii;
        ExtractorIF::PtrType extractor;
        InstMetaData::PtrType meta;
        DisassemblerIF::PtrType dasm;
        typename AnnotationType::PtrType anno;
    };

    typedef Stash<Opcode, StashEntry, 7> ExtractionStashType;

public:
    IFactory(const std::string& name, const Opcode stencil, const InstMetaData::PtrType& meta) :
        name_(name), stencil_(stencil), meta_(meta), stash_(new ExtractionStashType("ExtractionStash"))
    {
        // TODO: Temporarily make a new disassembler object.
        // Once we have fully implemented the Disassembler, the builder will need to
        // supply us with the disassembler to use
        dasm_ = std::make_shared<Disassembler>();
    }

    IFactory(const std::string& name, const Opcode stencil,
             const InstMetaData::PtrType& meta, const typename AnnotationType::PtrType anno) :
        IFactory(name, stencil, meta)
    {
        addInstructionVariantAnnotation(name, anno);
    }

    std::string getName() const override
    {
        //FormType    f;
        std::stringstream ss;
        ss << "IFactory('" << name_ << "')"
           << ", stencil = 0x" << std::hex << stencil_;
        return ss.str();
    }

    Opcode getStencil() const override
    {
        return stencil_;
    }

    /**
     * \brief getInfo: returns the information associated with the instructions managed by this IFactory
     * \param mnemonic The mnemonic of the instruction (could be different than the factory name)
     * \param icode Instruction's opcode
     * \param extractor The extractor to use for this instruction
     * \return
     *
     * NOTE: The leaf IFactory (this class) may be associated with several different instructions. The
     * IFactorySpecialCaseComposite class (top of this file) is always the TRIE parent for the leaves, and
     * makes the final decision on which exact instruction we've decoded and passes us the instruction's
     * mnemonic, icode, and extractor.
     *
     * NOTE: This method is in the critical decode performance path
     */
    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType
    getInfo(const std::string& mnemonic, Opcode icode,
            const ExtractorIF::PtrType& extractor) override
    {
        OpcodeInfo::PtrType optr = nullptr;
        const StashEntry* entry = stash_->lookup(icode);

        // Stash miss...
        if (entry == nullptr) {
            std::string use_mnemonic = mnemonic;
            ExtractorIF::PtrType use_extractor = extractor;
            InstMetaData::PtrType use_meta = getMeta_(mnemonic);
            // TODO: Do we need to support instruction variants for disassembly?
            DisassemblerIF::PtrType use_dasm = dasm_;
            InstructionUniqueID use_uid = getInstructionUID_(mnemonic);  // TODO: lookup work may be wasted
            // typename AnnotationType::PtrType use_anno = getAnnotation_(mnemonic);     // TODO: lookup work may be wasted
            typename AnnotationType::PtrType use_anno = findAnnotation_(mnemonic);     // TODO: lookup work may be wasted

            const typename Overlay<InstType, AnnotationType>::PtrType olay = findMatchingOverlay_(icode);
            // TODO: We can get a false hit for compressed instructions
            // Since the expansion can make c.addi4spn look like addi, the mv overlay for addi could
            // be triggered. FOR NOW, we also check the mnemonic against the overlay's base mnemonic
            if ((olay != nullptr) && (olay->getBaseMnemonic() == mnemonic)) {
                use_mnemonic = olay->getMnemonic();
                if (olay->getExtractor() != nullptr) {
                    use_extractor = olay->getExtractor();
                }
                use_meta = olay->getMetaData();
                use_dasm = olay->getDasm();
                use_uid = olay->getUID();
                use_anno = olay->getAnnotation();
            }

            const StashEntry* new_entry = stash_->allocate(icode, icode,
                                                          std::make_shared<DecodedInstructionInfo>(use_mnemonic,
                                                                                                   use_uid,
                                                                                                   use_extractor,
                                                                                                   use_meta,
                                                                                                   icode),
                                                          use_extractor, use_meta, use_dasm, use_anno);
            optr = std::make_shared<OpcodeInfo>(icode, new_entry->dii, use_extractor, new_entry->meta, use_dasm);
            return std::make_shared<typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo>(optr, new_entry->anno);
        } else {
            optr = std::make_shared<OpcodeInfo>(icode, entry->dii, entry->extractor, entry->meta, entry->dasm);
            return std::make_shared<typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo>(optr, entry->anno);
        }
    }

    /**
     * \brief Version of getInfo which does not use the DecodedInstInfo cache. This is called by
     * DTable::makeInstDirectly (i.e. no opcode supplied)
     * \param mnemonic
     * \param icode
     * \param extractor
     * \return
     */
    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfoBypassCache(const std::string& mnemonic,
                                                                                            const ExtractorIF::PtrType& extractor)
    {
        // TODO: Overlays are not supported yet, since no opcode provided
        const DecodedInstructionInfo::PtrType& new_dii = std::make_shared<DecodedInstructionInfo>(mnemonic,
                                                                                                  getInstructionUID_(
                                                                                                      mnemonic),
                                                                                                  extractor, meta_,
                                                                                                  Opcode(0));
        OpcodeInfo::PtrType optr = std::make_shared<OpcodeInfo>(Opcode(0), new_dii, extractor, meta_, dasm_);

        // return std::make_shared<typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo>(optr,
                                                                                             // getAnnotation_(mnemonic));
        return std::make_shared<typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo>(optr,
                                                                                             findAnnotation_(mnemonic));
    }

    void addInstructionVariantAnnotation(const std::string& mnemonic, const typename AnnotationType::PtrType& anno)
    {
        if (annotation_map_.find(mnemonic) == annotation_map_.end()) {
            annotation_map_[mnemonic] = anno;
        } else {
            // TODO: Enable this check once we can assure uniqueness
            // We can get here if there is a dtable cache miss, and the instruction is being re-built
            // throw  BuildErrorDuplicateAnnotation(mnemonic, name_);
        }
    }

    void addInstructionVariantUID(const std::string& mnemonic, const InstructionUniqueID uid)
    {
        if (uid_map_.find(mnemonic) == uid_map_.end()) {
            uid_map_[mnemonic] = uid;
        }
#if 0
        // TODO: Enable this code once we can assure uniqueness
        try {
            uid_map_.addInstWithUIDUniquely(mnemonic);
        } catch (const BuildErrorDuplicateMnemonic& ex) {
            // We can get here if there is a dtable cache miss, and the instruction is being re-built
            // throw  BuildErrorDuplicateMnemonic(mnemonic);
        }
#endif
    }

    /**
     * @brief The builder uses this function to add compressed instruction meta data to the expanded
     * instruction's metadata
     * @param mnemonic
     * @param new_meta
     */
     // TODO: Do we need something like this for disassemblers?
    void mergeInstructionVariantMetaData(const std::string& mnemonic, const InstMetaData::PtrType& new_meta)
    {
        // Clone the factory metadata and merge in the new stuff
        InstMetaData::PtrType combined_meta = meta_->clone();
        combined_meta->merge(new_meta);

        // Register the combined metadata with the mnemonic
        if (meta_map_.find(mnemonic) == meta_map_.end()) {
            meta_map_[mnemonic] = combined_meta;
        } else {
            // TODO: Enable this check once we can assure uniqueness
            // We can get here if there is a dtable cache miss, and the instruction is being re-built
            // throw  BuildErrorDuplicate(mnemonic, name_);
        }
    }

    void addOverlay(const typename Overlay<InstType, AnnotationType>::PtrType& olay)
    {
        // Insert to keep the vector sorted by decreasing number of mask bits in the overlay
        // A higher number indicates a more specific match
        // The intended effect is to sort the overlay list vector in decreasing order of specificity
        const uint32_t n_mask_bits = olay->getNumMaskBits();
        bool inserted = false;
        for (auto iter = overlay_list_.begin(); iter != overlay_list_.end(); ++iter) {
            if ((*iter)->getNumMaskBits() < n_mask_bits) {
                overlay_list_.insert(iter, olay);
                inserted = true;
                break; // table_ vector will be reallocated after insert, iterator now invalid!
            }
        }
        if (!inserted) {
            overlay_list_.push_back(olay);
        }
    }

    void flushCaches() override
    {
        stash_.reset(new ExtractionStashType("ExtractionStash"));
    }

    void print(std::ostream& os, const uint32_t) const override
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << getName() << std::endl;
        os.flags(os_state);
    }

protected:
    const std::string name_; // factory name
    const Opcode stencil_;     // For debugging
    InstMetaData::PtrType meta_;
    DisassemblerIF::PtrType dasm_;
    std::map<const std::string, InstructionUniqueID> uid_map_;
    std::map<const std::string, typename AnnotationType::PtrType> annotation_map_;
    std::map<const std::string, typename InstMetaData::PtrType> meta_map_;
    std::unique_ptr<ExtractionStashType> stash_;
    std::vector<typename Overlay<InstType, AnnotationType>::PtrType> overlay_list_;

protected:

    /**
     * @brief findAnnotation_: Lookup the annotation associated with the mnemonic.
     * Failing that, try lookup of the annotation by the factory name (e.g. for
     * expanded/compressed instructions). Otherwise return NULL
     *
     * @param mnemonic
     * @return
     */
    const typename AnnotationType::PtrType findAnnotation_(const std::string& mnemonic) const
    {
        assert(!annotation_map_.empty());
        auto iter = annotation_map_.find(mnemonic);
        if (iter == annotation_map_.end()) {
            // If this mnemonic is an expansion (i.e. compressed instruction), we will
            // not find it in the annotation map. Compressed/expanded instructions map
            // to another (existing) factory, and that factory's annotations will be used
            // instead. For these instructions, return the annotation for this factory name
            iter = annotation_map_.find(name_);
            if (iter == annotation_map_.end()) {
                return nullptr; // Annotation not found for mnemonic or factory name
            } else {
                return iter->second; // Annotation found for factory name
            }
        } else {
            return iter->second; // Annotation found for mnemonic
        }
    }

#if 0
    // Custom IFactories (e.g. for non-ISA instructions such as CMOV) should override
    // this method to avoid throwing an exception for a missing annotation
    virtual const typename AnnotationType::PtrType getAnnotation_(const std::string& mnemonic) const
    {
        const typename AnnotationType::PtrType panno = findAnnotation_(mnemonic);
        if (panno == nullptr) {
            throw BuildErrorMissingAnnotation(mnemonic, name_);
        }
        return panno;
    }
#endif

    typename Overlay<InstType, AnnotationType>::PtrType findMatchingOverlay_(Opcode icode) const
    {
        for (const auto& olay : overlay_list_) {
            if (olay->isMatch(icode)) {
                return olay;
            }
        }
        return nullptr;
    }


private:
    InstructionUniqueID getInstructionUID_(const std::string& mnemonic) const
    {
        InstructionUniqueID uid = uid_map_.at(mnemonic);
        assert((uid != INVALID_UID) && "UID is invalid");
        return uid;
    }

    /**
     * \brief Get the meta data for this instruction. If we don't find the
     * metadata, it means there was no instruction variant registered, and we
     * just use the metadata associated with the factory
     *
     * \param mnemonic
     * \return
     */
     // TODO: do we need to support instruction variants for disassemblers?
    InstMetaData::PtrType getMeta_(const std::string& mnemonic) const
    {
        const auto iter = meta_map_.find(mnemonic);
        if (iter == meta_map_.end()) {
            return meta_;
        }
        return iter->second;
    }

private:
    const Field* getField() const override
    {
        assert(false);
        return nullptr;
    }

    void addIFactory(const Opcode, const typename IFactoryIF<InstType, AnnotationType>::PtrType&) override
    { assert(false); }

    void addIFactory(const std::string&, const Opcode,
                const typename IFactoryIF<InstType, AnnotationType>::PtrType&,
                const ExtractorIF::PtrType&) override
    { assert(false); }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getNode(const Opcode) override
    { return nullptr; }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getDefault() const override
    { return nullptr; }

    void addDefaultIFactory(const typename IFactoryIF<InstType, AnnotationType>::PtrType&) override
    { assert(false); }

#if 0
    virtual typename InstType::PtrType makeInst(Opcode) {
        assert(false);
        return nullptr;
    }
#endif

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(Opcode) override
    {
        assert(false);
        return nullptr;
    }
};

} // namespace mavis
