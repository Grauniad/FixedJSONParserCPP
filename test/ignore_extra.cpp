#include "gtest/gtest.h"
#include <SimpleJSON.h>

using namespace std;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

NewStringField(target)
NewStringField(final)

using BaseParser = SimpleParsedJSON<target, final>;
NewEmbededObject(inner, BaseParser);
using InnerParser = SimpleParsedJSON<inner, target>;

TEST(FieldInBase, ValidJSON ) {
    BaseParser  json(BaseParser::UnknownFieldMode::IGNORE);
    std::string rawJson = R"JSON(
        {
           "target": "Hello World!"
        }
    )JSON";

    std::string error;
    ASSERT_TRUE(json.Parse(rawJson.c_str(), error));
    ASSERT_EQ(json.Get<target>(), "Hello World!");
}

TEST(FieldInBase, ExtraParallelScalars ) {
    BaseParser  json(BaseParser::UnknownFieldMode::IGNORE);
    std::string rawJson = R"JSON(
        {
           "target": "Hello World!",
           "dummy": "Don't want this!",
           "dummy_int": 1,
           "dummy_neg_int": -1,
           "dummy_large_int": 999999999999,
           "dummy_large_neg_int": -999999999999,
           "dummy_double": 123.345,
           "dummy_null": null,
           "dummy_bool": true
        }
    )JSON";

    std::string error;
    ASSERT_TRUE(json.Parse(rawJson.c_str(), error)) << error;
    ASSERT_EQ(json.Get<target>(), "Hello World!");
}

TEST(FieldInBase, ExtraParallelArrays ) {
    BaseParser  json(BaseParser::UnknownFieldMode::IGNORE);
    std::string rawJson = R"JSON(
        {
           "dummy": [],
           "dummy_strings": ["Hello", "World"],
           "dummy_ints": [1,2,3],
           "dummy_doubles": [1.3,2.0],
           "dummy_hetro": ["hello", 2, "worlds", 2.0],
           "target": "Hello World!"
        }
    )JSON";

    std::string error;
    ASSERT_TRUE(json.Parse(rawJson.c_str(), error)) << error;
    ASSERT_EQ(json.Get<target>(), "Hello World!");
}

TEST(FieldInBase, ExtraParallelObject ) {
    BaseParser  json(BaseParser::UnknownFieldMode::IGNORE);
    std::string rawJson = R"JSON(
        {
           "dummy_object": {
               "dummy": "Don't want this!",
               "dummy_int": 1,
               "dummy_neg_int": -1,
               "dummy_large_int": 999999999999,
               "dummy_large_neg_int": -999999999999,
               "dummy_double": 123.345,
               "dummy_null": null,
               "dummy_bool": true,
               "dummy": [],
               "dummy_strings": ["Hello", "World"],
               "dummy_ints": [1,2,3],
               "dummy_doubles": [1.3,2.0],
               "dummy_hetro": ["hello", 2, "worlds", 2.0],
               "target": "Inner Hello World!"
           },

           "target": "Hello World!",

           "dummy_object_2": {
               "dummy": "Don't want this!",
               "dummy_int": 1,
               "dummy_neg_int": -1,
               "dummy_large_int": 999999999999,
               "dummy_large_neg_int": -999999999999,
               "dummy_double": 123.345,
               "dummy_null": null,
               "dumm_object_3": {
                   "dummy": "Don't want this!",
                   "dummy_int": 1,
                   "dummy_neg_int": -1,
                   "dummy_large_int": 999999999999,
                   "dummy_large_neg_int": -999999999999,
                   "dummy_double": 123.345,
                   "dummy_null": null,
                   "dummy_bool": true,
                   "dummy": [],
                   "dummy_strings": ["Hello", "World"],
                   "dummy_ints": [1,2,3],
                   "dummy_doubles": [1.3,2.0],
                   "dummy_hetro": ["hello", 2, "worlds", 2.0],
                   "target": "Inner Hello World! (3)"
               },
               "dummy_bool": true,
               "dummy": [],
               "dummy_strings": ["Hello", "World"],
               "dummy_ints": [1,2,3],
               "dummy_doubles": [1.3,2.0],
               "dummy_hetro": ["hello", 2, "worlds", 2.0],
               "target": "Inner Hello World! (2)"
           },

           "final": "Final String"
        }
    )JSON";

    std::string error;
    ASSERT_TRUE(json.Parse(rawJson.c_str(), error)) << error;
    ASSERT_EQ(json.Get<target>(), "Hello World!");
    ASSERT_EQ(json.Get<final>(), "Final String");
}

TEST(FieldInBase, InnerObject ) {
    InnerParser json(InnerParser::UnknownFieldMode::IGNORE);
    std::string rawJson = R"JSON(
        {
           "ignore_this": {
               "and_this": {
                  "dummy": "Don't want this"
               },
               "dummy": "Don't want this"
            },
            "inner": {
               "target": "Hello World!"
            },
            "target": "Outer"
        }
    )JSON";

    std::string error;
    ASSERT_TRUE(json.Parse(rawJson.c_str(), error)) << error;
    ASSERT_EQ(json.Get<inner>().Get<target>(), "Hello World!");
    ASSERT_EQ(json.Get<target>(), "Outer");
}
