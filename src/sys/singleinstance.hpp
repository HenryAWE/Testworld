// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SYS_SINGLEINSTANCE_HPP
#define TESTWORLD_SYS_SINGLEINSTANCE_HPP

namespace awe
{
    class SingleInstance
    {
    public:
        ~SingleInstance();

        static SingleInstance& GetInstance();

        bool Acquire();
    };
}


#endif
