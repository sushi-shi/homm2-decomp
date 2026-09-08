#include <SAVE/Combat.h>

#include <algorithm>

namespace savegame {
namespace {
constexpr u32 FragmentTag = 0x31425443; // CTB1, little endian.
}

std::vector<u8> CombatFragment(std::span<const u8> bytes, std::size_t offset) {
    if (bytes.empty() || bytes.size() > MaximumCombatBytes || offset >= bytes.size())
        throw FormatError("invalid combat transfer size");
    Writer writer;
    writer.Word(FragmentTag);
    writer.Word(static_cast<u32>(bytes.size()));
    writer.Word(static_cast<u32>(offset));
    writer.Raw(bytes.subspan(offset, std::min(CombatFragmentBytes, bytes.size() - offset)));
    return std::move(writer.bytes);
}

bool CombatTransfer::Add(std::span<const u8> fragment, std::string& error) {
    try {
        Reader reader(fragment);
        if (reader.Word() != FragmentTag) throw FormatError("unknown combat transfer version");
        const u32 total = reader.Word();
        const u32 offset = reader.Word();
        const auto data = reader.Raw(reader.Remaining());
        if (total == 0 || total > MaximumCombatBytes || data.empty()
            || data.size() > CombatFragmentBytes || offset > total || data.size() > total - offset)
            throw FormatError("invalid combat transfer bounds");
        if (m_bytes.empty()) {
            if (offset != 0) throw FormatError("combat transfer does not start at offset zero");
            m_bytes.resize(total);
        }
        if (total != m_bytes.size() || offset > m_received)
            throw FormatError("inconsistent combat transfer");
        if (offset < m_received) {
            if (data.size() > m_received - offset
                || !std::equal(data.begin(), data.end(), m_bytes.data() + offset))
                throw FormatError("conflicting combat transfer fragment");
            return true;
        }
        std::copy(data.begin(), data.end(), m_bytes.data() + offset);
        m_received += data.size();
        return true;
    } catch (const FormatError& failure) {
        error = failure.what();
        return false;
    }
}

}
