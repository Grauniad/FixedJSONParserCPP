#include "gtest/gtest.h"
#include <sstream>
#include <SimpleJSONFmt.h>

constexpr auto eof = std::ios_base::eofbit;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class FmtTest: public ::testing::Test {
public:

    void AssertOutput(const std::string& expected) {
        FmtJSON::Fmt(in, out);
        ASSERT_EQ(out.str(), expected);
    }


protected:
    std::stringstream in;
    std::stringstream out;
};

TEST_F(FmtTest, EmptyString) {
    AssertOutput("");
}

TEST_F(FmtTest, HelloWorld) {
    in << "Hello World!";
    AssertOutput("Hello World!");
}

TEST_F(FmtTest, NotJSON) {
    in << "Hello { World! }";
    AssertOutput("Hello { World! }");
}

TEST_F(FmtTest, SingleFieldJSON) {
    in << R"JSON({"msg":"Hello World!"})JSON";
    const std::string pretty =
            R"JSON({
    "msg": "Hello World!"
}
)JSON";
    AssertOutput(pretty);
}

TEST_F(FmtTest, TextSuffixJSON) {
    in << R"JSON({"msg":"Hello World!"} <-- That was JSON)JSON";
    const std::string pretty =
            R"PRETTY({
    "msg": "Hello World!"
}
 <-- That was JSON)PRETTY";
    AssertOutput(pretty);
}

TEST_F(FmtTest, TextPrefixJSON) {
    in << R"JSON(This is JSON:{"msg":"Hello World!"})JSON";
    const std::string pretty =
          R"PRETTY(This is JSON:{
    "msg": "Hello World!"
}
)PRETTY";
    AssertOutput(pretty);
}

TEST_F(FmtTest, LeadingSpacesTextPrefixJSON) {
    in << R"JSON(     This is JSON:{"msg":"Hello World!"})JSON";
    const std::string pretty =
            R"PRETTY(     This is JSON:{
    "msg": "Hello World!"
}
)PRETTY";
    AssertOutput(pretty);
}

TEST_F(FmtTest, BracesBracesEverywhere) {
    const std::string braces = "{}{}{{{{}{}{}{}{}}}}}{}{}{}{}{}{{{{}}}{}{}{}{}{}{}}}}{}{}{}{}{}";
    in << braces;
    AssertOutput(braces);
}

TEST_F(FmtTest, TrailingBrace) {
    const std::string braces = "End With Brace {";
    in << braces;
    AssertOutput(braces);
}

TEST_F(FmtTest, TrailingBraceSuffixc) {
    const std::string braces = "End With Brace{";
    in << braces;
    AssertOutput(braces);
}

TEST_F(FmtTest, JSONInASeaOfBraces) {
    const std::string braces = R"MSG({}{}{{{{}{}{}{}{}}}}}{}{}{}{}{}{{{{"msg":"Hello World!"}}}{}{}{}{}{}{}}}}{}{}{}{}{})MSG";
    const std::string pretty = R"MSG({}{}{{{{}{}{}{}{}}}}}{}{}{}{}{}{{{{
    "msg": "Hello World!"
}
}}{}{}{}{}{}{}}}}{}{}{}{}{})MSG";
    in << braces;
    AssertOutput(pretty);
}
