#pragma once

#include <algorithm>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Uncomment to enable detection of cycles in the dependency graph
// #define ENABLE_GRAPH_SANITY_CHECKER

#ifdef ENABLE_GRAPH_SANITY_CHECKER
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#endif

// Clang versions prior to 16 had a bug that broke std::views
#if(__clang__ && __clang_major__ < 16)
#define STD_VIEWS_BUG 1
#endif

#include "JSONUtils.hpp"
#include "DecoderExceptions.h"
#include "mavis/Mavis.h"

namespace mavis::extension_manager
{
    class ExtensionManagerException : public std::exception
    {
      private:
        const std::string msg_;

      public:
        explicit ExtensionManagerException(const std::string & msg) : msg_(msg) {}

        const char* what() const noexcept override { return msg_.c_str(); }
    };

    class JSONParseError : public ExtensionManagerException
    {
      public:
        JSONParseError(const std::string & path, const boost::system::error_code & ec) :
            ExtensionManagerException("Error parsing file " + path + ": " + ec.to_string())
        {
        }
    };

    class UninitializedISASpecException : public ExtensionManagerException
    {
      public:
        UninitializedISASpecException() :
            ExtensionManagerException(
                "Attempted to modify extension manager state without calling setISASpecJSON first")
        {
        }
    };

    class UninitializedISAException : public ExtensionManagerException
    {
      public:
        UninitializedISAException() :
            ExtensionManagerException(
                "Attempted to modify extension manager state without calling setISA first")
        {
        }
    };

    class InvalidJSONDirectoryException : public ExtensionManagerException
    {
      public:
        explicit InvalidJSONDirectoryException(const std::string & path) :
            ExtensionManagerException(path + " does not contain Mavis JSONs")
        {
        }
    };

    class InvalidJSONValueException : public ExtensionManagerException
    {
      public:
        explicit InvalidJSONValueException(const std::string & key) :
            ExtensionManagerException("Invalid value for JSON key " + key)
        {
        }
    };

    class MissingRequiredJSONKeyException : public ExtensionManagerException
    {
      public:
        explicit MissingRequiredJSONKeyException(const std::string & key) :
            ExtensionManagerException("Required JSON key " + key + " is missing")
        {
        }
    };

    class MetaExtensionUnexpectedJSONKeyException : public ExtensionManagerException
    {
      public:
        explicit MetaExtensionUnexpectedJSONKeyException(const std::string & key) :
            ExtensionManagerException("Meta-extensions should not contain the " + key + " key")
        {
        }
    };

    class InvalidISAStringException : public ExtensionManagerException
    {
      public:
        explicit InvalidISAStringException(const std::string & isa) :
            ExtensionManagerException("Invalid ISA string specified: " + isa)
        {
        }

        InvalidISAStringException(const std::string & isa, const std::string & message) :
            ExtensionManagerException("Error processing isa string (" + isa + "): " + message)
        {
        }
    };

    class DuplicateExtensionException : public ExtensionManagerException
    {
      public:
        explicit DuplicateExtensionException(const std::string & ext) :
            ExtensionManagerException("Extension " + ext + " has a duplicate definition")
        {
        }
    };

    class UnknownExtensionExceptionBase : public ExtensionManagerException
    {
      public:
        explicit UnknownExtensionExceptionBase(const std::string & msg) :
            ExtensionManagerException(msg)
        {
        }
    };

    class MissingRequiredExtensionException : public ExtensionManagerException
    {
      public:
        MissingRequiredExtensionException(const std::string & ext, const std::string & req_ext) :
            ExtensionManagerException(ext + " extension requires " + req_ext
                                      + " to also be present in the ISA string")
        {
        }
    };

    class ConflictingExtensionException : public ExtensionManagerException
    {
      public:
        ConflictingExtensionException(const std::string & ext, const std::string & conflict_ext) :
            ExtensionManagerException(ext + " extension conflicts with " + conflict_ext)
        {
        }
    };

    template <typename IterableT> static std::string iterableToString(const IterableT & iterable)
    {
        if (iterable.empty())
        {
            return "[]";
        }

        const auto it = iterable.begin();
        return std::accumulate(std::next(it), iterable.end(), '[' + *it,
                               [](std::string val, const std::string & item)
                               { return std::move(val) + ',' + item; })
               + ']';
    }

    class ExtensionNotAllowedException : public ExtensionManagerException
    {
      public:
        ExtensionNotAllowedException(const std::string & ext,
                                     const std::unordered_set<std::string> & allowlist,
                                     const std::unordered_set<std::string> & blocklist) :
            ExtensionManagerException("Attempted to enable " + ext
                                      + " extension, but it is not allowed."
                                        " allowlist = "
                                      + iterableToString(allowlist)
                                      + " blocklist = " + iterableToString(blocklist))
        {
        }
    };

    class SelfReferentialException : public ExtensionManagerException
    {
      public:
        explicit SelfReferentialException(const std::string & ext) :
            ExtensionManagerException("Extension " + ext + " has a dependency on itself")
        {
        }
    };

    class UnresolvedDependencyException : public ExtensionManagerException
    {
      public:
        UnresolvedDependencyException(const std::string & ext, const std::string & dep) :
            ExtensionManagerException("Could not resolve dependency between " + ext + " and " + dep)
        {
        }
    };

    class CyclicDependencyException : public ExtensionManagerException
    {
      public:
        explicit CyclicDependencyException(const std::string & msg) :
            ExtensionManagerException("Dependency " + msg + " is stuck in an enable loop")
        {
        }
    };

    class ELFNotFoundException : public ExtensionManagerException
    {
      public:
        explicit ELFNotFoundException(const std::string & elf) :
            ExtensionManagerException("Could not open ELF " + elf)
        {
        }
    };

    enum class DependencyType
    {
        META,
        ALIAS,
        ENABLES,
        ENABLING
    };

    template <DependencyType dep_type> struct DependencyTraits
    {
        using value_type = std::string;
    };

    template <> struct DependencyTraits<DependencyType::ENABLING>
    {
        using value_type = std::vector<std::string>;
    };

    template <typename DataT> struct JSONVectorConverter
    {
        static std::vector<DataT> get(const boost::json::value & obj)
        {
            if (obj.is_array())
            {
                return boost::json::value_to<std::vector<DataT>>(obj);
            }

            return std::vector<DataT>(1, boost::json::value_to<DataT>(obj));
        }
    };

    template <typename DataT> struct JSONVectorConverter<std::vector<DataT>>
    {
        static std::vector<std::vector<DataT>> get(const boost::json::value & obj)
        {
            std::vector<std::vector<DataT>> result;

            if (obj.is_array())
            {
                for (const auto & elem : obj.as_array())
                {
                    result.emplace_back(JSONVectorConverter<DataT>::get(elem));
                }
            }
            else
            {
                result.emplace_back(std::vector<DataT>(1, boost::json::value_to<DataT>(obj)));
            }

            return result;
        }
    };

    class ExtensionInfoBase;
    using ExtensionInfoBasePtr = std::shared_ptr<ExtensionInfoBase>;

    class ExtensionInfoBase
    {
      private:
        enum class State
        {
            UNSET,             // Extension state has not been set
            DISABLED,          // Extension was explicitly disabled by disableExtension
            ENABLED,           // Extension was explicitly enabled by setISA or enableExtension
            IMPLICITLY_ENABLED // Extension was implicitly enabled via a dependency relationship
        };

        const std::string extension_;
        const std::string json_;
        // Internal-only extensions are Mavis-specific abstractions that express complex extension
        // behavior (e.g. an extension whose members are conditionally enabled based on the presence
        // of other extensions)
        const bool is_internal_extension_;
        State state_ = State::UNSET;
        bool masked_ = false;

        template <State old_state, State new_state> void transitionState_()
        {
            if (state_ == old_state)
            {
                state_ = new_state;
            }
        }

