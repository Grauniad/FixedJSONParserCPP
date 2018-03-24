#include <SimpleJSON.h>
#include <iostream>
#include <util_time.h>

using namespace std;
using namespace nstimestamp;

#include "rapidjson/document.h"
using namespace rapidjson;

namespace {
    const size_t NUM_ITERATIONS = 1000000;
    const char *json = "{\"project\":\"rapidjson\",\"stars\":10}";
    NewStringField(project);
    NewUIntField(stars);
    using UserData = SimpleParsedJSON<project, stars>;
}

std::string Fixed() {
    std::string error;
    thread_local UserData data;
    data.Clear();
    if (data.Parse(json, error)) {
        data.Get<stars>() += 1;
        return data.GetJSONString();
    } else {
        return "";
    }
}

std::string NativeRapidJSON() {
    // 1. Parse a JSON string into DOM.
    Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    return buffer.GetString();
}

int main(int argc, char **argv) {
    Time start;
    for (size_t i = 0; i< NUM_ITERATIONS; ++i) {
        Fixed();
    }
    Time end;
    long FIXED = end.DiffUSecs(start);
    std::cout << "Fixed took: " << end.DiffUSecs(start) / (1.0*NUM_ITERATIONS) << "us/itter" << std::endl;

    start.SetNow();
    for (size_t i = 0; i< NUM_ITERATIONS; ++i) {
        NativeRapidJSON();
    }
    end.SetNow();
    long NATIVE = end.DiffUSecs(start);
    std::cout << "Native took: " << end.DiffUSecs(start) / (1.0*NUM_ITERATIONS) << "us/itter" << std::endl;
    std::cout << "Speedup: " << (100.0*(NATIVE - FIXED)) / (NATIVE) << "%" << std::endl;
    return 0;
}
