#include <IRONFIST/xml_utils.h>

#include <cstring>

namespace UtilsXML {

tinyxml2::XMLError QueryShortText(tinyxml2::XMLElement* el, i16* dest) {
    i32 val;
    tinyxml2::XMLError res = el->QueryIntText(&val);
    *dest = static_cast<i16>(val);
    return res;
}

tinyxml2::XMLError QueryCharText(tinyxml2::XMLElement* el, char* dest) {
    i32 val;
    tinyxml2::XMLError res = el->QueryIntText(&val);
    *dest = static_cast<char>(val);
    return res;
}

tinyxml2::XMLError QueryCharText(tinyxml2::XMLElement* el, i8* dest) {
    i32 val;
    tinyxml2::XMLError res = el->QueryIntText(&val);
    *dest = static_cast<i8>(val);
    return res;
}

tinyxml2::XMLError QueryCharText(tinyxml2::XMLElement* el, u8* dest) {
    i32 val;
    tinyxml2::XMLError res = el->QueryIntText(&val);
    *dest = static_cast<u8>(val);
    return res;
}

void QueryText(tinyxml2::XMLElement* el, char* dest) {
    const char* text = el->GetText();
    if (text)
        strcpy(dest, text);
    else
        strcpy(dest, "");
}

void QueryText(tinyxml2::XMLElement* el, std::string& dest) {
    const char* text = el->GetText();
    dest = text != nullptr ? text : "";
}

const char* QueryTextAttribute(tinyxml2::XMLElement* el, const char* attribute) {
    const char* text = nullptr;
    el->QueryStringAttribute(attribute, &text);
    if (text)
        return text;
    else
        return "invalid name";
}

void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name,
    const std::vector<i32>& src
) {
    for (size_t i = 0; i < src.size(); i++) {
        tinyxml2::XMLElement* elem = doc->NewElement(name);
        elem->SetAttribute("index", static_cast<i32>(i));
        elem->SetAttribute("value", src[i]);
        dest->InsertEndChild(elem);
    }
}

void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name,
    const std::map<i32, std::string>& src
) {
    for (auto& i : src) {
        tinyxml2::XMLElement* elem = doc->NewElement(name);
        elem->SetAttribute("index", i.first);
        elem->SetAttribute("value", i.second.c_str());
        dest->InsertEndChild(elem);
    }
}

void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name,
    const std::map<i32, i32>& src
) {
    for (auto& i : src) {
        tinyxml2::XMLElement* elem = doc->NewElement(name);
        elem->SetAttribute("index", i.first);
        elem->SetAttribute("value", i.second);
        dest->InsertEndChild(elem);
    }
}

void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name,
    const std::map<i32, std::set<i32>>& src
) {
    for (auto& i : src) {
        for (auto val : i.second) {
            tinyxml2::XMLElement* elem = doc->NewElement(name);
            elem->SetAttribute("index", i.first);
            elem->SetAttribute("value", val);
            dest->InsertEndChild(elem);
        }
    }
}

void ReadMask(tinyxml2::XMLElement* src, i64* dest) {
    std::string text = src->GetText();
    i32 i = 0;
    i64 result = 0;
    for (auto c : text) {
        if (c == '0' || c == '1') {
            if (c == '1')
                result |= (1LL << (sizeof(i64) * 8 - i - 1));
            i++;
        }
    }
    // flipping
    result = (result >> 32) | ((result & 0xFFFFFFFF) << 32);
    *dest = result;
}

void ReadBinary(tinyxml2::XMLElement* src, i32* dest) {
    std::string text = src->GetText();
    i32 i = 0;
    i32 result = 0;
    for (auto c : text) {
        if (c == '0' || c == '1') {
            if (c == '1')
                result |= (1 << i);
            i++;
        }
    }
    *dest = result;
}

XMLFile::XMLFile() {
    tempDoc = new tinyxml2::XMLDocument(true);
}

XMLFile::~XMLFile() {
    if (tempDoc)
        delete tempDoc;
}

const char* XMLFile::GetError() {
    return tempDoc->ErrorStr();
}

}