        template <State... states> bool isState_() const { return (... || (state_ == states)); }

      public:
        ExtensionInfoBase(const std::string & ext, const std::string & json,
                          const bool is_internal_extension) :
            extension_(ext),
            json_(json),
            is_internal_extension_(is_internal_extension)
        {
        }

        ExtensionInfoBase(const std::string & ext, const bool is_internal_extension) :
            extension_(ext),
            is_internal_extension_(is_internal_extension)
        {
        }

        const std::string & getExtension() const { return extension_; }

        void setEnabled()
        {
            unmask();
            state_ = State::ENABLED;
        }

        void setImplicitlyEnabled() { transitionState_<State::UNSET, State::IMPLICITLY_ENABLED>(); }

        void clearImplicitlyEnabled()
        {
            transitionState_<State::IMPLICITLY_ENABLED, State::UNSET>();
        }

        bool isEnabled() const
        {
            return !masked_ && isState_<State::ENABLED, State::IMPLICITLY_ENABLED>();
        }

        bool isDisabled() const { return isState_<State::DISABLED>(); }

        void setDisabled()
        {
            unmask();
            state_ = State::DISABLED;
        }

        void reset() { state_ = State::UNSET; }

        void mask() { masked_ = true; }

        void unmask() { masked_ = false; }

        const std::string & getJSON() const { return json_; }

        bool isInternalExtension() const { return is_internal_extension_; }
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
        virtual const std::string & getName() const = 0;
        virtual const std::string & getJSON() const = 0;

        virtual bool isMetaExtension() const { return false; }

        virtual bool isInternalOnly() const { return false; }
    };

    class Extension : public ExtensionBase
    {
      private:
        const ExtensionInfoBasePtr ext_;

      public:
        explicit Extension(const ExtensionInfoBasePtr & ext) : ext_(ext) {}

        const std::string & getName() const override final { return ext_->getExtension(); }

        const std::string & getJSON() const override final { return ext_->getJSON(); }

        bool isInternalOnly() const override final { return ext_->isInternalExtension(); }
    };

    class MetaExtension : public ExtensionBase
    {
      private:
        const std::string ext_;

      public:
        explicit MetaExtension(const std::string & ext) : ext_(ext) {}

        const std::string & getName() const override final { return ext_; }

        const std::string & getJSON() const override final
        {
            static const std::string EMPTY{};
            return EMPTY;
        }

        bool isMetaExtension() const override final { return true; }
    };

    class ExtensionMapView;

    class ExtensionMap
    {
      private:
        friend class ExtensionMapView;

        using EnabledMap = std::unordered_map<std::string, std::unique_ptr<ExtensionBase>>;
        EnabledMap enabled_extensions_;
        mutable std::set<std::string> sorted_enabled_extensions_set_;
        mutable std::string sorted_enabled_extensions_str_;

        void enable_(const std::string & extension, std::unique_ptr<ExtensionBase> && ext_ptr)
        {
            enabled_extensions_.emplace(extension, std::move(ext_ptr));
            sorted_enabled_extensions_set_.emplace(extension);
        }

        // Returns false if an extension is internal-only or (optionally) if it's a meta-extension
        static bool filterExt_(const EnabledMap::value_type & ext_pair, const bool include_meta)
        {
            const auto & ext = ext_pair.second;
            return !ext->isInternalOnly() && (include_meta || !ext->isMetaExtension());
        }

#ifdef STD_VIEWS_BUG
        // Mimics the behavior of std::views::filter
        class FilteredView
        {
            public:
                class iterator
                {
                    private:
                        const bool include_meta_;
                        const EnabledMap::const_iterator end_it_;
                        EnabledMap::const_iterator it_;

                        void advance_()
                        {
                            while(it_ != end_it_ && !filterExt_(*it_, include_meta_))
                            {
                                ++it_;
                            }
                        }

                    public:
                        iterator(const EnabledMap::const_iterator& it, const EnabledMap::const_iterator& end_it, const bool include_meta) :
                            include_meta_(include_meta),
                            end_it_(end_it),
                            it_(it)
                        {
                            advance_();
                        }

                        explicit iterator(const EnabledMap::const_iterator& end_it) :
                            include_meta_(false),
                            end_it_(end_it),
                            it_(end_it)
                        {
                        }

                        iterator& operator++()
                        {
                            ++it_;
                            advance_();
                            return *this;
                        }

                        iterator operator++(int)
                        {
                            const auto temp = *this;
                            ++it_;
                            advance_();
                            return temp;
                        }

                        const EnabledMap::value_type& operator*() const
                        {
                            return *it_;
                        }

                        const EnabledMap::value_type* operator->() const
                        {
                            return it_.operator->();
                        }

                        bool operator==(const iterator& rhs) const
                        {
                            return it_ == rhs.it_;
                        }

                        bool operator!=(const iterator& rhs) const
                        {
                            return it_ != rhs.it_;
                        }
                };

            private:
                const EnabledMap& enabled_extensions_;
                const bool include_meta_;

            public:
                explicit FilteredView(const EnabledMap& enabled_extensions, const bool include_meta) :
                    enabled_extensions_(enabled_extensions),
                    include_meta_(include_meta)
                {
                }

                iterator begin() const
                {
                    return iterator(enabled_extensions_.begin(), enabled_extensions_.end(), include_meta_);
                }

                iterator end() const
                {
                    return iterator(enabled_extensions_.end());
                }
        };
#endif

        // Returns a lazy view into enabled_extensions_ that filters out internal-only extensions
        // Optionally also filters meta-extensions
        auto getFilteredView_(const bool include_meta) const
        {
#ifdef STD_VIEWS_BUG
            return FilteredView(enabled_extensions_, include_meta);
#else
            return std::views::filter(enabled_extensions_, [include_meta](const auto & ext_pair)
                                      { return filterExt_(ext_pair, include_meta); });
#endif
        }

        // Returns true if the extension is enabled and not filtered
        bool isEnabledFiltered_(const std::string & ext, const bool include_meta) const
        {
            const auto it = enabled_extensions_.find(ext);
            return it != enabled_extensions_.end() && filterExt_(*it, include_meta);
        }

      public:
        void enableExtension(const ExtensionInfoBasePtr & ext_info)
        {
            enable_(ext_info->getExtension(), std::make_unique<Extension>(ext_info));
        }

        void enableMetaExtension(const std::string & extension)
        {
            enable_(extension, std::make_unique<MetaExtension>(extension));
        }

        const std::string & getSortedExtensions() const
        {
            if (sorted_enabled_extensions_str_.empty())
            {
                for (const auto & ext : sorted_enabled_extensions_set_)
                {
                    sorted_enabled_extensions_str_ += ext;
                }

                sorted_enabled_extensions_set_.clear();
            }

            return sorted_enabled_extensions_str_;
        }

        bool isEnabled(const std::string & ext) const { return enabled_extensions_.contains(ext); }

        void clear()
        {
            enabled_extensions_.clear();
            sorted_enabled_extensions_set_.clear();
            sorted_enabled_extensions_str_.clear();
        }

        auto begin() const { return enabled_extensions_.begin(); }

        auto end() const { return enabled_extensions_.end(); }
    };

    // Provides a read-only view into an ExtensionMap that filters out internal-only extensions
    // Invalidated whenever the underlying ExtensionMap is modified
    class ExtensionMapView
    {
      private:
        using ViewType =
            std::invoke_result_t<decltype(&ExtensionMap::getFilteredView_), ExtensionMap, bool>;

        const ExtensionMap & ext_map_;
        const bool include_meta_;
        mutable ViewType map_filter_{ext_map_.getFilteredView_(include_meta_)};

      public:
        ExtensionMapView(const ExtensionMap & ext_map, const bool include_meta) :
            ext_map_(ext_map),
            include_meta_(include_meta)
        {
        }

