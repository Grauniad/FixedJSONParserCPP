//
// Created by lukeh on 25/11/2019.
//

#include "gtest/gtest.h"
#include <SimpleJSON.h>

using namespace std;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

NewIntField(IntField1)

TEST(JSONParsing,EmbededObjectInParallelArray) {
    std::string rawJson = R"JSON({
        "OuterObjects1": {
            "Objects": [
                {
                    "IntField1": 1
                },
                {
                    "IntField1": 2
                }
            ]
        },
        "OuterObjects2": {
            "Objects": [
                {
                    "IntField1": 3
                },
                {
                    "IntField1": 4
                },
                {
                    "IntField1": 5
                }
            ]
        }
    }
    )JSON";
    typedef SimpleParsedJSON<IntField1> Object;
    NewObjectArray(Objects,Object);

    using Outer = SimpleParsedJSON<Objects>;
    NewEmbededObject(OuterObjects1, Outer);
    NewEmbededObject(OuterObjects2, Outer);

    typedef SimpleParsedJSON<OuterObjects1, OuterObjects2> JSON;

    JSON json;

    std::string error;
    bool ok = json.Parse(rawJson.c_str(),error);

    ASSERT_TRUE(ok);

    auto& outer1 = json.Get<OuterObjects1>();
    auto& outer2 = json.Get<OuterObjects2>();

    auto& objects1 = outer1.Get<Objects>();
    auto& objects2 = outer2.Get<Objects>();

    ASSERT_EQ(objects1.size() , 2 );

    auto& object1 = *objects1[0];
    auto& object2 = *objects1[1];

    ASSERT_EQ(object1.Get<IntField1>() , 1 );

    ASSERT_EQ(object2.Get<IntField1>() , 2 );

    ASSERT_EQ(objects2.size() , 3 );

    auto& object3 = *objects2[0];
    auto& object4 = *objects2[1];
    auto& object5 = *objects2[2];

    ASSERT_EQ(object3.Get<IntField1>() , 3 );

    ASSERT_EQ(object4.Get<IntField1>() , 4 );

    ASSERT_EQ(object5.Get<IntField1>() , 5 );

}

TEST(JSONParsing,EmbededObjectInArray) {
    std::string rawJson = R"JSON({
        "OuterObjects": [
            {
                "Objects": [
                    {
                        "IntField1": 1
                    },
                    {
                        "IntField1": 2
                    }
                ]
            },
            {
                "Objects": [
                    {
                        "IntField1": 3
                    },
                    {
                        "IntField1": 4
                    },
                    {
                        "IntField1": 5
                    }
                ]
            }
        ]
    }
    )JSON";

    typedef SimpleParsedJSON<IntField1> Object;
    NewObjectArray(Objects,Object);

    NewObjectArray(OuterObjects,SimpleParsedJSON<Objects>);;

    typedef SimpleParsedJSON<OuterObjects> JSON;

    JSON json;

    std::string error;
    bool ok = json.Parse(rawJson.c_str(),error);

    ASSERT_TRUE(ok);

    auto& outerObjects = json.Get<OuterObjects>();

    ASSERT_EQ(outerObjects.size() , 2);

    auto& outer1 = *outerObjects[0];
    auto& outer2 = *outerObjects[1];

    auto& objects1 = outer1.Get<Objects>();
    auto& objects2 = outer2.Get<Objects>();

    ASSERT_EQ(objects1.size() , 2 );

    auto& object1 = *objects1[0];
    auto& object2 = *objects1[1];

    ASSERT_EQ(object1.Get<IntField1>() , 1 );

    ASSERT_EQ(object2.Get<IntField1>() , 2 );

    ASSERT_EQ(objects2.size() , 3 );

    auto& object3 = *objects2[0];
    auto& object4 = *objects2[1];
    auto& object5 = *objects2[2];

    ASSERT_EQ(object3.Get<IntField1>() , 3 );

    ASSERT_EQ(object4.Get<IntField1>() , 4 );

    ASSERT_EQ(object5.Get<IntField1>() , 5 );

}
