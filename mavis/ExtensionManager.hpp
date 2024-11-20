#pragma once

#include <filesystem>
#include <list>
#include <unordered_set>

#include "DecoderExceptions.h"
#include "json.hpp"
#include "mavis/Mavis.h"

namespace mavis
{
    class ExtensionManagerException : public std::exception
    {
        private:
            const std::string msg_;

        public:
            explicit ExtensionManagerException(const std::string& msg) :
                msg_(msg)
            {
            }

            const char* what() const noexcept override
            {
                return msg_.c_str();
            }
    };

    class InvalidJSONDirectoryException : public ExtensionManagerException
    {
        public:
            explicit InvalidJSONDirectoryException(const std::string& path) :
                ExtensionManagerException(path + " does not contain Mavis JSONs")
            {
            }
    };

    class MissingRequiredJSONKeyException : public ExtensionManagerException
    {
        public:
            explicit MissingRequiredJSONKeyException(const std::string& key) :
                ExtensionManagerException("Required JSON key " + key + " is missing")
            {
            }
    };

    class MetaExtensionUnexpectedJSONKeyException : public ExtensionManagerException
    {
        public:
            explicit MetaExtensionUnexpectedJSONKeyException(const std::string& key) :
                ExtensionManagerException("Meta-extensions should not contain the " + key + " key")
            {
            }
    };

    class InvalidISAStringException : public ExtensionManagerException
    {
        public:
            explicit InvalidISAStringException(const std::string& isa) :
                ExtensionManagerException("Invalid ISA string specified: " + isa)
            {
            }

            InvalidISAStringException(const std::string& isa, const std::string& message) :
                ExtensionManagerException("Error processing isa string (" + isa + "): " + message)
            {
            }
    };

    class DuplicateExtensionException : public ExtensionManagerException
    {
        public:
            explicit DuplicateExtensionException(const std::string& ext) :
                ExtensionManagerException("Extension " + ext + " has a duplicate definition")
            {
            }
    };

    class UnknownExtensionException : public ExtensionManagerException
    {
        public:
            explicit UnknownExtensionException(const std::string& ext) :
                ExtensionManagerException("Unknown extension: " + ext)
            {
            }
    };

    class MissingRequiredExtensionException : public ExtensionManagerException
    {
        public:
            MissingRequiredExtensionException(const std::string& ext, const std::string& req_ext) :
                ExtensionManagerException(ext + " extension requires " + req_ext + " to also be present in the ISA string")
            {
            }
    };

    class ConflictingExtensionException : public ExtensionManagerException
    {
        public:
            ConflictingExtensionException(const std::string& ext, const std::string& conflict_ext) :
                ExtensionManagerException(ext + " extension conflicts with " + conflict_ext)
            {
            }
    };

    class CircularDependencyException : public ExtensionManagerException
    {
        public:
            CircularDependencyException() :
                ExtensionManagerException("Circular dependency detected!")
            {
            }
    };

    class SelfReferentialException : public ExtensionManagerException
    {
        public:
            explicit SelfReferentialException(const std::string& ext) :
                ExtensionManagerException("Extension " + ext + " has a dependency on itself")
            {
            }
    };

    class UnresolvedDependencyException : public ExtensionManagerException
    {
        public:
            explicit UnresolvedDependencyException(const std::string& ext) :
                ExtensionManagerException("Attempted to construct an invalid unresolved dependency for extension " + ext)
            {
            }
    };
}

class ExtensionManager
{
    public:
        class Extension
        {
            private:
                static constexpr uint32_t DEFAULT_MAJOR_VER_ = 2;
                static constexpr uint32_t DEFAULT_MINOR_VER_ = 0;
                const std::string ext_;
                const uint32_t major_ver_;
                const uint32_t minor_ver_;

            public:
                explicit Extension(const std::string& ext, const uint32_t major_ver = DEFAULT_MAJOR_VER_, const uint32_t minor_ver = DEFAULT_MINOR_VER_) :
                    ext_(ext),
                    major_ver_(major_ver),
                    minor_ver_(minor_ver)
                {
                }

                const std::string& getName() const
                {
                    return ext_;
                }
        };

    private:
        class ExtensionInfo;

        using ExtensionInfoPtr = std::shared_ptr<ExtensionInfo>;

