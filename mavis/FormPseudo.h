#pragma once

#include <string>

namespace mavis
{

    /**
     * PseudoForms
     * @tparam T Form type identifier (const char)
     */
    // Only specialized PseudoForms will compile
    template <const char T> class PseudoForm;

    /**
     * '*' pseudo-form (for the root decode tree node)
     */
    template <> class PseudoForm<'*'>
    {
      public:
        enum idType : uint32_t
        {
            FAMILY = 0,
            __N
        };

      private:
        static inline const Field family_ = {"family", 0, 16};

      public:
        static inline std::string getName() { return "'*' PseudoForm"; }

        static inline const Field & getField(const idType fid)
        {
            assert(fid == idType::FAMILY);
            (void)fid;
            return family_;
        }

        static inline uint64_t extract(const idType fid, const uint64_t icode)
        {
            assert(fid == idType::FAMILY);
            (void)fid;
            return family_.extract(icode);
        }

        static inline void print(std::ostream & os)
        {
            std::ios_base::fmtflags os_state(os.flags());
            os << "Fields of PseudoForm<'*'>:"
               << "\t" << family_ << std::endl;
            os.flags(os_state);
        }
    };

} // namespace mavis
