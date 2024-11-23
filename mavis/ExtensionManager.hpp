#pragma once

#include <filesystem>
#include <list>
#include <string_view>
#include <unordered_set>

#include "DecoderExceptions.h"
#include "json.hpp"
#include "mavis/Mavis.h"

#include "elfio/elfio.hpp"

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
            explicit UnknownExtensionException(const uint32_t xlen, const std::string& ext) :
                ExtensionManagerException("Unknown extension for xlen=" + std::to_string(xlen) + ": " + ext)
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

    class InvalidBaseExtensionException : public ExtensionManagerException
    {
        private:
            static inline std::string genBaseExtensionString_(const std::unordered_set<std::string>& base_extensions)
            {
                std::ostringstream ss;
                bool first = true;
                for(const auto& ext: base_extensions)
                {
                    if(first)
                    {
                        first = false;
                    }
                    else
                    {
                        ss << ", ";
                    }

                    ss << ext;
                }

                return ss.str();
            }

        public:
            explicit InvalidBaseExtensionException(const std::string& ext, const std::unordered_set<std::string>& base_extensions) :
                ExtensionManagerException(ext + " is not a valid base extension. Valid extensions are: " + genBaseExtensionString_(base_extensions))
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

    class ELFNotFoundException : public ExtensionManagerException
    {
        public:
            explicit ELFNotFoundException(const std::string& elf) :
                ExtensionManagerException("Could not open ELF " + elf)
            {
            }
    };

    class ISANotFoundInELFException : public ExtensionManagerException
    {
        public:
            explicit ISANotFoundInELFException(const std::string& elf) :
                ExtensionManagerException("Could not find a RISCV ISA string in " + elf)
            {
            }
    };
}

class ExtensionManager
{
    private:
        static constexpr uint32_t DEFAULT_MAJOR_VER_ = 2;
        static constexpr uint32_t DEFAULT_MINOR_VER_ = 0;

        class ExtensionInfo;

        using ExtensionInfoPtr = std::shared_ptr<ExtensionInfo>;

        class ExtensionInfo
        {
            private:
                const std::string extension_;
                const std::string json_;
                uint32_t major_ver_ = DEFAULT_MAJOR_VER_;
                uint32_t minor_ver_ = DEFAULT_MINOR_VER_;
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

                explicit ExtensionInfo(const std::string& ext) :
                    extension_(ext)
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

                void setVersion(const uint32_t major_ver, const uint32_t minor_ver)
                {
                    major_ver_ = major_ver;
                    minor_ver_ = minor_ver;
                }

                uint32_t getMajorVersion() const { return major_ver_; }
                uint32_t getMinorVersion() const { return minor_ver_; }
                const std::string& getJSON() const { return json_; }
        };

    public:
        enum class UnknownExtensionAction
        {
            ERROR,
            WARN,
            IGNORE
        };

        class ExtensionBase
        {
            public:
                virtual ~ExtensionBase() = default;
                virtual const std::string& getName() const = 0;
                virtual const std::string& getJSON() const = 0;
                virtual uint32_t getMajorVersion() const = 0;
                virtual uint32_t getMinorVersion() const = 0;
        };

        class Extension : public ExtensionBase
        {
            private:
                const ExtensionInfoPtr ext_;

            public:
                explicit Extension(const ExtensionInfoPtr& ext) :
                    ext_(ext)
                {
                }

                const std::string& getName() const override final
                {
                    return ext_->getExtension();
                }

                const std::string& getJSON() const override final
                {
                    return ext_->getJSON();
                }

                uint32_t getMajorVersion() const override final
                {
                    return ext_->getMajorVersion();
                }

                uint32_t getMinorVersion() const override final
                {
                    return ext_->getMinorVersion();
                }
        };

        class MetaExtension : public ExtensionBase
        {
            private:
                const std::string ext_;

            public:
                explicit MetaExtension(const std::string& ext) :
                    ext_(ext)
                {
                }

                const std::string& getName() const override final
                {
                    return ext_;
                }

                const std::string& getJSON() const override final
                {
                    static const std::string EMPTY{};
                    return EMPTY;
                }

