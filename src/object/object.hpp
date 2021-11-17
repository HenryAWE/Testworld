// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_OBJECT_OBJECT_HPP
#define TESTWORLD_OBJECT_OBJECT_HPP

#include <memory>
#include <vector>
#include "attribute.hpp"


namespace awe
{
    class Object
    {
    public:
        Object();
        ~Object();

        template <typename T, typename... Args>
        T& AddAttr(Args&&... args)
        {
            m_attr.push_back(
                std::make_shared<T>(std::forward<Args>(args)...)
            );
            return *(T*)m_attr.back().get();
        }

        attribute::Base& GetAttrByIdx(std::size_t index);
        template <typename T>
        T* GetAttrPtr() noexcept
        {
            for(auto& i : m_attr)
            {
                T* p = dynamic_cast<T*>(i.get());
                if(p) return p;
            }
            return nullptr;
        }

    private:
        std::vector<std::shared_ptr<attribute::Base>> m_attr;
    };
}


#endif