        class ExtensionInfo
        {
            private:
                const std::string extension_;
                const std::string json_;
                bool enabled_ = false;
                bool force_enabled_ = false;
                std::vector<ExtensionInfoPtr> enables_extensions_;
                std::vector<std::vector<ExtensionInfoPtr>> enabling_extensions_;
                std::vector<ExtensionInfoPtr> required_extensions_;
                std::vector<ExtensionInfoPtr> conflicting_extensions_;

                static bool allEnabled_(const std::vector<ExtensionInfoPtr>& extensions)
                {
                    return std::all_of(extensions.begin(), extensions.end(), [](const ExtensionInfoPtr& ext){ return ext->isEnabled(); });
                }

                bool anyEnablingExtensionsEnabled_() const
                {
                    return enabling_extensions_.empty() || std::any_of(enabling_extensions_.begin(), enabling_extensions_.end(), [](const std::vector<ExtensionInfoPtr>& exts) { return allEnabled_(exts); });
                }

                template<bool assert_enabled>
                void validateConstraints_(const std::vector<ExtensionInfoPtr>& constraint_extensions) const
                {
                    std::vector<ExtensionInfoPtr>::const_iterator it;

                    using ExceptionType = std::conditional_t<assert_enabled, mavis::MissingRequiredExtensionException, mavis::ConflictingExtensionException>;

                    if constexpr(assert_enabled)
                    {
                        it = std::find_if_not(constraint_extensions.begin(),
                                              constraint_extensions.end(),
                                              [](const ExtensionInfoPtr& ext){ return ext->isEnabled(); });
                    }
                    else
                    {
                        it = std::find_if(constraint_extensions.begin(),
                                          constraint_extensions.end(),
                                          [](const ExtensionInfoPtr& ext){ return ext->isEnabled(); });
                    }

                    if(it != constraint_extensions.end())
                    {
                        throw ExceptionType(extension_, (*it)->getExtension());
                    }
                }

                void assertNotSelfReferential_(const ExtensionInfoPtr& ext) const
                {
                    if(ext.get() == this)
                    {
                        throw mavis::SelfReferentialException(extension_);
                    }
                }

                void assertNotSelfReferential_(const std::vector<ExtensionInfoPtr>& exts) const
                {
                    if(std::any_of(exts.begin(), exts.end(), [this](const ExtensionInfoPtr& ext) { return ext.get() == this; }))
                    {
                        throw mavis::SelfReferentialException(extension_);
                    }
                }

            public:
                ExtensionInfo(const std::string& ext, const std::string& json) :
                    extension_(ext),
                    json_(json)
                {
                }

                const std::string& getExtension() const
                {
                    return extension_;
                }

                void addEnablesExtension(const ExtensionInfoPtr& ext)
                {
                    assertNotSelfReferential_(ext);
                    enables_extensions_.emplace_back(ext);
                }

                void addEnablingExtensions(const std::vector<ExtensionInfoPtr>& exts)
                {
                    assertNotSelfReferential_(exts);
                    enabling_extensions_.emplace_back(exts);
                }

                void addRequiredExtension(const ExtensionInfoPtr& ext)
                {
                    assertNotSelfReferential_(ext);

                    required_extensions_.emplace_back(ext);
                }

                void addConflictingExtension(const ExtensionInfoPtr& ext)
                {
                    assertNotSelfReferential_(ext);

                    conflicting_extensions_.emplace_back(ext);
                }

                void forceEnabled()
                {
                    setEnabled();
                    force_enabled_ = true;
                }

                void setEnabled()
                {
                    enabled_ = true;
                    for(const auto& ext: enables_extensions_)
                    {
                        ext->forceEnabled();
                    }
                }

                bool isEnabled() const
                {
                    return force_enabled_ || (enabled_ && anyEnablingExtensionsEnabled_());
                }

                void finalize()
                {
                    if(!enabled_)
                    {
                        return;
                    }

                    validateConstraints_<true>(required_extensions_);
                    validateConstraints_<false>(conflicting_extensions_);

                    enabled_ = force_enabled_ || (enabled_ && anyEnablingExtensionsEnabled_());
                }
        };

        class XLENState
        {
            private:
                class UnresolvedDependency
                {
                    public:
                        enum class Type
                        {
                            ENABLES,
                            ENABLING,
                            REQUIRED,
                            CONFLICT
                        };

                    private:
                        const Type type_;
                        const ExtensionInfoPtr ext_;
                        std::unordered_set<std::string> dep_ext_;

