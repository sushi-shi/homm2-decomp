#!/usr/bin/env python3
"""Write the emission-order probe manifests recorded in this directory.

Run from the repository root inside `nix develop .#build`, then feed each
manifest to the runner:

    python3 docs/matching/emission-order/generate_manifests.py
    for name in audiere-owner audiere-node-shape any-vs-plain dimmer-nesting dimmer-flags; do
        python3 -m homm2.permute.emission_order build/probe/$name.json \
            --output build/probe/$name
    done

The real-unit manifests read the current AudiereEffects/AudiereMusic sources
and soundBackends.h; variant headers shadow the repository copy through
`/I{work}`. Results are summarized in results.md beside this script.
"""

from __future__ import annotations

import json
from pathlib import Path

OUT = Path("build/probe")
BASE_FLAGS = ["/nologo", "/c", "/Od", "/MT", "/Gr", "/G5", "/Ob1", "/Gf", "/Gi-",
              "/GX", "/DNO_STRICT", "/Gy"]
OB = [{"name": "ob1"}, {"name": "ob2", "flags_add": ["/Ob2"], "flags_remove": ["/Ob1"]}]
GY = [{"name": "gy"}, {"name": "nogy", "flags_remove": ["/Gy"]}]
AUDIERE_ALIASES = [
    ["^\\?PurgeFinishedAudiereSamples", "U"], ["^\\?FindAudiereSample", "F"],
    ["^\\?PlayAudiereSample", "P"], ["^\\?AudiereSampleIterationActive", "I"],
    ["^\\?\\?1(\\?\\$)?Audiere(Sample)?Node", "N"],
    ["^\\?\\?1\\?\\$RefPtr@VOutputStream", "S"], ["^\\?\\?4\\?\\$RefPtr@VOutputStream", "A"],
    ["^\\?\\?1\\?\\$RefPtr@VAudioDevice", "D"], ["^\\?\\?1\\?\\$RefPtr@VSampleSource", "s"],
    ["^\\?\\?4\\?\\$RefPtr@VSampleSource", "a"], ["^_\\$E\\d+$", "E"],
    ["^\\?id@\\?\\$ctype@G@std@@\\$E", "C"], ["^\\?StopAudiereMusic", "M"],
]
NODE_DEF = "H2_RETAIL_INLINE AudiereSampleNode::~AudiereSampleNode() {}\n"


def read(path: str) -> str:
    return Path(path).read_text(encoding="latin-1")


def write(name: str, manifest: dict) -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / f"{name}.json").write_text(json.dumps(manifest, indent=1))


def audiere_owner() -> None:
    """Where the node destructor is declared/defined, across Effects and Music."""
    effects, music = read("src/BASE/AudiereEffects.cpp"), read("src/BASE/AudiereMusic.cpp")
    header = read("include/BASE/soundBackends.h")
    effects = effects.replace("\n" + NODE_DEF, "\n@@d_eof@@", 1)
    for slot, anchor in (("d_before_purge", "VA(0x004cc740"), ("d_after_find", "VA(0x004cc8f0"),
                         ("d_after_play", "VA(0x004ccc90")):
        effects = effects.replace(anchor, f"@@{slot}@@" + anchor, 1)
    music = music.replace("H2_ENUM_END(AudiereMusicConstant)\n",
                          "H2_ENUM_END(AudiereMusicConstant)\n@@m_top@@", 1)
    music = music.rstrip("\n") + "\n@@m_eof@@"
    header = header.replace("    H2_RETAIL_INLINE ~AudiereSampleNode();\n", "@@hdecl@@", 1)
    inline_def = "inline AudiereSampleNode::~AudiereSampleNode() {}\n\n"
    plain_def = "AudiereSampleNode::~AudiereSampleNode() {}\n\n"
    effects_options = [{"name": "e_none"}]
    for slot in ("d_before_purge", "d_after_find", "d_after_play", "d_eof"):
        for form, text in (("inline", inline_def), ("plain", plain_def)):
            effects_options.append({"name": f"e_{slot[2:]}_{form}", "slots": {slot: text}})
    write("audiere-owner", {
        "schema": 1,
        "files": {"AudiereEffects.cpp": effects, "AudiereMusic.cpp": music,
                  "BASE/soundBackends.h": header},
        "units": ["AudiereEffects.cpp", "AudiereMusic.cpp"],
        "flags": BASE_FLAGS + ["/I{work}"],
        "defaults": {slot: "" for slot in ("d_eof", "d_before_purge", "d_after_find",
                                            "d_after_play", "m_top", "m_eof")},
        "axes": [
            {"name": "header", "options": [
                {"name": "h_inline", "slots": {"hdecl": "    inline ~AudiereSampleNode();\n"}},
                {"name": "h_plain", "slots": {"hdecl": "    ~AudiereSampleNode();\n"}},
                {"name": "h_inclass", "slots": {"hdecl": "    ~AudiereSampleNode() {}\n"}},
                {"name": "h_implicit", "slots": {"hdecl": ""}}]},
            {"name": "effects_def", "options": effects_options},
            {"name": "music_def", "options": [
                {"name": "m_none"}, {"name": "m_top", "slots": {"m_top": "\n" + plain_def}},
                {"name": "m_eof", "slots": {"m_eof": "\n" + plain_def}}]},
            {"name": "inline", "options": OB}],
        "aliases": AUDIERE_ALIASES, "track": ["P", "U"],
    })


