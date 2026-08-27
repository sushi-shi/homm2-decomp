#ifndef HOMM2_IRONFIST_SAVE_XML_H
#define HOMM2_IRONFIST_SAVE_XML_H

#include <string>

#include <Ints.h>
#include <IRONFIST/scripting.h>
#include <IRONFIST/xml_utils.h>

/*
 * Ironfist's XML save format: the whole game object graph plus the
 * Ironfist-only state (script text, map variables, vision/chase/ban state,
 * generated artifacts). Element names match upstream Ironfist saves; the
 * calendar-event index tables are written at the recovered widths, and
 * unknown elements are ignored on load.
 */
class IronfistXML : public UtilsXML::XMLFile {
public:
    tinyxml2::XMLError Save(const char* fileName);
    tinyxml2::XMLError Read(const char* fileName);

private:
    template <typename T, size_t M, size_t N>
    void WriteCampaignDDArray(
        tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name, const T (&src)[M][N]
    ) {
        for (size_t i = 0; i < M; i++)
            for (size_t j = 0; j < N; j++) {
                tinyxml2::XMLElement* elem = doc->NewElement(name);
                elem->SetAttribute("campID", static_cast<i32>(i));
                elem->SetAttribute("mapID", static_cast<i32>(j));
                elem->SetAttribute("value", static_cast<i32>(src[i][j]));
                dest->InsertEndChild(elem);
            }
    }
    luaTable* ReadTable(tinyxml2::XMLNode* root);
    void ReadTableElement(tinyxml2::XMLElement* elem, luaTable* lt);
    void WriteMapVarTable(tinyxml2::XMLNode* dest, std::string id, luaTable* lt);
    void WriteMapVariables(tinyxml2::XMLNode* dest);
    void ReadCampaign(tinyxml2::XMLNode* root, i32 campaignType);
    void ReadCampaignSavedHero(tinyxml2::XMLNode* root);
    void ReadMapHeader(tinyxml2::XMLNode* root);
    void ReadMap(tinyxml2::XMLNode* root);
    void ReadMapExtra(tinyxml2::XMLNode* root);
    void ReadPlayerData(tinyxml2::XMLNode* root, i32 dataIndex);
    void ReadHero(tinyxml2::XMLNode* root, i32 heroIndex);
    void ReadTown(tinyxml2::XMLNode* root, i32 townIdx);
    void ReadRoot(tinyxml2::XMLNode* root);
};

// Campaign types as Ironfist's saves spell them.
enum IronfistCampaignType {
    IRONFIST_CAMPAIGN_NONE      = 0,
    IRONFIST_CAMPAIGN_ORIGINAL  = 1,
    IRONFIST_CAMPAIGN_EXPANSION = 2
};

i32 Ironfist_GetCampaignType(void);
b32 Ironfist_LoadGame(const char* fileName, i32 loadFromFile);
i32 Ironfist_SaveGame(const char* saveFile, i32 autosave);
std::string GetSaveFileExtension(b32 isPickLoad);

#endif