                        UnresolvedDependency(const Type type, std::unordered_set<std::string>&& dep_ext, const ExtensionInfoPtr ext) :
                            type_(type),
                            ext_(ext),
                            dep_ext_(std::move(dep_ext))
                        {
                            if(type != Type::ENABLING && dep_ext.size() > 1)
                            {
                                throw mavis::UnresolvedDependencyException(ext->getExtension());
                            }
                        }

                    public:
                        UnresolvedDependency(const Type type, const std::vector<std::string>& dep_ext, const ExtensionInfoPtr ext) :
                            UnresolvedDependency(type, std::unordered_set<std::string>(dep_ext.begin(), dep_ext.end()), ext)
                        {
                        }

                        UnresolvedDependency(const Type type, const std::string& dep_ext, const ExtensionInfoPtr ext) :
                            UnresolvedDependency(type, std::unordered_set<std::string>{dep_ext}, ext)
                        {
                        }

                        Type getType() const
                        {
                            return type_;
                        }

                        const std::unordered_set<std::string>& getDependentExtensions() const
                        {
                            return dep_ext_;
                        }

                        void removeDependentExtension(const std::string& ext)
                        {
                            dep_ext_.erase(ext);
                        }

                        void addDependentExtension(const std::string& ext)
                        {
                            dep_ext_.emplace(ext);
                        }

                        void addDependentExtensions(const std::vector<std::string>& exts)
                        {
                            for(const auto& ext: exts)
                            {
                                addDependentExtension(ext);
                            }
                        }

                        const ExtensionInfoPtr& getExtension() const
                        {
                            return ext_;
                        }
                };

                std::unordered_set<std::string> base_extensions_;
                std::unordered_map<std::string, ExtensionInfoPtr> extensions_;
                std::unordered_map<std::string, std::vector<std::string>> meta_extensions_;
                std::unordered_map<std::string, std::string> aliases_;
                std::list<UnresolvedDependency> pending_dependencies_;

                const ExtensionInfoPtr& getExtensionInfo_(const std::string& extension) const
                {
                    try
                    {
                        return extensions_.at(extension);
                    }
                    catch(const std::out_of_range&)
                    {
                        throw mavis::UnknownExtensionException(extension);
                    }
                }

                template<typename ContainerType>
                std::vector<ExtensionInfoPtr> getExtensions_(const ContainerType& ext_strs)
                {
                    std::vector<ExtensionInfoPtr> exts;

                    for(const auto& ext: ext_strs)
                    {
                        const auto it = extensions_.find(ext);
                        if(it == extensions_.end())
                        {
                            break;
                        }
                        exts.emplace_back(it->second);
                    }

                    return exts;
                }

                std::vector<ExtensionInfoPtr> getDependenciesOrDefer_(const UnresolvedDependency::Type type, const std::vector<std::string>& dependent_exts, const ExtensionInfoPtr& ext)
                {
                    const std::vector<ExtensionInfoPtr> exts = getExtensions_(dependent_exts);

                    if(exts.size() != dependent_exts.size())
                    {
                        pending_dependencies_.emplace_back(UnresolvedDependency::Type::ENABLING, dependent_exts, ext);
                        return {};
                    }

                    return exts;
                }

                const ExtensionInfoPtr& getDependencyOrDefer_(const UnresolvedDependency::Type type, const std::string& dependent_ext, const ExtensionInfoPtr& ext)
                {
                    static const ExtensionInfoPtr NOT_FOUND{nullptr};

                    const auto dep_it = extensions_.find(dependent_ext);
                    if(dep_it == extensions_.end())
                    {
                        pending_dependencies_.emplace_back(UnresolvedDependency::Type::ENABLING, dependent_ext, ext);
                        return NOT_FOUND;
                    }

                    return dep_it->second;
                }

            public:
                const ExtensionInfo& getExtensionInfo(const std::string& extension) const
                {
                    return *getExtensionInfo_(extension);
                }

                ExtensionInfo& addExtension(const std::string& extension, const std::string& json)
                {
                    if(const auto it = extensions_.find(extension); it != extensions_.end())
                    {
                        throw mavis::DuplicateExtensionException(extension);
                    }

                    const auto result = extensions_.emplace(extension, std::make_shared<ExtensionInfo>(extension, json));

                    return *result.first->second;
                }

                void addBaseExtension(const std::string& ext)
                {
                    base_extensions_.emplace(ext);
                }

