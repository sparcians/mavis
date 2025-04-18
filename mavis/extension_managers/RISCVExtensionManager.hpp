#pragma once

#include "elfio/elfio.hpp"
#include "mavis/ExtensionManager.hpp"

namespace mavis::extension_manager::riscv
{
    class UnknownRISCVExtensionException : public UnknownExtensionExceptionBase
    {
      public:
        explicit UnknownRISCVExtensionException(const uint32_t xlen, const std::string & ext) :
            UnknownExtensionExceptionBase("Unknown extension for riscv xlen=" + std::to_string(xlen)
                                          + ": " + ext)
        {
        }
    };

    class InvalidBaseExtensionException : public ExtensionManagerException
    {
      private:
        static inline std::string
        genBaseExtensionString_(const std::unordered_set<std::string> & base_extensions)
        {
            std::ostringstream ss;
            bool first = true;
            for (const auto & ext : base_extensions)
            {
                if (first)
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
        explicit InvalidBaseExtensionException(
            const std::string & ext, const std::unordered_set<std::string> & base_extensions) :
            ExtensionManagerException(ext + " is not a valid base extension. Valid extensions are: "
                                      + genBaseExtensionString_(base_extensions))
        {
        }
    };

    class ISANotFoundInELFException : public ExtensionManagerException
    {
      public:
        explicit ISANotFoundInELFException(const std::string & elf) :
            ExtensionManagerException("Could not find a RISCV ISA string in " + elf)
        {
        }
    };

    static constexpr uint32_t RISCV_DEFAULT_MAJOR_VER = 2;
    static constexpr uint32_t RISCV_DEFAULT_MINOR_VER = 0;

    class RISCVExtensionInfo : public ExtensionInfoBase
    {
      private:
        uint32_t major_ver_ = RISCV_DEFAULT_MAJOR_VER;
        uint32_t minor_ver_ = RISCV_DEFAULT_MINOR_VER;

      public:
        RISCVExtensionInfo(const std::string & ext, const std::string & json) :
            ExtensionInfoBase(ext, json)
        {
        }

        explicit RISCVExtensionInfo(const std::string & ext) : ExtensionInfoBase(ext) {}

        void setVersion(const uint32_t major_ver, const uint32_t minor_ver)
        {
            major_ver_ = major_ver;
            minor_ver_ = minor_ver;
        }

        uint32_t getMajorVersion() const { return major_ver_; }

        uint32_t getMinorVersion() const { return minor_ver_; }
    };

    class RISCVXLENState : public ExtensionStateBase<RISCVExtensionInfo>
    {
      private:
        std::unordered_set<std::string> base_extensions_;

        void throwUnknownExtensionException_(const std::string & extension) const override
        {
            throw UnknownRISCVExtensionException(getXLEN(), extension);
        }

      public:
        RISCVXLENState(const uint32_t xlen, const UnknownExtensionAction unknown_extension_action) :
            ExtensionStateBase<RISCVExtensionInfo>(xlen, unknown_extension_action)
        {
        }

        uint32_t getXLEN() const { return arch_id_; }

        void addBaseExtension(const std::string & ext) { base_extensions_.emplace(ext); }

        void enableBaseExtension(const std::string & ext)
        {
            if (base_extensions_.count(ext) == 0)
            {
                throw InvalidBaseExtensionException(ext, base_extensions_);
            }

            enableExtension(ext);
        }

        void enableBaseExtension(const char ext) { enableBaseExtension(std::string(1, ext)); }

        void setExtensionVersion(const std::string & ext, const uint32_t major_ver,
                                 const uint32_t minor_ver)
        {
            if (auto it = meta_extensions_.find(ext); it != meta_extensions_.end())
            {
                for (const auto & child : it->second)
                {
                    setExtensionVersion(child, major_ver, minor_ver);
                }
            }
            else if (auto alias_it = aliases_.find(ext); alias_it != aliases_.end())
            {
                setExtensionVersion(alias_it->second, major_ver, minor_ver);
            }
            else if (!config_extensions_.count(ext))
            {
                getExtensionInfo_(ext)->setVersion(major_ver, minor_ver);
            }
        }

        void setExtensionVersion(const char ext, const uint32_t major_ver, const uint32_t minor_ver)
        {
            setExtensionVersion(std::string(1, ext), major_ver, minor_ver);
        }
    };

    class RISCVExtensionManager : public ExtensionManager<RISCVExtensionInfo, RISCVXLENState>
    {
      private:
        class VersionNumberExtractionError : public std::exception
        {
          public:
            const char* what() const noexcept override
            {
                return "Failed to extract version number";
            }
        };

        static constexpr std::string_view DIGITS_{"0123456789"};

        uint32_t xlen_ = 0;

        template <typename ValType, typename OtherValType>
        static bool isOneOf_(const ValType val, const OtherValType other)
        {
            return (val == other);
        }

        template <typename ValType, typename OtherValType, typename... OtherValTypes>
        static bool isOneOf_(const ValType val, const OtherValType other,
                             const OtherValTypes... rest)
        {
            return (val == other) || isOneOf_(val, rest...);
        }

        static bool getCharIfValid_(const std::string_view & isa_view, char & front_char)
        {
            if (isa_view.empty())
            {
                front_char = '\0';
                return false;
            }

            front_char = isa_view.front();

            return true;
        }

        static bool inSingleCharExtRange_(const std::string_view & isa_view, char & front_char)
        {
            return getCharIfValid_(isa_view, front_char) && !isOneOf_(front_char, 'z', 's', 'x');
        }

        static uint32_t extractNumber_(std::string_view & isa_view)
        {
            const auto number_end = isa_view.find_first_not_of(DIGITS_);

            const uint32_t val = std::stoul(std::string(isa_view.substr(0, number_end)));
            isa_view.remove_prefix(number_end == std::string_view::npos ? isa_view.size()
                                                                        : number_end);
            return val;
        }

        static std::pair<uint32_t, uint32_t> extractVersions_(std::string_view & isa_view)
        {
            const uint32_t major_ver = extractNumber_(isa_view);
            uint32_t minor_ver = RISCV_DEFAULT_MINOR_VER;

            if (!isa_view.empty() && isa_view[0] == 'p')
            {
                isa_view.remove_prefix(1);

                if (char next_char; !getCharIfValid_(isa_view, next_char) || !isdigit(next_char))
                {
                    throw VersionNumberExtractionError();
                }

                minor_ver = extractNumber_(isa_view);
            }

            return std::make_pair(major_ver, minor_ver);
        }

        static inline std::string toLowercase_(const std::string & str)
        {
            std::string lowercase;
            lowercase.reserve(str.size());
            std::transform(str.begin(), str.end(), std::back_inserter(lowercase),
                           [](const char chr) { return std::tolower(chr); });
            return lowercase;
        }

        static bool getBoolJSONValue_(const nlohmann::json & jobj, const std::string & key)
        {
            if (const auto it = jobj.find(key); it != jobj.end())
            {
                return *it;
            }

            return false;
        }

        // Handles the case where someone specifies an XLEN as a string - just convert
        // it to an int
        uint32_t convertMultiArchString_(const std::string & multiarch_str) const override
        {
            return std::stoul(multiarch_str);
        }

        void processArchSpecificExtensionInfo_(RISCVXLENState & arch_extensions,
                                               const std::string & ext,
                                               const nlohmann::json & ext_obj,
                                               const ExtensionType extension_type) const override
        {
            if (getBoolJSONValue_(ext_obj, "is_base_extension"))
            {
                if (extension_type == ExtensionType::CONFIG)
                {
                    throw MetaExtensionUnexpectedJSONKeyException("is_base_extension");
                }
                else
                {
                    arch_extensions.addBaseExtension(ext);
                }
            }
        }

        // Reads a ULEB128 value into the result bitset from the buffer at ptr
        // Returns a pointer to the first byte after the ULEB128 value
        // Based on the pseudocode at
        // https://en.wikipedia.org/wiki/LEB128#Decode_unsigned_integer
        static const char* readULEB128_(std::bitset<128> & result, const char* ptr)
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

        static uint32_t readU32_(const char* ptr, const ELFIO::endianness_convertor & convertor)
        {
            uint32_t val;
            std::memcpy(&val, ptr, sizeof(uint32_t));
            return convertor(val);
        }

        static const char* findISAStringInELF_(const ELFIO::elfio & elf_reader)
        {
            static constexpr ELFIO::Elf_Word SHT_RISCV_ATTRIBUTES = 0x70000003;
            static constexpr std::string_view riscv_attributes_sec_name(".riscv.attributes");
            static constexpr std::string_view riscv_vendor("riscv");
            static constexpr size_t riscv_vendor_length = riscv_vendor.size() + 1;
            static constexpr uint32_t Tag_file = 1;
            static constexpr uint32_t Tag_RISCV_arch = 5;

            const ELFIO::section* riscv_attr_sec = elf_reader.sections[riscv_attributes_sec_name];

            if (riscv_attr_sec && riscv_attr_sec->get_type() == SHT_RISCV_ATTRIBUTES)
            {
                const char* data = riscv_attr_sec->get_data();
                const char* const end = data + riscv_attr_sec->get_size();

                // The .riscv.attributes section should start with an 'A' byte, followed
                // by the first sub-section
                if (*data == 'A')
                {
                    const auto & endian_conv = elf_reader.get_convertor();

                    const char* sub_sec = data + 1;

                    std::bitset<128> tag;

                    while (sub_sec < end)
                    {
                        // Each sub-section has the following format:
                        // * length (uint32_t)
                        // * vendor name (null-terminated string)
                        // * sub-sub-sections
                        const uint32_t sub_section_length = readU32_(sub_sec, endian_conv);
                        const char* vendor = sub_sec + sizeof(uint32_t);

                        // The ISA string lives in the "riscv" sub-section
                        if (riscv_vendor.compare(vendor) == 0)
                        {
                            const char* sub_sub_sec = vendor + riscv_vendor_length;

                            while (sub_sub_sec < end)
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

                                // The ISA string applies to the entire ELF, so its
                                // sub-sub-section should have tag == Tag_file
                                if (tag == Tag_file)
                                {
                                    const char* const sub_sub_sec_end =
                                        sub_sub_sec + sub_sub_sec_len;

                                    ptr += sizeof(uint32_t);

                                    // ptr now points to the first attribute-value pair
                                    while (ptr < sub_sub_sec_end)
                                    {
                                        ptr = readULEB128_(tag, ptr);
                                        if (tag == Tag_RISCV_arch) // This is the ISA string. It
                                                                   // starts at ptr
                                        {
                                            return ptr;
                                        }
                                        else if (tag.test(0)) // Some other string value
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

        const char* getMultiArchKey_() const override { return "xlen"; }

        std::string getISAFromELF_(const std::string & elf) const override
        {
            ELFIO::elfio elf_reader;
            if (!elf_reader.load(elf))
            {
                throw ELFNotFoundException(elf);
            }

            const char* isa_str = findISAStringInELF_(elf_reader);

            if (!isa_str)
            {
                throw ISANotFoundInELFException(elf);
            }

            return isa_str;
        }

        void setISAImpl_(const std::string & isa) override
        {
            // ISA string must at least contain rv, the XLEN, and the base ISA,
            // e.g. rv32i
            static constexpr size_t MIN_LENGTH = 5;

            isa_ = toLowercase_(isa);

            std::string_view isa_view(isa_);
            if (isa_view.size() < MIN_LENGTH || isa_view.find("rv") != 0 || !isdigit(isa_view[2]))
            {
                throw InvalidISAStringException(isa_);
            }

            // Remove "rv" prefix
            isa_view.remove_prefix(2);

            try
            {
                size_t num_chars = 0;
                xlen_ = std::stoul(std::string(isa_view), &num_chars);
                // Remove XLEN prefix
                isa_view.remove_prefix(num_chars);
                enabled_arch_ = extensions_.find(xlen_);
                if (enabled_arch_ == extensions_.end())
                {
                    throw std::out_of_range("");
                }
            }
            catch (const std::invalid_argument &)
            {
                throw InvalidISAStringException(isa_, "Could not determine xlen");
            }
            catch (const std::out_of_range &)
            {
                throw InvalidISAStringException(isa_, "Invalid xlen");
            }

            if (isa_view.empty())
            {
                throw InvalidISAStringException(isa_, "Missing base extension");
            }

            auto & xlen_extension = enabled_arch_->second;
            const std::string base_isa(1, isa_view.front());

            xlen_extension.enableBaseExtension(base_isa);

            // Remove base ISA
            isa_view.remove_prefix(1);

            if (!isa_view.empty())
            {
                char ext;

                if (getCharIfValid_(isa_view, ext) && isdigit(ext))
                {
                    try
                    {
                        const auto [major_ver, minor_ver] = extractVersions_(isa_view);
                        xlen_extension.setExtensionVersion(base_isa, major_ver, minor_ver);
                    }
                    catch (const VersionNumberExtractionError &)
                    {
                        throw InvalidISAStringException(
                            isa_, "Invalid version number specified for extension " + base_isa);
                    }
                }

                while (inSingleCharExtRange_(isa_view, ext))
                {
                    if (ext == '_')
                    {
                        isa_view.remove_prefix(1);
                        continue;
                    }

                    const std::string ext_str(1, ext);
                    xlen_extension.enableExtension(ext_str);

                    isa_view.remove_prefix(1);

                    if (inSingleCharExtRange_(isa_view, ext) && isdigit(ext))
                    {
                        try
                        {
                            const auto [major_ver, minor_ver] = extractVersions_(isa_view);
                            xlen_extension.setExtensionVersion(ext_str, major_ver, minor_ver);
                        }
                        catch (const VersionNumberExtractionError &)
                        {
                            throw InvalidISAStringException(
                                isa_, "Invalid version number specified for extension " + ext_str);
                        }
                    }
                }

                while (!isa_view.empty())
                {
                    if (isa_view[0] == '_')
                    {
                        isa_view.remove_prefix(1);
                        continue;
                    }

                    auto current_ext = isa_view.substr(0, isa_view.find('_'));
                    const size_t ext_length = current_ext.size();

                    bool has_version = false;
                    uint32_t major_ver = RISCV_DEFAULT_MAJOR_VER;
                    uint32_t minor_ver = RISCV_DEFAULT_MINOR_VER;

                    if (isdigit(current_ext.back()))
                    {
                        auto ver_pos = current_ext.find_last_not_of(DIGITS_);
                        if (ver_pos == std::string_view::npos)
                        {
                            throw InvalidISAStringException(
                                isa_, "Invalid version number specified for extension "
                                          + std::string(current_ext));
                        }
                        else if (current_ext[ver_pos] == 'p')
                        {
                            if (ver_pos == 0)
                            {
                                throw InvalidISAStringException(
                                    isa_, "Invalid version number specified for extension "
                                              + std::string(current_ext));
                            }

                            ver_pos = current_ext.find_last_not_of(DIGITS_, ver_pos - 1);

                            if (ver_pos == std::string_view::npos)
                            {
                                throw InvalidISAStringException(
                                    isa_, "Invalid version number specified for extension "
                                              + std::string(current_ext));
                            }
                        }

                        ++ver_pos;

                        auto ver_str = current_ext.substr(ver_pos);
                        current_ext.remove_suffix(ver_str.size());

                        try
                        {
                            std::tie(major_ver, minor_ver) = extractVersions_(ver_str);
                        }
                        catch (const VersionNumberExtractionError &)
                        {
                            throw InvalidISAStringException(
                                isa_, "Invalid version number specified for extension "
                                          + std::string(current_ext));
                        }

                        has_version = true;
                    }

                    const std::string ext_str(current_ext);
                    xlen_extension.enableExtension(ext_str);

                    if (has_version)
                    {
                        xlen_extension.setExtensionVersion(ext_str, major_ver, minor_ver);
                    }

                    isa_view.remove_prefix(ext_length);
                }
            }

            xlen_extension.finalize(enabled_extensions_);
        }

      public:
        explicit RISCVExtensionManager(
            const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR) :
            ExtensionManager<RISCVExtensionInfo, RISCVXLENState>(unknown_extension_action)
        {
        }

        static RISCVExtensionManager fromISASpecJSON(
            const std::string & spec_json, const std::string & mavis_json_dir,
            const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
        {
            return fromISASpecJSON_<RISCVExtensionManager>(spec_json, mavis_json_dir,
                                                           unknown_extension_action);
        }

        static RISCVExtensionManager fromISA(
            const std::string & isa, const std::string & spec_json,
            const std::string & mavis_json_dir,
            const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
        {
            return fromISA_<RISCVExtensionManager>(isa, spec_json, mavis_json_dir,
                                                   unknown_extension_action);
        }

        static RISCVExtensionManager fromELF(
            const std::string & elf, const std::string & spec_json,
            const std::string & mavis_json_dir,
            const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
        {
            return fromELF_<RISCVExtensionManager>(elf, spec_json, mavis_json_dir,
                                                   unknown_extension_action);
        }

        uint32_t getXLEN() const { return xlen_; }
    };
} // namespace mavis::extension_manager::riscv
