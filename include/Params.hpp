/**
 * @file Params.hpp
 * @brief Parses params from command line
 * @version 0.1
 * @date 2024-10-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PARAMS_HPP_
#define PARAMS_HPP_

#include <cxxopts.hpp>
#include <filesystem>
#include <string>

namespace mmd {
/**
 * @brief Parses params from command line
 * No logics here, only the conversion of the path to the absolute path
 */
class Params
{
public:
    Params(int argc, char **argv)
    {
        namespace fs = std::filesystem;
        cxxopts::Options options("mmdsrv", "UDP logger");
        cxxopts::ParseResult params;

        // clang-format off
        options.add_options()
        ("h,help",    "Print usage")
        ("a,address", "Set server address, IPv4 or IPv6", cxxopts::value<std::string>()->default_value("0.0.0.0"))
        ("p,port",    "Set server port", cxxopts::value<uint16_t>()->default_value("7768"))
        ("f,filter",  "Set filter string", cxxopts::value<std::string>()->default_value("*"))
        ("o,output",  "Set output file name", cxxopts::value<std::string>()->default_value("./mmdsrv.log"));
        // clang-format on

        params = options.parse(argc, argv);

        _address = params["address"].as<std::string>();
        _port = params["port"].as<uint16_t>();
        _filter = params["filter"].as<std::string>();
        _filename = fs::absolute(params["output"].as<std::string>()).string();
        _requestedHelp = params.count("help") > 0;
        _help = options.help();
    };

    [[nodiscard]] std::string address() const
    {
        return _address;
    }
    [[nodiscard]] uint16_t port() const
    {
        return _port;
    }
    [[nodiscard]] std::string filter() const
    {
        return _filter;
    }
    [[nodiscard]] std::string filename() const
    {
        return _filename;
    }
    [[nodiscard]] bool requestedHelp() const
    {
        return _requestedHelp;
    }
    [[nodiscard]] std::string help() const
    {
        return _help;
    }

private:
    std::string _address;
    uint16_t _port;
    std::string _filter;
    std::string _filename;
    std::string _help;
    bool _requestedHelp;
};
} // namespace mmd

#endif // PARAMS_HPP_