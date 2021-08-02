// Author: HenryAWE
// License: The 3-clause BSD License

#include <memory>
#include "window/window.hpp"


namespace awe
{
    class App
    {
        App();
    public:
        ~App();

        static App& GetInstance();

        void CreateWindow();

        void Mainloop();

        void Quit();

    private:
        std::shared_ptr<Window> m_window;
    };
}