def audiere_node_shape() -> None:
    """Plain vs class-template node and destructor placement inside Effects."""
    effects = read("src/BASE/AudiereEffects.cpp").replace(NODE_DEF, "@@effdef@@", 1)
    header = read("include/BASE/soundBackends.h")
    start = header.index("struct AudiereSampleNode {")
    header = header[:start] + "@@node@@" + header[header.index("};", start) + 3:]
    members = ("    audiere::OutputStreamPtr stream;\n    {resource}* sampleResource;\n"
               "    {name}* next;\n\n    {name}({resource}* resource, {name}* nextNode) {{\n"
               "        stream = NULL;\n        sampleResource = resource;\n"
               "        next = nextNode;\n    }}\n@@dtordecl@@}};\n")
    plain = "struct AudiereSampleNode {\n" + members.format(
        resource="class sample", name="AudiereSampleNode")
    template = ("template<class Resource> struct AudiereNode {\n"
                + members.format(resource="Resource", name="AudiereNode")
                + "typedef AudiereNode<class sample> AudiereSampleNode;\n")
    tdef = "template<class Resource> {inline}AudiereNode<Resource>::~AudiereNode() {{}}\n"
    shapes = [
        ("plain_eof_inline", plain, "    inline ~AudiereSampleNode();\n",
         "inline AudiereSampleNode::~AudiereSampleNode() {}\n"),
        ("plain_inclass", plain, "    ~AudiereSampleNode() {}\n", ""),
        ("plain_implicit", plain, "", ""),
        ("tmpl_inclass", template, "    ~AudiereNode() {}\n", ""),
        ("tmpl_implicit", template, "", ""),
        ("tmpl_eof", template, "    ~AudiereNode();\n", tdef.format(inline="")),
        ("tmpl_eof_inline", template, "    inline ~AudiereNode();\n",
         tdef.format(inline="inline ")),
        ("tmpl_hdr_after", template + tdef.format(inline="inline "),
         "    inline ~AudiereNode();\n", ""),
        ("tmpl_explicit_eof", template, "    ~AudiereNode();\n",
         tdef.format(inline="") + "template struct AudiereNode<sample>;\n"),
    ]
    write("audiere-node-shape", {
        "schema": 1,
        "files": {"AudiereEffects.cpp": effects, "BASE/soundBackends.h": header},
        "units": ["AudiereEffects.cpp"], "flags": BASE_FLAGS + ["/I{work}"],
        "axes": [{"name": "node", "options": [
                     {"name": name, "slots": {"node": node, "dtordecl": decl, "effdef": define}}
                     for name, node, decl, define in shapes]},
                 {"name": "inline", "options": OB}, {"name": "gy", "options": GY}],
        "aliases": AUDIERE_ALIASES, "track": ["P", "U"],
        "expect": {"AudiereEffects.cpp": "U F P I S A E E N C"},
    })


def any_vs_plain() -> None:
    """LINK selection between an inline (Any) and an ordinary destructor copy."""
    header = ("struct Stream { virtual void unref() = 0; };\n"
              "template<class T> struct Ref { T* p; Ref() { p = 0; } "
              "~Ref() { if (p) p->unref(); p = 0; } };\n"
              "struct Node { Ref<Stream> s; Node* next; @@hdecl@@ };\n")
    first = ('#include "node.h"\nNode* gList;\n'
             "void Purge() { while (gList) { Node* n = gList; gList = n->next; delete n; } }\n"
             "@@adef@@\nint main() { Purge(); return 0; }\n")
    second = '#include "node.h"\n@@bdef@@\nint Other() { return 1; }\n'
    write("any-vs-plain", {
        "schema": 1, "files": {"node.h": header, "a.cpp": first, "b.cpp": second},
        "units": ["a.cpp", "b.cpp"], "flags": BASE_FLAGS,
        "defaults": {"hdecl": "~Node();", "adef": "", "bdef": ""},
        "axes": [
            {"name": "a", "options": [
                {"name": "a_inline", "slots": {"adef": "inline Node::~Node() {}"}},
                {"name": "a_plain", "slots": {"adef": "Node::~Node() {}"}}, {"name": "a_none"}]},
            {"name": "b", "options": [
                {"name": "b_plain", "slots": {"bdef": "Node::~Node() {}"}},
                {"name": "b_inline_used", "slots": {
                    "bdef": "inline Node::~Node() {}\nvoid Kill(Node* n) { delete n; }"}},
                {"name": "b_none"}]},
            {"name": "order", "options": [{"name": "ab"},
                                          {"name": "ba", "units": ["b.cpp", "a.cpp"]}]}],
        "aliases": [["^\\?\\?1Node", "N"], ["^\\?Purge", "U"], ["^\\?Other", "O"],
                    ["^\\?Kill", "K"], ["^_main", "M"], ["^\\?\\?1\\?\\$Ref", "S"]],
        "track": ["U"],
        "link": {"libs": ["LIBCMT.LIB"], "flags": ["/SUBSYSTEM:CONSOLE", "/OPT:NOREF"]},
    })


