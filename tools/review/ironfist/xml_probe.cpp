#include <IRONFIST/save_xml.h>
#include <cstdio>

int main() {
    ironfist::save::XmlFile xml;
    auto* root = xml.tempDoc->NewElement("probe");
    xml.tempDoc->InsertEndChild(root);
    ironfist::script::LuaTable table;
    table["a"].type = ironfist::script::MapVariableType::Table;
    table["a"].table["child"].type = ironfist::script::MapVariableType::String;
    table["a"].table["child"].value = "one";
    table["b"].type = ironfist::script::MapVariableType::String;
    table["b"].value = "two";
    table["c"] = table["a"];
    table["d"] = table["b"];
    xml.WriteMapVarTable(root, "test", table);
    tinyxml2::XMLPrinter printer;
    xml.tempDoc->Print(&printer);
    std::printf("%s\n", printer.CStr());
    auto restored = xml.ReadTable(root->FirstChild());
    std::printf("restored nested tables=%zu leaf=%s\n", restored.size(), restored["a"].table["child"].value.c_str());
    char players[2][32] = {"Alice", "Bob"};
    ironfist::xml::WriteArray(xml.tempDoc, root, "playerNames", players);
    char name[32] = {};
    auto* names = root->FirstChildElement("playerNames");
    ironfist::xml::QueryText(names, name);
    std::printf("player XML attribute=%s restored name='%s'\n", names->Attribute("value"), name);
}
