#pragma once

#include <map>
#include <fstream>
#include "BuilderBase.hpp"
#include "Extractor.h"
#include "IFactory.h"
#include "IFactoryCustom.h"
#include "Overlay.hpp"

namespace mavis
{

    template <typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
    class IFactoryBuilder :
        public FactoryBuilderBase<IFactory<InstType, AnnotationType>, InstType, AnnotationType,
                                  AnnotationTypeAllocator>
    {
      public:
        typedef std::shared_ptr<IFactoryBuilder<InstType, AnnotationType, AnnotationTypeAllocator>>
            PtrType;

      private:
        typedef IFactory<InstType, AnnotationType> FactoryType;

        // We're inheriting from a templated base class, so we need to tell the compiler how to find
        // base class members we use here in the derived class. NOTE: We have to declare the "using"
        // reference here, outside the methods in order for it work.
        //
        // Another way to do help the compiler find base class members is to explicitly de-reference
        // them with "this->". I've used that method for other members in the code below. I've opted
        // to declare registry_ with "using" here instead, since registry_ is an std::map() and I'm
        // doing index operations on it. The syntax is cleaner for indexing registry_ this way.
        using FactoryBuilderBase<FactoryType, InstType, AnnotationType,
                                 AnnotationTypeAllocator>::registry_;

      public:
        explicit IFactoryBuilder(const FileNameListType & anno_files,
                                 AnnotationTypeAllocator & annotation_allocator,
                                 const InstUIDList & uid_list = {},
                                 const AnnotationOverrides & anno_overrides = {}) :
            FactoryBuilderBase<FactoryType, InstType, AnnotationType, AnnotationTypeAllocator>(
                anno_files, annotation_allocator, uid_list, anno_overrides)
        {
            // Pre-populate the registry_ with custom instruction factories...

            // TODO: Need a real extraction info object
            InstMetaData::PtrType einfo_nop(new InstMetaData(InstMetaData::ISA::RV64I));

            // TODO: Clean this up. Should we move the UID registration inside the custom IFactory
            // constructor? I didn't do this here due to the need to make InstructionRegistry public
            // and to forward-declare the registry inside of the custom IFactory constructor
            //
            // Allow annotations for custom factories to be optional (suppress the exception on
            // anno_registry_.findAnnotation())
            registry_["nop"].reset(new IFactory_NOP<InstType, AnnotationType>(
                einfo_nop, this->findAnnotation("nop", true)));
            const InstructionUniqueID uid_nop = this->registerInst("nop");
            registry_["nop"]->addInstructionVariantUID("nop", uid_nop);

            // TODO: Need a real extraction info object
            InstMetaData::PtrType einfo_cmov(new InstMetaData(InstMetaData::ISA::RV64I));

            // TODO: Clean this up. Should we move the UID registration inside the custom IFactory
            // constructor? I didn't do this here due to the need to make InstructionRegistry public
            // and to forward-declare the registry inside of the custom IFactory constructor
            //
            // Allow annotations for custom factories to be optional (suppress the exception on
            // anno_registry_.findAnnotation())
            registry_["cmov"].reset(new IFactory_CMOV<InstType, AnnotationType>(
                einfo_cmov, this->findAnnotation("cmov", true)));
            const InstructionUniqueID uid_cmov = this->registerInst("cmov");
            registry_["cmov"]->addInstructionVariantUID("cmov", uid_cmov);
        }

        /**
         * \brief Build an IFactory for the given instruction
         * \tparam ArgTypes
         * \param mnemonic
         * \param factory_name
         * \param xpand_name
         * \param stencil
         * \param meta
         * \param args
         * \return
         *
         * This method will build a new IFactory for the given instruction, or if the factory
         * already exists, will return that IFactory (possibly augmented with the new information)
         *
         * There are a couple of cases where the factory may already exist:
         *
         * 1. If the JSON for the instruction lists a specific factory to use ("factory" clause)
         * 2. If the JSON for the instruction indicates the instruction is compressed and needs to
         * use an existing factory for expansion ("expand" clause)
         */
        template <typename... ArgTypes>
        typename IFactoryIF<InstType, AnnotationType>::PtrType
        build(const std::string & mnemonic, const std::string & factory_name,
              const std::string & xpand_name, const uint64_t stencil,
              const InstMetaData::PtrType & meta, ArgTypes &&... args)
        {
            typename IFactory<InstType, AnnotationType>::PtrType ifact;

            // Is this a "normal" instruction? (non-expanded, so xpand_name is empty)
            if (xpand_name.empty())
            {
                // Try to find a matching annotation... first try matching the factory's name
                // typename AnnotationType::PtrType panno =
                // anno_registry_.findAnnotation(factory_name); We suppress exceptions if we intend
                // to try lookup by the mnemonic on failure
                typename AnnotationType::PtrType panno =
                    this->findAnnotation(factory_name, mnemonic != factory_name);

                if (panno == nullptr)
                {
                    // Try to find a matching annotation for the instruction's mnemonic...
                    // panno = anno_registry_.findAnnotation(mnemonic);
                    // NOTE: panno may still be NULL here. findAnnotation() will have thrown an
                    // exception if there were annotation files provided
                    panno = this->findAnnotation(mnemonic);
                }

                // Try to find the named IFactory
                // ifact = FactoryBuilderBase<FactoryType, InstType, AnnotationType,
                // AnnotationTypeAllocator>::findIFact(factory_name);
                ifact = this->findIFact(factory_name);
                if (ifact == nullptr)
                {
                    // We need to build a new factory...
                    ifact.reset(
                        new FactoryType(factory_name, stencil, meta, std::forward(args)...));
                    registry_[factory_name] = ifact;
                }
                // If the mnemonic and factory name are not the same, we set up an alias from
                // the mnemonic to the factory
                if (mnemonic != factory_name)
                {
                    registry_[mnemonic] = ifact;
                }

                // This factory may cover several variants (e.g. csr's which use the JSON "factory"
                // clause). Add this instruction's annotation to the factory
                ifact->addInstructionVariantAnnotation(mnemonic, panno);
                const InstructionUniqueID inst_uid = this->registerInst(mnemonic);
                ifact->addInstructionVariantUID(mnemonic, inst_uid);
            }
            else
            {
                // This is a compressed instruction (names the factory via the "expand" clause)

                // If we have both a factory and xpand clause, the xpand clause will be used
                if (mnemonic != factory_name)
                {
                    // TODO: Make this into an exception?
                    std::ios_base::fmtflags cerr_state(std::cerr.flags());
                    std::cerr << "*** '" << mnemonic << "': "
                              << " Expansion factory '" << xpand_name << "' "
                              << " overrides named factory '" << factory_name << "'" << std::endl;
                    std::cerr.flags(cerr_state);
                }

                // We require that the factory named by the JSON "expand" clause already exists
                ifact = this->findIFact(xpand_name);
                if (ifact == nullptr)
                {
                    throw BuildErrorUnknownExpansion(mnemonic, xpand_name);
                }

                // Here, we register the instruction with the expansion's UID
                const InstructionUniqueID xpand_uid = this->findInstructionUID(xpand_name);
                this->inst_registry_.aliasInstWithUID(mnemonic, xpand_uid);
                ifact->addInstructionVariantUID(mnemonic, xpand_uid);

                // For compressed instructions, we want to merge its meta information with the
                // expansion factory's meta information
                ifact->mergeInstructionVariantMetaData(mnemonic, meta);
            }
            return ifact;
        }

        /**
         * @brief buildOverlay: populate an overlay object with builder information for IFactory
         * convenience
         * @param olay
         */
        void buildOverlay(typename Overlay<InstType, AnnotationType>::PtrType & olay,
                          const std::string & jfile)
        {
            const std::string olay_mnemonic = olay->getMnemonic();
            const std::string olay_base_mnemonic = olay->getBaseMnemonic();
            const InstMetaData::PtrType base_meta = this->findMetaData(olay->getBaseMnemonic());

            if (base_meta == nullptr)
            {
                throw BuildErrorOverlayBaseNotFound(olay_mnemonic, olay_base_mnemonic, jfile);
            }

            olay->setBaseMetaData(base_meta);
            olay->setUID(this->registerInst(olay_mnemonic));

            // Attempt to find the annotation for the overlay.
            // If not found, we use the annotation for the base
            typename AnnotationType::PtrType panno = this->findAnnotation(olay_mnemonic);
            if (panno == nullptr)
            {
                panno = this->findAnnotation(olay_base_mnemonic);
            }
            if ((panno == nullptr) && this->anno_registry_.isPopulated())
            {
                throw BuildErrorOverlayMissingAnnotation(olay_mnemonic, olay_base_mnemonic);
            }
            olay->setAnnotation(panno);
        }
    };

} // namespace mavis
