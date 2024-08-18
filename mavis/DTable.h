#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include "json.hpp"
#include "FormRegistry.h"
#include "FormPseudo.h"
#include "IFactory.h"
#include "IFactoryBuilder.h"
#include "InstMetaData.h"
#include "InstMetaDataRegistry.hpp"
#include "ExtractorTraceInfo.h"
#include "ExtractorDirectInfo.h"
#include "ExtractorDirectImplementations.hpp"
#include "ExtractorRegistry.h"
#include "DecoderTypes.h"
#include "DecoderExceptions.h"
#include "Tag.hpp"
#include "Pattern.hpp"
#include "MatchSet.hpp"

namespace mavis {
// TODO: How to write decode stanzas in the JSON file
// TODO: Is there any advantage to passing the builder into DTable?
// TODO: Provide builder with generalized annotations dictionary
// TODO: Generic Registry?
// TODO: Get rid of cache stats
// TODO: Special extractors for SFENCE.VMA (TLB invalidates) and JALR (LR detect for RAS push/pop)
// Special cases are determined by register values and register value relationships
// Do we need an extractor facility for test condition lambda's?
// TODO: Explicit override on virtual functions
// TODO: Copius Doxygen!!
// TODO: Stricter matching criteria for overlays: based on number of 1-bits in mask
// TODO: Tune instruction and factory extraction caches
// TODO: Better unit testing for Mavis

/**
 * DTable : decode table (per field TRIE of IFactory composites)
 *
 * The DTable is constructed by reading in the ISA JSON files. Below is an example of an ISA JSON
 * entry for a instruction from the rv64a extension:
 *
 *     "mnemonic" : "amoxor.d",
 *     "tags" : ["a", "g"],
 *     "form" : "AMO",
 *     "stencil" : "0x2000302f",
 *     "type" : ["int", "load", "atomic"],
 *     "l-oper" : "all",
 *     "data" : 64
 *
 * Each entry in the ISA JSON files defines a "Form" and a "Stencil". The Form defines the
 * instruction format defined by the RISC-V ISA; it defines the meaning of each bit in the
 * instruction as "fields". It also defines which fields are "opcode fields" which are used to
 * uniquely identify the instruction.
 *
 * For this example, the form "AMO" defines the following opcode fields:
 *   - opcode (bits 0:6)
 *   - func3 (bits 12:14)
 *   - func5 (bits 27:31)
 *
 * | 31|   |   |   | 27| 26| 25| 24|   |   |   | 20| 19|   |   |   | 15| 14|   | 12| 11|   |   |   |  7|  6|   |   |   |   |   |  0|
 * |:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
 * |**func5**||||| | |**rs2**|||||**rs1**|||||**func3**|||**rd**|||||**opcode**|||||||
 *
 * The Stencil provides the fixed values of the opcode fields. For an instruction to be decoded as
 * an amoxor.d instruction, its opcode field values must match the values in the stencil exaclty.
 *
 * For this example, these are the values of the opcode fields:
 *   - opcode: 0x2F
 *   - func3: 0x3
 *   - func5: 0x04
 *
 * | 31|   |   |   | 27| 26| 25| 24|   |   |   | 20| 19|   |   |   | 15| 14|   | 12| 11|   |   |   |  7|  6|   |   |   |   |   |  0|
 * |:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
 * |**0x4**||||| | |**rs2**|||||**rs1**|||||**0x3**|||**rd**|||||**0x2F**|||||||
 *
 * When constructing the DTable, a new node is created for every unique opcode field value. For the
 * rv64a extension, this results in a 3-level table (one level for each opcode field). There is one
 * unique value for the opcode field (0x2F) and two unique values for the func3 field (0x2 and 0x3).
 * For each unique value of func3, there is a node for func5 with child nodes for each unique vaue
 * of func5. This results in 22 leaf nodes in the DTable, one for each instruction in the rv64a
 * extension.
 *
 * To decode an instruction, the DTable is traversed by getting the value of the field of the
 * current node to determine which node to go to next. All of the instructions in the rv64a
 * extension have the same opcode value, so the next step is to get the value of the func3 field.
 * The value of func3 will determine which func5 node to go to. Since func5 is the last opcode
 * field, the next node will be a leaf node with the instruction Extractor.
 *
 * ![DTableTraversal](https://github.com/sparcians/mavis/mavis/MavisDTableTraversalExample.png)
 */
template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
class DTable
{
public:
    typedef std::shared_ptr<DTable<InstType,AnnotationType,AnnotationTypeAllocator>>        PtrType;

private:
    typedef typename std::vector<std::string> FieldNameListType;
    // typedef typename std::set<std::string>      FieldNameSetType;

