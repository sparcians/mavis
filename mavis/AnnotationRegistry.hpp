#pragma once

#include <map>
#include <set>
#include <fstream>
#include <boost/json.hpp>

#include "DecoderTypes.h"
#include "Extractor.h"
#include "DecoderExceptions.h"

namespace mavis {

template<typename AnnotationType, typename AnnotationTypeAllocator>
class AnnotationRegistry
{
public:
    typedef std::shared_ptr<AnnotationRegistry> PtrType;

    // trim from both ends (in place)
    inline std::string string_ws_trim(std::string s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                                                                return !std::isspace(ch);
                                                            }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                                                       return !std::isspace(ch);
                                                   }).base(), s.end());
        return s;
    }

    /**
     * @brief Annotations registry
     * @param anno_files
     */
    explicit AnnotationRegistry(const FileNameListType &anno_files,
                                AnnotationTypeAllocator & annotation_allocator,
                                const AnnotationOverrides & anno_overrides):
        anno_file_list_(anno_files)
    {

        // Now populate the uarch info registry from the provided JSON files...
        for (const auto &afile : anno_file_list_) {
            if (!afile.empty()) {
                // Process and store the uarch information...
                std::ifstream fs;

                try {
                    fs.open(afile);
                } catch (const std::ifstream::failure &ex) {
                    throw BadAnnotationFile(afile);
                }

                boost::system::error_code ec;
                boost::json::value json;
                try {
                    json = boost::json::parse(fs, ec);
                }
                catch(std::exception & ex) {
                    std::cerr << __FUNCTION__ << ": ERROR parsing: '" << afile << "' " << ex.what() << std::endl;
                    throw;
                }

                if(json.is_null() || ec) {
                    std::cerr << __FUNCTION__ << ": ERROR parsing: '" << afile << "' " << ec.what() << std::endl;
                    throw boost::system::system_error(ec);
                }

                auto& jobj = json.as_array();

                // Attempt to apply the annotation to the given jobj
                std::map<std::string, boost::json::object> jobj_annotations;
                for (auto ann : anno_overrides) {
                    const std::string mnemonic  = string_ws_trim(ann.first);
                    const std::string attribute = string_ws_trim(ann.second);
                    if(attribute.find(':') == std::string::npos) {
                        std::cerr << __FUNCTION__ << ": ERROR: Bad attribnute in annotation override: " << attribute
                                  << " \n expected name:value" << std::endl;
                        throw;
                    }
                    const std::string attr_name = string_ws_trim(attribute.substr(0, attribute.find(':')));
                    const std::string attr_value = string_ws_trim(attribute.substr(attribute.find(':') + 1));
                    if(attr_name.empty() or attr_value.empty()) {
                        std::cerr << __FUNCTION__ << ": ERROR: Bad attribnute in annotation override: " << attribute
                                  << " \n expected name:value" << std::endl;
                        throw;
                    }
                    jobj_annotations[mnemonic][attr_name] = boost::json::parse(attr_value);
                }

                // This is a little set of mnemonics registered within the current afile, so
                // we can detect if there are duplicate entries within the same file. We allow
                // duplicates across files...
                std::set<std::string> processed;

                // I'm assuming the json objects are large, so I'll process the information
                // into the internal form, and throw the json object away
                for (auto &inst_value : jobj)
                {
                    auto& inst = inst_value.as_object();
                    const std::string mnemonic = boost::json::value_to<std::string>(inst["mnemonic"]);
                    if(const auto it = jobj_annotations.find(mnemonic); it != jobj_annotations.end()) {
                        for(const auto& item: it->second) {
                            inst.insert_or_assign(item.key(), item.value());
                        }
                    }
                    const typename AnnotationType::PtrType &anno = privateFindAnnotation_(mnemonic);
                    if (anno == not_found_) {
                        typename AnnotationType::PtrType new_anno = annotation_allocator(inst);
                        registry_[mnemonic] = new_anno;
                    } else if (processed.find(mnemonic) == processed.end()) {
                        anno->update(inst);
                    } else {
                        throw AnnotationNotUniqueInFile(mnemonic, afile);
                    }
                    processed.insert(mnemonic);
                }

                fs.close();
            }
        }
    }

    const typename AnnotationType::PtrType &findAnnotation(const std::string &mnemonic,
        bool suppress_exception = false) const
    {
        const typename AnnotationType::PtrType& anno = privateFindAnnotation_(mnemonic);
        if (anno == not_found_) {
            if (anno_file_list_.empty() || suppress_exception) {
                return not_found_;
            }
            return not_found_;
            //throw BuildErrorMissingAnnotation(mnemonic);
        } else {
            return anno;
        }
    }

    bool isVacant() const
    {
        return registry_.empty();
    }

    bool isPopulated() const
    {
        return !isVacant();
    }

private:
    const FileNameListType anno_file_list_;
    std::map<std::string, typename AnnotationType::PtrType> registry_;
    const typename AnnotationType::PtrType not_found_;

    const typename AnnotationType::PtrType &privateFindAnnotation_(const std::string &mnemonic) const
    {
        const auto elem = registry_.find(mnemonic);
        if (elem == registry_.end()) {
            return not_found_;
        } else {
            return elem->second;
        }
    }
};

} // namespace mavis
