#ifndef PARAMS_HPP_
#define PARAMS_HPP_

#include <cxxopts.hpp>
#include <string>

namespace mmd {
class Params
{
public:
    Params(int argc, char **argv)
        : options("mmdsrv", "UDP message logger")
    {
        // clang-format off
        options.add_options()
        ("h,help",    "Print usage")
        ("a,address", "Set server address, IPv4 is supported", cxxopts::value<std::string>()->default_value("0.0.0.0"))
        ("p,port",    "Set server port", cxxopts::value<uint16_t>()->default_value("7768"))
        ("f,filter",  "Set filter string", cxxopts::value<std::string>()->default_value("*"))
        ("o,output",  "Set output file name", cxxopts::value<std::string>()->default_value("./mmdsrv.log"));
        // clang-format on

        params = options.parse(argc, argv);
    };

    std::string address() const
    {
        return params["address"].as<std::string>();
    }
    uint16_t port() const
    {
        return params["port"].as<uint16_t>();
    }
    std::string filter() const
    {
        return params["filter"].as<std::string>();
    }
    std::string filename() const
    {
        return params["output"].as<std::string>();
    }
    bool requestedHelp() const
    {
        return params.count("help") > 0;
    }
    std::string help() const
    {
        return options.help();
    }

private:
    cxxopts::Options options;
    cxxopts::ParseResult params;
};
} // namespace mmd

#endif // PARAMS_HPP_