        // Returns whether the specified extension is enabled and non-internal
        bool isEnabled(const std::string & ext) const
        {
            return ext_map_.isEnabledFiltered_(ext, include_meta_);
        }

        auto begin() const { return map_filter_.begin(); }

        auto end() const { return map_filter_.end(); }
    };

    enum class RuleType
    {
        REQUIRED,
        CONFLICT
    };

    class Rule
    {
      private:
        const RuleType type_;
        const std::string src_;
        const std::string dest_;

      public:
        Rule(const RuleType type, const std::string & src, const std::string & dest) :
            type_(type),
            src_(src),
            dest_(dest)
        {
            if (src_ == dest_)
            {
                throw SelfReferentialException(src_);
            }
        }

        void check(const ExtensionMap & enabled_extensions) const
        {
            const bool src_is_enabled = enabled_extensions.isEnabled(src_);
            const bool dest_is_enabled = enabled_extensions.isEnabled(dest_);

            switch (type_)
            {
                case RuleType::REQUIRED:
                    if (src_is_enabled && !dest_is_enabled)
                    {
                        throw MissingRequiredExtensionException(src_, dest_);
                    }
                    break;
                case RuleType::CONFLICT:
                    if (src_is_enabled && dest_is_enabled)
                    {
                        throw ConflictingExtensionException(src_, dest_);
                    }
                    break;
            }
        }

        RuleType getType() const { return type_; }

        const std::string & getSource() const { return src_; }

        const std::string & getDest() const { return dest_; }
    };