                void addEnablesExtension(const std::string& ext, const std::string& dependent_ext)
                {
                    const auto& ext_ptr = getExtensionInfo_(ext);
                    if(const auto& dep_ext = getDependencyOrDefer_(UnresolvedDependency::Type::ENABLES, dependent_ext, ext_ptr); dep_ext)
                    {
                        ext_ptr->addEnablesExtension(dep_ext);
                    }
                }

                void addEnablingExtensions(const std::string& ext, const std::vector<std::vector<std::string>>& dependent_exts)
                {
                    const auto& ext_ptr = getExtensionInfo_(ext);

                    for(const auto& dep_ext_bundle: dependent_exts)
                    {
                        if(const auto dep_exts = getDependenciesOrDefer_(UnresolvedDependency::Type::ENABLING, dep_ext_bundle, ext_ptr); !dep_exts.empty())
                        {
                            ext_ptr->addEnablingExtensions(dep_exts);
                        }
                    }
                }

                void addRequiredExtension(const std::string& ext, const std::string& dependent_ext)
                {
                    const auto& ext_ptr = getExtensionInfo_(ext);
                    if(const auto& dep_ext = getDependencyOrDefer_(UnresolvedDependency::Type::REQUIRED, dependent_ext, ext_ptr); dep_ext)
                    {
                        ext_ptr->addRequiredExtension(dep_ext);
                    }
                }

                void addConflictingExtension(const std::string& ext, const std::string& dependent_ext)
                {
                    const auto& ext_ptr = getExtensionInfo_(ext);
                    if(const auto& dep_ext = getDependencyOrDefer_(UnresolvedDependency::Type::CONFLICT, dependent_ext, ext_ptr); dep_ext)
                    {
                        ext_ptr->addConflictingExtension(dep_ext);
                    }
                }

                void addMetaExtensions(const std::string& extension, const std::vector<std::string>& meta_extensions)
                {
                    for(const auto& meta_extension: meta_extensions)
                    {
                        const auto result = meta_extensions_.try_emplace(meta_extension);
                        result.first->second.emplace_back(extension);
                    }
                }

                void addAlias(const std::string& extension, const std::string& alias)
                {
                    aliases_.emplace(alias, extension);
                }

                void finalizeDependencies()
                {
                    // Resolve any meta-extensions and aliases first
                    for(auto& dep_info: pending_dependencies_)
                    {
                        bool retry = false;

                        do
                        {
                            retry = false;

                            for(const auto& dep: dep_info.getDependentExtensions())
                            {
                                if(auto meta_it = meta_extensions_.find(dep); meta_it != meta_extensions_.end())
                                {
                                    dep_info.removeDependentExtension(dep);
                                    dep_info.addDependentExtensions(meta_it->second);
                                    retry = true;
                                    break;
                                }
                                else if(auto alias_it = aliases_.find(dep); alias_it != aliases_.end())
                                {
                                    dep_info.removeDependentExtension(dep);
                                    dep_info.addDependentExtension(alias_it->second);
                                    retry = true;
                                    break;
                                }
                            }
                        }
                        while(retry);    
                    }

                    while(!pending_dependencies_.empty())
                    {
                        auto it = pending_dependencies_.begin();

                        uint32_t num_resolved = 0;

                        while(it != pending_dependencies_.end())
                        {
                            const auto& ext = it->getExtension();
                            const auto& dep_exts = it->getDependentExtensions();

                            if(std::none_of(dep_exts.begin(), dep_exts.end(), [this](const std::string& dep_ext) { return extensions_.count(dep_ext) == 0; }))
                            {
                                switch(it->getType())
                                {
                                    case UnresolvedDependency::Type::ENABLES:
                                        ext->addEnablesExtension(getExtensionInfo_(*dep_exts.begin()));
                                        break;
                                    case UnresolvedDependency::Type::ENABLING:
                                        ext->addEnablingExtensions(getExtensions_(dep_exts));
                                        break;
                                    case UnresolvedDependency::Type::REQUIRED:
                                        ext->addRequiredExtension(getExtensionInfo_(*dep_exts.begin()));
                                        break;
                                    case UnresolvedDependency::Type::CONFLICT:
                                        ext->addConflictingExtension(getExtensionInfo_(*dep_exts.begin()));
                                        break;
                                }

                                it = pending_dependencies_.erase(it);
                                ++num_resolved;
                            }
                            else
                            {
                                ++it;
                            }
                        }

                        if(num_resolved == 0)
                        {
                            throw mavis::CircularDependencyException();
                        }
                    }
                }

