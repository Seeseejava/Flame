#pragma once 

#include "Runtime/Core/PublicSingleton.h"

#include <unordered_map>

namespace Flame
{
    template <template <typename> typename Derived, typename LibType>
    class LibraryBase : public PublicSingleton<Derived<LibType>>
    {
    public:
        void Add(const std::string& name, const Ref<LibType>& mem)
        {
            FLAME_CORE_ASSERT(m_Library.find(name) == m_Library.end(), "Already have this member in Library!");
            m_Library[name] = mem;
        }
        [[nodiscard]] Ref<LibType> Get(const std::string& name)
        {
            FLAME_CORE_ASSERT(m_Library.find(name) != m_Library.end(), "Can't find this member in Library!");
            return m_Library[name];
        }

        void Set(const std::string& name, const Ref<LibType>& mem)
        {
            FLAME_CORE_ASSERT(m_Library.find(name) != m_Library.end(),  "Can't find this member in Library!");
            m_Library[name] = mem;
        }

    protected:
        std::unordered_map<std::string, Ref<LibType>> m_Library;
    };

    template<typename LibType>
    class Library : public LibraryBase<Library, LibType>
    {
    public:
        Library();
    };
}