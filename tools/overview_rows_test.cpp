#include <SOURCE/Overview.h>

#include <cstdio>
#include <type_traits>

// Only pointer-slot ownership is exercised; this executable does not link the
// engine or instantiate real widgets and their resource/window dependencies.
class textWidget {};
class iconWidget {};

static_assert(std::is_same_v<decltype(textWidgetDynamic), OverviewTextWidgetRow*>);
static_assert(std::is_same_v<decltype(iconWidgetDynamic), OverviewIconWidgetRow*>);
static_assert(std::extent_v<OverviewTextWidgetRow> == OVERVIEW_DYNAMIC_WIDGETS_PER_ROW);
static_assert(std::extent_v<OverviewIconWidgetRow> == OVERVIEW_DYNAMIC_WIDGETS_PER_ROW);
static_assert(sizeof(OverviewTextWidgetRow) == OVERVIEW_DYNAMIC_WIDGETS_PER_ROW * sizeof(textWidget*));
static_assert(sizeof(OverviewIconWidgetRow) == OVERVIEW_DYNAMIC_WIDGETS_PER_ROW * sizeof(iconWidget*));

int main() {
    OverviewTextWidgetRow textRows[OVERVIEW_VISIBLE_ROWS]{};
    OverviewIconWidgetRow iconRows[OVERVIEW_VISIBLE_ROWS]{};
    textWidget textTokens[OVERVIEW_VISIBLE_ROWS][OVERVIEW_DYNAMIC_WIDGETS_PER_ROW];
    iconWidget iconTokens[OVERVIEW_VISIBLE_ROWS][OVERVIEW_DYNAMIC_WIDGETS_PER_ROW];
    OverviewTextWidgetRow* texts = textRows;
    OverviewIconWidgetRow* icons = iconRows;

    for (i32 row = 0; row < OVERVIEW_VISIBLE_ROWS; ++row) {
        for (i32 slot = 0; slot < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; ++slot) {
            if (texts[row][slot] != nullptr || icons[row][slot] != nullptr) {
                std::fputs("Overview slot was not initialized\n", stderr);
                return 1;
            }
            texts[row][slot] = &textTokens[row][slot];
            icons[row][slot] = &iconTokens[row][slot];
        }
    }
    for (i32 row = 0; row < OVERVIEW_VISIBLE_ROWS; ++row) {
        for (i32 slot = 0; slot < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; ++slot) {
            if (texts[row][slot] != &textTokens[row][slot]
                || icons[row][slot] != &iconTokens[row][slot]) {
                std::fputs("Overview rows or slots overlap\n", stderr);
                return 1;
            }
            texts[row][slot] = nullptr;
            icons[row][slot] = nullptr;
        }
    }
    for (i32 row = 0; row < OVERVIEW_VISIBLE_ROWS; ++row) {
        for (i32 slot = 0; slot < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; ++slot) {
            if (texts[row][slot] != nullptr || icons[row][slot] != nullptr) {
                std::fputs("Overview slot was not cleared\n", stderr);
                return 1;
            }
        }
    }
    return 0;
}