    // Very simple "direct-mapped" cache of opcode-tagged objects
    template<typename ObjectType, uint32_t Size, bool CollectStats = false>
    class Cache
    {
    private:
        struct Line
        {
            uint32_t tag = 0;
            typename ObjectType::PtrType handle;
        };
        std::array<Line, Size> table_;
        typename ObjectType::PtrType not_found_;
        uint32_t hits_ = 0;
        uint32_t accesses_ = 0;
        uint32_t collisions_ = 0;

        uint32_t hash_(const Opcode icode)
        {
            return icode % Size;
        }

    public:
        ~Cache()
        {
            if constexpr (CollectStats) {
                std::ios_base::fmtflags cout_state(std::cout.flags());
                std::cout << "Cache<" << std::dec << Size << ">: "
                          << hits_ << "/" << accesses_
                          << " (collisions: " << collisions_ << ")"
                          << std::endl;
                std::cout.flags(cout_state);
            }
        }

        const typename ObjectType::PtrType &lookup(const Opcode icode)
        {
            if constexpr (CollectStats) ++accesses_;
            uint32_t hash = hash_(icode);
            if ((table_[hash].handle != nullptr) && (table_[hash].tag == icode)) {
                if constexpr (CollectStats) ++hits_;
                return table_[hash].handle;
            } else {
                if constexpr (CollectStats) collisions_ += (table_[hash].handle != nullptr);
                return not_found_;
            }
        }

        void allocate(const Opcode icode, const typename ObjectType::PtrType &handle)
        {
            uint32_t hash = hash_(icode);
            table_[hash].tag = icode;
            table_[hash].handle = handle;
        }
    };

    constexpr static inline uint32_t CACHE_SIZE = 1023;
    using InstCache = Cache<InstType, CACHE_SIZE>;
    using IFactoryCache = Cache<typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo, CACHE_SIZE>;

public:
    explicit DTable(typename IFactoryBuilder<InstType, AnnotationType, AnnotationTypeAllocator>::PtrType builder) :
        builder_(builder), icache_(new InstCache()), ocache_(new IFactoryCache())
    {
        //Form<'*'>   form;
        //root_ = new IFactoryDenseComposite(form.getField(Form<'*'>::FAMILY));
#if 0
        const auto selector = [](uint32_t icode) {
            if ((icode & 0x3) != 0x3) {
                return 0; // 16-bit
            } else if (((icode & 0x3) == 3) && ((icode & 0x1c) != 0x1c)) {
                return 1; // 32-bit
            } else if ((icode & 0x3f) == 0x1f) {
                return 2; // 48-bit
            } else if ((icode & 0x7f) == 0x3f) {
                return 3; // 64-bit
            } else if (((icode & 0x7f) == 0x7f) && ((icode & 0x7000) != 0x7000)) {
                return 4; // 80 + 16*nnn bit
            } else if ((icode & 0x707f) == 0x707f) {
                return 5; // reserved for 192-bit
            } else {
                return -1;
            }
        };

        root_ = new IFactorySelectorComposite(form.getField(Form<'*'>::FAMILY), selector, 6);
#else
        root_.reset(new IFactoryMatchListComposite<InstType, AnnotationType, 6>(
            PseudoForm<'*'>::getField(PseudoForm<'*'>::FAMILY), {
                [](uint32_t icode) { return (icode & 0x3ul) != 0x3ul; },
                [](uint32_t icode) { return ((icode & 0x3ul) == 3ul) && ((icode & 0x1cul) != 0x1cul); },
                [](uint32_t icode) { return (icode & 0x3ful) == 0x1ful; },
                [](uint32_t icode) { return (icode & 0x7ful) == 0x3ful; },
                [](uint32_t icode) { return ((icode & 0x7ful) == 0x7ful) && ((icode & 0x7000ul) != 0x7000ul); },
                [](uint32_t icode) { return (icode & 0x707ful) == 0x707ful; },
            }));
#endif
    }

