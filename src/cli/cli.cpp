// Author: HenryAWE
// License: The 3-clause BSD License

#include "cli.hpp"
#include <stdexcept>
#include <sstream>
#include <regex>


namespace awe::cli
{
    namespace detailed
    {
        void LocaleValidate(std::string input)
        {
            namespace po = boost::program_options;
            std::regex re("[a-z]{2,3}-[A-Z]{2,3}");

            if(!std::regex_match(input, re))
                throw std::invalid_argument(
                    "invalid argument for \'--language\': \'" +
                    input +
                    '\''
                );
        }

        boost::program_options::options_description GetDesc()
        {
            namespace po = boost::program_options;

            po::options_description desc("Allowed Options");
            desc.add_options()
                ("help", "Get help message")
                ("language,L", po::value<std::string>(), "Program language")
                ("opengl-debug", "Enable OpenGL debug context");

            return desc;
        }
    }

    void CommandlineInterface::Parse(int argc, char* argv[])
    {
        namespace po = boost::program_options;

        auto desc = detailed::GetDesc();
        auto parser = po::command_line_parser(argc, argv);
        parser.options(desc).allow_unregistered();
        try
        {
            po::store(parser.run(), m_vm);
            po::notify(m_vm);
            if(Exists("language"))
            {
                detailed::LocaleValidate(GetVal<std::string>("language"));
            }
        }
        catch(std::exception& e)
        {
            m_e = e.what();
        }
    }

    std::string CommandlineInterface::GetHelp() const
    {
        std::stringstream ss;
        ss << detailed::GetDesc();
        return ss.str();
    }

    std::string CommandlineInterface::GetError() const
    {
        if(!HasError())
            return std::string();

        std::stringstream ss;
        ss << *m_e;
        return ss.str();
    }
}
