#include <BASE/ImageDecode.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/Icon2b.h>
#include <BASE/Iconf2b.h>
#include <BASE/Iconm2b.h>
#include <BASE/Iconmf2b.h>
#include <BASE/Icond2b.h>
#include <BASE/Icondf2b.h>
#include <BASE/icon2bc.h>
#include <BASE/iconf2bc.h>
#include <BASE/icon2by.h>
#include <BASE/iconf2by.h>
#include <BASE/icon2bs.h>
#include <SOURCE/dimPalette.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <limits>
#include <vector>

namespace {
int failures = 0;
void Expect(bool value, const char* message) {
    if (!value) {
        std::fprintf(stderr, "%s\n", message);
        ++failures;
    }
}

std::vector<u8> Body(std::initializer_list<u8> stream, i16 width = 4, i16 height = 1,
                     bool mask = false) {
    std::vector<u8> bytes{0, 0, 0, 0, static_cast<u8>(width), static_cast<u8>(width >> 8),
                          static_cast<u8>(height), static_cast<u8>(height >> 8),
                          static_cast<u8>(mask ? 32 : 0), 13, 0, 0, 0};
    bytes.insert(bytes.end(), stream);
    return bytes;
}

struct Fixture {
    std::vector<u8> bytes;
    icon source{0};
    bitmap target;
    std::array<u8, 34> pixels;
    explicit Fixture(std::vector<u8> body) : bytes(std::move(body)) {
        source.m_data = bytes.data();
        source.m_dataSize = static_cast<u32>(bytes.size());
        source.m_frameCount = 1;
        target.m_width = 8;
        target.m_height = 4;
        target.m_pixels = pixels.data() + 1;
        pixels.fill(0xaa);
    }
    bool Draw(i32 x = 0, i32 y = 0, const images::IconOptions& options = {}) {
        return images::DrawIcon(&source, &target, x, y, 0, ICON_DRAW_NO_CLIP, 0, 0, 0, 0, options);
    }
    bool Untouched() const {
        return std::all_of(pixels.begin(), pixels.end(), [](u8 value) { return value == 0xaa; });
    }
    void Guards() const {
        Expect(pixels.front() == 0xaa && pixels.back() == 0xaa, "destination canary overwritten");
    }
};

void Rejections() {
    const std::array malformed{
        Body({}), Body({4, 1, 2}), Body({0xc1}), Body({0xc1, 4}),
        Body({0xc0}), Body({0xc0, 0}), Body({0xc2}), Body({0}),
        Body({0xc5, 2, 0x80}), Body({0, 1, 7, 0x80}), Body({0, 0, 0x80}),
        Body({0x80}, -1), Body({0x80}, 4, -1)
    };
    for (const auto& body : malformed) {
        Fixture fixture(body);
        const char* error = nullptr;
        Expect(!images::ValidateIconPayload(body, 1, error) && error != nullptr,
               "malformed ICN accepted by the production loader validator");
        Expect(!fixture.Draw() && fixture.Untouched(), "malformed draw changed destination");
    }
    Fixture index(Body({4, 1, 2, 3, 4, 0x80}));
    for (i32 frame : {-1, 1, std::numeric_limits<i32>::max()})
        Expect(!images::DrawIcon(&index.source, &index.target, 0, 0, frame,
                   ICON_DRAW_CLIP, 0, 0, 8, 4) && index.Untouched(), "invalid frame index accepted");
    for (u32 offset : {0u, 12u, 19u, 0xffffffffu}) {
        Fixture bad(index.bytes);
        for (u32 byte = 0; byte < 4; ++byte)
            bad.bytes[9 + byte] = static_cast<u8>(offset >> (byte * 8));
        Expect(!bad.Draw() && bad.Untouched(), "invalid frame offset accepted");
    }
    for (i16 count : {static_cast<i16>(0), static_cast<i16>(-1), static_cast<i16>(2)}) {
        index.source.m_frameCount = count;
        Expect(!index.Draw() && index.Untouched(), "invalid frame count accepted");
    }
    Fixture partial(Body({1, 7, 0xc1}));
    Expect(!partial.Draw() && partial.Untouched(), "partial stream painted before rejection");
    Fixture mask(Body({5, 0x80}, 4, 1, true));
    images::IconOptions options;
    options.paint = images::IconPaint::MaskFill;
    Expect(!mask.Draw(0, 0, options) && mask.Untouched(), "overwide mask accepted");
    Fixture empty(Body({0x80}, 0, 0));
    Expect(empty.Draw() && empty.Untouched(), "empty terminated frame rejected");
}

void BoundsAndPixels() {
    Fixture literal(Body({4, 1, 2, 3, 4, 0x80}));
    Expect(literal.Draw(-2) && literal.pixels[1] == 3 && literal.pixels[2] == 4,
           "unclipped negative placement did not crop literal pixels");
    literal.Guards();
    Fixture flipped(Body({4, 1, 2, 3, 4, 0x80}));
    images::IconOptions options;
    options.mirrored = true;
    Expect(flipped.Draw(1, 3, options) && flipped.pixels[25] == 2 && flipped.pixels[26] == 1,
           "mirrored edge crop selected the wrong source pixels");
    flipped.Guards();
    for (i32 x : {std::numeric_limits<i32>::min(), std::numeric_limits<i32>::max()}) {
        Fixture outside(literal.bytes);
        Expect(outside.Draw(x, x) && outside.Untouched(), "extreme coordinates overflowed");
        Expect(images::DrawIcon(&outside.source, &outside.target, 0, 0, 0,
                   ICON_DRAW_CLIP, x, x, std::numeric_limits<i32>::max(), 0)
                   && outside.Untouched(), "empty extreme clip wrote pixels");
    }
    Fixture shear(Body({1, 1, 0, 1, 2, 0, 1, 3, 0x80}, 1, 3));
    const std::array<i8, 1> shifts{2};
    options = {};
    options.sheared = true;
    options.shear = shifts;
    Expect(images::DrawIcon(&shear.source, &shear.target, 0, -1, 0,
               ICON_DRAW_CLIP, 0, 0, 8, 4, options), "short shear span rejected");
    Expect(shear.pixels[1] == 2 && shear.pixels[11] == 3,
           "shear must bound row reads and retain the previous-row origin");
    shear.Guards();
    Fixture mask(Body({4, 0x80}, 4, 1, true));
    options = {};
    options.paint = images::IconPaint::MaskDim;
    options.color = 11;
    uDimPal[2][3][0xaa] = 71;
    Expect(mask.Draw(0, 0, options) && mask.pixels[1] == 71, "mask dim table 11 incorrect");
    options.color = 12;
    Expect(!mask.Draw(0, 0, options), "invalid mask dim level accepted");
}

void LiveEntryPoints() {
    Fixture missing(Body({})); // the audit's exact missing-stream shape
    std::array<u8, 256> table{};
    std::array<i8, 4> shear{};
    auto* src = &missing.source;
    auto* dst = &missing.target;
    IconToBitmap(src, dst, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4, 0);
    FlipIconToBitmap(src, dst, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4, 0);
    IconToBitmapColorTable(src, dst, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4, 0, table.data(), 1);
    FlipIconToBitmapColorTable(src, dst, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4, 0, table.data());
    IconToBitmapYModify(src, dst, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4, 0, shear);
    FlipIconToBitmapYModify(src, dst, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4, 0, shear);
    MonoIconToBitmap(src, dst, 0, 0, 0, 7, ICON_DRAW_CLIP, 0, 0, 8, 4);
    FlipMonoIconToBitmap(src, dst, 0, 0, 0, 7, ICON_DRAW_CLIP, 0, 0, 8, 4);
    DimIconToBitmap(src, dst, 0, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4);
    FlipDimIconToBitmap(src, dst, 0, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4);
    IconToBitmapScale(src, dst, 0, 0, 0, ICON_DRAW_CLIP, 0, 0, 8, 4, 16);
    Expect(missing.Untouched(), "a live entry point painted the missing ICN stream");
    auto square = Body({}, 32, 32);
    for (i32 row = 0; row < 32; ++row)
        square.insert(square.end(), {0xc1, 32, 9, 0});
    square.push_back(0x80);
    Fixture scale(square);
    for (i32 size : {-1, 0, 33, std::numeric_limits<i32>::max()})
        IconToBitmapScale(&scale.source, &scale.target, 0, 0, 0, ICON_DRAW_NO_CLIP, 0, 0, 0, 0, size);
    Expect(scale.Untouched(), "invalid scale changed destination");
    IconToBitmapScale(&scale.source, &scale.target, -1, 0, 0, ICON_DRAW_NO_CLIP, 0, 0, 0, 0, 16);
    Expect(std::all_of(scale.pixels.begin() + 1, scale.pixels.end() - 1,
                      [](u8 value) { return value == 9; }), "scaled image did not crop at the surface");
    scale.Guards();
    Fixture clipped(square);
    IconToBitmapScale(&clipped.source, &clipped.target, 0, 0, 0, ICON_DRAW_CLIP, 2, 1, 2, 1, 16);
    Expect(std::count(clipped.pixels.begin(), clipped.pixels.end(), 9) == 2
           && clipped.pixels[11] == 9 && clipped.pixels[12] == 9,
           "scaled drawing ignored the requested clip rectangle");
    clipped.Guards();
}

void Mutations() {
    u32 state = 7;
    const auto random = [&]() { return state = state * 1664525u + 1013904223u; };
    for (int iteration = 0; iteration < 5000; ++iteration) {
        auto body = Body({4, 1, 2, 3, 4, 0, 0x80}, 4, 2);
        body.resize(random() % 80);
        for (u8& value : body)
            if ((random() & 7) == 0)
                value = static_cast<u8>(random() >> 24);
        Fixture fixture(std::move(body));
        images::IconOptions options;
        options.mirrored = (iteration & 1) != 0;
        options.paint = static_cast<images::IconPaint>(iteration % 3);
        options.color = iteration % 16;
        const bool drawn = fixture.Draw(static_cast<i32>(random() % 20) - 10,
                                        static_cast<i32>(random() % 12) - 6, options);
        if (!drawn)
            Expect(fixture.Untouched(), "rejected mutation partially rendered");
        fixture.Guards();
    }
}
}

int main() {
    Rejections();
    BoundsAndPixels();
    LiveEntryPoints();
    Mutations();
    return failures == 0 ? 0 : 1;
}