    void configure(const FileNameListType &isa_files,
                   const MatchSet<Pattern>& inclusions = MatchSet<Pattern>(),
                   const MatchSet<Pattern>& exclusions = MatchSet<Pattern>());

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(const Opcode icode)
    {
        const typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType &ohandle = ocache_->lookup(icode);
        if (ohandle == nullptr) {
            const typename IFactory<InstType, AnnotationType>::IFactoryInfo::PtrType &new_ohandle = root_->getInfo(
                icode);
            if (new_ohandle != nullptr) {
                ocache_->allocate(icode, new_ohandle);
                return new_ohandle;
            } else {
                throw UnknownOpcode(icode);
            }
        } else {
            return ohandle;
        }
    }

    template<class InstTypeAllocator, typename ...ArgTypes>
    typename InstType::PtrType makeInst(const Opcode icode, InstTypeAllocator &allocator, ArgTypes &&... args)
    {
        const typename InstType::PtrType &ihandle = icache_->lookup(icode);

        if (ihandle == nullptr) {
            // Cache miss...
            // const typename IFactory<InstType, AnnotationType>::IFactoryInfo::PtrType &info = root_->getInfo(icode);
            const typename IFactory<InstType, AnnotationType>::IFactoryInfo::PtrType &info = getInfo(icode);
            if (info != nullptr) {
                const auto new_ihandle = allocator(info->opinfo, info->uinfo, args...);
                icache_->allocate(icode, new_ihandle);

                // THIS IS IMPORTANT!
                // NOTE: we do not return the cached instruction
                // directly. We return a copy.  This assures that the
                // cached version stays pristine (newly initialized
                // with no state changes which might be retained via
                // the InstType copy constructor)
                return allocator(*new_ihandle);

            } else {
                throw UnknownOpcode(icode);
            }
        } else {
            // Cache hit... return copy of our pristine cache entry
            return allocator(*ihandle);
        }
    }

    /**
     * makeInstFromTrace -- use information from trace to generate an instruction
     * @tparam TraceInfoType
     * @param tinfo
     * @return
     */
    template<typename TraceInfoType, class InstTypeAllocator, typename ...ArgTypes>
    typename InstType::PtrType
    makeInstFromTrace(const TraceInfoType &tinfo, InstTypeAllocator &allocator, ArgTypes &&... args)
    {
        // First, try to decode the binary opcode normally
        // This is the most efficient avenue
        auto inst = makeInst(tinfo.getOpcode(), allocator, std::forward<ArgTypes>(args)...);

        // Sanity check our instruction against the trace's expectations
        //if (inst == nullptr || (std::string(inst->getMnemonic()) != tinfo.getMnemonic())) {
        if (std::string(inst->getMnemonic()) != tinfo.getMnemonic()) {
            // We didn't match the trace, so we'll let the trace
            // override ours
            // TODO: Need a real einfo here!
            InstMetaData::PtrType einfo(new InstMetaData(InstMetaData::ISA::RV32I));
            typename IFactoryIF<InstType, AnnotationType>::PtrType ifact = builder_->build(tinfo.getMnemonic(),
                                                                                          tinfo.getMnemonic(), "",
                                                                                          0, einfo);
            ExtractorIF::PtrType extractor(new ExtractorTraceInfo<TraceInfoType>(tinfo));
            const typename IFactory<InstType, AnnotationType>::IFactoryInfo::PtrType &info =
                ifact->getInfo(tinfo.getMnemonic(), tinfo.getOpcode(), extractor);

            inst = allocator(info->opinfo, info->uinfo, std::forward<ArgTypes>(args)...);
            icache_->allocate(tinfo.getOpcode(), inst);
        }

        return inst;
    }

    /**
     * makeInstDirectly -- use information from ExtractDirectInfo to generate an instruction
     * @param ex_info
     * @return
     */
    template<class InstTypeAllocator, typename ...ArgTypes>
    typename InstType::PtrType makeInstDirectly(const ExtractorDirectInfoIF &ex_info,
                                                InstTypeAllocator &allocator, ArgTypes &&... args)
    {
        typename InstType::PtrType inst = nullptr;

        std::string mnemonic = ex_info.getMnemonic();
        const InstructionUniqueID uid = ex_info.getUID();

        // Try to look up the factory by UID (if present)
        typename IFactory<InstType, AnnotationType>::PtrType ifact = nullptr;
        if (uid != mavis::INVALID_UID) {
            mnemonic = builder_->findInstructionMnemonic(uid);
            ifact = builder_->findIFact(uid);
        } else {
            // Look up the factory for the given mnemonic
            ifact = builder_->findIFact(mnemonic);
        }

        // Look up the factory for the given mnemonic
        if (ifact == nullptr) {
            throw UnknownMnemonic(mnemonic);
        } else {
            const typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType &info =
                ifact->getInfoBypassCache(mnemonic, ex_info.clone());
            inst = allocator(info->opinfo, info->uinfo, std::forward<ArgTypes>(args)...);
        }

        return inst;
    }

