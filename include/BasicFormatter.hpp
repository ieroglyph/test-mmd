#ifndef BASICFORMATTER_HPP_
#define BASICFORMATTER_HPP_

#include "FormatWorker.hpp"

namespace mmd {

enum class DataType {
    Ascii,
    Utf8,
    Binary,
};

class BasicFormatter
{
public:
    static FormattedData format(const ReceivedData &rdata)
    {
        FormattedData fdata{};
        const auto dataType = detectDataType(rdata.data, rdata.datasize);
        const auto formattedSize = [dataType, &fdata, &rdata] {
            if (dataType == DataType::Binary) {
                return fmt::format_to(
                        std::begin(fdata.data), FMT_COMPILE("ts;{};addr;{};type;bin;data;{:02x}\n"),
                        rdata.timestamp, rdata.address,
                        fmt::join(std::span(std::begin(rdata.data), rdata.datasize), ""));
            } else {
                return fmt::format_to(std::begin(fdata.data),
                                      FMT_COMPILE("ts;{};addr;{};type;{};data;{:s}\n"),
                                      rdata.timestamp, rdata.address,
                                      (dataType == DataType::Ascii ? "ascii" : "utf8"),
                                      std::span(std::begin(rdata.data), rdata.datasize));
            }
        }();
        fdata.datasize = std::distance(fdata.data, formattedSize);
        return fdata;
    }

    static DataType detectDataType(const char *data, size_t datasize)
    {
        bool isAscii = true;

        // Check if the data is ASCII or UTF-8
        for (size_t i{ 0 }, s{ datasize }; i < s;) {
            const auto c = static_cast<unsigned char>(data[i]);
            if (c < 0x80) { // ascii range
                i++;
                continue;
            }
            isAscii = false;
            if ((c & 0xE0) == 0xC0) { // 110xxxxx, two-byte sequence
                if (i + 1 >= s) return DataType::Binary;
                unsigned char c1 = static_cast<unsigned char>(data[i + 1]);
                if ((c1 & 0xC0) != 0x80) return DataType::Binary;
                // Overlong encoding check
                if (c == 0xC0 && c1 == 0x80) return DataType::Binary;
                i += 2;
            } else if ((c & 0xF0) == 0xE0) { // 1110xxxx, three-byte sequence
                if (i + 2 >= s) return DataType::Binary;
                unsigned char c1 = static_cast<unsigned char>(data[i + 1]);
                unsigned char c2 = static_cast<unsigned char>(data[i + 2]);
                if ((c1 & 0xC0) != 0x80) return DataType::Binary;
                if ((c2 & 0xC0) != 0x80) return DataType::Binary;
                i += 3;
            } else if ((c & 0xF8) == 0xF0) { // 11110xxx, four-byte sequence
                if (i + 3 >= s) return DataType::Binary;
                unsigned char c1 = static_cast<unsigned char>(data[i + 1]);
                unsigned char c2 = static_cast<unsigned char>(data[i + 2]);
                unsigned char c3 = static_cast<unsigned char>(data[i + 3]);
                if ((c1 & 0xC0) != 0x80) return DataType::Binary;
                if ((c2 & 0xC0) != 0x80) return DataType::Binary;
                if ((c3 & 0xC0) != 0x80) return DataType::Binary;
                i += 4;
            } else {
                return DataType::Binary; // Invalid UTF-8 leading byte
            }
        }

        return isAscii ? DataType::Ascii : DataType::Utf8;
    }
};

static_assert(Formatter<BasicFormatter>, "Formatter is not a formatter");

} // namespace mmd

#endif // BASICFORMATTER_HPP_