                uint32_t getMajorVersion() const override final
                {
                    return 0;
                }

                uint32_t getMinorVersion() const override final
                {
                    return 0;
                }
        };

        using ExtensionMap = std::unordered_map<std::string, std::unique_ptr<ExtensionBase>>;

    private:
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

                const uint32_t xlen_;
                const UnknownExtensionAction unknown_extension_action_;
                std::unordered_set<std::string> base_extensions_;
                std::unordered_map<std::string, ExtensionInfoPtr> extensions_;
                std::unordered_map<std::string, std::vector<std::string>> meta_extensions_;
                std::unordered_map<std::string, bool> config_extensions_;
                std::unordered_map<std::string, std::string> aliases_;
                std::list<UnresolvedDependency> pending_dependencies_;
                std::unordered_set<std::string> enabled_meta_extensions_;

                const ExtensionInfoPtr& getExtensionInfo_(const std::string& extension) const
                {
                    try
                    {
                        return extensions_.at(extension);
                    }
                    catch(const std::out_of_range&)
                    {
                        throw mavis::UnknownExtensionException(xlen_, extension);
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
                        pending_dependencies_.emplace_back(type, dependent_exts, ext);
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
                        pending_dependencies_.emplace_back(type, dependent_ext, ext);
                        return NOT_FOUND;
                    }

                    return dep_it->second;
                }

            public:
                XLENState(const uint32_t xlen, const UnknownExtensionAction unknown_extension_action) :
                    xlen_(xlen),
                    unknown_extension_action_(unknown_extension_action)
                {
                }

                const ExtensionInfo& getExtensionInfo(const std::string& extension) const
                {
                    return *getExtensionInfo_(extension);
                }

                ExtensionInfo& addExtension(const std::string& extension, const std::string& json = "")
                {
                    if(const auto it = extensions_.find(extension); it != extensions_.end())
                    {
                        throw mavis::DuplicateExtensionException(extension);
                    }

                    const auto result = extensions_.emplace(extension, std::make_shared<ExtensionInfo>(extension, json));

                    return *result.first->second;
                }

                void addConfigExtension(const std::string& ext)
                {
                    config_extensions_.emplace(ext, false);
                }

                void addBaseExtension(const std::string& ext)
                {
                    base_extensions_.emplace(ext);
                }

                void enableBaseExtension(const std::string& ext)
                {
                    if(base_extensions_.count(ext) == 0)
                    {
                        throw mavis::InvalidBaseExtensionException(ext, base_extensions_);
                    }

                    enableExtension(ext);
                }

                void enableBaseExtension(const char ext)
                {
                    enableBaseExtension(std::string(1, ext));
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

                void setExtensionVersion(const std::string& ext, const uint32_t major_ver, const uint32_t minor_ver)
                {
                    if(auto it = meta_extensions_.find(ext); it != meta_extensions_.end())
                    {
                        for(const auto& child: it->second)
                        {
                            setExtensionVersion(child, major_ver, minor_ver);
                        }
                    }
                    else if(auto alias_it = aliases_.find(ext); alias_it != aliases_.end())
                    {
                        setExtensionVersion(alias_it->second, major_ver, minor_ver);
                    }
                    else if(!config_extensions_.count(ext))
                    {
                        getExtensionInfo_(ext)->setVersion(major_ver, minor_ver);
                    }
                }

                void setExtensionVersion(const char ext, const uint32_t major_ver, const uint32_t minor_ver)
                {
                    setExtensionVersion(std::string(1, ext), major_ver, minor_ver);
                }

                void enableExtension(const std::string& ext)
                {
                    bool is_meta = true;
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
                    else if(auto config_it = config_extensions_.find(ext); config_it != config_extensions_.end())
                    {
                        config_it->second = true;
                    }
                    else
                    {
                        try
                        {
                            getExtensionInfo_(ext)->setEnabled();
                            is_meta = false;
                        }
                        catch(const mavis::UnknownExtensionException&)
                        {
                            switch(unknown_extension_action_)
                            {
                                case UnknownExtensionAction::ERROR:
                                    throw;
                                case UnknownExtensionAction::WARN:
                                    std::cerr << "WARNING: ISA string contains an unknown extension (" << ext << "). Ignoring.";
                                    break;
                                case UnknownExtensionAction::IGNORE:
                                    break;
                            }
                        }
                    }

                    if(is_meta)
                    {
                        enabled_meta_extensions_.emplace(ext);
                    }
                }

                void enableExtension(const char ext)
                {
                    enableExtension(std::string(1, ext));
                }

                void finalize(ExtensionMap& enabled_extensions)
                {
                    for(const auto& ext: extensions_)
                    {
                        ext.second->finalize();
                    }

                    for(const auto& ext: extensions_)
                    {
                        const auto& ext_info = ext.second;
                        if(ext_info->isEnabled())
                        {
                            enabled_extensions.emplace(ext_info->getExtension(), std::make_unique<Extension>(ext_info));
                        }
                    }

                    for(const auto& ext: enabled_meta_extensions_)
                    {
                        enabled_extensions.emplace(ext, std::make_unique<MetaExtension>(ext));
                    }
                }
        };

