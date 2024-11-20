#pragma once

#include <filesystem>
#include <list>

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
    
    class CircularDependencyException : public ExtensionManagerException
    {
        public:
            explicit CircularDependencyException() :
                ExtensionManagerException("Circular dependency detected!")
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
        class ExtensionInfo
        {
            private:
                const std::string extension_;
                const std::string json_;
                bool enabled_ = false;
                std::vector<std::shared_ptr<ExtensionInfo>> enabling_extensions_;
                std::vector<std::shared_ptr<ExtensionInfo>> required_extensions_;

                static bool allEnabled_(const std::vector<std::shared_ptr<ExtensionInfo>>& extensions)
                {
                    return extensions.empty() || std::all_of(extensions.begin(), extensions.end(), [](const std::shared_ptr<ExtensionInfo>& ext){ return ext->isEnabled(); });
                }

                std::vector<std::shared_ptr<ExtensionInfo>>::const_iterator findFirstMissingRequiredExtension_() const
                {
                    return std::find_if_not(required_extensions_.begin(),
                                            required_extensions_.end(),
                                            [](const std::shared_ptr<ExtensionInfo>& ext){ return ext->isEnabled(); });
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

                void addEnablingExtension(const std::shared_ptr<ExtensionInfo>& ext)
                {
                    enabling_extensions_.emplace_back(ext);
                }

                void addRequiredExtension(const std::shared_ptr<ExtensionInfo>& ext)
                {
                    required_extensions_.emplace_back(ext);
                }

                void setEnabled() { enabled_ = true; }

                bool isEnabled() const {
                    return enabled_ && allEnabled_(enabling_extensions_);
                }

                void finalize()
                {
                    if(!enabled_)
                    {
                        return;
                    }

                    if(auto req_it = findFirstMissingRequiredExtension_(); req_it != required_extensions_.end())
                    {
                        throw mavis::MissingRequiredExtensionException(extension_, (*req_it)->getExtension());
                    }

                    enabled_ = allEnabled_(enabling_extensions_);
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
                            ENABLING,
                            REQUIRED
                        };

                    private:
                        const Type type_;
                        const std::string dep_ext_;
                        const std::shared_ptr<ExtensionInfo> ext_;

                    public:
                        UnresolvedDependency(const Type type, const std::string& dep_ext, const std::shared_ptr<ExtensionInfo> ext) :
                            type_(type),
                            dep_ext_(dep_ext),
                            ext_(ext)
                        {
                        }

                        Type getType() const
                        {
                            return type_;
                        }

                        const std::string& getDependentExtension() const
                        {
                            return dep_ext_;
                        }

                        const std::shared_ptr<ExtensionInfo>& getExtension() const
                        {
                            return ext_;
                        }
                };

                std::unordered_map<std::string, std::shared_ptr<ExtensionInfo>> extensions_;
                std::unordered_map<std::string, std::vector<std::shared_ptr<ExtensionInfo>>> meta_extensions_;
                std::list<UnresolvedDependency> pending_dependencies_;

                const std::shared_ptr<ExtensionInfo>& getExtensionInfo_(const std::string& extension) const
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

                const std::shared_ptr<ExtensionInfo>& getDependencyOrDefer_(const UnresolvedDependency::Type type, const std::string& dependent_ext, const std::shared_ptr<ExtensionInfo>& ext)
                {
                    static const std::shared_ptr<ExtensionInfo> NOT_FOUND{nullptr};

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
                    const auto result = extensions_.try_emplace(extension, json);
                    if(!result.second)
                    {
                        throw mavis::DuplicateExtensionException(extension);
                    }

                    return *result.first->second;
                }

                void addEnablingExtension(const std::string& ext, const std::string& dependent_ext)
                {
                    const auto& ext_ptr = getExtensionInfo_(ext);
                    if(const auto& dep_ext = getDependencyOrDefer_(UnresolvedDependency::Type::ENABLING, dependent_ext, ext_ptr); dep_ext)
                    {
                        ext_ptr->addEnablingExtension(dep_ext);
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

                void addMetaExtension(const std::string& meta_extension, const std::string& extension)
                {
                    const auto result = meta_extensions_.try_emplace(meta_extension);
                    result.first->second.emplace_back(getExtensionInfo_(extension));
                }

                void finalizeDependencies()
                {
                    auto it = pending_dependencies_.begin();

                    while(!pending_dependencies_.empty())
                    {
                        uint32_t num_resolved = 0;

                        while(it != pending_dependencies_.end())
                        {
                            const auto& ext = it->getExtension();
                            const auto& dep_ext = it->getDependentExtension();

                            const auto dep_it = extensions_.find(dep_ext);
                            if(dep_it == extensions_.end())
                            {
                                ++it;
                                continue;
                            }

                            const auto& dep_ptr = dep_it->second;
                            switch(it->getType())
                            {
                                case UnresolvedDependency::Type::ENABLING:
                                    ext->addEnablingExtension(dep_ptr);
                                    break;
                                case UnresolvedDependency::Type::REQUIRED:
                                    ext->addRequiredExtension(dep_ptr);
                                    break;
                            }

                            it = pending_dependencies_.erase(it);
                            ++num_resolved;
                        }

                        if(num_resolved == 0)
                        {
                            throw mavis::CircularDependencyException();
                        }
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
        static T getRequiredJSONValue_(const std::string& isa_file, const nlohmann::json& jobj, const std::string& key)
        {
            try
            {
                return jobj.at(key);
            }
            catch(const nlohmann::json::exception&)
            {
                throw mavis::BadISAFile(isa_file);
            }
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
            fs >> jobj;

            const uint32_t xlen = getRequiredJSONValue_<uint32_t>(jfile, jobj, "xlen");
            const std::string ext = getRequiredJSONValue_<std::string>(jfile, jobj, "extension");

            const auto xlen_result = extensions_.try_emplace(xlen);
            auto& xlen_extensions = xlen_result.first->second;

            xlen_extensions.addExtension(ext, jfile);
            if(const auto meta_extension_it = jobj.find("meta_extension"); meta_extension_it != jobj.end())
            {
                xlen_extensions.addMetaExtension(*meta_extension_it, ext);
            }

            if(const auto enabled_by_it = jobj.find("enabled_by"); enabled_by_it != jobj.end())
            {
                for(const auto& enabling_ext: *enabled_by_it)
                {
                    xlen_extensions.addEnablingExtension(ext, enabling_ext);
                }
            }

            if(const auto requires_it = jobj.find("requires"); requires_it != jobj.end())
            {
                for(const auto& required_ext: *requires_it)
                {
                    xlen_extensions.addRequiredExtension(ext, required_ext);
                }
            }
        }

    public:
        explicit ExtensionManager(const std::string& isa, const std::string& json_dir) :
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

            for (const auto& dir_entry : std::filesystem::directory_iterator{json_dir_path}) 
            {
                if(dir_entry.is_regular_file())
                {
                    const auto& jfile = dir_entry.path();
                    processExtensionJSON_(jfile);
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
