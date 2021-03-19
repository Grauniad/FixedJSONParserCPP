#include "gtest/gtest.h"
#include <SimpleJSON.h>
#include <util_time.h>

using namespace std;
using namespace nstimestamp;

namespace json {
    /*
     * For our purposes we consider a relatively light object:
     *
     * {
     *     reqId: 23,
     *     people:  [ {
     *         name: "person",
     *         age: 23,
     *         alias: "cool dude"
     *     },
     *     {
     *          //....
     *     }.
     *     //...
     *     ]
     * }
     *
     *
     */
    NewStringField(name);
    NewStringField(alias);
    NewUIntField(age);
    using Person =  SimpleParsedJSON<name, alias, age>;
    NewObjectArray(people, Person);

    NewUI64Field(reqId);

    using Result = SimpleParsedJSON<reqId, people>;

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class JsonArrayTest: public ::testing::Test {
protected:
    struct TPerson {
        std::string name;
        std::string alias;
        unsigned int age;
    };
    JsonArrayTest() {
        json::Result parsedData;
        AddData(dataSet1, parsedData);
        dataSet1JSON = parsedData.GetJSONString();
        parsedData.Clear();

        AddData(dataSet2, parsedData);
        dataSet2JSON = parsedData.GetJSONString();
        parsedData.Clear();

        AddData(dataSet3, parsedData);
        dataSet3JSON = parsedData.GetJSONString();
        parsedData.Clear();

    }

    const std::vector<TPerson> dataSet1 = {
            {"luke", "Grauniad", 23},
            {"bill", "llib", 32},
            {"teddy", "bear", 5},
            {"teddy", "bear", 5},
    };

    const std::vector<TPerson> dataSet2 = {
            {"ben", "Grauniad", 23},
            {"neb", "llib", 7},
    };

    const std::vector<TPerson> dataSet3 = {
            {"luke", "Grauniad", 23},
            {"bill", "llib", 32},
            {"teddy", "bear", 5},
            {"teddy", "bear", 5},
            {"another", "one", 6},
            {"and", "another", 6},
            {"yet", "another", 6},
            {"another", "seems appropriate", 6},
            {"yet", "more", 6},
    };
    std::string dataSet1JSON;
    std::string dataSet2JSON;
    std::string dataSet3JSON;

    void AddData(const std::vector<TPerson>& data, json::Result& result) {
        using namespace json;
        auto& peopleArray = result.Get<people>();
        const size_t preSize = peopleArray.size();
        peopleArray.reserve(preSize + data.size());

        for (const TPerson& dperson: data) {
            peopleArray.InsertAtBack();
            Person& person = *peopleArray.back();
            person.Get<name>() = dperson.name;
            person.Get<age>() = dperson.age;
            person.Get<alias>() = dperson.alias;
        }
    }

    void CheckData(const std::vector<TPerson>& data, json::Result& result) {
        using namespace json;
        auto& peopleArray = result.Get<people>();
        ASSERT_EQ(peopleArray.size(), data.size());

        for (size_t i = 0; i < peopleArray.size(); ++i) {
            Person& person = *peopleArray[i];
            const TPerson& dataPerson = data[i];

            ASSERT_EQ(person.Get<name>(), dataPerson.name);
            ASSERT_EQ(person.Get<alias>(), dataPerson.alias);
            ASSERT_EQ(person.Get<age>(), dataPerson.age);

        }
    }

};

TEST_F(JsonArrayTest, NewObject) {
    using namespace json;
    Result result;

    ASSERT_FALSE(result.Supplied<reqId>());
    ASSERT_EQ(result.Get<reqId>(), 0);

    ASSERT_FALSE(result.Supplied<people>());
    ASSERT_EQ(result.Get<people>().size(), 0);
}

TEST_F(JsonArrayTest, NonCachedParses) {
    using namespace json;
    Result result;
    std::string error;
    Time start;
    for (size_t i =0; i < 1000; ++i) {
        ASSERT_TRUE(result.Parse(dataSet1JSON.c_str(), error));
        CheckData(dataSet1, result);
        result.Clear();

        ASSERT_TRUE(result.Parse(dataSet2JSON.c_str(), error));
        CheckData(dataSet2, result);
        result.Clear();

        ASSERT_TRUE(result.Parse(dataSet3JSON.c_str(), error));
        CheckData(dataSet3, result);
        result.Clear();
    }
    Time end;
    std::cout << "Non cached loop: " << end.DiffUSecs(start) << "us" << std::endl;
}

TEST_F(JsonArrayTest, CachedParses) {
    using namespace json;
    Result result;
    std::string error;
    Time start;
    for (size_t i =0; i < 1000; ++i) {
        ASSERT_TRUE(result.Parse(dataSet1JSON.c_str(), error));
        CheckData(dataSet1, result);
        result.Get<people>().ClearToCache(10);
        result.Clear();

        ASSERT_TRUE(result.Parse(dataSet2JSON.c_str(), error));
        CheckData(dataSet2, result);
        result.Get<people>().ClearToCache(10);
        result.Clear();

        ASSERT_TRUE(result.Parse(dataSet3JSON.c_str(), error));
        CheckData(dataSet3, result);
        result.Get<people>().ClearToCache(10);
        result.Clear();
    }
    Time end;
    std::cout << "Non cached loop: " << end.DiffUSecs(start) << "us" << std::endl;
}

TEST_F(JsonArrayTest, Populate) {
    using namespace json;
    Result result;
    Time start;
    for (size_t i =0; i < 1000; ++i) {
        AddData(dataSet1, result);
        CheckData(dataSet1, result);
        result.Clear();

        AddData(dataSet2, result);
        CheckData(dataSet2, result);
        result.Clear();

        AddData(dataSet3, result);
        CheckData(dataSet3, result);
        result.Clear();
    }
    Time end;
    std::cout << "Non cached loop: " << end.DiffUSecs(start) << "us" << std::endl;
}

TEST_F(JsonArrayTest, PopulateWithCache) {
    using namespace json;
    Result result;
    Time start;
    for (size_t i =0; i < 1000; ++i) {
        AddData(dataSet1, result);
        CheckData(dataSet1, result);
        result.Get<people>().ClearToCache(10);

        AddData(dataSet2, result);
        CheckData(dataSet2, result);
        result.Get<people>().ClearToCache(10);

        AddData(dataSet3, result);
        CheckData(dataSet3, result);
        result.Get<people>().ClearToCache(10);
    }
    Time end;
    std::cout << "Cached loop: " << end.DiffUSecs(start) << "us" << std::endl;
}