                void enableExtension(const std::string& ext)
                {
                    if(auto it = meta_extensions_.find(ext); it != meta_extensions_.end())
                    {
                        for(const auto& child: it->second)
                        {
                            enableExtension(child);
                        }
                    }
                    else if(auto alias_it = aliases_.find(ext); alias_it != aliases_.end())
                    {
                        enableExtension(alias_it->second);
                    }
                    else
                    {
                        getExtensionInfo_(ext)->setEnabled();
                    }
                }
        };

        static constexpr std::array<uint32_t, 2> VALID_XLENS_{32, 64};

        const std::string isa_;
        uint32_t xlen_;
        std::unordered_map<uint32_t, XLENState> extensions_;

        std::unordered_map<std::string, Extension> enabled_extensions_;

        void enableExtension_(const Extension& ext)
        {
            enabled_extensions_.try_emplace(ext.getName(), ext);
        }

        template<typename ... ExtArgs>
        void enableExtension_(const std::string& ext, ExtArgs&&... ext_args)
        {
            enabled_extensions_.try_emplace(ext, Extension(ext, std::forward<ExtArgs>(ext_args)...));
        }

        template<typename ... ExtArgs>
        void enableExtension_(const char ext, ExtArgs&&... ext_args)
        {
            enableExtension_(std::string(1, ext), std::forward<ExtArgs>(ext_args)...);
        }

        static inline std::string toLowercase_(const std::string& str)
        {
            std::string lowercase;
            lowercase.reserve(str.size());
            std::transform(str.begin(), str.end(), std::back_inserter(lowercase), [](const char chr) { return std::tolower(chr); });
            return lowercase;
        }

        template<typename T>
        static T getRequiredJSONValue_(const nlohmann::json& jobj, const std::string& key)
        {
            try
            {
                return jobj.at(key);
            }
            catch(const nlohmann::json::exception&)
            {
                throw mavis::MissingRequiredJSONKeyException(key);
            }
        }

        static bool getBoolJSONValue_(const nlohmann::json& jobj, const std::string& key)
        {
            if(const auto it = jobj.find(key); it != jobj.end())
            {
                return *it;
            }

            return false;
        }

        void processExtensionJSON_(const std::string& jfile)
        {
            std::ifstream fs;

            try
            {
                fs.open(jfile);
            }
            catch (const std::ifstream::failure &ex)
            {
                throw mavis::BadISAFile(jfile);
            }

            nlohmann::json jobj;

            try
            {
                fs >> jobj;
            }
            catch(const nlohmann::json::parse_error&)
            {
                std::cerr << "Error parsing file " << jfile << std::endl;
                throw;
            }

            try
            {
                const std::string ext = getRequiredJSONValue_<std::string>(jobj, "extension");

                std::vector<uint32_t> xlens;

                if(const auto it = jobj.find("xlen"); it != jobj.end())
                {
                    if(it->is_array())
                    {
                        xlens = it->get<std::vector<uint32_t>>();
                    }
                    else
                    {
                        xlens.emplace_back(*it);
                    }
                }
                else
                {
                    throw mavis::MissingRequiredJSONKeyException("xlen");
                }

                for(const auto xlen: xlens)
                {
                    auto& xlen_extensions = extensions_.try_emplace(xlen).first->second;

                    const bool is_meta_extension = getBoolJSONValue_(jobj, "is_meta_extension");
                    const bool is_base_extension = getBoolJSONValue_(jobj, "is_base_extension");

                    if(!is_meta_extension)
                    {
                        xlen_extensions.addExtension(ext, jfile);
                    }

                    if(is_base_extension)
                    {
                        xlen_extensions.addBaseExtension(ext);
                    }

                    if(const auto it = jobj.find("meta_extension"); it != jobj.end())
                    {
                        std::vector<std::string> meta_extensions;

                        if(it->is_array())
                        {
                            meta_extensions = it->get<std::vector<std::string>>();
                        }
                        else
                        {
                            meta_extensions.emplace_back(*it);
                        }
                        xlen_extensions.addMetaExtensions(ext, meta_extensions);
                    }

                    if(const auto it = jobj.find("aliases"); it != jobj.end())
                    {
                        if(is_meta_extension)
                        {
                            throw mavis::MetaExtensionUnexpectedJSONKeyException("aliases");
                        }

                        for(const auto& alias_ext: *it)
                        {
                            xlen_extensions.addAlias(ext, alias_ext);
                        }
                    }

                    if(const auto it = jobj.find("enables"); it != jobj.end())
                    {
                        if(is_meta_extension)
                        {
                            throw mavis::MetaExtensionUnexpectedJSONKeyException("enables");
                        }

                        for(const auto& enabled_ext: *it)
                        {
                            xlen_extensions.addEnablesExtension(ext, enabled_ext);
                        }
                    }

                    if(const auto it = jobj.find("enabled_by"); it != jobj.end())
                    {
                        if(is_meta_extension)
                        {
                            throw mavis::MetaExtensionUnexpectedJSONKeyException("enabled_by");
                        }

                        std::vector<std::vector<std::string>> enabling_extensions;

                        if(it->front().is_array())
                        {
                            enabling_extensions = it->get<std::vector<std::vector<std::string>>>();
                        }
                        else
                        {
                            enabling_extensions.emplace_back(*it);
                        }

                        xlen_extensions.addEnablingExtensions(ext, enabling_extensions);
                    }

                    if(const auto it = jobj.find("requires"); it != jobj.end())
                    {
                        if(is_meta_extension)
                        {
                            throw mavis::MetaExtensionUnexpectedJSONKeyException("requires");
                        }

                        for(const auto& required_ext: *it)
                        {
                            xlen_extensions.addRequiredExtension(ext, required_ext);
                        }
                    }

                    if(const auto it = jobj.find("conflicts"); it != jobj.end())
                    {
                        if(is_meta_extension)
                        {
                            throw mavis::MetaExtensionUnexpectedJSONKeyException("conflicts");
                        }

                        for(const auto& conflict_ext: *it)
                        {
                            xlen_extensions.addConflictingExtension(ext, conflict_ext);
                        }
                    }
                }
            }
            catch(const mavis::ExtensionManagerException&)
            {
                std::cerr << "Error parsing file " << jfile << std::endl;
                throw;
            }
        }

