#pragma once

#include <unordered_map>
#include "DecoderExceptions.h"
#include "GenericRegistryTraits.h"
#include "Utils.h"

namespace mavis
{
    // For each KeyType in KeyTypes..., maps KeyType::name to a smart pointer managing a
    // WrapperType<KeyType> The wrapper type must have a non-templated base type A specialization
    // must be defined for GenericRegistryTraits<WrapperType> as follows:
    // template<> struct
    // GenericRegistryTraits<WrapperType>
    // {
    //     using BaseType = base type;
    //     using PtrType = smart pointer<base type>;
    // };
    // ChildRegistry can be set to a non-void GenericRegistry specialization in order to add
    // additional registrations that might be defined in a different header
    template <typename ChildRegistry, template <typename> typename WrapperType,
              typename... KeyTypes>
    class GenericRegistry
    {
      private:
        using registry_traits = GenericRegistryTraits<WrapperType>;
        using base_type = typename registry_traits::BaseType;
        using ptr_type = typename registry_traits::PtrType;
        using ptr_traits = utils::smart_ptr_traits<ptr_type>;

        static_assert(std::conjunction_v<std::is_base_of<typename ptr_traits::element_type,
                                                         WrapperType<KeyTypes>>...>,
                      "Every specialization of WrapperType<KeyTypes>... must derive from "
                      "ptr_type::element_type");

        GenericRegistry() = default;

      public:
        static const ptr_type & get(const std::string & name)
        {
            static const std::unordered_map<std::string, ptr_type> registry = []
            {
                std::unordered_map<std::string, ptr_type> registry;

                // Calls emplace for every type in KeyTypes
                (registry.emplace(KeyTypes::name,
                                  ptr_traits::template construct<WrapperType<KeyTypes>>()),
                 ...);

                return registry;
            }();

            const auto it = registry.find(name);

            // If there is no child registry, throw an exception
            if constexpr (std::is_void_v<ChildRegistry>)
            {
                if (it == registry.end()) [[unlikely]]
                {
                    throw RegistryNotFoundException();
                }
            }
            // Otherwise, check the child registry
            else
            {
                if (it == registry.end())
                {
                    return ChildRegistry::get(name);
                }
            }

            return it->second;
        }
    };
} // namespace mavis
