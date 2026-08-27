#ifndef HOMM2_ENUM_CODE_H
#define HOMM2_ENUM_CODE_H

#include <Ints.h>

#include <type_traits>

// Enum values cross numeric storage boundaries in map records, messages,
// network packets, and index-based tables. Keep the actual C++ conversion in
// this reviewed boundary and expose only domain-named entry points to game
// code. This makes the provenance visible at every call site without turning
// arbitrary integral values into implicitly convertible enums.
template <typename Integer>
concept H2EnumCode = std::is_integral_v<Integer>
    && !std::is_same_v<std::remove_cv_t<Integer>, bool>;

namespace h2_enum_code_detail {
    template <typename Enum, H2EnumCode Integer>
    constexpr Enum Convert(Integer value) {
        return static_cast<Enum>(value);
    }
}

enum class ArtifactType : i8;
enum class ArmyFacing : i8;
enum class BuildingSlotType : i8;
enum class ButtonSelectMode : i16;
enum class CampaignTrackType : i32;
enum class CombatHexDirection : i32;
enum class CombatMessageCommand : i32;
enum class CombatSide : i8;
enum class ConfigOperaMode : i32;
enum class ConfigVolumeLevel : i32;
enum class CreatureType : i8;
enum class ExpansionCampaignMap : i32;
enum class FactionType : i8;
enum class FileRequesterMapSizeFilter : i32;
enum class FlotsamReward : i32;
enum class FontAlignment : i16;
enum class FontDrawMode : i16;
enum class GameDifficulty : i8;
enum class GenericSiteType : i32;
enum class HeroSecondarySkill : i32;
enum class HeroSkillLevel : u8;
enum class HeroPortrait : u8;
enum class IconDrawOrientation : i8;
enum class MapDirection : i8;
enum class MapObjectType : i16;
enum class MineType : i8;
enum class NetworkPacketType : i32;
enum class PlayerHandicap : i8;
enum class PlayerPersonality : i32;
enum class RecruitSiteType : i32;
enum class ResourceType : i16;
enum class SpellType : i8;
enum class TerrainType : i8;
enum class TilesetId : u8;
enum class TownThievesGuildCategory : i32;
enum class UndeadEventLevel : i32;
enum class WidgetFlag : i16;
enum class WidgetKind : i16;
enum class WindowFlag : i32;

#define H2_NAMED_ENUM_CODE_BOUNDARY(EnumType)                                      \
    constexpr EnumType EnumType##FromCode(H2EnumCode auto value) {                \
        return h2_enum_code_detail::Convert<EnumType>(value);                     \
    }                                                                              \
    constexpr EnumType EnumType##FromOrdinal(H2EnumCode auto value) {             \
        return h2_enum_code_detail::Convert<EnumType>(value);                     \
    }

H2_NAMED_ENUM_CODE_BOUNDARY(ArtifactType)
H2_NAMED_ENUM_CODE_BOUNDARY(ArmyFacing)
H2_NAMED_ENUM_CODE_BOUNDARY(BuildingSlotType)
H2_NAMED_ENUM_CODE_BOUNDARY(ButtonSelectMode)
H2_NAMED_ENUM_CODE_BOUNDARY(CampaignTrackType)
H2_NAMED_ENUM_CODE_BOUNDARY(CombatHexDirection)
H2_NAMED_ENUM_CODE_BOUNDARY(CombatMessageCommand)
H2_NAMED_ENUM_CODE_BOUNDARY(CombatSide)
H2_NAMED_ENUM_CODE_BOUNDARY(ConfigOperaMode)
H2_NAMED_ENUM_CODE_BOUNDARY(ConfigVolumeLevel)
H2_NAMED_ENUM_CODE_BOUNDARY(CreatureType)
H2_NAMED_ENUM_CODE_BOUNDARY(ExpansionCampaignMap)
H2_NAMED_ENUM_CODE_BOUNDARY(FactionType)
H2_NAMED_ENUM_CODE_BOUNDARY(FileRequesterMapSizeFilter)
H2_NAMED_ENUM_CODE_BOUNDARY(FlotsamReward)
H2_NAMED_ENUM_CODE_BOUNDARY(FontAlignment)
H2_NAMED_ENUM_CODE_BOUNDARY(FontDrawMode)
H2_NAMED_ENUM_CODE_BOUNDARY(GameDifficulty)
H2_NAMED_ENUM_CODE_BOUNDARY(GenericSiteType)
H2_NAMED_ENUM_CODE_BOUNDARY(HeroSecondarySkill)
H2_NAMED_ENUM_CODE_BOUNDARY(HeroSkillLevel)
H2_NAMED_ENUM_CODE_BOUNDARY(HeroPortrait)
H2_NAMED_ENUM_CODE_BOUNDARY(IconDrawOrientation)
H2_NAMED_ENUM_CODE_BOUNDARY(MapDirection)
H2_NAMED_ENUM_CODE_BOUNDARY(MapObjectType)
H2_NAMED_ENUM_CODE_BOUNDARY(MineType)
H2_NAMED_ENUM_CODE_BOUNDARY(NetworkPacketType)
H2_NAMED_ENUM_CODE_BOUNDARY(PlayerHandicap)
H2_NAMED_ENUM_CODE_BOUNDARY(PlayerPersonality)
H2_NAMED_ENUM_CODE_BOUNDARY(RecruitSiteType)
H2_NAMED_ENUM_CODE_BOUNDARY(ResourceType)
H2_NAMED_ENUM_CODE_BOUNDARY(SpellType)
H2_NAMED_ENUM_CODE_BOUNDARY(TerrainType)
H2_NAMED_ENUM_CODE_BOUNDARY(TilesetId)
H2_NAMED_ENUM_CODE_BOUNDARY(TownThievesGuildCategory)
H2_NAMED_ENUM_CODE_BOUNDARY(UndeadEventLevel)
H2_NAMED_ENUM_CODE_BOUNDARY(WidgetFlag)
H2_NAMED_ENUM_CODE_BOUNDARY(WidgetKind)
H2_NAMED_ENUM_CODE_BOUNDARY(WindowFlag)

#undef H2_NAMED_ENUM_CODE_BOUNDARY

#endif