    public:
        ExtensionManager(const std::string& isa, const std::string& json_dir) :
            isa_(toLowercase_(isa))
        {
            // ISA string must at least contain rv, the XLEN, and the base ISA,
            // e.g. rv32i
            static constexpr size_t MIN_LENGTH = 5;
            if(isa_.size() < MIN_LENGTH || isa_.find("rv") != 0 || !std::isdigit(isa_[2]))
            {
                throw mavis::InvalidISAStringException(isa_);
            }

            try
            {
                xlen_ = std::stoul(isa_.substr(2, 3));

                if(std::find(VALID_XLENS_.begin(), VALID_XLENS_.end(), xlen_) == VALID_XLENS_.end())
                {
                    throw std::out_of_range("");
                }
            }
            catch(const std::invalid_argument&)
            {
                throw mavis::InvalidISAStringException(isa_, "Could not determine xlen");
            }
            catch(const std::out_of_range&)
            {
                throw mavis::InvalidISAStringException(isa_, "Invalid xlen");
            }

            const std::filesystem::path json_dir_path(json_dir);
            if(!std::filesystem::is_directory(json_dir_path))
            {
                throw mavis::InvalidJSONDirectoryException(json_dir);
            }

            std::unordered_set<std::string> processed_files;

            for (const auto& dir_entry : std::filesystem::directory_iterator{json_dir_path}) 
            {
                if(dir_entry.is_regular_file())
                {
                    const auto jfile = std::filesystem::canonical(dir_entry.path());
                    if(processed_files.count(jfile) != 0)
                    {
                        continue;
                    }
                    processExtensionJSON_(jfile);
                    processed_files.emplace(jfile);
                }
            }

            for(auto& xlen_extension: extensions_)
            {
                xlen_extension.second.finalizeDependencies();
            }

            const char base_isa = isa[4];

            if(base_isa == 'g')
            {
                enableExtension_('i');
                enableExtension_('m');
                enableExtension_('a');
                enableExtension_('f');
                enableExtension_('d');
                enableExtension_("zicsr");
                enableExtension_("zifencei");
            }
            else if(base_isa == 'i' || base_isa == 'e')
            {
                enableExtension_(base_isa);
            }
            else
            {
                throw mavis::InvalidISAStringException(isa_, "Invalid base ISA");
            }


        }
};
