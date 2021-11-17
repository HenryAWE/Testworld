// Author: HenryAWE
// License: The 3-clause BSD License

#include "scene.hpp"


namespace awe
{
    Object* Scene::FindObjByName(const std::string& name)
    {
        for(auto& i : m_objlist)
        {
            if(auto* attr = i->GetAttrPtr<attribute::Name>(); attr)
            {
                if(attr->name == name)
                    return i.get();
            }
        }
        return nullptr;
    }
}
