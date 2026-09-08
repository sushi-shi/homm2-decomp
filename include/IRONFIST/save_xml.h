#ifndef HOMM2_IRONFIST_SAVE_XML_H
#define HOMM2_IRONFIST_SAVE_XML_H

#include <string>

#include <Ints.h>
#include <IRONFIST/session.h>
#include <IRONFIST/xml_utils.h>

namespace ironfist::save {

/*
 * Codec for detached SessionData. Reading allocates only value-owned storage
 * and leaves the running game, scripts, and managers untouched. The runtime
 * applies the decoded records and controls initialization separately.
 */
class XmlFile : public xml::XMLFile {
public:
    tinyxml2::XMLError Save(const char* fileName, const SessionData& data);
    tinyxml2::XMLError Read(const char* fileName, SessionData& data);
    const char* GetError();

private:
    std::string m_decodeError;
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
    script::LuaTable ReadTable(tinyxml2::XMLNode* root);
    void ReadTableElement(tinyxml2::XMLElement* elem, script::LuaTable& table);
    void WriteMapVarTable(
        tinyxml2::XMLNode* dest, const std::string& id, const script::LuaTable& table
    );
    void WriteMapVariables(tinyxml2::XMLNode* dest, const SessionData& data);
    void ReadCampaign(tinyxml2::XMLNode* root, i32 campaignType, SessionData& data);
    void ReadCampaignSavedHero(tinyxml2::XMLNode* root, SessionData& data);
    void ReadMapHeader(tinyxml2::XMLNode* root, SessionData& data);
    void ReadMap(tinyxml2::XMLNode* root, SessionData& data);
    void ReadMapExtra(tinyxml2::XMLNode* root, SessionData& data);
    void ReadPlayerData(tinyxml2::XMLNode* root, i32 dataIndex, SessionData& data);
    void ReadHero(tinyxml2::XMLNode* root, i32 heroIndex, SessionData& data);
    void ReadTown(tinyxml2::XMLNode* root, i32 townIdx, SessionData& data);
    void ReadRoot(tinyxml2::XMLNode* root, SessionData& data);
};

// Campaign types as Ironfist's saves spell them.
enum CampaignType {
    CAMPAIGN_NONE      = 0,
    CAMPAIGN_ORIGINAL  = 1,
    CAMPAIGN_EXPANSION = 2
};


} // namespace ironfist::save

#endif