    template <typename ExtensionInfo> class ExtensionStateBase
    {
      private:
        using ExtensionInfoPtr = std::shared_ptr<ExtensionInfo>;

        // Represents a dependency where one extension implicitly enables another extension
        class EnablesDependency
        {
          private:
            const std::string src_;
            const std::string dest_;

          public:
            EnablesDependency(const std::string & src, const std::string & dest) :
                src_(src),
                dest_(dest)
            {
                if (src_ == dest_)
                {
                    throw SelfReferentialException(src_);
                }
            }

            // Returns true if there was a change to the enabled extensions
            bool process(const std::unordered_map<std::string, ExtensionInfoPtr> & extensions) const
            {
                try
                {
                    const auto & ext_ptr = extensions.at(dest_);
                    const bool already_enabled = ext_ptr->isEnabled();

                    if (extensions.at(src_)->isEnabled())
                    {
                        ext_ptr->setImplicitlyEnabled();
                    }

                    return already_enabled != ext_ptr->isEnabled();
                }
                catch (const std::out_of_range &)
                {
                    throw UnresolvedDependencyException(src_, dest_);
                }
            }

            std::string stringize() const { return "enables: " + src_ + " -> " + dest_; }
        };

        // Represents a dependency where one extension is implicitly enabled if a combination of
        // other extensions are enabled
        class EnablingDependency
        {
          private:
            const std::vector<std::string> enabling_extensions_;
            const std::string enabled_extension_;

            class UnknownDependencyException : public std::exception
            {
              private:
                const std::string unknown_ext_;

              public:
                explicit UnknownDependencyException(const std::string & unknown_ext) :
                    unknown_ext_(unknown_ext)
                {
                }

                const char* what() const noexcept override { return unknown_ext_.c_str(); }

                const std::string & getExtension() const { return unknown_ext_; }
            };

          public:
            EnablingDependency(const std::vector<std::string> & enabling_extensions,
                               const std::string & enabled_extension) :
                enabling_extensions_(enabling_extensions),
                enabled_extension_(enabled_extension)
            {
                if (std::find(enabling_extensions_.begin(), enabling_extensions_.end(),
                              enabled_extension_)
                    != enabling_extensions_.end())
                {
                    throw SelfReferentialException(enabled_extension_);
                }
            }

            bool process(
                const std::unordered_map<std::string, std::vector<std::string>> & meta_extensions,
                const std::unordered_set<std::string> & enabled_meta_extensions,
                const std::unordered_map<std::string, ExtensionInfoPtr> & extensions) const
            {
                const auto & ext_ptr = extensions.at(enabled_extension_);
                const bool already_enabled = ext_ptr->isEnabled();

                try
                {
                    if (std::all_of(enabling_extensions_.begin(), enabling_extensions_.end(),
                                    [&meta_extensions, &enabled_meta_extensions,
                                     &extensions](const std::string & enabling_extension)
                                    {
                                        if (enabled_meta_extensions.count(enabling_extension) != 0)
                                        {
                                            return true;
                                        }

                                        if (const auto it = extensions.find(enabling_extension);
                                            it != extensions.end())
                                        {
                                            return it->second->isEnabled();
                                        }
                                        // If we get here then this must be a disabled meta
                                        // extension. Otherwise we don't know what the extension is
                                        // and it's an error
                                        else if (meta_extensions.count(enabling_extension) == 0)
                                        {
                                            throw UnknownDependencyException(enabling_extension);
                                        }

                                        return false;
                                    }))
                    {
                        ext_ptr->setImplicitlyEnabled();
                    }
                }
                catch (const UnknownDependencyException & ex)
                {
                    throw UnresolvedDependencyException(ex.getExtension(), enabled_extension_);
                }

                return already_enabled != ext_ptr->isEnabled();
            }

            std::string stringize() const
            {
                return "enabled_by: " + iterableToString(enabling_extensions_) + " -> "
                       + enabled_extension_;
            }
        };

        virtual void throwUnknownExtensionException_(const std::string & extension) const
        {
            throw UnknownExtensionExceptionBase("Unknown extension specified: " + extension);
        }

        bool handleUnknownExtensionException_(const std::string & ext) const
        {
            switch (unknown_extension_action_)
            {
                case UnknownExtensionAction::ERROR:
                    return true;
                case UnknownExtensionAction::WARN:
                    std::cerr << "WARNING: ISA string contains an unknown extension (" << ext
                              << "). Ignoring." << std::endl;
                    break;
                case UnknownExtensionAction::IGNORE:
                    break;
            }

            return false;
        }

        template <typename ContainerType>
        std::vector<ExtensionInfoPtr> getExtensions_(const ContainerType & ext_strs)
        {
            std::vector<ExtensionInfoPtr> exts;

            for (const auto & ext : ext_strs)
            {
                const auto it = extensions_.find(ext);
                if (it == extensions_.end())
                {
                    break;
                }
                exts.emplace_back(it->second);
            }

            return exts;
        }

        bool extensionAllowed_(const std::string & ext) const
        {
            if (!extension_allowlist_.empty() && extension_allowlist_.count(ext) == 0)
            {
                return false;
            }

            if (extension_blocklist_.count(ext) != 0)
            {
                return false;
            }

            return true;
        }

      protected:
        const ExtensionInfoPtr & getExtensionInfo_(const std::string & extension) const
        {
            try
            {
                return extensions_.at(extension);
            }
            catch (const std::out_of_range &)
            {
                throwUnknownExtensionException_(extension);
            }

            __builtin_unreachable();
        }

        template <typename Callback>
        bool recurseExtension_(const std::string & extension, Callback && callback)
        {
            if (auto it = meta_extensions_.find(extension); it != meta_extensions_.end())
            {
                for (const auto & child : it->second)
                {
                    callback(child);
                }

                return true;
            }
            else if (auto alias_it = aliases_.find(extension); alias_it != aliases_.end())
            {
                callback(alias_it->second);

                return true;
            }

            return false;
        }

        template <typename UnaryOp>
        void iterateDependentExtensions_(const std::string & ext, UnaryOp && op)
        {
            const auto & required_rules = rules_[RuleType::REQUIRED];

            std::for_each(required_rules.begin(), required_rules.end(),
                          [this, &ext, &op](const Rule & rule)
                          {
                              if (rule.getDest() == ext)
                              {
                                  op(getExtensionInfo_(rule.getSource()));
                              }
                          });
        }

#ifdef ENABLE_GRAPH_SANITY_CHECKER
        using DepGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;

        DepGraph::vertex_descriptor getVertex_(const std::string & ext)
        {
            auto it = dep_graph_vertices_.find(ext);

            if (it == dep_graph_vertices_.end())
            {
                it = dep_graph_vertices_.emplace(ext, boost::add_vertex(dependency_graph_)).first;
            }

            return it->second;
        }

        class cycle_detector : public boost::dfs_visitor<>
        {
          private:
            bool has_cycle_ = false;

          public:
            template <class Edge, class Graph> void back_edge(Edge, Graph &) { has_cycle_ = true; }

            bool cycleDetected() const { return has_cycle_; }
        };

        void addDependencyGraphEdge_(const std::string & ext, const std::string & dep)
        {
            boost::add_edge(getVertex_(ext), getVertex_(dep), dependency_graph_);
            cycle_detector vis;
            boost::depth_first_search(dependency_graph_, visitor(vis));
            if (vis.cycleDetected())
            {
                throw ExtensionManagerException("Cycle detected after adding dependency " + ext
                                                + " -> " + dep);
            }
        }
#endif

        const uint32_t arch_id_;
        const UnknownExtensionAction unknown_extension_action_;
        std::unordered_map<std::string, ExtensionInfoPtr> extensions_;
        std::unordered_map<std::string, std::vector<std::string>> meta_extensions_;
        std::unordered_map<std::string, bool> config_extensions_;
        std::unordered_map<std::string, std::string> aliases_;
        std::unordered_set<std::string> enabled_meta_extensions_;
        std::unordered_set<std::string> extension_allowlist_;
        std::unordered_set<std::string> extension_blocklist_;
        std::unordered_map<RuleType, std::vector<Rule>> rules_;
        std::vector<EnablesDependency> enables_dependencies_;
        std::vector<EnablingDependency> enabling_dependencies_;

#ifdef ENABLE_GRAPH_SANITY_CHECKER
        DepGraph dependency_graph_;
        std::unordered_map<std::string, DepGraph::vertex_descriptor> dep_graph_vertices_;
#endif

      public:
        ExtensionStateBase(const uint32_t arch_id,
                           const UnknownExtensionAction unknown_extension_action) :
            arch_id_(arch_id),
            unknown_extension_action_(unknown_extension_action)
        {
        }

        const ExtensionInfo & getExtensionInfo(const std::string & extension) const
        {
            return *getExtensionInfo_(extension);
        }

        void addMetaExtension(const std::string & ext) { meta_extensions_.try_emplace(ext); }

        template <typename... ExtensionArgs>
        ExtensionInfo & addExtension(const std::string & extension, ExtensionArgs &&... args)
        {
            if (const auto it = extensions_.find(extension); it != extensions_.end())
            {
                throw DuplicateExtensionException(extension);
            }

            const auto result = extensions_.emplace(
                extension,
                std::make_shared<ExtensionInfo>(extension, std::forward<ExtensionArgs>(args)...));

#ifdef ENABLE_GRAPH_SANITY_CHECKER
            getVertex_(extension);
#endif
            return *result.first->second;
        }

        void addConfigExtension(const std::string & ext) { config_extensions_.emplace(ext, false); }

        template <DependencyType type>
        void addDependency(const std::string & extension, const std::string & dep)
        {
            const auto & ext_ptr = getExtensionInfo_(extension);

            static_assert(type == DependencyType::ENABLES || type == DependencyType::ENABLING);

#ifdef ENABLE_GRAPH_SANITY_CHECKER
            if constexpr (type == DependencyType::ENABLES)
            {
                addDependencyGraphEdge_(dep, extension);
            }
            else
            {
                addDependencyGraphEdge_(extension, dep);
            }
#else
            (void)dep;
#endif

        }

        template <DependencyType type>
        void addDependency(const std::string & extension, const std::vector<std::string> & deps)
        {
            if constexpr (type == DependencyType::META)
            {
                for (const auto & meta_extension : deps)
                {
                    try
                    {
                        meta_extensions_.at(meta_extension).emplace_back(extension);
                    }
                    catch (const std::out_of_range &)
                    {
                        throwUnknownExtensionException_(meta_extension);
                    }
#ifdef ENABLE_GRAPH_SANITY_CHECKER
                    addDependencyGraphEdge_(extension, meta_extension);
#endif
                }
            }
            else if constexpr (type == DependencyType::ALIAS)
            {
                for (const auto & alias : deps)
                {
                    aliases_.emplace(alias, extension);
                }
            }
            else
            {
                for (const auto & dep_ext : deps)
                {
                    if constexpr (type == DependencyType::ENABLES)
                    {
                        enables_dependencies_.emplace_back(extension, dep_ext);
#ifdef ENABLE_GRAPH_SANITY_CHECKER
                        addDependencyGraphEdge_(dep_ext, extension);
#endif
                    }
#ifdef ENABLE_GRAPH_SANITY_CHECKER
                    else
                    {
                        addDependencyGraphEdge_(extension, dep_ext);
                    }
#endif
                }

                if constexpr (type == DependencyType::ENABLING)
                {
                    enabling_dependencies_.emplace_back(deps, extension);
                }
            }
        }

        template <DependencyType type>
        void addDependency(const std::string & extension,
                           const std::vector<std::vector<std::string>> & deps)
        {
            static_assert(type == DependencyType::ENABLING);

            for (const auto & dep : deps)
            {
                addDependency<type>(extension, dep);

#ifdef ENABLE_GRAPH_SANITY_CHECKER
                for (const auto & dep_ext : dep)
                {
                    addDependencyGraphEdge_(extension, dep_ext);
                }
#endif
            }
        }

        template <RuleType rule>
        void addRule(const std::string & extension, const std::string & dep)
        {
            rules_[rule].emplace_back(rule, extension, dep);
        }

        template <RuleType rule>
        void addRule(const std::string & extension, const std::vector<std::string> & deps)
        {
            for (const auto & dep : deps)
            {
                addRule<rule>(extension, dep);
            }
        }

        void allowExtension(const std::string & extension)
        {
            recurseExtension_(extension,
                              [this](const std::string & child) { allowExtension(child); });

            extension_blocklist_.erase(extension);
            extension_allowlist_.emplace(extension);
        }

        void clearAllowedExtensions() { extension_allowlist_.clear(); }

        void blockExtension(const std::string & extension)
        {
            recurseExtension_(extension,
                              [this](const std::string & child) { blockExtension(child); });

            extension_allowlist_.erase(extension);
            extension_blocklist_.emplace(extension);
        }

        void clearBlockedExtensions() { extension_blocklist_.clear(); }

        bool enableExtension(const std::string & ext)
        {
            if (!extensionAllowed_(ext))
            {
                throw ExtensionNotAllowedException(ext, extension_allowlist_, extension_blocklist_);
            }

            bool is_meta = true;
            bool is_unknown = false;
            if (recurseExtension_(ext,
                                  [this](const std::string & child) { enableExtension(child); }))
            {
                // It was either a meta extension or an alias
            }
            else if (auto config_it = config_extensions_.find(ext);
                     config_it != config_extensions_.end())
            {
                config_it->second = true;
            }
            else
            {
                try
                {
                    const auto & ext_info = getExtensionInfo_(ext);

                    // If this extension was explicitly disabled, we need
                    // to unmask any other extensions that require it
                    if (ext_info->isDisabled())
                    {
                        iterateDependentExtensions_(ext,
                                                    [](const ExtensionInfoBasePtr & dep_ext_info)
                                                    { dep_ext_info->unmask(); });
                    }

                    ext_info->setEnabled();
                    is_meta = false;
                }
                catch (const UnknownExtensionExceptionBase &)
                {
                    if (handleUnknownExtensionException_(ext))
                    {
                        throw;
                    }
                    is_unknown = true;
                    is_meta = false;
                }
            }

            if (is_meta)
            {
                enabled_meta_extensions_.emplace(ext);
            }
            return is_unknown;
        }

        void disableExtension(const std::string & ext)
        {
            bool is_meta = true;
            if (recurseExtension_(ext,
                                  [this](const std::string & child) { disableExtension(child); }))
            {
                // It was either a meta extension or an alias
            }
            else if (auto config_it = config_extensions_.find(ext);
                     config_it != config_extensions_.end())
            {
                config_it->second = false;
            }
            else
            {
                try
                {
                    // If any extensions require this extension to be present,
                    // they need to be masked
                    iterateDependentExtensions_(ext, [](const ExtensionInfoBasePtr & dep_ext_info)
                                                { dep_ext_info->mask(); });

                    getExtensionInfo_(ext)->setDisabled();
                    is_meta = false;
                }
                catch (const UnknownExtensionExceptionBase &)
                {
                    if (handleUnknownExtensionException_(ext))
                    {
                        throw;
                    }
                }
            }

            if (is_meta)
            {
                enabled_meta_extensions_.erase(ext);
            }
        }

        void enableExtension(const char ext) { enableExtension(std::string(1, ext)); }

        void clearImplicitlyEnabled()
        {
            for (const auto & extension : extensions_)
            {
                extension.second->clearImplicitlyEnabled();
            }
        }

        void finalize(ExtensionMap & enabled_extensions)
        {
            // When finalize() is called, the extension manager has already *explicitly*
            // enabled/disabled extensions via setISA, enableExtension, or disableExtension. Now we
            // just need to scan for extensions that are *implicitly* enabled by those extensions
            bool keep_going;

            std::unordered_set<const EnablesDependency*> activated_enables_dependencies;
            std::unordered_set<const EnablingDependency*> activated_enabling_dependencies;

            do
            {
                keep_going = false;

                for (const auto & dep : enables_dependencies_)
                {
                    const bool dependency_activated = dep.process(extensions_);

                    // Track every time a dependency is activated for the first time
                    // Throw an exception if one gets activated twice to avoid an infinite loop
                    if (dependency_activated
                        && !activated_enables_dependencies.emplace(&dep).second)
                    {
                        throw CyclicDependencyException(dep.stringize());
                    }

                    keep_going |= dependency_activated;
                }

                for (const auto & dep : enabling_dependencies_)
                {
                    const bool dependency_activated =
                        dep.process(meta_extensions_, enabled_meta_extensions_, extensions_);

                    // Track every time a dependency is activated for the first time
                    // Throw an exception if one gets activated twice to avoid an infinite loop
                    if (dependency_activated
                        && !activated_enabling_dependencies.emplace(&dep).second)
                    {
                        throw CyclicDependencyException(dep.stringize());
                    }

                    keep_going |= dependency_activated;
                }
            } while (keep_going);

            for (const auto & ext : extensions_)
            {
                const auto & ext_info = ext.second;
                if (ext_info->isEnabled())
                {
                    enabled_extensions.enableExtension(ext_info);
                }
            }

            for (const auto & ext : enabled_meta_extensions_)
            {
                enabled_extensions.enableMetaExtension(ext);
            }

            // Finally, check for any rule violations
            for (const auto & rule_vector : rules_)
            {
                for (const auto & rule : rule_vector.second)
                {
                    rule.check(enabled_extensions);
                }
            }
        }

        void reset()
        {
            for (const auto & ext : extensions_)
            {
                ext.second->reset();
            }

            for (auto & ext : config_extensions_)
            {
                ext.second = false;
            }

            enabled_meta_extensions_.clear();
        }

        bool isDisabled(const std::string & ext) const { return extensions_.at(ext)->isDisabled(); }

        bool isSupported(const std::string & ext) const
        {
            return extensions_.count(ext) != 0 || meta_extensions_.count(ext) != 0;
        }
    };

