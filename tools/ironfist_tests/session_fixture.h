#pragma once

#include <cstring>
#include <IRONFIST/runtime.h>
#include <IRONFIST/save_xml.h>

// Existing focused regressions decode fragments rather than complete saves.
// Supply their unchanged engine records explicitly, then use the same runtime
// application boundary as a full load. No codec mutates the fixture's engine.
inline void DecodeSessionFragment(ironfist::save::XmlFile& file, tinyxml2::XMLNode* root) {
    ironfist::SessionData data;
    data.records = ironfist::runtime::CaptureSession().records;
    std::memcpy(data.playerNames, cPlayerNames, sizeof(data.playerNames));
    file.ReadRoot(root, data);
    ironfist::runtime::RestoreSession(data);
}
