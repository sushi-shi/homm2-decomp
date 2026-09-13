"""Pin mod-only consumers that need index migration with the checkpoint layout."""

from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]

for filename, one_based_index in (("TOWN.cpp", "m_buildState"), ("GAME.cpp", "slot")):
    source = (ROOT / "src/SOURCE" / filename).read_text()
    assignments = re.findall(
        r"m_spellCounts\[([^]]+)\]\s*=\s*([^;]*CyborgSpellLimits[^;]*);",
        source,
    )
    assert assignments, f"{filename}: missing Cyborg spell-limit initialization"
    expected = f"{one_based_index}-TOWN_MAGE_GUILD_FIRST_LEVEL"
    for index, _ in assignments:
        assert re.sub(r"\s+", "", index) == expected, (filename, index)

xml = (ROOT / "src/IRONFIST/save_xml.cpp").read_text()
assert '"field_2773", gpGame->m_townOwners' in xml
assert '"builtToday", gpGame->m_townBuiltToday' in xml
assert re.search(r"numSpellsOfLevel\[j\]\s*=\s*twn->m_spellCounts\[j\s*\]", xml)
assert re.search(r"twn->m_spellCounts\[index\]\s*=", xml)
assert not re.search(r"m_spellCounts\[[^]]*\+\s*TOWN_MAGE_GUILD_FIRST_LEVEL", xml)
