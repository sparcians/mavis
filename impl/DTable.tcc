#ifndef TCC_MAVIS_DTABLE
#define TCC_MAVIS_DTABLE

#include <mavis/Overlay.hpp>
#include "mavis/DTable.h" // Needed for CLION

namespace mavis {

/**
 * @brief parseInstInfo_
 * @tparam InstType
 * @tparam AnnotationType
 * @tparam AnnotationTypeAllocator
 * @param jfile
 * @param inst
 * @param mnemonic
 */
template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
void DTable<InstType, AnnotationType, AnnotationTypeAllocator>::parseInstInfo_(const std::string& jfile,
                                                                               const nlohmann::json& inst,
                                                                               const std::string& mnemonic,
                                                                               const MatchSet<Tag>& tags)
{
    // Convert the instruction stencil to binary
    Opcode istencil = 0;
    if (inst.find("stencil") != inst.end()) {
        istencil = stoll(static_cast<std::string>(inst["stencil"]), nullptr, 16);
    }

    // Parse the fixed fields list, if present
    FieldNameListType flist;
    if (inst.find("fixed") != inst.end()) {
        flist = inst["fixed"].get<FieldNameListType>();
    }

    // Parse the ignore fields list, if present
    FieldNameSetType ignore_set;
    if (inst.find("ignore") != inst.end()) {
        ignore_set = inst["ignore"].get<FieldNameSetType>();
    }

    // Parse the xform (extraction form) override, if present
    //const ExtractorIF::PtrType override_extractor = nullptr;
    ExtractorIF::PtrType override_extractor = nullptr;
    if (inst.find("xform") != inst.end()) {
        override_extractor = extractors_.getExtractor(inst["xform"]);
    }

    // Parse the factory name override, if present
    // Defaults to being the same as the mnemonic
    //std::string factory_name = inst["mnemonic"];
    std::string factory_name = mnemonic;
    if (inst.find("factory") != inst.end()) {
        factory_name = std::string(inst["factory"]);
    }

    // Parse the expansion factory, if present
    std::string xpand_name;
    if (inst.find("expand") != inst.end()) {
        xpand_name = std::string(inst["expand"]);
    }

    // Is this an instruction overlay?
    if (inst.find("overlay") != inst.end()) {
        typename Overlay<InstType, AnnotationType>::PtrType olay =
            std::make_shared<Overlay<InstType, AnnotationType>>(mnemonic, inst["overlay"], inst, override_extractor);
        builder_->buildOverlay(olay, jfile);
        // std::cout << *olay << std::endl;
        typename IFactory<InstType, AnnotationType>::PtrType ifact = builder_->findIFact(
            olay->getBaseMnemonic());
        if (ifact == nullptr) {
            throw BuildErrorOverlayBaseNotFound(olay->getMnemonic(), olay->getBaseMnemonic(), jfile);
        }
        ifact->addOverlay(olay);
    } else {
        const FormBase *form_wrap = forms_.findFormWrapper(static_cast<std::string>(inst["form"]));
        if (form_wrap == nullptr) {
            throw BuildErrorUnknownForm(jfile, mnemonic, inst["form"]);
        }

        InstMetaData::PtrType meta = builder_->makeInstMetaData(mnemonic, inst, !xpand_name.empty(), tags);
        try {
            typename IFactoryIF<InstType, AnnotationType>::PtrType ifact = build_(form_wrap, mnemonic, istencil,
                   flist, ignore_set, factory_name, xpand_name, override_extractor, meta);

            // Check for encoding aliases for this factory and add them to the tree
            StringListType alias_stencils;
            if (inst.find("alias") != inst.end()) {
                alias_stencils = inst["alias"].get<StringListType>();
                for (const auto& astencil : alias_stencils) {
                    Opcode opc = stoll(astencil, nullptr, 16);
                    build_(form_wrap, mnemonic, opc, flist, ignore_set, factory_name, xpand_name, override_extractor, meta, ifact);
                }
            }
        } catch (const BuildErrorInstructionAlias &ex) {
            std::cerr << ex.what() << std::endl;
        }
    }
}

/**
 * DTable::configure
 * @tparam InstType
 * @tparam AnnotationType
 * @tparam FileNamesPack
 * @param args
 */
template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
void DTable<InstType, AnnotationType, AnnotationTypeAllocator>::configure(const FileNameListType &isa_files,
                                                                          const MatchSet<Pattern>& inclusions,
                                                                          const MatchSet<Pattern>& exclusions)
{

    // Now populate the default factories from the provided JSON files...
    for (const auto &jfile : isa_files) {
        std::ifstream fs;
        std::ios_base::iostate exceptionMask = fs.exceptions() | std::ios::failbit;
        fs.exceptions(exceptionMask);

        try {
            fs.open(jfile);
        } catch (const std::ifstream::failure &ex) {
            throw BadISAFile(jfile);
        }

        nlohmann::json jobj;
        fs >> jobj;

        // Read in the instructions JSON file and process fields that pertain to decoding...
        for (const auto &inst : jobj) {
            std::string mnemonic;
            if (inst.find("mnemonic") != inst.end()) {
                mnemonic = std::string(inst["mnemonic"]);
                // We have an instruction... Look for filtering tag
                MatchSet<Tag>   tags;
                if (inst.find("tags") != inst.end()) {
                    tags = MatchSet<Tag>(inst["tags"].get<std::vector<std::string>>());
                }

                if ((inclusions.isEmpty() && exclusions.isEmpty()) || (inclusions.isEmpty() && tags.isEmpty())) {
                    // Inclusions & exclusions are empty, or inclusions empty and tags empty, no filtering active
                    parseInstInfo_(jfile, inst, mnemonic, tags);
                } else if (!tags.isEmpty()) {
                    bool included = inclusions.isEmpty() || tags.matchAnyAny(inclusions);
                    if (included) {
                        bool excluded = !exclusions.isEmpty() && tags.matchAnyAny(exclusions);
                        if (!excluded) {
                            parseInstInfo_(jfile, inst, mnemonic, tags);
                        }
                    }
                }
                // Otherwise, since tags.isEmpty(), we know inclusions are not empty and we reject the instruction
            } else if (inst.find("pseudo") != inst.end()) {
                // Skip any pseudo instruction records, those are handled
                // by the pseudo builder configuration
                continue;
            } else {
                // If there's a stencil clause, provide that as part of the exception
                // to help the poor user find where he's missing a mnemonic
                if (inst.find("stencil") != inst.end()) {
                    throw BuildErrorMissingMnemonic(jfile, inst["stencil"]);
                }
                throw BuildErrorMissingMnemonic(jfile);
            }

        }

        fs.close();
    }
    // At this point, we could throw away the builder_
}

/**
 * DTable::build leaf IFactory and Special Case nodes
 * @tparam FormType
 * @param mnemonic
 * @param istencil
 */
template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
typename IFactoryIF<InstType, AnnotationType>::PtrType
DTable<InstType, AnnotationType, AnnotationTypeAllocator>::buildLeaf_(const FormBase *form,
                                                                      const typename IFactoryIF<InstType, AnnotationType>::PtrType &currNode,
                                                                      const std::string &mnemonic, const Opcode istencil,
                                                                      const FieldNameListType &flist,
                                                                      const std::string &factory_name, const std::string &xpand_name,
                                                                      ExtractorIF::PtrType override_extractor,
                                                                      InstMetaData::PtrType &meta,
                                                                      const typename IFactoryIF<InstType, AnnotationType>::PtrType &shared_ifact)
{
    if (currNode == nullptr) {
        std::cout << root_ << std::endl;
        assert(currNode != nullptr);
    }

    // If we're given an expansion, use that mnemonic instead
    //if (!xpand_name.empty()) {
    //mnemonic = xpand_name;
    //}

    // If we're not given an extraction form override, use
    // the form specified by the FormType template parameter
    if (override_extractor == nullptr) {
        override_extractor = extractors_.getExtractor(form->getName());
    }

    // IFactorySpecialCaseComposite<InstType, AnnotationType> *parent =  // just for now...
    // dynamic_cast<IFactorySpecialCaseComposite<InstType, AnnotationType> *>(currNode);
    std::shared_ptr<IFactorySpecialCaseComposite<InstType, AnnotationType>> parent =  // just for now...
        std::dynamic_pointer_cast<IFactorySpecialCaseComposite<InstType, AnnotationType>>(currNode);

    // If parent is NULL, then we're not at an IFactorySpecialCaseComposite node
    // This happens when we're trying to build an instruction whose encoding is
    // the same as a prior instruction, but the Forms of the two instructions are incompatible
    if (parent == nullptr) {
        throw BuildErrorOpcodeConflict(mnemonic, istencil);
    }
    if (!flist.empty()) {
        parent->addSpecialCase(form, mnemonic, istencil, flist);
        meta->addFixedFields(flist);
    }

    // Check for the presence of an aliased (shared) ifactory
    typename IFactoryIF<InstType, AnnotationType>::PtrType ifact;
    if (shared_ifact == nullptr) {
        // Normal case: no alias, build a new ifactory
        ifact = builder_->build(mnemonic, factory_name, xpand_name, istencil, meta);
    } else {
        // Alias case: use the given (shared) ifactory
        ifact = shared_ifact;
    }

    // At LEAF...
    if (parent->getNode(istencil) == nullptr) {
        //currNode->addIFactory(istencil, new IFactory<Form_R>(mnemonic));
        // IFactoryIF *ifact = builder_.build<FormType>(factory_name, istencil);
        parent->addIFactory(mnemonic, istencil, ifact, override_extractor);
        // Show the newly added LEAF factory
        // std::cout << "LEAF: " << parent->getNode(stencil)->getName() << std::endl;
    } else if (parent->getDefault() == nullptr) {
        // Here, there's already a special case entry, but not a default. This can happen
        // when the more specific encodings are processed before the more general encodings
        // in the JSON file.
        parent->addDefaultIFactory(mnemonic, istencil, ifact, override_extractor);
    } else {
        throw BuildErrorInstructionAlias(istencil, mnemonic, parent->getNode(istencil)->getName());
    }

    return ifact;
}

/**
 * DTable::build (general case: branch nodes are dense composites)
 * @tparam FormType
 * @param mnemonic
 * @param istencil
 */
template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
typename IFactoryIF<InstType, AnnotationType>::PtrType
DTable<InstType, AnnotationType, AnnotationTypeAllocator>::build_(const FormBase *form, const std::string &mnemonic,
                                                                       const Opcode istencil,
                                                                       const FieldNameListType &flist,
                                                                       const FieldNameSetType &ignore_set, const std::string &factory_name,
                                                                       const std::string &xpand_name,
                                                                       const ExtractorIF::PtrType &override_extractor,
                                                                       InstMetaData::PtrType &einfo,
                                                                       const typename IFactoryIF<InstType, AnnotationType>::PtrType shared_ifact)
{
    assert(form != nullptr);

    typename IFactoryIF<InstType, AnnotationType>::PtrType currNode = root_;

    const FieldsType &fields = form->getOpcodeFields();
    const uint32_t n_fields = fields.size();
    assert(n_fields > 0);

    // At ROOT node...
    if (currNode->getNode(istencil) == nullptr) {
        currNode->addIFactory(istencil,
                              typename IFactoryIF<InstType, AnnotationType>::PtrType(
                                  new IFactoryDenseComposite<InstType, AnnotationType>(fields[0])));
    }
    currNode = currNode->getNode(istencil); // Advance...
    assert(currNode->getField() != nullptr);
    if (!currNode->getField()->isEquivalent(fields[0])) {
        throw BuildErrorFieldsIncompatible(mnemonic, *currNode->getField(), fields[0]);
    }

    // At branch nodes...
    const uint32_t last_field = n_fields - 1;
    for (uint32_t i = 0; i < last_field; ++i) {
        // Check whether to ignore this field... If so, we add the child as this
        // node's default, otherwise, we add the child to the node's TRIE table
        if (ignore_set.find(fields[i].getName()) != ignore_set.end()) {
            if (currNode->getDefault() == nullptr) {
                currNode->addDefaultIFactory(
                    typename IFactoryIF<InstType, AnnotationType>::PtrType(
                        new IFactoryDenseComposite<InstType, AnnotationType>(fields[i + 1])));
            }
            currNode = currNode->getDefault(); // Advance...
        } else {
            if (currNode->getNode(istencil) == nullptr) {
                currNode->addIFactory(istencil, typename IFactoryIF<InstType, AnnotationType>::PtrType(
                    new IFactoryDenseComposite<InstType, AnnotationType>(fields[i + 1])));
            }
            currNode = currNode->getNode(istencil); // Advance...
        }
        assert(currNode->getField() != nullptr);
        if (!currNode->getField()->isEquivalent(fields[i + 1])) {
            std::cerr << "ERROR with field collision on tree:" << std::endl;
            currNode->print(std::cerr);
            throw BuildErrorFieldsIncompatible(mnemonic, *currNode->getField(), fields[i + 1]);
        }
    }

    // Add the special case exclusions node to the last field's node
    if (ignore_set.find(fields[last_field].getName()) != ignore_set.end()) {
        if (currNode->getDefault() == nullptr) {
            currNode->addDefaultIFactory(
                typename IFactoryIF<InstType, AnnotationType>::PtrType(
                    new IFactorySpecialCaseComposite<InstType, AnnotationType>()));
        }
        currNode = currNode->getDefault(); // Advance...
    } else {
        if (currNode->getNode(istencil) == nullptr) {
            currNode->addIFactory(istencil,
                                  typename IFactoryIF<InstType, AnnotationType>::PtrType(
                                      new IFactorySpecialCaseComposite<InstType, AnnotationType>()));
        }
        currNode = currNode->getNode(istencil); // Advance...
    }

    return buildLeaf_(form, currNode, mnemonic, istencil, flist, factory_name, xpand_name, override_extractor, einfo, shared_ifact);
}

} // namespace mavis

#endif // TCC_MAVIS_DTABLE
