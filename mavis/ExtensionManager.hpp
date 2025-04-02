#pragma once

#include <filesystem>
#include <list>
#include <string_view>
#include <unordered_set>

#include "DecoderExceptions.h"
#include "json.hpp"
#include "mavis/Mavis.h"

namespace mavis::extension_manager
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

    class UninitializedISASpecException : public ExtensionManagerException
    {
        public:
            UninitializedISASpecException() :
                ExtensionManagerException("Attempted to modify extension manager state without calling setISASpecJSON first")
            {
            }
    };

    class UninitializedISAException : public ExtensionManagerException
    {
        public:
            UninitializedISAException() :
                ExtensionManagerException("Attempted to modify extension manager state without calling setISA first")
            {
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

    class InvalidJSONValueException : public ExtensionManagerException
    {
        public:
            explicit InvalidJSONValueException(const std::string& key) :
                ExtensionManagerException("Invalid value for JSON key " + key)
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

    class UnknownExtensionExceptionBase : public ExtensionManagerException
    {
        public:
            explicit UnknownExtensionExceptionBase(const std::string& msg) :
                ExtensionManagerException(msg)
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

    class ExtensionNotAllowedException : public ExtensionManagerException
    {
        private:
            static inline std::string setToString_(const std::unordered_set<std::string>& set)
            {
                std::string result("[");
                bool first = true;
                for(const auto& str: set)
                {
                    if(first)
                    {
                        first = false;
                    }
                    else
                    {
                        result += ',';
                    }

                    result += str;
                }

                result += "]";

                return result;
            }

        public:
            ExtensionNotAllowedException(const std::string& ext, const std::unordered_set<std::string>& allowlist, const std::unordered_set<std::string>& blocklist) :
                ExtensionManagerException(
                    "Attempted to enable " + ext + " extension, but it is not allowed."
                    " allowlist = " + setToString_(allowlist) +
                    " blocklist = " + setToString_(blocklist)
                )
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

    enum class DependencyType
    {
        META,
        ALIAS,
        ENABLES,
        ENABLING,
        REQUIRED,
        CONFLICTING
    };

    template<DependencyType dep_type>
    struct DependencyTraits
    {
        using value_type = std::string;
    };

    template<>
    struct DependencyTraits<DependencyType::ENABLING>
    {
        using value_type = std::vector<std::string>;
    };

    template<typename DataT>
    struct JSONVectorConverter
    {
        static std::vector<DataT> get(const nlohmann::json& obj)
        {
            if(obj.is_array())
            {
                return obj.get<std::vector<DataT>>();
            }

            return std::vector<DataT>(1, obj.get<DataT>());
        }
    };

    template<typename DataT>
    struct JSONVectorConverter<std::vector<DataT>>
    {
        static std::vector<std::vector<DataT>> get(const nlohmann::json& obj)
        {
            std::vector<std::vector<DataT>> result;

            if(obj.is_array())
            {
                for(const auto& elem: obj)
                {
                    result.emplace_back(JSONVectorConverter<DataT>::get(elem));
                }
            }
            else
            {
                result.emplace_back(std::vector<DataT>(1, obj.get<DataT>()));
            }

            return result;
        }
    };

    class ExtensionInfoBase;
    using ExtensionInfoBasePtr = std::shared_ptr<ExtensionInfoBase>;

    class ExtensionInfoBase
    {
        private:
            const std::string extension_;
            const std::string json_;
            bool enabled_ = false;
            bool force_enabled_ = false;
            std::vector<ExtensionInfoBasePtr> enables_extensions_;
            std::vector<std::vector<ExtensionInfoBasePtr>> enabling_extensions_;
            std::vector<ExtensionInfoBasePtr> required_extensions_;
            std::vector<ExtensionInfoBasePtr> conflicting_extensions_;

            static bool allEnabled_(const std::vector<ExtensionInfoBasePtr>& extensions)
            {
                return std::all_of(extensions.begin(), extensions.end(), [](const ExtensionInfoBasePtr& ext){ return ext->isEnabled(); });
            }

            bool anyEnablingExtensionsEnabled_() const
            {
                return enabling_extensions_.empty() || std::any_of(enabling_extensions_.begin(), enabling_extensions_.end(), [](const std::vector<ExtensionInfoBasePtr>& exts) { return allEnabled_(exts); });
            }

            template<bool assert_enabled>
            void validateConstraints_(const std::vector<ExtensionInfoBasePtr>& constraint_extensions) const
            {
                std::vector<ExtensionInfoBasePtr>::const_iterator it;

                using ExceptionType = std::conditional_t<assert_enabled, MissingRequiredExtensionException, ConflictingExtensionException>;

                if constexpr(assert_enabled)
                {
                    it = std::find_if_not(constraint_extensions.begin(),
                                          constraint_extensions.end(),
                                          [](const ExtensionInfoBasePtr& ext){ return ext->isEnabled(); });
                }
                else
                {
                    it = std::find_if(constraint_extensions.begin(),
                                      constraint_extensions.end(),
                                      [](const ExtensionInfoBasePtr& ext){ return ext->isEnabled(); });
                }

                if(it != constraint_extensions.end())
                {
                    throw ExceptionType(extension_, (*it)->getExtension());
                }
            }

            void assertNotSelfReferential_(const ExtensionInfoBasePtr& ext) const
            {
                if(ext.get() == this)
                {
                    throw SelfReferentialException(extension_);
                }
            }

            void assertNotSelfReferential_(const std::vector<ExtensionInfoBasePtr>& exts) const
            {
                if(std::any_of(exts.begin(), exts.end(), [this](const ExtensionInfoBasePtr& ext) { return ext.get() == this; }))
                {
                    throw SelfReferentialException(extension_);
                }
            }

        public:
            ExtensionInfoBase(const std::string& ext, const std::string& json) :
                extension_(ext),
                json_(json)
            {
            }

            explicit ExtensionInfoBase(const std::string& ext) :
                extension_(ext)
            {
            }

            const std::string& getExtension() const
            {
                return extension_;
            }

            void addEnablesExtension(const ExtensionInfoBasePtr& ext)
            {
                assertNotSelfReferential_(ext);
                enables_extensions_.emplace_back(ext);
            }

            void addEnablingExtensions(const std::vector<ExtensionInfoBasePtr>& exts)
            {
                assertNotSelfReferential_(exts);
                enabling_extensions_.emplace_back(exts);
            }

            template<typename DerivedExtensionInfo>
            std::enable_if_t<std::is_base_of_v<ExtensionInfoBase, DerivedExtensionInfo>> addEnablingExtensions(const std::vector<std::shared_ptr<DerivedExtensionInfo>>& exts)
            {
                addEnablingExtensions(std::vector<ExtensionInfoBasePtr>(exts.begin(), exts.end()));
            }

            void addRequiredExtension(const ExtensionInfoBasePtr& ext)
            {
                assertNotSelfReferential_(ext);

                required_extensions_.emplace_back(ext);
            }

            void addConflictingExtension(const ExtensionInfoBasePtr& ext)
            {
                assertNotSelfReferential_(ext);

                conflicting_extensions_.emplace_back(ext);
            }

            template<DependencyType type>
            void addDependency(const ExtensionInfoBasePtr& ext)
            {
                static_assert(type != DependencyType::ENABLING);

                switch(type)
                {
                    case DependencyType::ENABLES:
                        addEnablesExtension(ext);
                        break;
                    case DependencyType::REQUIRED:
                        addRequiredExtension(ext);
                        break;
                    case DependencyType::CONFLICTING:
                        addConflictingExtension(ext);
                        break;
                }
            }

            template<DependencyType type>
            void addDependency(const std::vector<ExtensionInfoBasePtr>& exts)
            {
                if constexpr(type == DependencyType::ENABLING)
                {
                    addEnablingExtensions(exts);
                }
                else
                {
                    for(const auto& ext: exts)
                    {
                        addDependency<type>(ext);
                    }
                }
            }

            template<DependencyType type, typename DerivedExtensionInfo>
            std::enable_if_t<std::is_base_of_v<ExtensionInfoBase, DerivedExtensionInfo>> addDependency(const std::vector<std::shared_ptr<DerivedExtensionInfo>>& exts)
            {
                addDependency<type>(std::vector<ExtensionInfoBasePtr>(exts.begin(), exts.end()));
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

            void setDisabled()
            {
                enabled_ = false;
                force_enabled_ = false;
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

            const std::string& getJSON() const { return json_; }
    };

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
    };

    class Extension : public ExtensionBase
    {
        private:
            const ExtensionInfoBasePtr ext_;

        public:
            explicit Extension(const ExtensionInfoBasePtr& ext) :
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
    };

    using ExtensionMap = std::unordered_map<std::string, std::unique_ptr<ExtensionBase>>;

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
    };

    template<typename ExtensionInfo>
    class ExtensionStateBase
    {
        private:
            using ExtensionInfoPtr = std::shared_ptr<ExtensionInfo>;

            class UnresolvedDependency
            {
                private:
                    const DependencyType type_;
                    const ExtensionInfoPtr ext_;
                    std::unordered_set<std::string> dep_ext_;

                    UnresolvedDependency(const DependencyType type, std::unordered_set<std::string>&& dep_ext, const ExtensionInfoPtr ext) :
                        type_(type),
                        ext_(ext),
                        dep_ext_(std::move(dep_ext))
                    {
                        if(type != DependencyType::ENABLING && dep_ext.size() > 1)
                        {
                            throw UnresolvedDependencyException(ext->getExtension());
                        }
                    }

                public:
                    UnresolvedDependency(const DependencyType type, const std::vector<std::string>& dep_ext, const ExtensionInfoPtr ext) :
                        UnresolvedDependency(type, std::unordered_set<std::string>(dep_ext.begin(), dep_ext.end()), ext)
                    {
                    }

                    UnresolvedDependency(const DependencyType type, const std::string& dep_ext, const ExtensionInfoPtr ext) :
                        UnresolvedDependency(type, std::unordered_set<std::string>{dep_ext}, ext)
                    {
                    }

                    DependencyType getType() const
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

            class UnresolvableDependencyException : public ExtensionManagerException
            {
                private:
                    static std::string getDependencyString_(const std::list<UnresolvedDependency>& dependencies)
                    {
                        std::ostringstream ss;

                        for(const auto& dep_info: dependencies)
                        {
                            ss << '\t' << dep_info.getExtension()->getExtension() << " -> " << '[';

                            bool first = true;
                            for(const auto& dep: dep_info.getDependentExtensions())
                            {
                                if(first)
                                {
                                    first = false;
                                }
                                else
                                {
                                    ss << ',';
                                }
                                ss << dep;
                            }

                            ss << ']' << std::endl;
                        }

                        return ss.str();
                    }

                public:
                    explicit UnresolvableDependencyException(const std::list<UnresolvedDependency>& dependencies) :
                        ExtensionManagerException("Unresolvable dependencies detected:\n" + getDependencyString_(dependencies))
                    {
                    }
            };

            virtual void throwUnknownExtensionException_(const std::string& extension) const
            {
                throw UnknownExtensionExceptionBase("Unknown extension specified: " + extension);
            }

            bool handleUnknownExtensionException_(const std::string& ext) const
            {
                switch(unknown_extension_action_)
                {
                    case UnknownExtensionAction::ERROR:
                        return true;
                    case UnknownExtensionAction::WARN:
                        std::cerr << "WARNING: ISA string contains an unknown extension (" << ext << "). Ignoring.";
                        break;
                    case UnknownExtensionAction::IGNORE:
                        break;
                }

                return false;
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

            std::vector<ExtensionInfoPtr> getDependencyOrDefer_(const DependencyType type, const std::vector<std::string>& dependent_exts, const ExtensionInfoPtr& ext)
            {
                const std::vector<ExtensionInfoPtr> exts = getExtensions_(dependent_exts);

                if(exts.size() != dependent_exts.size())
                {
                    pending_dependencies_.emplace_back(type, dependent_exts, ext);
                    return {};
                }

                return exts;
            }

            const ExtensionInfoPtr& getDependencyOrDefer_(const DependencyType type, const std::string& dependent_ext, const ExtensionInfoPtr& ext)
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

            bool extensionAllowed_(const std::string& ext) const
            {
                if(!extension_allowlist_.empty() && extension_allowlist_.count(ext) == 0)
                {
                    return false;
                }

                if(extension_blocklist_.count(ext) != 0)
                {
                    return false;
                }

                return true;
            }

        protected:
            const ExtensionInfoPtr& getExtensionInfo_(const std::string& extension) const
            {
                try
                {
                    return extensions_.at(extension);
                }
                catch(const std::out_of_range&)
                {
                    throwUnknownExtensionException_(extension);
                }

                __builtin_unreachable();
            }

            template<typename Callback>
            bool recurseExtension_(const std::string& extension, Callback&& callback)
            {
                if(auto it = meta_extensions_.find(extension); it != meta_extensions_.end())
                {
                    for(const auto& child: it->second)
                    {
                        callback(child);
                    }

                    return true;
                }
                else if(auto alias_it = aliases_.find(extension); alias_it != aliases_.end())
                {
                    callback(alias_it->second);

                    return true;
                }

                return false;
            }

            const uint32_t arch_id_;
            const UnknownExtensionAction unknown_extension_action_;
            std::unordered_map<std::string, ExtensionInfoPtr> extensions_;
            std::unordered_map<std::string, std::vector<std::string>> meta_extensions_;
            std::unordered_map<std::string, bool> config_extensions_;
            std::unordered_map<std::string, std::string> aliases_;
            std::list<UnresolvedDependency> pending_dependencies_;
            std::unordered_set<std::string> enabled_meta_extensions_;
            std::unordered_set<std::string> extension_allowlist_;
            std::unordered_set<std::string> extension_blocklist_;

        public:
            ExtensionStateBase(const uint32_t arch_id, const UnknownExtensionAction unknown_extension_action) :
                arch_id_(arch_id),
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
                    throw DuplicateExtensionException(extension);
                }

                const auto result = extensions_.emplace(extension, std::make_shared<ExtensionInfo>(extension, json));

                return *result.first->second;
            }

            void addConfigExtension(const std::string& ext)
            {
                config_extensions_.emplace(ext, false);
            }

            template<DependencyType type>
            void addDependency(const std::string& extension, const std::string& dep)
            {
                const auto& ext_ptr = getExtensionInfo_(extension);

                if(const auto& dep_ext = getDependencyOrDefer_(type, dep, ext_ptr); dep_ext)
                {
                    ext_ptr->template addDependency<type>(dep_ext);
                }
            }

            template<DependencyType type>
            void addDependency(const std::string& extension, const std::vector<std::string>& deps)
            {
                if constexpr(type == DependencyType::META)
                {
                    for(const auto& meta_extension: deps)
                    {
                        const auto result = meta_extensions_.try_emplace(meta_extension);
                        result.first->second.emplace_back(extension);
                    }
                }
                else if constexpr(type == DependencyType::ALIAS)
                {
                    for(const auto& alias: deps)
                    {
                        aliases_.emplace(alias, extension);
                    }
                }
                else
                {
                    const auto& ext_ptr = getExtensionInfo_(extension);

                    if(const auto dep_ext = getDependencyOrDefer_(type, deps, ext_ptr); !dep_ext.empty())
                    {
                        ext_ptr->template addDependency<type>(dep_ext);
                    }
                }
            }

            template<DependencyType type>
            void addDependency(const std::string& extension, const std::vector<std::vector<std::string>>& deps)
            {
                static_assert(type == DependencyType::ENABLING);

                for(const auto& dep: deps)
                {
                    addDependency<type>(extension, dep);
                }
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
                                case DependencyType::META:
                                case DependencyType::ALIAS:
                                    throw UnresolvedDependencyException(ext->getExtension());
                                case DependencyType::ENABLES:
                                    ext->addEnablesExtension(getExtensionInfo_(*dep_exts.begin()));
                                    break;
                                case DependencyType::ENABLING:
                                    ext->addEnablingExtensions(getExtensions_(dep_exts));
                                    break;
                                case DependencyType::REQUIRED:
                                    ext->addRequiredExtension(getExtensionInfo_(*dep_exts.begin()));
                                    break;
                                case DependencyType::CONFLICTING:
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
                        throw UnresolvableDependencyException(pending_dependencies_);
                    }
                }
            }

            void allowExtension(const std::string& extension)
            {
                recurseExtension_(extension, [this](const std::string& child){ allowExtension(child); });

                extension_blocklist_.erase(extension);
                extension_allowlist_.emplace(extension);
            }

            void clearAllowedExtensions()
            {
                extension_allowlist_.clear();
            }

            void blockExtension(const std::string& extension)
            {
                recurseExtension_(extension, [this](const std::string& child){ blockExtension(child); });

                extension_allowlist_.erase(extension);
                extension_blocklist_.emplace(extension);
            }

            void clearBlockedExtensions()
            {
                extension_blocklist_.clear();
            }

            void enableExtension(const std::string& ext)
            {
                if(!extensionAllowed_(ext))
                {
                    throw ExtensionNotAllowedException(ext, extension_allowlist_, extension_blocklist_);
                }

                bool is_meta = true;
                if(recurseExtension_(ext, [this](const std::string& child) { enableExtension(child); }))
                {
                    // It was either a meta extension or an alias
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
                    catch(const UnknownExtensionExceptionBase&)
                    {
                        if(handleUnknownExtensionException_(ext))
                        {
                            throw;
                        }
                    }
                }

                if(is_meta)
                {
                    enabled_meta_extensions_.emplace(ext);
                }
            }

            void disableExtension(const std::string& ext)
            {
                bool is_meta = true;
                if(recurseExtension_(ext, [this](const std::string& child) { disableExtension(child); }))
                {
                    // It was either a meta extension or an alias
                }
                else if(auto config_it = config_extensions_.find(ext); config_it != config_extensions_.end())
                {
                    config_it->second = false;
                }
                else
                {
                    try
                    {
                        getExtensionInfo_(ext)->setDisabled();
                        is_meta = false;
                    }
                    catch(const UnknownExtensionExceptionBase&)
                    {
                        if(handleUnknownExtensionException_(ext))
                        {
                            throw;
                        }
                    }
                }

                if(is_meta)
                {
                    enabled_meta_extensions_.erase(ext);
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

            void reset()
            {
                for(const auto& ext: extensions_)
                {
                    ext.second->setDisabled();
                }

                for(auto& ext: config_extensions_)
                {
                    ext.second = false;
                }

                enabled_meta_extensions_.clear();
            }
    };

    template<typename ExtensionInfo, typename ExtensionState>
    class ExtensionManager
    {
        protected:
            const UnknownExtensionAction unknown_extension_action_;
            std::string mavis_json_dir_;
            std::string isa_;
            using ArchMap = std::unordered_map<uint32_t, ExtensionState>;
            ArchMap extensions_;
            typename ArchMap::iterator enabled_arch_{extensions_.end()};
            ExtensionMap enabled_extensions_;
            mutable std::vector<std::string> enabled_jsons_;

            enum class ExtensionType
            {
                META,
                CONFIG,
                NORMAL
            };

            virtual void processArchSpecificExtensionInfo_(ExtensionState&, const std::string&, const nlohmann::json&, const ExtensionType) const
            {
            }

            virtual uint32_t convertMultiArchString_(const std::string& multiarch_str) const = 0;

            virtual std::vector<uint32_t> convertMultiArchVector_(const std::vector<std::string>& multiarch_str_vec) const
            {
                std::vector<uint32_t> arches;
                arches.resize(multiarch_str_vec.size());

                std::transform(multiarch_str_vec.begin(), multiarch_str_vec.end(), arches.begin(), [this](const std::string& multiarch_str) { return convertMultiArchString_(multiarch_str); });

                return arches;
            }

            virtual std::vector<uint32_t> getMultiArchVector_(const nlohmann::json& multiarch_obj) const
            {
                if(multiarch_obj.is_array())
                {
                    if(!multiarch_obj.empty())
                    {
                        if(multiarch_obj.front().is_string())
                        {
                            return convertMultiArchVector_(multiarch_obj.get<std::vector<std::string>>());
                        }
                        else if(multiarch_obj.front().is_number())
                        {
                            return multiarch_obj.get<std::vector<uint32_t>>();
                        }
                    }

                    throw InvalidJSONValueException(getMultiArchKey_());
                }
                else
                {
                    return std::vector<uint32_t>(1, multiarch_obj.get<uint32_t>());
                }
            }

            virtual const char* getMultiArchKey_() const
            {
                return nullptr;
            }

            template<typename ManagerType>
            static ManagerType fromELF_(const std::string& elf, const std::string& spec_json, const std::string& mavis_json_dir, const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
            {
                auto man = fromISASpecJSON_<ManagerType>(spec_json, mavis_json_dir, unknown_extension_action);
                man.setISAFromELF(elf);
                return man;
            }

            template<typename ManagerType>
            static ManagerType fromISA_(const std::string& isa, const std::string& spec_json, const std::string& mavis_json_dir, const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
            {
                auto man = fromISASpecJSON_<ManagerType>(spec_json, mavis_json_dir, unknown_extension_action);
                man.setISA(isa);
                return man;
            }

            template<typename ManagerType>
            static ManagerType fromISASpecJSON_(const std::string& spec_json, const std::string& mavis_json_dir, const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
            {
                ManagerType man(unknown_extension_action);
                man.setISASpecJSON(spec_json, mavis_json_dir);
                return man;
            }

            virtual std::string getISAFromELF_(const std::string& elf) const = 0;

            virtual void setISAImpl_(const std::string& isa) = 0;

        private:
            template<bool is_normal_extension, DependencyType dep_type>
            constexpr static bool isDependencyAllowed_()
            {
                switch(dep_type)
                {
                    case DependencyType::META:
                        return true;
                    case DependencyType::ALIAS:
                    case DependencyType::ENABLES:
                    case DependencyType::ENABLING:
                    case DependencyType::REQUIRED:
                    case DependencyType::CONFLICTING:
                        return is_normal_extension;
                };
            }

            template<DependencyType dep_type>
            constexpr static const char* getDependencyKey_()
            {
                switch(dep_type)
                {
                    case DependencyType::META:
                        return "meta_extension";
                    case DependencyType::ALIAS:
                        return "aliases";
                    case DependencyType::ENABLES:
                        return "enables";
                    case DependencyType::ENABLING:
                        return "enabled_by";
                    case DependencyType::REQUIRED:
                        return "requires";
                    case DependencyType::CONFLICTING:
                        return "conflicts";
                };
            }

            template<DependencyType dep_type>
            static std::vector<typename DependencyTraits<dep_type>::value_type> getDependencyValue_(const nlohmann::json& obj)
            {
                return JSONVectorConverter<typename DependencyTraits<dep_type>::value_type>::get(obj);
            }

            template<bool is_normal_extension, DependencyType dep_type>
            static void processOptionalDependency_(ExtensionState& extensions, const std::string& ext, const nlohmann::json& obj)
            {
                constexpr const char* key = getDependencyKey_<dep_type>();

                if(const auto it = obj.find(key); it != obj.end())
                {
                    if constexpr(!isDependencyAllowed_<is_normal_extension, dep_type>())
                    {
                        throw MetaExtensionUnexpectedJSONKeyException(key);
                    }

                    extensions.template addDependency<dep_type>(ext, getDependencyValue_<dep_type>(*it));
                }
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
                    throw MissingRequiredJSONKeyException(key);
                }
            }

            template<ExtensionType extension_type>
            void processExtension_(const nlohmann::json& ext_obj)
            {
                static constexpr bool is_normal_extension = extension_type == ExtensionType::NORMAL;
                static constexpr bool is_config_extension = extension_type == ExtensionType::CONFIG;

                const std::string ext = getRequiredJSONValue_<std::string>(ext_obj, "extension");

                std::vector<uint32_t> arches;

                const char* multiarch_key = getMultiArchKey_();

                if(multiarch_key)
                {
                    if(const auto it = ext_obj.find(multiarch_key); it != ext_obj.end())
                    {
                        arches = getMultiArchVector_(*it);
                    }
                    else
                    {
                        throw MissingRequiredJSONKeyException(multiarch_key);
                    }
                }
                else
                {
                    arches.emplace_back(0);
                }

                for(const auto arch: arches)
                {
                    auto& arch_extensions = extensions_.try_emplace(arch, arch, unknown_extension_action_).first->second;

                    if constexpr(is_normal_extension)
                    {
                        if(auto json_it = ext_obj.find("json"); json_it != ext_obj.end())
                        {
                            arch_extensions.addExtension(ext, *json_it);
                        }
                        else
                        {
                            arch_extensions.addExtension(ext);
                        }
                    }
                    else if constexpr(is_config_extension)
                    {
                        arch_extensions.addConfigExtension(ext);
                    }

                    processArchSpecificExtensionInfo_(arch_extensions, ext, ext_obj, extension_type);
                    processOptionalDependency_<is_normal_extension, DependencyType::META>(arch_extensions, ext, ext_obj);
                    processOptionalDependency_<is_normal_extension, DependencyType::ALIAS>(arch_extensions, ext, ext_obj);
                    processOptionalDependency_<is_normal_extension, DependencyType::ENABLES>(arch_extensions, ext, ext_obj);
                    processOptionalDependency_<is_normal_extension, DependencyType::ENABLING>(arch_extensions, ext, ext_obj);
                    processOptionalDependency_<is_normal_extension, DependencyType::REQUIRED>(arch_extensions, ext, ext_obj);
                    processOptionalDependency_<is_normal_extension, DependencyType::CONFLICTING>(arch_extensions, ext, ext_obj);
                }
            }

            void assertISASpecInitialized_() const
            {
                if(extensions_.empty())
                {
                    throw UninitializedISASpecException();
                }
            }

            void assertISAInitialized_() const
            {
                if(enabled_arch_ == extensions_.end())
                {
                    throw UninitializedISAException();
                }
            }

            void refresh_()
            {
                assertISAInitialized_();
                enabled_extensions_.clear();
                enabled_jsons_.clear();
                enabled_arch_->second.finalize(enabled_extensions_);
            }

            template<bool refresh = true>
            void enableExtension_(const std::string& ext)
            {
                if(!isEnabled(ext))
                {
                    enabled_arch_->second.enableExtension(ext);

                    if constexpr(refresh)
                    {
                        refresh_();
                    }
                }
            }

            template<bool refresh = true>
            void disableExtension_(const std::string& ext)
            {
                if(isEnabled(ext))
                {
                    enabled_arch_->second.disableExtension(ext);

                    if constexpr(refresh)
                    {
                        refresh_();
                    }
                }
            }

            template<typename InstType,
                     typename AnnotationType,
                     typename InstTypeAllocator,
                     typename AnnotationTypeAllocator,
                     typename ... MavisArgs>
            Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator> constructMavis_(MavisArgs&&... mavis_args) const
            {
                return Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>(getJSONs(), std::forward<MavisArgs>(mavis_args)...);
            }

        public:
            explicit ExtensionManager(const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR) :
                unknown_extension_action_(unknown_extension_action)
            {
            }

            ExtensionManager(const ExtensionManager&) = delete;
            ExtensionManager(ExtensionManager&&) = default;
            virtual ~ExtensionManager() = default;

            void setISASpecJSON(const std::string& jfile, const std::string& mavis_json_dir)
            {
                mavis_json_dir_ = mavis_json_dir;

                std::ifstream fs;

                try
                {
                    fs.open(jfile);
                }
                catch (const std::ifstream::failure &ex)
                {
                    throw BadISAFile(jfile);
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
                catch(const ExtensionManagerException&)
                {
                    std::cerr << "Error parsing file " << jfile << std::endl;
                    throw;
                }

                for(auto& ext: extensions_)
                {
                    ext.second.finalizeDependencies();
                }
            }

            void setISAFromELF(const std::string& elf)
            {
                setISA(getISAFromELF_(elf));
            }

            void setISA(const std::string& isa)
            {
                if(!isa_.empty())
                {
                    for(auto& ext: extensions_)
                    {
                        ext.second.reset();
                    }

                    enabled_extensions_.clear();
                    enabled_jsons_.clear();
                }

                setISAImpl_(isa);
            }

            bool isEnabled(const std::string& extension) const
            {
                return enabled_extensions_.count(extension) != 0;
            }

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
                            enabled_jsons_.emplace_back(mavis_json_dir_ + "/" + json);
                        }
                    }
                }

                return enabled_jsons_;
            }

            // Adds the specified extension to the allowlist for arch_key
            // NOTE: Allowlist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void allowExtension(const uint32_t arch_key, const std::string& extension)
            {
                assertISASpecInitialized_();
                extensions_.at(arch_key).allowExtension(extension);
            }

            // Adds the specified extension to all allowlists
            // NOTE: Allowlist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void allowExtension(const std::string& extension)
            {
                assertISASpecInitialized_();
                for(auto& ext_info: extensions_)
                {
                    ext_info.second.allowExtension(extension);
                }
            }

            // Adds the specified extensions to the allowlist for arch_key
            // NOTE: Allowlist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void allowExtensions(const uint32_t arch_key, const std::vector<std::string>& extensions)
            {
                for(const auto& ext: extensions)
                {
                    allowExtension(arch_key, ext);
                }
            }

            // Adds the specified extensions to all allowlists
            // NOTE: Allowlist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void allowExtensions(const std::vector<std::string>& extensions)
            {
                for(const auto& ext: extensions)
                {
                    allowExtension(ext);
                }
            }

            // Clears the allowlist for arch_key
            // NOTE: Allowlist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void clearAllowedExtensions(const uint32_t arch_key)
            {
                assertISASpecInitialized_();
                extensions_.at(arch_key).clearAllowedExtensions();
            }

            // Clears all allowlists
            // NOTE: Allowlist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void clearAllowedExtensions()
            {
                assertISASpecInitialized_();
                for(auto& ext_info: extensions_)
                {
                    ext_info.second.clearAllowedExtensions();
                }
            }

            // Adds the specified extension to the blocklist for arch_key
            // NOTE: Blocklist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void blockExtension(const uint32_t arch_key, const std::string& extension)
            {
                assertISASpecInitialized_();
                extensions_.at(arch_key).blockExtension(extension);
            }

            // Adds the specified extension to all blocklists
            // NOTE: Blocklist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void blockExtension(const std::string& extension)
            {
                assertISASpecInitialized_();
                for(auto& ext_info: extensions_)
                {
                    ext_info.second.blockExtension(extension);
                }
            }

            // Adds the specified extensions to the blocklist for arch_key
            // NOTE: Blocklist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void blockExtensions(const uint32_t arch_key, const std::vector<std::string>& extensions)
            {
                for(const auto& ext: extensions)
                {
                    blockExtension(arch_key, ext);
                }
            }

            // Adds the specified extensions to all blocklists
            // NOTE: Blocklist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void blockExtensions(const std::vector<std::string>& extensions)
            {
                for(const auto& ext: extensions)
                {
                    blockExtension(ext);
                }
            }

            // Clears the blocklist for arch_key
            // NOTE: Blocklist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void clearBlockedExtensions(const uint32_t arch_key)
            {
                assertISASpecInitialized_();
                extensions_.at(arch_key).clearBlockedExtensions();
            }

            // Clears all blocklists
            // NOTE: Blocklist changes will only apply to future invocations of setISA.
            // Existing enabled extensions are not affected.
            void clearBlockedExtensions()
            {
                assertISASpecInitialized_();
                for(auto& ext_info: extensions_)
                {
                    ext_info.second.clearBlockedExtensions();
                }
            }

            // Enables the specified extension for the currently selected arch
            void enableExtension(const std::string& ext)
            {
                assertISAInitialized_();
                enableExtension_(ext);
            }

            // Enables the specified extensions for the currently selected arch
            void enableExtensions(const std::vector<std::string>& extensions)
            {
                assertISAInitialized_();
                for(const auto& ext: extensions)
                {
                    enableExtension_<false>(ext);
                }

                refresh_();
            }

            // Disables the specified extension for the currently selected arch
            void disableExtension(const std::string& ext)
            {
                assertISAInitialized_();
                disableExtension_(ext);
            }

            // Disables the specified extensions for the currently selected arch
            void disableExtensions(const std::vector<std::string>& extensions)
            {
                assertISAInitialized_();
                for(const auto& ext: extensions)
                {
                    disableExtension_<false>(ext);
                }

                refresh_();
            }

            template<typename InstType,
                     typename AnnotationType,
                     typename InstTypeAllocator       = SharedPtrAllocator<InstType>,
                     typename AnnotationTypeAllocator = SharedPtrAllocator<AnnotationType>>
            Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
            constructMavis(const FileNameListType& anno_files,
                           const InstUIDList& uid_list,
                           const AnnotationOverrides& anno_overrides,
                           const MatchSet<Pattern>& inclusions,
                           const MatchSet<Pattern>& exclusions,
                           const InstTypeAllocator& inst_allocator = SharedPtrAllocator<InstType>(),
                           const AnnotationTypeAllocator& annotation_allocator = SharedPtrAllocator<AnnotationType>()) const
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
                     typename InstTypeAllocator       = SharedPtrAllocator<InstType>,
                     typename AnnotationTypeAllocator = SharedPtrAllocator<AnnotationType>>
            Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
            constructMavis(const FileNameListType& anno_files,
                           const InstUIDList& uid_list,
                           const AnnotationOverrides& anno_overrides = {},
                           const InstTypeAllocator& inst_allocator = SharedPtrAllocator<InstType>(),
                           const AnnotationTypeAllocator& annotation_allocator = SharedPtrAllocator<AnnotationType>()) const
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
                     typename InstTypeAllocator       = SharedPtrAllocator<InstType>,
                     typename AnnotationTypeAllocator = SharedPtrAllocator<AnnotationType>>
            Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
            constructMavis(const FileNameListType& anno_files,
                           const MatchSet<Pattern>& inclusions,
                           const MatchSet<Pattern>& exclusions,
                           const InstTypeAllocator& inst_allocator = SharedPtrAllocator<InstType>(),
                           const AnnotationTypeAllocator& annotation_allocator = SharedPtrAllocator<AnnotationType>()) const
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
                     typename InstTypeAllocator       = SharedPtrAllocator<InstType>,
                     typename AnnotationTypeAllocator = SharedPtrAllocator<AnnotationType>>
            Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
            constructMavis(const FileNameListType& anno_files,
                           const InstTypeAllocator& inst_allocator = SharedPtrAllocator<InstType>(),
                           const AnnotationTypeAllocator& annotation_allocator = SharedPtrAllocator<AnnotationType>()) const
            {
                return constructMavis_<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>(
                    anno_files,
                    inst_allocator,
                    annotation_allocator
                );
            }
    };
}
