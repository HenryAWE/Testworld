// Author: HenryAWE
// License: The 3-clause BSD License


#ifndef TESTWORLD_OBJECT_SCENE_HPP
#define TESTWORLD_OBJECT_SCENE_HPP

#include <list>
#include <memory>
#include "object.hpp"


namespace awe
{
    class Scene
    {
    public:
        template <typename T, typename... Args>
        T& CreateObject(Args&&... args)
        {
            static_assert(std::is_base_of_v<Object, T>);
            m_objlist.push_back(
                std::make_shared<T>(std::forward<Args>(args)...)
            );
            return *(T*)m_objlist.back().get();
        }

        [[nodiscard]]
        std::list<std::shared_ptr<Object>>& GetObjList() noexcept
        {
            return m_objlist;
        }
        [[nodiscard]]
        const std::list<std::shared_ptr<Object>>& GetObjList() const noexcept
        {
            return m_objlist;
        }

        Object* FindObjByName(const std::string& name);

    private:
        std::list<std::shared_ptr<Object>> m_objlist;
    };
}

#endif
