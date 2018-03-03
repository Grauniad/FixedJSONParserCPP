#include <SimpleJSON.h>
#include <iostream>
#include <util_time.h>

using namespace std;
using namespace nstimestamp;

#include "rapidjson/document.h"
using namespace rapidjson;

//namespace {
    const size_t NUM_ITERATIONS = 1000;
    const std::string JSON = R"JSON(
        {
            "glossary": {
                "title": "example glossary",
                "GlossDiv": {
                    "title": "S",
                    "GlossList": {
                        "GlossEntry": {
                            "ID": "SGML",
                            "SortAs": "SGML",
                            "GlossTerm": "Standard Generalized Markup Language",
                            "Acronym": "SGML",
                            "Abbrev": "ISO 8879:1986",
                            "GlossDef": {
                                "para": "A meta-markup language, used to create markup languages such as DocBook.",
                                "GlossSeeAlso": ["GML", "XML"]
                            },
                            "GlossSee": "markup"
                        }
                    }
                }
            }
        }
    )JSON";

    NewStringField(Abbrev);
    NewStringField(Acronym);
    NewStringArrayField(GlossSeeAlso);
    NewStringField(para);
    NewStringField(GlossSee);
    NewStringField(GlossTerm);
    NewStringField(ID);
    NewStringField(SortAs);
    NewStringField(title);

    namespace GlossDef_fields {
        typedef SimpleParsedJSON <GlossSeeAlso, para> JSON;
    }
    NewEmbededObject(GlossDef, GlossDef_fields::JSON);

    namespace GlossEntry_fields {
        typedef SimpleParsedJSON <
        Abbrev,
        Acronym,
        GlossDef,
        GlossSee,
        GlossTerm,
        ID,
        SortAs
        > JSON;
    }
    NewEmbededObject(GlossEntry, GlossEntry_fields::JSON);

    namespace GlossList_fields {
        typedef SimpleParsedJSON <
        GlossEntry
        > JSON;
    }
    NewEmbededObject(GlossList, GlossList_fields::JSON);

    namespace GlossDiv_fields {
            typedef SimpleParsedJSON <
            GlossList,
            title
            > JSON;
        }
    NewEmbededObject(GlossDiv, GlossDiv_fields::JSON);

    namespace glossary_fields {
        typedef SimpleParsedJSON <
        GlossDiv,
        title
        > JSON;
    }
    NewEmbededObject(glossary, glossary_fields::JSON);

    typedef SimpleParsedJSON <glossary> OutputJSON;
//}

std::string Fixed(const char* json) {
    std::string error;
    thread_local OutputJSON data;
    data.Clear();
    auto& entry = data.Get<glossary>().Get<GlossDiv>().Get<GlossList>().Get<GlossEntry>();
    if (data.Parse(json, error) && entry.Supplied<ID>()) {
        return entry.Get<ID>();
    } else {
        return "";
    }
}

std::string NativeRapidJSON(const char* json) {
    std::string result = "";
    Document d;
    d.Parse(json);

    auto glossIt = d.FindMember("glossary");
    if (glossIt != d.MemberEnd() && glossIt->value.IsObject()) {
        auto GlossDivIt = glossIt->value.FindMember("GlossDivIt");
        if (GlossDivIt != glossIt->value.MemberEnd() && GlossDivIt->value.IsObject()) {
            auto GlossList = GlossDivIt->value.FindMember("GlossList");
            if (GlossList != GlossDivIt->value.MemberEnd() && GlossList->value.IsObject()) {
                auto GlossEntry = GlossList->value.FindMember("GlossEntry");
                if (GlossEntry != GlossList->value.MemberEnd() && GlossEntry->value.IsObject()) {
                    auto idIt = GlossEntry->value.FindMember("ID");
                    if (idIt != GlossEntry->value.MemberEnd() && idIt->value.IsString()) {
                        return idIt->value.GetString();
                    }
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    Fixed(JSON.c_str());
    NativeRapidJSON(JSON.c_str());
    Time start;
    for (size_t i = 0; i< NUM_ITERATIONS; ++i) {
        NativeRapidJSON(JSON.c_str());
    }
    Time end;
    long NATIVE = end.DiffUSecs(start);
    std::cout << "Native took: " << end.DiffUSecs(start) / (1.0*NUM_ITERATIONS) << "us/itter" << std::endl;

    start.SetNow();
    for (size_t i = 0; i< NUM_ITERATIONS; ++i) {
        Fixed(JSON.c_str());
    }
    end.SetNow();
    long FIXED = end.DiffUSecs(start);
    std::cout << "Fixed took: " << end.DiffUSecs(start) / (1.0*NUM_ITERATIONS) << "us/itter" << std::endl;

    std::cout << "Speedup: " << (100.0*(NATIVE - FIXED)) / (NATIVE) << "%" << std::endl;
    return 0;
}
