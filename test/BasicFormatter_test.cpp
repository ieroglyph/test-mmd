#ifndef BASICFORMATTER_TEST_HPP_
#define BASICFORMATTER_TEST_HPP_

#include <catch2/catch_test_macros.hpp>

#include "BasicFormatter.hpp"

#include <iostream>
#include <ranges>
#include <string>
#include <string_view>

namespace test_data {
static constexpr char AsciiData[] = "Here we have some ASCII encoded data";
static constexpr char8_t Utf8Data[] = u8"Тук имаме някои UTF-8 кодирани данни Εδώ δεδομένα";
static constexpr unsigned char BinaryData[] = { 0x18, 0x22, 0xf2, 0xfe, 0xff, 0x11, 0x23, 0x51 };
} // namespace test_data

TEST_CASE("Sanity test")
{
    REQUIRE(2 * 2 == 4);
}

using namespace mmd;

void REQUIRE_TYPE_EQUAL(const ReceivedData &rdata, std::string_view expected)
{
    using std::views::drop;
    using std::views::split;

    BasicFormatter formatter{};

    const auto fdata = formatter.format(rdata);

    std::string_view rsw(fdata.data, fdata.datasize);

    auto filtered = rsw | split(';') | drop(5);
    auto it = filtered.begin();

    REQUIRE(it != std::ranges::end(filtered));

    auto fifth = *it;
    std::string s;
    std::ranges::copy(fifth, std::back_inserter(s));

    REQUIRE(s == expected);
}

TEST_CASE("Asii stuff is detected as ascii stuff", "[formatter]")
{
    ReceivedData rdata{};

    std::string_view sw(test_data::AsciiData);

    rdata.datasize = sw.size();
    memcpy(rdata.data, sw.data(), sw.size());

    REQUIRE_TYPE_EQUAL(rdata, "ascii");
}

TEST_CASE("UTF-8 stuff is detected as UTF-8 stuff", "[formatter]")
{
    ReceivedData rdata{};

    rdata.datasize = sizeof(test_data::Utf8Data);
    memcpy(rdata.data, test_data::Utf8Data, rdata.datasize);

    REQUIRE_TYPE_EQUAL(rdata, "utf8");
}

TEST_CASE("Binary stuff is detected as binary stuff", "[formatter]")
{
    ReceivedData rdata{};

    rdata.datasize = sizeof(test_data::BinaryData);
    memcpy(rdata.data, &test_data::BinaryData[0], rdata.datasize);

    REQUIRE_TYPE_EQUAL(rdata, "bin");
}
#endif // BASICFORMATTER_TEST_HPP_