    /**
     * \brief Morph an existing instruction by providing new direct extraction info
     * \param inst
     * \param ex_info
     */
    void morphInst(typename InstType::PtrType inst, const ExtractorDirectInfoIF &ex_info) const
    {
        // Look up the factory for the given mnemonic
        typename IFactory<InstType, AnnotationType>::PtrType ifact = builder_->findIFact(ex_info.getMnemonic());
        if (ifact == nullptr) {
            throw UnknownMnemonic(ex_info.getMnemonic());
        } else {
            // We should not need to invalidate the instruction cache for this instruction,
            // since what we cache is a pristine version of the instruction generated from the
            // opcode (see makeInst() above).
            const typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType &info =
                ifact->getInfoBypassCache(ex_info.getMnemonic(), ex_info.clone());
            inst->morph(info->opinfo, info->uinfo);
        }
    }

    void flushCaches()
    {
        icache_.reset(new InstCache());
        ocache_.reset(new IFactoryCache());
        root_->flushCaches();
    }

    void print(std::ostream &os) const
    {
        root_->print(os);
    }

private:
    typename IFactoryIF<InstType, AnnotationType>::PtrType root_ = nullptr;
    typename IFactoryBuilder<InstType, AnnotationType, AnnotationTypeAllocator>::PtrType builder_;
    ExtractorRegistry extractors_;
    FormRegistry forms_;

    // Toplevel caches. Both are "tagged" by the opcode:
    // 1. icache_: Cache of instruction object prototypes (for makeInst)
    // 2. ocache_: Cache of IFactories (for getInfo)
    std::unique_ptr<InstCache> icache_;
    std::unique_ptr<IFactoryCache> ocache_;

    void parseInstInfo_(const std::string& jfile, const nlohmann::json& inst, const std::string& mnemonic,
                        const MatchSet<Tag>& tags);

    typename IFactoryIF<InstType, AnnotationType>::PtrType
    buildLeaf_(const FormBase *form, const typename IFactoryIF<InstType, AnnotationType>::PtrType &currNode,
                    const std::string &mnemonic, Opcode istencil, const FieldNameListType &flist,
                    const std::string &factory_name, const std::string &xpand_name,
                    ExtractorIF::PtrType override_extractor,
                    InstMetaData::PtrType &meta,
                    const typename IFactoryIF<InstType, AnnotationType>::PtrType &shared_ifact);

    typename IFactoryIF<InstType, AnnotationType>::PtrType
    build_(const FormBase *form, const std::string &mnemonic, Opcode istencil,
                const FieldNameListType &flist,
                const FieldNameSetType &ignore_set, const std::string &factory_name, const std::string &xpand_name,
                const ExtractorIF::PtrType &override_extractor, InstMetaData::PtrType &einfo,
                typename IFactoryIF<InstType, AnnotationType>::PtrType shared_ifact = nullptr);

    template<typename FormType>
    void buildSpecial_(const std::string &mnemonic, Opcode istencil, const FieldNameListType &flist,
                       const FieldNameSetType &ignore_set, const std::string &factory_name,
                       const std::string &xpand_name,
                       const ExtractorIF::PtrType &override_extractor, const InstMetaData::PtrType &einfo);

#if 0
    /**
     * populate
     * @tparam FormType
     * @param mnemonic
     * @param istencil The hard-coded bits of the instruction (opcode + func bits)
     */
    template<typename FormType>
    IFactoryIF<InstType, AnnotationType>::PtrType populateBranches_(const Opcode istencil, const FieldNameSetType& ignore_set);
#endif
};

    template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
inline std::ostream &operator<<(std::ostream &os, const DTable<InstType, AnnotationType, AnnotationTypeAllocator> &dt)
{
    dt.print(os);
    return os;
}

} // namespace mavis

#include "impl/DTable.tcc"
#include "impl/DTableBuildSpecial.tcc"