def dimmer(name: str, nests: list[str], dtors: list[str], flags: list[dict]) -> None:
    """Reduced dimmerWidget: two constructors, Read, Main, Draw, destructor."""
    base = ("struct msg;\nclass wbase { public: wbase(short,short); virtual ~wbase(); "
            "virtual int Main(msg&); virtual void Draw(); short x, y; void Dim(); };\n")
    source = ('#include "base.h"\n@@open@@\nclass dw : public wbase { public: dw(); '
              "dw(short a, short b); @@dtordecl@@ virtual void Draw(); "
              "virtual int Main(msg&); void Read(); };\n@@close@@\n"
              "@@Q@@dw() : wbase(0,0) {}\n@@Q@@dw(short a, short b) : wbase(a,b) {}\n"
              "void @@Q@@Read() { x = 1; }\nint @@Q@@Main(msg& m) { return wbase::Main(m); }\n"
              "void @@Q@@Draw() { Dim(); }\n@@dtordef@@\n")
    nest_options = {
        "plain": ("", "", "dw::"),
        "nested_struct": ("struct outer {", "};", "outer::dw::"),
        "nested_class_virtual": ("class outer : public wbase { public: outer(); "
                                 "virtual void Draw();", "};", "outer::dw::"),
        "namespace": ("namespace ns {", "}", "ns::dw::"),
    }
    dtor_options = {
        "ool_end": ("virtual ~dw();", "@@Q@@~dw() {}"),
        "inline_end": ("inline virtual ~dw();", "inline @@Q@@~dw() {}"),
        "inclass": ("virtual ~dw() {}", ""),
        "undefined": ("virtual ~dw();", ""),
    }
    write(name, {
        "schema": 1, "files": {"base.h": base, "d.cpp": source}, "units": ["d.cpp"],
        "flags": BASE_FLAGS,
        "axes": [
            {"name": "nest", "options": [
                {"name": key, "slots": dict(zip(("open", "close", "Q"), nest_options[key]))}
                for key in nests]},
            {"name": "dtor", "options": [
                {"name": key, "slots": dict(zip(("dtordecl", "dtordef"), dtor_options[key]))}
                for key in dtors]},
            {"name": "flag", "options": flags}],
        "aliases": [["^\\?\\?0dw@[a-z@]*@QAE@XZ", "c0"], ["^\\?\\?0dw@[a-z@]*@QAE@FF@Z", "cA"],
                    ["^\\?Read@dw", "R"], ["^\\?Main@dw", "M"], ["^\\?Draw@dw", "D"],
                    ["^\\?\\?_Gdw", "G"], ["^\\?\\?1dw", "X"]],
        "expect": {"d.cpp": "c0 cA R M D G X"},
    })


def main() -> None:
    audiere_owner()
    audiere_node_shape()
    any_vs_plain()
    dimmer("dimmer-nesting", ["plain", "nested_struct", "nested_class_virtual", "namespace"],
           ["ool_end", "inline_end", "inclass", "undefined"], GY)
    flags = [{"name": name, "flags_remove": ["/Gy"], "flags_add": extra} for name, extra in (
        ("nogy", []), ("Zi", ["/Zi"]), ("Z7", ["/Z7"]), ("GZ", ["/GZ"]), ("Ge", ["/Ge"]),
        ("Gh", ["/Gh"]), ("Gs0", ["/Gs0"]), ("Gm_Zi", ["/Gm", "/Zi"]), ("Oy-", ["/Oy-"]))]
    dimmer("dimmer-flags", ["plain"], ["ool_end", "inline_end", "inclass"], flags)


if __name__ == "__main__":
    main()
