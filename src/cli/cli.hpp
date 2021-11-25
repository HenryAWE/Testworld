// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_CLI_CLI_HPP
#define TESTWORLD_CLI_CLI_HPP

#include <string>
#include <boost/program_options.hpp>
#include <optional>


namespace awe::cli
{
    class CommandlineInterface
    {
    public:
        void Parse(int argc, char* argv[]);

        template <typename T>
        T GetVal(const std::string& name)
        {
            return m_vm[name].as<T>();
        }
        bool Exists(const std::string& name)
        {
            return m_vm.count(name) != 0;
        }
        std::size_t Count(const std::string& name)
        {
            return m_vm.count(name);
        }

        std::string GetHelp() const;
        bool HasError() const noexcept
        {
            return m_e.has_value();
        }
        std::string GetError() const;

    private:
        boost::program_options::variables_map m_vm;
        std::optional<std::string> m_e;
    };
}

#endif