        const UnknownExtensionAction unknown_extension_action_;
        std::string isa_;
        uint32_t xlen_ = 0;
        using XLENMap = std::unordered_map<uint32_t, XLENState>;
        XLENMap extensions_;
        ExtensionMap enabled_extensions_;
        mutable std::vector<std::string> enabled_jsons_;

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

        template<typename ValType, typename OtherValType>
        static bool isOneOf_(const ValType val, const OtherValType other)
        {
            return (val == other);
        }

        template<typename ValType, typename OtherValType, typename ... OtherValTypes>
        static bool isOneOf_(const ValType val, const OtherValType other, const OtherValTypes... rest)
        {
            return (val == other) || isOneOf_(val, rest...);
        }

        static uint32_t digitToInt_(const char digit)
        {
            return digit - '0';
        }

        enum class ExtensionType
        {
            META,
            CONFIG,
            NORMAL
        };

        template<ExtensionType extension_type>
        void processExtension_(const nlohmann::json& ext_obj)
        {
            static constexpr bool is_normal_extension = extension_type == ExtensionType::NORMAL;
            static constexpr bool is_config_extension = extension_type == ExtensionType::CONFIG;

            const std::string ext = getRequiredJSONValue_<std::string>(ext_obj, "extension");

            std::vector<uint32_t> xlens;

            if(const auto it = ext_obj.find("xlen"); it != ext_obj.end())
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
                auto& xlen_extensions = extensions_.try_emplace(xlen, xlen, unknown_extension_action_).first->second;

                if constexpr(is_normal_extension)
                {
                    if(auto json_it = ext_obj.find("json"); json_it != ext_obj.end())
                    {
                        xlen_extensions.addExtension(ext, *json_it);
                    }
                    else
                    {
                        xlen_extensions.addExtension(ext);
                    }
                }
                else if constexpr(is_config_extension)
                {
                    xlen_extensions.addConfigExtension(ext);
                }

                if(getBoolJSONValue_(ext_obj, "is_base_extension"))
                {
                    if constexpr(is_config_extension)
                    {
                        throw mavis::MetaExtensionUnexpectedJSONKeyException("is_base_extension");
                    }
                    else
                    {
                        xlen_extensions.addBaseExtension(ext);
                    }
                }

                if(const auto it = ext_obj.find("meta_extension"); it != ext_obj.end())
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

                if(const auto it = ext_obj.find("aliases"); it != ext_obj.end())
                {
                    if constexpr(!is_normal_extension)
                    {
                        throw mavis::MetaExtensionUnexpectedJSONKeyException("aliases");
                    }

                    for(const auto& alias_ext: *it)
                    {
                        xlen_extensions.addAlias(ext, alias_ext);
                    }
                }

                if(const auto it = ext_obj.find("enables"); it != ext_obj.end())
                {
                    if constexpr(!is_normal_extension)
                    {
                        throw mavis::MetaExtensionUnexpectedJSONKeyException("enables");
                    }

                    for(const auto& enabled_ext: *it)
                    {
                        xlen_extensions.addEnablesExtension(ext, enabled_ext);
                    }
                }

                if(const auto it = ext_obj.find("enabled_by"); it != ext_obj.end())
                {
                    if constexpr(!is_normal_extension)
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

                if(const auto it = ext_obj.find("requires"); it != ext_obj.end())
                {
                    if constexpr(!is_normal_extension)
                    {
                        throw mavis::MetaExtensionUnexpectedJSONKeyException("requires");
                    }

                    for(const auto& required_ext: *it)
                    {
                        xlen_extensions.addRequiredExtension(ext, required_ext);
                    }
                }

                if(const auto it = ext_obj.find("conflicts"); it != ext_obj.end())
                {
                    if constexpr(!is_normal_extension)
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

        void processISASpecJSON_(const std::string& jfile)
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
                if(auto meta_extensions_it = jobj.find("meta_extensions"); meta_extensions_it != jobj.end())
                {
                    for(const auto& meta_ext_obj: *meta_extensions_it)
                    {
                        processExtension_<ExtensionType::META>(meta_ext_obj);
                    }
                }

                if(auto config_extensions_it = jobj.find("config_extensions"); config_extensions_it != jobj.end())
                {
                    for(const auto& config_ext_obj: *config_extensions_it)
                    {
                        processExtension_<ExtensionType::CONFIG>(config_ext_obj);
                    }
                }

                if(auto extensions_it = jobj.find("extensions"); extensions_it != jobj.end())
                {
                    for(const auto& ext_obj: *extensions_it)
                    {
                        processExtension_<ExtensionType::NORMAL>(ext_obj);
                    }
                }
            }
            catch(const mavis::ExtensionManagerException&)
            {
                std::cerr << "Error parsing file " << jfile << std::endl;
                throw;
            }

            for(auto& xlen_extension: extensions_)
            {
                xlen_extension.second.finalizeDependencies();
            }
        }

        template<typename InstType,
                 typename AnnotationType,
                 typename InstTypeAllocator,
                 typename AnnotationTypeAllocator,
                 typename ... MavisArgs>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator> constructMavis_(MavisArgs&&... mavis_args)
        {
            return Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>(getJSONs(), std::forward<MavisArgs>(mavis_args)...);
        }

        // Reads a ULEB128 value into the result bitset from the buffer at ptr
        // Returns a pointer to the first byte after the ULEB128 value
        // Based on the pseudocode at https://en.wikipedia.org/wiki/LEB128#Decode_unsigned_integer
        static const char* readULEB128_(std::bitset<128>& result, const char* ptr)
        {
            size_t shift = 0;

            result.reset();
            while (true)
            {
                const char cur_byte = *ptr;
                result |= std::bitset<128>(cur_byte & 0x7f) << shift;
                ++ptr;
                if ((cur_byte >> 7) == 0)
                {
                    break;
                }
                shift += 7;
            }

            return ptr;
        }

        static uint32_t readU32_(const char* ptr, const ELFIO::endianness_convertor& convertor)
        {
            uint32_t val;
            std::memcpy(&val, ptr, sizeof(uint32_t));
            return convertor(val);
        }

        static const char* findISAStringInELF_(const ELFIO::elfio& elf_reader)
        {
            static constexpr ELFIO::Elf_Word SHT_RISCV_ATTRIBUTES = 0x70000003;
            static constexpr std::string_view riscv_attributes_sec_name(".riscv.attributes");
            static constexpr std::string_view riscv_vendor("riscv");
            static constexpr size_t riscv_vendor_length = riscv_vendor.size() + 1;
            static constexpr uint32_t Tag_file = 1;
            static constexpr uint32_t Tag_RISCV_arch = 5;

            const ELFIO::section* riscv_attr_sec = elf_reader.sections[riscv_attributes_sec_name];

            if(riscv_attr_sec && riscv_attr_sec->get_type() == SHT_RISCV_ATTRIBUTES)
            {
                const char* data = riscv_attr_sec->get_data();
                const char* const end = data + riscv_attr_sec->get_size();

                // The .riscv.attributes section should start with an 'A' byte, followed by the first sub-section
                if(*data == 'A')
                {
                    const auto& endian_conv = elf_reader.get_convertor();

                    const char* sub_sec = data + 1;

                    std::bitset<128> tag;

                    while(sub_sec < end)
                    {
                        // Each sub-section has the following format:
                        // * length (uint32_t)
                        // * vendor name (null-terminated string)
                        // * sub-sub-sections
                        const uint32_t sub_section_length = readU32_(sub_sec, endian_conv);
                        const char* vendor = sub_sec + sizeof(uint32_t);

                        // The ISA string lives in the "riscv" sub-section
                        if(riscv_vendor.compare(vendor) == 0)
                        {
                            const char* sub_sub_sec = vendor + riscv_vendor_length;

                            while(sub_sub_sec < end)
                            {
                                // Each sub-sub-section has the following format:
                                // * tag (ULEB128)
                                // * length (uint32_t)
                                // * attribute-value pairs
                                // Each attribute-value pair consists of a ULEB128 tag and either:
                                // * ULEB128 integer (if attribute tag is even)
                                // * null-terminated string (if attribute tag is odd)
                                const char* ptr = readULEB128_(tag, sub_sub_sec);
                                const uint32_t sub_sub_sec_len = readU32_(ptr, endian_conv);

                                // The ISA string applies to the entire ELF, so its sub-sub-section should have tag == Tag_file
                                if(tag == Tag_file)
                                {
                                    const char* const sub_sub_sec_end = sub_sub_sec + sub_sub_sec_len;

                                    ptr += sizeof(uint32_t);

                                    // ptr now points to the first attribute-value pair
                                    while(ptr < sub_sub_sec_end)
                                    {
                                        ptr = readULEB128_(tag, ptr);
                                        if(tag == Tag_RISCV_arch) // This is the ISA string. It starts at ptr
                                        {
                                            return ptr;
                                        }
                                        else if(tag.test(0)) // Some other string value
                                        {
                                            ptr += strlen(ptr) + 1;
                                        }
                                        else // ULEB128 value
                                        {
                                            ptr = readULEB128_(tag, ptr);
                                        }
                                    }
                                }
                                // Jump to the next sub-sub-section
                                sub_sub_sec += sub_sub_sec_len;
                            }
                        }
                        sub_sec += sub_section_length;
                    }
                }
            }

            return nullptr;
        }

    public:
        explicit ExtensionManager(const std::string& spec_json, const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR) :
            unknown_extension_action_(unknown_extension_action)
        {
            processISASpecJSON_(spec_json);
        }

        ExtensionManager(const std::string& isa, const std::string& spec_json, const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR) :
            ExtensionManager(spec_json, unknown_extension_action)
        {
            setISA(isa);
        }

        static ExtensionManager fromELF(const std::string& elf, const std::string& spec_json, const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
        {
            ExtensionManager man(spec_json, unknown_extension_action);
            man.setISAFromELF(elf);
            return man;
        }

        void setISAFromELF(const std::string& elf)
        {
            ELFIO::elfio elf_reader;
            if(!elf_reader.load(elf))
            {
                throw mavis::ELFNotFoundException(elf);
            }

            const char* isa_str = findISAStringInELF_(elf_reader);

            if(!isa_str)
            {
                throw mavis::ISANotFoundInELFException(elf);
            }

            setISA(isa_str);
        }

        void setISA(const std::string& isa)
        {
            // ISA string must at least contain rv, the XLEN, and the base ISA,
            // e.g. rv32i
            static constexpr size_t MIN_LENGTH = 5;

            isa_ = toLowercase_(isa);

            std::string_view isa_view(isa_);
            if(isa_view.size() < MIN_LENGTH || isa_view.find("rv") != 0 || !isdigit(isa_view[2]))
            {
                throw mavis::InvalidISAStringException(isa_);
            }

            // Remove "rv" prefix
            isa_view.remove_prefix(2);

            XLENMap::iterator xlen_extension_it;

            try
            {
                size_t num_chars = 0;
                xlen_ = std::stoul(std::string(isa_view), &num_chars);
                // Remove XLEN prefix
                isa_view.remove_prefix(num_chars);
                xlen_extension_it = extensions_.find(xlen_);
                if(xlen_extension_it == extensions_.end())
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

            if(isa_view.empty())
            {
                throw mavis::InvalidISAStringException(isa_, "Missing base extension");
            }

            auto& xlen_extension = xlen_extension_it->second;
            const std::string base_isa(1, isa_view.front());

            xlen_extension.enableBaseExtension(base_isa);

            // Remove base ISA
            isa_view.remove_prefix(1);

            if(!isa_view.empty())
            {
                char ext;

                auto get_char_if_valid = [&isa_view](char& front_char)
                {
                    if(isa_view.empty())
                    {
                        front_char = '\0';
                        return false;
                    }

                    front_char = isa_view.front();

                    return true;
                };

                auto in_single_char_ext_range = [&get_char_if_valid](char& front_char)
                {
                    return get_char_if_valid(front_char) && !isOneOf_(front_char, 'z', 's', 'x');
                };

                if(get_char_if_valid(ext) && isdigit(ext))
                {
                    uint32_t major_ver = digitToInt_(ext);
                    uint32_t minor_ver = DEFAULT_MINOR_VER_;
                    isa_view.remove_prefix(1);
                    if(get_char_if_valid(ext) && ext == 'p')
                    {
                        isa_view.remove_prefix(1);
                        if(!get_char_if_valid(ext) || !isdigit(ext))
                        {
                            throw mavis::InvalidISAStringException(isa_, "Invalid version number specified for extension " + base_isa);
                        }

                        minor_ver = digitToInt_(ext);
                        isa_view.remove_prefix(1);
                    }
                    xlen_extension.setExtensionVersion(base_isa, major_ver, minor_ver);
                }

                while(in_single_char_ext_range(ext))
                {
                    if(ext == '_')
                    {
                        isa_view.remove_prefix(1);
                        continue;
                    }

                    const std::string ext_str(1, ext);
                    xlen_extension.enableExtension(ext_str);

                    isa_view.remove_prefix(1);

                    if(in_single_char_ext_range(ext) && isdigit(ext))
                    {
                        uint32_t major_ver = digitToInt_(ext);
                        uint32_t minor_ver = DEFAULT_MINOR_VER_;

                        isa_view.remove_prefix(1);
                        if(in_single_char_ext_range(ext) && ext == 'p')
                        {
                            isa_view.remove_prefix(1);

                            if(!(in_single_char_ext_range(ext) && isdigit(ext)))
                            {
                                throw mavis::InvalidISAStringException(isa_, "Invalid version number specified for extension " + ext_str);
                            }

                            minor_ver = digitToInt_(ext);

                            isa_view.remove_prefix(1);
                        }

                        // next_it now points to one-past the last part of the version number

                        xlen_extension.setExtensionVersion(ext_str, major_ver, minor_ver);
                    }
                }

                while(!isa_view.empty())
                {
                    if(isa_view[0] == '_')
                    {
                        isa_view.remove_prefix(1);
                        continue;
                    }

                    auto current_ext = isa_view.substr(0, isa_view.find('_'));
                    const size_t ext_length = current_ext.size();

                    bool has_version = false;
                    uint32_t major_ver = DEFAULT_MAJOR_VER_;
                    uint32_t minor_ver = DEFAULT_MINOR_VER_;

                    if(auto rit = current_ext.rbegin(); isdigit(*rit))
                    {
                        size_t num_to_remove = 1;

                        has_version = true;

                        major_ver = digitToInt_(*(rit++));

                        if(rit == current_ext.rend())
                        {
                            throw mavis::InvalidISAStringException(isa_, "Invalid extension " + std::string(current_ext));
                        }

                        if(*rit == 'p')
                        {
                            ++rit;
                            if(rit == current_ext.rend() || !isdigit(*rit))
                            {
                                throw mavis::InvalidISAStringException(isa_, "Invalid version number specified for extension " + std::string(current_ext));
                            }

                            minor_ver = major_ver;
                            major_ver = digitToInt_(*rit);
                            num_to_remove = 3;
                        }

                        current_ext.remove_suffix(num_to_remove);
                    }

                    const std::string ext_str(current_ext);
                    xlen_extension.enableExtension(ext_str);

                    if(has_version)
                    {
                        xlen_extension.setExtensionVersion(ext_str, major_ver, minor_ver);
                    }

                    isa_view.remove_prefix(ext_length);
                }
            }

            xlen_extension.finalize(enabled_extensions_);
        }

        bool isEnabled(const std::string& extension) const
        {
            return enabled_extensions_.count(extension) != 0;
        }

        uint32_t getXLEN() const { return xlen_; }

        const ExtensionMap& getEnabledExtensions() const
        {
            return enabled_extensions_;
        }

        const std::vector<std::string>& getJSONs() const
        {
            if(enabled_jsons_.empty())
            {
                for(const auto& ext: enabled_extensions_)
                {
                    const auto& json = ext.second->getJSON();
                    if(!json.empty())
                    {
                        enabled_jsons_.emplace_back(json);
                    }
                }
            }

            return enabled_jsons_;
        }

        template<typename InstType,
                 typename AnnotationType,
                 typename InstTypeAllocator       = mavis::SharedPtrAllocator<InstType>,
                 typename AnnotationTypeAllocator = mavis::SharedPtrAllocator<AnnotationType>>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis(const mavis::FileNameListType& anno_files,
                       const mavis::InstUIDList& uid_list,
                       const mavis::AnnotationOverrides& anno_overrides,
                       const mavis::MatchSet<mavis::Pattern>& inclusions,
                       const mavis::MatchSet<mavis::Pattern>& exclusions,
                       const InstTypeAllocator& inst_allocator = mavis::SharedPtrAllocator<InstType>(),
                       const AnnotationTypeAllocator& annotation_allocator = mavis::SharedPtrAllocator<AnnotationType>())
        {
            return constructMavis_<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>(
                anno_files,
                uid_list,
                anno_overrides,
                inclusions,
                exclusions,
                inst_allocator,
                annotation_allocator
            );
        }

        template<typename InstType,
                 typename AnnotationType,
                 typename InstTypeAllocator       = mavis::SharedPtrAllocator<InstType>,
                 typename AnnotationTypeAllocator = mavis::SharedPtrAllocator<AnnotationType>>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis(const mavis::FileNameListType& anno_files,
                       const mavis::InstUIDList& uid_list,
                       const mavis::AnnotationOverrides& anno_overrides = {},
                       const InstTypeAllocator& inst_allocator = mavis::SharedPtrAllocator<InstType>(),
                       const AnnotationTypeAllocator& annotation_allocator = mavis::SharedPtrAllocator<AnnotationType>())
        {
            return constructMavis_<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>(
                anno_files,
                uid_list,
                anno_overrides,
                inst_allocator,
                annotation_allocator
            );
        }

        template<typename InstType,
                 typename AnnotationType,
                 typename InstTypeAllocator       = mavis::SharedPtrAllocator<InstType>,
                 typename AnnotationTypeAllocator = mavis::SharedPtrAllocator<AnnotationType>>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis(const mavis::FileNameListType& anno_files,
                       const mavis::MatchSet<mavis::Pattern>& inclusions,
                       const mavis::MatchSet<mavis::Pattern>& exclusions,
                       const InstTypeAllocator& inst_allocator = mavis::SharedPtrAllocator<InstType>(),
                       const AnnotationTypeAllocator& annotation_allocator = mavis::SharedPtrAllocator<AnnotationType>())
        {
            return constructMavis_<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>(
                anno_files,
                inclusions,
                exclusions,
                inst_allocator,
                annotation_allocator
            );
        }

        template<typename InstType,
                 typename AnnotationType,
                 typename InstTypeAllocator       = mavis::SharedPtrAllocator<InstType>,
                 typename AnnotationTypeAllocator = mavis::SharedPtrAllocator<AnnotationType>>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis(const mavis::FileNameListType& anno_files,
                       const InstTypeAllocator& inst_allocator = mavis::SharedPtrAllocator<InstType>(),
                       const AnnotationTypeAllocator& annotation_allocator = mavis::SharedPtrAllocator<AnnotationType>())
        {
            return constructMavis_<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>(
                anno_files,
                inst_allocator,
                annotation_allocator
            );
        }

};
