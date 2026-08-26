#ifndef HOMM2_IRONFIST_XML_UTILS_H
#define HOMM2_IRONFIST_XML_UTILS_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include <tinyxml2.h>

#include <Ints.h>

namespace ironfist::xml {

tinyxml2::XMLError QueryShortText(tinyxml2::XMLElement* el, i16* dest);
tinyxml2::XMLError QueryCharText(tinyxml2::XMLElement* el, char* dest);
tinyxml2::XMLError QueryCharText(tinyxml2::XMLElement* el, i8* dest);
tinyxml2::XMLError QueryCharText(tinyxml2::XMLElement* el, u8* dest);
void QueryText(tinyxml2::XMLElement* el, char* dest);
void QueryText(tinyxml2::XMLElement* el, std::string& dest);
const char* QueryTextAttribute(tinyxml2::XMLElement* el, const char* attribute);

template <typename T>
void PushBack(tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name, const T& val) {
    tinyxml2::XMLElement* elem = doc->NewElement(name);
    elem->SetText(val);
    dest->InsertEndChild(elem);
}

template <typename T, size_t N>
void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name, const T (&src)[N]
) {
    for (size_t i = 0; i < N; i++) {
        tinyxml2::XMLElement* elem = doc->NewElement(name);
        elem->SetAttribute("index", static_cast<i32>(i));
        elem->SetAttribute("value", static_cast<i32>(src[i]));
        dest->InsertEndChild(elem);
    }
}

template <size_t M, size_t N>
void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name, const char (&src)[M][N]
) {
    for (size_t i = 0; i < M; i++) {
        tinyxml2::XMLElement* elem = doc->NewElement(name);
        elem->SetAttribute("index", static_cast<i32>(i));
        elem->SetAttribute("value", src[i]);
        dest->InsertEndChild(elem);
    }
}

void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name,
    const std::vector<i32>& src
);
void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name,
    const std::map<i32, std::string>& src
);
void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name,
    const std::map<i32, i32>& src
);
void WriteArray(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* dest, const char* name,
    const std::map<i32, std::set<i32>>& src
);

void ReadMask(tinyxml2::XMLElement* src, i64* dest);
void ReadBinary(tinyxml2::XMLElement* src, i32* dest);

class XMLFile {
public:
    tinyxml2::XMLDocument* tempDoc = nullptr;
    XMLFile();
    ~XMLFile();
    const char* GetError();
};

} // namespace ironfist::xml

#endif