    template <typename ExtensionInfo, typename ExtensionState> class ExtensionManager
    {
      protected:
        const UnknownExtensionAction unknown_extension_action_;
        std::string mavis_json_dir_;
        std::string isa_;
        std::string unknown_extensions_;
        using ArchMap = std::unordered_map<uint32_t, ExtensionState>;
        ArchMap extensions_;
        typename ArchMap::iterator enabled_arch_{extensions_.end()};
        ExtensionMap enabled_extensions_;
        mutable std::vector<std::string> enabled_jsons_;
        std::function<void(const std::vector<std::string> &, bool)> extensions_changed_callback_;

        // Stores the initial settings for a Mavis instance so they can be retrieved later
        // when switching contexts
        class MavisSettingsRegistry
        {
          public:
            struct MavisSettings
            {
                const std::string default_context;
                const FileNameListType anno_files;
                const InstUIDList uid_list;
                const AnnotationOverrides anno_overrides;
                const MatchSet<Pattern> inclusions;
                const MatchSet<Pattern> exclusions;
            };

          private:
            std::unordered_map<std::type_index, std::unordered_map<uint64_t, MavisSettings>>
                mavis_settings_;

          public:
            template <typename InstType, typename AnnotationType, typename InstTypeAllocator,
                      typename AnnotationTypeAllocator>
            void emplace(const Mavis<InstType, AnnotationType, InstTypeAllocator,
                                     AnnotationTypeAllocator> & mavis,
                         const std::string & default_context, const FileNameListType & anno_files,
                         const InstUIDList & uid_list, const AnnotationOverrides & anno_overrides,
                         const MatchSet<Pattern> & inclusions, const MatchSet<Pattern> & exclusions)
            {
                mavis_settings_[std::type_index(
                                    typeid(Mavis<InstType, AnnotationType, InstTypeAllocator,
                                                 AnnotationTypeAllocator>))]
                    .emplace(mavis.getUID(), MavisSettings{default_context, anno_files, uid_list,
                                                           anno_overrides, inclusions, exclusions});
            }

            template <typename InstType, typename AnnotationType, typename InstTypeAllocator,
                      typename AnnotationTypeAllocator>
            const MavisSettings & get(const Mavis<InstType, AnnotationType, InstTypeAllocator,
                                                  AnnotationTypeAllocator> & mavis) const
            {
                return mavis_settings_
                    .at(std::type_index(typeid(Mavis<InstType, AnnotationType, InstTypeAllocator,
                                                     AnnotationTypeAllocator>)))
                    .at(mavis.getUID());
            }
        };

        mutable MavisSettingsRegistry mavis_settings_;

        enum class ExtensionType
        {
            META,
            CONFIG,
            NORMAL
        };

        virtual void processArchSpecificExtensionInfo_(ExtensionState &, const std::string &,
                                                       const boost::json::object &,
                                                       const ExtensionType) const
        {
        }

        virtual uint32_t
        convertMultiArchString_(const boost::json::string & multiarch_str) const = 0;

        virtual std::vector<uint32_t>
        convertMultiArchVector_(const boost::json::array & multiarch_str_vec) const
        {
            std::vector<uint32_t> arches;
            arches.resize(multiarch_str_vec.size());

            std::transform(multiarch_str_vec.begin(), multiarch_str_vec.end(), arches.begin(),
                           [this](const boost::json::value & multiarch_str)
                           { return convertMultiArchString_(multiarch_str.as_string()); });

            return arches;
        }

        virtual std::vector<uint32_t>
        getMultiArchVector_(const boost::json::value & multiarch_obj) const
        {
            if (multiarch_obj.is_array())
            {
                const auto & multiarch_array = multiarch_obj.as_array();

                if (!multiarch_array.empty())
                {
                    if (multiarch_array.front().is_string())
                    {
                        return convertMultiArchVector_(multiarch_array);
                    }
                    else if (multiarch_array.front().is_number())
                    {
                        return boost::json::value_to<std::vector<uint32_t>>(multiarch_obj);
                    }
                }

                throw InvalidJSONValueException(getMultiArchKey_());
            }
            else
            {
                return std::vector<uint32_t>(1, boost::json::value_to<uint32_t>(multiarch_obj));
            }
        }

        virtual const char* getMultiArchKey_() const { return nullptr; }

        template <typename ManagerType>
        static ManagerType fromELF_(
            const std::string & elf, const std::string & spec_json,
            const std::string & mavis_json_dir,
            const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
        {
            auto man =
                fromISASpecJSON_<ManagerType>(spec_json, mavis_json_dir, unknown_extension_action);
            man.setISAFromELF(elf);
            return man;
        }

        template <typename ManagerType>
        static ManagerType fromISA_(
            const std::string & isa, const std::string & spec_json,
            const std::string & mavis_json_dir,
            const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
        {
            auto man =
                fromISASpecJSON_<ManagerType>(spec_json, mavis_json_dir, unknown_extension_action);
            man.setISA(isa);
            return man;
        }

        template <typename ManagerType>
        static ManagerType fromISASpecJSON_(
            const std::string & spec_json, const std::string & mavis_json_dir,
            const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR)
        {
            ManagerType man(unknown_extension_action);
            man.setISASpecJSON(spec_json, mavis_json_dir);
            return man;
        }

        virtual std::string getISAFromELF_(const std::string & elf) const = 0;

        virtual void setISASpecJSONImpl_(const std::string &) {};

        virtual void setISAImpl_(const std::string & isa) = 0;

      private:
        template <bool is_normal_extension, DependencyType dep_type>
        constexpr static bool isDependencyAllowed_()
        {
            switch (dep_type)
            {
                case DependencyType::META:
                    return true;
                case DependencyType::ALIAS:
                case DependencyType::ENABLES:
                case DependencyType::ENABLING:
                    return is_normal_extension;
            };
        }

        template <DependencyType dep_type> constexpr static const char* getDependencyKey_()
        {
            switch (dep_type)
            {
                case DependencyType::META:
                    return "meta_extension";
                case DependencyType::ALIAS:
                    return "aliases";
                case DependencyType::ENABLES:
                    return "enables";
                case DependencyType::ENABLING:
                    return "enabled_by";
            };
        }

        template <DependencyType dep_type>
        static std::vector<typename DependencyTraits<dep_type>::value_type>
        getDependencyValue_(const boost::json::value & obj)
        {
            return JSONVectorConverter<typename DependencyTraits<dep_type>::value_type>::get(obj);
        }

        template <bool is_normal_extension, RuleType rule_type>
        constexpr static bool isRuleAllowed_()
        {
            switch (rule_type)
            {
                case RuleType::REQUIRED:
                case RuleType::CONFLICT:
                    return is_normal_extension;
            };
        }

        template <RuleType rule_type> constexpr static const char* getRuleKey_()
        {
            switch (rule_type)
            {
                case RuleType::REQUIRED:
                    return "requires";
                case RuleType::CONFLICT:
                    return "conflicts";
            };
        }

        template <RuleType rule_type>
        static std::vector<std::string> getRuleValue_(const boost::json::value & obj)
        {
            return JSONVectorConverter<std::string>::get(obj);
        }

        template <bool is_normal_extension, DependencyType dep_type>
        static void processOptionalDependency_(ExtensionState & extensions, const std::string & ext,
                                               const boost::json::object & obj)
        {
            constexpr const char* key = getDependencyKey_<dep_type>();

            if (const auto it = obj.find(key); it != obj.end())
            {
                if constexpr (!isDependencyAllowed_<is_normal_extension, dep_type>())
                {
                    throw MetaExtensionUnexpectedJSONKeyException(key);
                }

                const auto dep_value = getDependencyValue_<dep_type>(it->value());
                extensions.template addDependency<dep_type>(ext, dep_value);

                // Add an implicit required: rule for any enables: rule. This makes
                // it possible to track dependencies properly when enabling/disabling
                // extensions on the fly
                if constexpr (dep_type == DependencyType::ENABLES)
                {
                    extensions.template addRule<RuleType::REQUIRED>(ext, dep_value);
                }
            }
        }

        template <bool is_normal_extension, RuleType rule_type>
        static void processRule_(ExtensionState & extensions, const std::string & ext,
                                 const boost::json::object & obj)
        {
            constexpr const char* key = getRuleKey_<rule_type>();

            if (const auto it = obj.find(key); it != obj.end())
            {
                if constexpr (!isRuleAllowed_<is_normal_extension, rule_type>())
                {
                    throw MetaExtensionUnexpectedJSONKeyException(key);
                }

                extensions.template addRule<rule_type>(ext, getRuleValue_<rule_type>(it->value()));
            }
        }

        template <typename T>
        static T getRequiredJSONValue_(const boost::json::value & jobj, const std::string & key)
        {
            try
            {
                return boost::json::value_to<T>(jobj.at(key));
            }
            catch (const boost::system::system_error &)
            {
                throw MissingRequiredJSONKeyException(key);
            }
        }

        template <ExtensionType extension_type>
        void processExtension_(const boost::json::object & ext_obj)
        {
            static constexpr bool is_normal_extension = extension_type == ExtensionType::NORMAL;
            static constexpr bool is_config_extension = extension_type == ExtensionType::CONFIG;
            static constexpr bool is_meta_extension = extension_type == ExtensionType::META;

            const std::string ext = getRequiredJSONValue_<std::string>(ext_obj, "extension");

            std::vector<uint32_t> arches;

            const char* multiarch_key = getMultiArchKey_();

            if (multiarch_key)
            {
                if (const auto it = ext_obj.find(multiarch_key); it != ext_obj.end())
                {
                    arches = getMultiArchVector_(it->value());
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

            for (const auto arch : arches)
            {
                auto & arch_extensions =
                    extensions_.try_emplace(arch, arch, unknown_extension_action_).first->second;

                if constexpr (is_normal_extension)
                {
                    bool internal_extension = false;

                    if (auto json_it = ext_obj.find("internal"); json_it != ext_obj.end())
                    {
                        internal_extension = json_it->value().as_bool();
                    }

                    if (auto json_it = ext_obj.find("json"); json_it != ext_obj.end())
                    {
                        arch_extensions.addExtension(
                            ext, std::string(json_it->value().as_string().c_str()),
                            internal_extension);
                    }
                    else
                    {
                        arch_extensions.addExtension(ext, internal_extension);
                    }
                }
                else if constexpr (is_config_extension)
                {
                    arch_extensions.addConfigExtension(ext);
                }
                else if constexpr (is_meta_extension)
                {
                    arch_extensions.addMetaExtension(ext);
                }

                processArchSpecificExtensionInfo_(arch_extensions, ext, ext_obj, extension_type);
                processOptionalDependency_<is_normal_extension, DependencyType::META>(
                    arch_extensions, ext, ext_obj);
                processOptionalDependency_<is_normal_extension, DependencyType::ALIAS>(
                    arch_extensions, ext, ext_obj);
                processOptionalDependency_<is_normal_extension, DependencyType::ENABLES>(
                    arch_extensions, ext, ext_obj);
                processOptionalDependency_<is_normal_extension, DependencyType::ENABLING>(
                    arch_extensions, ext, ext_obj);
                processRule_<is_normal_extension, RuleType::REQUIRED>(arch_extensions, ext,
                                                                      ext_obj);
                processRule_<is_normal_extension, RuleType::CONFLICT>(arch_extensions, ext,
                                                                      ext_obj);
            }
        }

        void assertISASpecInitialized_() const
        {
            if (extensions_.empty())
            {
                throw UninitializedISASpecException();
            }
        } 

        void assertISAInitialized_() const
        {
            if (enabled_arch_ == extensions_.end())
            {
                throw UninitializedISAException();
            }
        }

        void refresh_()
        {
            assertISAInitialized_();
            enabled_extensions_.clear();
            enabled_jsons_.clear();
            enabled_arch_->second.clearImplicitlyEnabled();
            enabled_arch_->second.finalize(enabled_extensions_);
        }

        template <bool refresh = true> void enableExtension_(const std::string & ext)
        {
            if (!isEnabled(ext))
            {

                const bool was_unknown = enabled_arch_->second.enableExtension(ext);
                if(was_unknown){
                        unknown_extensions_ += ext; 
                }
                if constexpr (refresh)
                {
                    refresh_();
                    if (extensions_changed_callback_)
                    {
                        extensions_changed_callback_({ext}, true);
                    }
                }
            }
        }

        template <bool refresh = true> void disableExtension_(const std::string & ext)
        {
            const bool was_enabled = refresh ? isEnabled(ext) : false;

            enabled_arch_->second.disableExtension(ext);

            if (refresh && was_enabled)
            {
                refresh_();
                if (extensions_changed_callback_)
                {
                    extensions_changed_callback_({ext}, false);
                }
            }
        }

        template <typename InstType, typename AnnotationType, typename InstTypeAllocator,
                  typename AnnotationTypeAllocator, typename... MavisArgs>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis_(const FileNameListType & anno_files, const InstUIDList & uid_list,
                        const AnnotationOverrides & anno_overrides,
                        const MatchSet<Pattern> & inclusions, const MatchSet<Pattern> & exclusions,
                        const InstTypeAllocator & inst_allocator,
                        const AnnotationTypeAllocator & annotation_allocator) const
        {
            using MavisType =
                Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>;
            MavisType mavis(getJSONs(), anno_files, uid_list, anno_overrides, inclusions,
                            exclusions, inst_allocator, annotation_allocator);

            // Remember the initial set of settings for this Mavis instance
            mavis_settings_.emplace(mavis, enabled_extensions_.getSortedExtensions(), anno_files,
                                    uid_list, anno_overrides, inclusions, exclusions);

            return mavis;
        }

      public:
        explicit ExtensionManager(
            const UnknownExtensionAction unknown_extension_action = UnknownExtensionAction::ERROR) :
            unknown_extension_action_(unknown_extension_action)
        {
        }

        ExtensionManager(const ExtensionManager &) = delete;
        ExtensionManager(ExtensionManager &&) = default;
        virtual ~ExtensionManager() = default;

        std::string getUnknownExtensions() const
        {
            return unknown_extensions_;
        }

        void setISASpecJSON(const std::string & jfile, const std::string & mavis_json_dir)
        {
            mavis_json_dir_ = mavis_json_dir;

            const boost::json::value json = parseJSONWithException<BadISAFile>(jfile);

            try
            {
                const auto & jobj = json.as_object();

                if (auto meta_extensions_it = jobj.find("meta_extensions");
                    meta_extensions_it != jobj.end())
                {
                    for (const auto & meta_ext_obj : meta_extensions_it->value().as_array())
                    {
                        processExtension_<ExtensionType::META>(meta_ext_obj.as_object());
                    }
                }

                if (auto config_extensions_it = jobj.find("config_extensions");
                    config_extensions_it != jobj.end())
                {
                    for (const auto & config_ext_obj : config_extensions_it->value().as_array())
                    {
                        processExtension_<ExtensionType::CONFIG>(config_ext_obj.as_object());
                    }
                }

                if (auto extensions_it = jobj.find("extensions"); extensions_it != jobj.end())
                {
                    for (const auto & ext_obj : extensions_it->value().as_array())
                    {
                        processExtension_<ExtensionType::NORMAL>(ext_obj.as_object());
                    }
                }
            }
            catch (const ExtensionManagerException &)
            {
                std::cerr << "Error parsing file " << jfile << std::endl;
                throw;
            }

            setISASpecJSONImpl_(jfile);
        }

        void setISAFromELF(const std::string & elf) { setISA(getISAFromELF_(elf)); }

        void setISA(const std::string & isa)
        {
            if (!isa_.empty())
            {
                for (auto & ext : extensions_)
                {
                    ext.second.reset();
                }

                enabled_extensions_.clear();
                enabled_jsons_.clear();
            }

            setISAImpl_(isa);
        }

        bool isEnabled(const std::string & extension) const
        {
            return enabled_extensions_.isEnabled(extension);
        }

        bool isDisabled(const std::string & extension) const
        {
            assertISAInitialized_();
            return enabled_arch_->second.isDisabled(extension);
        }

        ExtensionMapView getEnabledExtensions(const bool include_meta_extensions = true) const
        {
            return ExtensionMapView(enabled_extensions_, include_meta_extensions);
        }

        void registerExtensionChangeCallback(std::function<void(const std::vector<std::string> &, bool)> cb)
        {
            extensions_changed_callback_ = cb;
        }

        const std::vector<std::string> & getJSONs() const
        {
            if (enabled_jsons_.empty())
            {
                for (const auto & ext : enabled_extensions_)
                {
                    const auto & json = ext.second->getJSON();
                    if (!json.empty())
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
        void allowExtension(const uint32_t arch_key, const std::string & extension)
        {
            assertISASpecInitialized_();
            extensions_.at(arch_key).allowExtension(extension);
        }

        // Adds the specified extension to all allowlists
        // NOTE: Allowlist changes will only apply to future invocations of setISA.
        // Existing enabled extensions are not affected.
        void allowExtension(const std::string & extension)
        {
            assertISASpecInitialized_();
            for (auto & ext_info : extensions_)
            {
                ext_info.second.allowExtension(extension);
            }
        }

        // Adds the specified extensions to the allowlist for arch_key
        // NOTE: Allowlist changes will only apply to future invocations of setISA.
        // Existing enabled extensions are not affected.
        void allowExtensions(const uint32_t arch_key, const std::vector<std::string> & extensions)
        {
            for (const auto & ext : extensions)
            {
                allowExtension(arch_key, ext);
            }
        }

        // Adds the specified extensions to all allowlists
        // NOTE: Allowlist changes will only apply to future invocations of setISA.
        // Existing enabled extensions are not affected.
        void allowExtensions(const std::vector<std::string> & extensions)
        {
            for (const auto & ext : extensions)
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
            for (auto & ext_info : extensions_)
            {
                ext_info.second.clearAllowedExtensions();
            }
        }

        // Adds the specified extension to the blocklist for arch_key
        // NOTE: Blocklist changes will only apply to future invocations of setISA.
        // Existing enabled extensions are not affected.
        void blockExtension(const uint32_t arch_key, const std::string & extension)
        {
            assertISASpecInitialized_();
            extensions_.at(arch_key).blockExtension(extension);
        }

        // Adds the specified extension to all blocklists
        // NOTE: Blocklist changes will only apply to future invocations of setISA.
        // Existing enabled extensions are not affected.
        void blockExtension(const std::string & extension)
        {
            assertISASpecInitialized_();
            for (auto & ext_info : extensions_)
            {
                ext_info.second.blockExtension(extension);
            }
        }

        // Adds the specified extensions to the blocklist for arch_key
        // NOTE: Blocklist changes will only apply to future invocations of setISA.
        // Existing enabled extensions are not affected.
        void blockExtensions(const uint32_t arch_key, const std::vector<std::string> & extensions)
        {
            for (const auto & ext : extensions)
            {
                blockExtension(arch_key, ext);
            }
        }

        // Adds the specified extensions to all blocklists
        // NOTE: Blocklist changes will only apply to future invocations of setISA.
        // Existing enabled extensions are not affected.
        void blockExtensions(const std::vector<std::string> & extensions)
        {
            for (const auto & ext : extensions)
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
            for (auto & ext_info : extensions_)
            {
                ext_info.second.clearBlockedExtensions();
            }
        }

        // Enables the specified extension for the currently selected arch
        void enableExtension(const std::string & ext)
        {
            assertISAInitialized_();
            enableExtension_(ext);
        }

        // Enables the specified extensions for the currently selected arch
        void enableExtensions(const std::vector<std::string> & extensions)
        {
            assertISAInitialized_();
            for (const auto & ext : extensions)
            {
                enableExtension_<false>(ext);
            }

            refresh_();
            if (extensions_changed_callback_)
            {
                extensions_changed_callback_(extensions, true);
            }
        }

        // Disables the specified extension for the currently selected arch, along with any other
        // extensions that require it
        void disableExtension(const std::string & ext)
        {
            assertISAInitialized_();
            disableExtension_(ext);
        }

        // Disables the specified extensions for the currently selected arch, along with any other
        // extensions that require them
        void disableExtensions(const std::vector<std::string> & extensions)
        {
            assertISAInitialized_();
            for (const auto & ext : extensions)
            {
                disableExtension_<false>(ext);
            }

            refresh_();
            if (extensions_changed_callback_)
            {
                extensions_changed_callback_(extensions, false);
            }
        }

        // Enable and disable extension(s) at once.
        void changeExtensions(const std::vector<std::string> & enable_extensions,
                              const std::vector<std::string> & disable_extensions)
        {
            assertISAInitialized_();

            for (const auto & ext : enable_extensions)
            {
                enableExtension_<false>(ext);
            }

            for (const auto & ext : disable_extensions)
            {
                disableExtension_<false>(ext);
            }

            refresh_();
            if (extensions_changed_callback_)
            {
                if (!enable_extensions.empty())
                {
                    extensions_changed_callback_(enable_extensions, true);
                }

                if (!disable_extensions.empty())
                {
                    extensions_changed_callback_(disable_extensions, false);
                }
            }
        }

        // Returns whether this extension is defined in Mavis
        bool isExtensionSupported(const std::string & ext)
        {
            assertISAInitialized_();
            return enabled_arch_->second.isSupported(ext);
        }

        // Returns whether this extension is defined in Mavis
        bool isExtensionSupported(const uint32_t arch_key, std::string & ext)
        {
            assertISASpecInitialized_();
            return extensions_.at(arch_key).isSupported(ext);
        }

        // Constructs a Mavis object using the currently enabled extensions
        template <typename InstType, typename AnnotationType,
                  typename InstTypeAllocator = SharedPtrAllocator<InstType>,
                  typename AnnotationTypeAllocator = SharedPtrAllocator<AnnotationType>>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis(const FileNameListType & anno_files, const InstUIDList & uid_list,
                       const AnnotationOverrides & anno_overrides,
                       const MatchSet<Pattern> & inclusions, const MatchSet<Pattern> & exclusions,
                       const InstTypeAllocator & inst_allocator = SharedPtrAllocator<InstType>(),
                       const AnnotationTypeAllocator & annotation_allocator =
                           SharedPtrAllocator<AnnotationType>()) const
        {
            return constructMavis_<InstType, AnnotationType, InstTypeAllocator,
                                   AnnotationTypeAllocator>(anno_files, uid_list, anno_overrides,
                                                            inclusions, exclusions, inst_allocator,
                                                            annotation_allocator);
        }

        // Constructs a Mavis object using the currently enabled extensions
        template <typename InstType, typename AnnotationType,
                  typename InstTypeAllocator = SharedPtrAllocator<InstType>,
                  typename AnnotationTypeAllocator = SharedPtrAllocator<AnnotationType>>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis(const FileNameListType & anno_files, const InstUIDList & uid_list,
                       const AnnotationOverrides & anno_overrides = {},
                       const InstTypeAllocator & inst_allocator = SharedPtrAllocator<InstType>(),
                       const AnnotationTypeAllocator & annotation_allocator =
                           SharedPtrAllocator<AnnotationType>()) const
        {
            return constructMavis_<InstType, AnnotationType, InstTypeAllocator,
                                   AnnotationTypeAllocator>(
                anno_files, uid_list, anno_overrides, {}, {}, inst_allocator, annotation_allocator);
        }

        // Constructs a Mavis object using the currently enabled extensions
        template <typename InstType, typename AnnotationType,
                  typename InstTypeAllocator = SharedPtrAllocator<InstType>,
                  typename AnnotationTypeAllocator = SharedPtrAllocator<AnnotationType>>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis(const FileNameListType & anno_files, const MatchSet<Pattern> & inclusions,
                       const MatchSet<Pattern> & exclusions,
                       const InstTypeAllocator & inst_allocator = SharedPtrAllocator<InstType>(),
                       const AnnotationTypeAllocator & annotation_allocator =
                           SharedPtrAllocator<AnnotationType>()) const
        {
            return constructMavis_<InstType, AnnotationType, InstTypeAllocator,
                                   AnnotationTypeAllocator>(
                anno_files, {}, {}, inclusions, exclusions, inst_allocator, annotation_allocator);
        }

        // Constructs a Mavis object using the currently enabled extensions
        template <typename InstType, typename AnnotationType,
                  typename InstTypeAllocator = SharedPtrAllocator<InstType>,
                  typename AnnotationTypeAllocator = SharedPtrAllocator<AnnotationType>>
        Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator>
        constructMavis(const FileNameListType & anno_files,
                       const InstTypeAllocator & inst_allocator = SharedPtrAllocator<InstType>(),
                       const AnnotationTypeAllocator & annotation_allocator =
                           SharedPtrAllocator<AnnotationType>()) const
        {
            return constructMavis_<InstType, AnnotationType, InstTypeAllocator,
                                   AnnotationTypeAllocator>(anno_files, {}, {}, {}, {},
                                                            inst_allocator, annotation_allocator);
        }

        // Gets the name that the extension manager would use for a Mavis context based on the
        // currently enabled extensions
        const std::string & getContextName() const
        {
            return enabled_extensions_.getSortedExtensions();
        }

        // Switches a Mavis object's context to the currently enabled extensions.
        // Manages context names internally and should only be used with a Mavis object created with
        // ExtensionManager::constructMavis
        template <typename InstType, typename AnnotationType, typename InstTypeAllocator,
                  typename AnnotationTypeAllocator>
        void switchMavisContext(Mavis<InstType, AnnotationType, InstTypeAllocator,
                                      AnnotationTypeAllocator> & mavis) const
        {
            static const std::string BASE_CONTEXT{"BASE"};

            try
            {
                const auto & mavis_settings = mavis_settings_.get(mavis);

                const auto & sorted_extensions = enabled_extensions_.getSortedExtensions();

                const auto & context_name = sorted_extensions == mavis_settings.default_context
                                                ? BASE_CONTEXT
                                                : sorted_extensions;

                return switchMavisContext(mavis, context_name, mavis_settings.anno_files,
                                          mavis_settings.uid_list, mavis_settings.anno_overrides,
                                          mavis_settings.inclusions, mavis_settings.exclusions);
            }
            catch (const std::out_of_range &)
            {
                throw ExtensionManagerException(
                    "Attempted to use the single-argument switchMavisContext with a Mavis instance "
                    "created outside of the manager. Use the multi-argument switchMavisContext "
                    "instead.");
            }
        }

        // Switches a Mavis object's context to the currently enabled extensions
        // Does not manage context names and can be used with any Mavis object
        template <typename InstType, typename AnnotationType, typename InstTypeAllocator,
                  typename AnnotationTypeAllocator, typename... ContextArgs>
        void switchMavisContext(
            Mavis<InstType, AnnotationType, InstTypeAllocator, AnnotationTypeAllocator> & mavis,
            const std::string & context_name, const FileNameListType & anno_files,
            ContextArgs &&... context_args) const
        {
            if (!mavis.hasContext(context_name))
            {
                mavis.makeContext(context_name, getJSONs(), anno_files,
                                  std::forward<ContextArgs>(context_args)...);
            }

            mavis.switchContext(context_name);
        }
    };
} // namespace mavis::extension_manager
