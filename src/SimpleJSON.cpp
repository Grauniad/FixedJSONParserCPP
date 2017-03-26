
#include "SimpleJSON.h"
#include "logger.h"

#include <sstream>

using namespace std;

/*****************************************************************************
 *                          Writers
 *****************************************************************************/
SimpleJSONBuilderCompactWriter::SimpleJSONBuilderCompactWriter()
    : rapidjson::Writer<rapidjson::StringBuffer>(
          static_cast<rapidjson::StringBuffer&>(*this))
{
}

void SimpleJSONBuilderCompactWriter::ResetAndClear() {
    rapidjson::StringBuffer::Clear();
    rapidjson::Writer<rapidjson::StringBuffer>::Reset(
        static_cast<rapidjson::StringBuffer&>(*this));
}

SimpleJSONBuilderPrettyWriter::SimpleJSONBuilderPrettyWriter()
    : rapidjson::PrettyWriter<rapidjson::StringBuffer>(
          static_cast<rapidjson::StringBuffer&>(*this))
{
}

void SimpleJSONBuilderPrettyWriter::ResetAndClear() {
    rapidjson::StringBuffer::Clear();
    rapidjson::PrettyWriter<rapidjson::StringBuffer>::Reset(
        static_cast<rapidjson::StringBuffer&>(*this));
}

/*****************************************************************************
 *                          Base Scalar Field
 *****************************************************************************/
FieldBase::FieldBase() 
    : supplied(false)
{
}

void FieldBase::Clear() {
    supplied = false;
}

bool FieldBase::StartObject() {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::EndObject(rapidjson::SizeType memberCount) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::String(const char* str, rapidjson::SizeType length, bool copy) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::Key(const char* str, rapidjson::SizeType length, bool copy) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::Int(int i) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::Int64(int64_t i) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::Uint(unsigned u) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::Uint64(uint64_t u) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::Double(double d) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::Bool(bool b) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::StartArray() {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::EndArray(rapidjson::SizeType elementCount) {
   throw spJSON::WrongTypeError{Name()};
}

bool FieldBase::Null() {
   supplied = false;
   return true;
}

/*****************************************************************************
 *                          SimpleParsedJSON - Auto-generate
 *****************************************************************************/

class SimpleParsedJSON_Generator {
public:

    SimpleParsedJSON_Generator(
            const std::string _namespaceName = "",
            const std::string _indent = "    ",
            spJSON::GeneratorOptions opts = spJSON::GeneratorOptions())
            : started(false),
              isArray(false),
              arrayTyped(false),
              childObject(nullptr),
              indent(_indent),
              namespaceName(_namespaceName),
              options(opts) {
        int nsIndentSize = indent.length() - 4;
        if (nsIndentSize < 0) {
            nsIndentSize = 0;
        }
        nsIndent = indent.substr(0, nsIndentSize);
    }

    SimpleParsedJSON_Generator &ActiveObject() {
        SimpleParsedJSON_Generator *obj = childObject.get();

        if (obj) {
            return obj->ActiveObject();
        } else {
            return *this;
        }
    }

    bool IgnoreField() {
        return (isArray && arrayTyped);
    }

    bool Key(const char *str, rapidjson::SizeType length, bool copy) {
        auto &active = ActiveObject();
        if (!active.IgnoreField()) {
            string field = str;
            active.keys[field] = "";
            active.current = active.keys.find(field);
            active.arrayTyped = false;
        }

        return true;
    }

    bool String(const char *str, rapidjson::SizeType length, bool copy) {
        auto &active = ActiveObject();
        if (active.isArray) {
            if (!active.arrayTyped) {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::String",
                        "Array " << active.namespaceName << "::" << active.current->first << " is a String.");

                active.current->second = stringArrayType.GetDefinition(
                        active.indent,
                        active.current->first);
                active.arrayTyped = true;
            }
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::String",
                    "Field " << active.namespaceName << "::" << active.current->first << " is a String.");
            active.current->second = stringType.GetDefinition(
                    active.indent,
                    active.current->first);
        }
        return true;
    }

    bool Double(double d) {
        auto &active = ActiveObject();
        if (active.isArray) {
            if (!active.arrayTyped) {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::Double",
                        "Array " << active.namespaceName << "::" << active.current->first << " is a Double.");

                active.current->second = doubleArrayType.GetDefinition(
                        active.indent,
                        active.current->first);

                active.arrayTyped = true;
            }
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::Double",
                    "Field " << active.namespaceName << "::" << active.current->first << " is a Double.");
            active.current->second = doubleType.GetDefinition(
                    active.indent,
                    active.current->first);
        }
        return true;
    }

    bool Int(int i) {
        auto &active = ActiveObject();
        if (active.isArray) {
            if (!active.arrayTyped) {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::Int",
                        "Array " << active.namespaceName << "::" << active.current->first << " is a Int.");

                active.current->second = intArrayType.GetDefinition(
                        active.indent,
                        active.current->first);
                active.arrayTyped = true;
            }
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::Int",
                    "Field " << active.namespaceName << "::" << active.current->first << " is a Int.");
            active.current->second = intType.GetDefinition(
                    active.indent,
                    active.current->first);
        }
        return true;
    }

    bool Int64(int64_t i) {
        auto &active = ActiveObject();
        if (active.isArray) {
            if (!active.arrayTyped) {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::Int64",
                        "Array " << active.namespaceName << "::" << active.current->first << " is a Int64.");

                active.current->second = int64ArrayType.GetDefinition(
                        active.indent,
                        active.current->first);
                active.arrayTyped = true;
            }
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::Int64",
                    "Field " << active.namespaceName << "::" << active.current->first << " is a Int64.");
            active.current->second = int64Type.GetDefinition(
                    active.indent,
                    active.current->first);
        }
        return true;
    }

    bool Uint(unsigned u) {
        auto &active = ActiveObject();
        if (active.isArray) {
            if (!active.arrayTyped) {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::Uint",
                        "Array " << active.namespaceName << "::" << active.current->first << " is a Uint.");

                active.current->second = uintArrayType.GetDefinition(
                        active.indent,
                        active.current->first);

                active.arrayTyped = true;
            }
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::Uint",
                    "Field " << active.namespaceName << "::" << active.current->first << " is a Uint.");
            active.current->second = uintType.GetDefinition(
                    active.indent,
                    active.current->first);
        }
        return true;
    }

    bool Uint64(uint64_t u) {
        auto &active = ActiveObject();
        if (active.isArray) {
            if (!active.arrayTyped) {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::Uint64",
                        "Array " << active.namespaceName << "::" << active.current->first << " is a Uint64.");

                active.current->second = uint64ArrayType.GetDefinition(
                        active.indent,
                        active.current->first);
                active.arrayTyped = true;
            }
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::Uint64",
                    "Field " << active.namespaceName << "::" << active.current->first << " is a Uint64.");
            active.current->second = uint64Type.GetDefinition(
                    active.indent,
                    active.current->first);
        }
        return true;
    }

    bool Bool(bool b) {
        auto &active = ActiveObject();
        if (active.isArray) {
            if (!active.arrayTyped) {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::Bool",
                        "Array " << active.namespaceName << "::" << active.current->first << " is a bool.");

                active.current->second = boolArrayType.GetDefinition(
                        active.indent,
                        active.current->first);
                active.arrayTyped = true;

            }
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::Bool",
                    "Field " << active.namespaceName << "::" << active.current->first << " is a bool.");

            active.current->second = boolType.GetDefinition(
                    active.indent,
                    active.current->first);
        }
        return true;
    }

    virtual bool StartArray() {
        auto &active = ActiveObject();

        if (!active.IgnoreField()) {
            active.isArray = true;
            active.arrayTyped = false;

            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::StartArray",
                    "Starting new array " << active.namespaceName << "::" << active.current->first);
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::StartArray",
                    "Ignoring new array " << active.namespaceName << "::" << active.current->first);
        }


        return true;
    }

    virtual bool EndArray(rapidjson::SizeType elementCount) {
        auto &active = ActiveObject();

        if (active.IgnoreField()) {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::EndArray",
                    "Ignoring end of duplicate array " << active.namespaceName << "::" << active.current->first);
        } else if (!active.arrayTyped) {
            SLOG_FROM(
                    LOG_VERBOSE,
                    "SimpleParsedJSON_Generator::EndArray",
                    "Array " << active.namespaceName << "::" << active.current->first <<
                             " finished, was NOT typed successfuly");
            active.keys.erase(active.current);
        } else {
            SLOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::EndArray",
                    "Array " << active.namespaceName << "::" << active.current->first <<
                             " finished, was typed successfuly");
        }
        active.isArray = false;

        return true;
    }

    bool StartObject() {
        if (started) {
            if (!IgnoreField()) {
                if (childObject.get()) {
                    LOG_FROM(
                            LOG_VERY_VERBOSE,
                            "SimpleParsedJSON_Generator::StartObject",
                            "forwarding to existing child object...");
                    childObject->StartObject();
                } else {
                    string childNamespace = current->first + "_fields";
                    SLOG_FROM(
                            LOG_VERY_VERBOSE,
                            "SimpleParsedJSON_Generator::StartObject",
                            "Starting a new child object: " << childNamespace);

                    childObject.reset(new SimpleParsedJSON_Generator(childNamespace, indent + "    "));
                    childObject->StartObject();
                }
            } else {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::StartObject",
                        "Skipping non first item for array "
                                << namespaceName << "::" << current->first)
            }
        } else {
            LOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::StartObject",
                    "Starting initial object");
            started = true;
        }
        return true;
    }

    bool EndObject(rapidjson::SizeType memberCount) {
        if (childObject.get()) {
            if (!IgnoreField()) {
                if (childObject->childObject.get()) {
                    LOG_FROM(
                            LOG_VERY_VERBOSE,
                            "SimpleParsedJSON_Generator::EndObject",
                            "Forwarding to child object...");
                    childObject->EndObject(memberCount);
                } else if (!childObject->IgnoreField()) {
                    const string &objName = current->first;
                    string jsonName = current->first + "_fields::JSON";

                    SLOG_FROM(
                            LOG_VERY_VERBOSE,
                            "SimpleParsedJSON_Generator::EndObject",
                            "Completed object " << jsonName);

                    stringstream buf;
                    buf << endl;
                    buf << childObject->GetCode("JSON");

                    if (isArray) {
                        buf << indent << "NewObjectArray(";
                    } else {
                        buf << indent << "NewEmbededObject(";
                    }
                    buf << objName << ", " << jsonName << ");";

                    current->second = buf.str();
                    childObject.reset(nullptr);

                    if (isArray) {
                        SLOG_FROM(
                                LOG_VERY_VERBOSE,
                                "SimpleParsedJSON_Generator::EndObject",
                                "Terminated array "
                                        << namespaceName << "::" << current->first
                                        << " having completed the first item");
                        arrayTyped = true;
                    }
                } else {
                    SLOG_FROM(
                            LOG_VERY_VERBOSE,
                            "SimpleParsedJSON_Generator::EndObject",
                            "Ignored end of child object, whilst processing" << current->first);
                }
            } else {
                SLOG_FROM(
                        LOG_VERY_VERBOSE,
                        "SimpleParsedJSON_Generator::EndObject",
                        "Skipping non first item for array "
                                << namespaceName << "::" << current->first)
            }
        } else if (started) {
            LOG_FROM(
                    LOG_VERY_VERBOSE,
                    "SimpleParsedJSON_Generator::EndObject",
                    "Terminated the object itself");
        } else {
            SLOG_FROM(
                    LOG_VERBOSE,
                    "SimpleParsedJSON_Generator::EndObject",
                    "End of non-existent object!");
        }
        return true;
    }

    bool Null() {
        auto &active = ActiveObject();
        if (options.ignoreNull == false) {
            throw "TODO!";
        } else if (active.isArray) {
            // Ignore the null - there might be another type to come...
        } else {
            active.keys.erase(active.current);
        }

        return true;
    }

    bool RawNumber(const char *str, size_t len, bool copy) {
        return this->String(str, len, copy);
    }

    string GetCode(const std::string &jsonName) {
        stringstream result;
        stringstream fields;
        if (namespaceName != "") {
            result << nsIndent << "namespace " << namespaceName << " {" << endl;
        }

        auto it = keys.begin();
        while (it != keys.end()) {
            result << it->second << endl;
            fields << indent << "    " << it->first;
            ++it;
            if (it == keys.end()) {
                fields << endl;
            } else {
                fields << "," << endl;
            }
        }

        result << endl;
        result << indent << "typedef SimpleParsedJSON<" << endl;
        result << fields.str();
        result << indent << "> " << jsonName << ";" << endl;
        if (namespaceName != "") {
            result << nsIndent << "}" << endl;
        }
        return result.str();
    }

private:
    bool started; // Indicates if we have found the start of the outer object yet
    bool isArray; // Indicates if the field currently being scanned is an array
    bool arrayTyped; // Indicates if we've already found the first item in the array

    unique_ptr<SimpleParsedJSON_Generator> childObject;

    class IFieldType {
    public:
        virtual ~IFieldType() {}

        virtual string GetDefinition(const std::string& indent, const std::string &name) = 0;
    };

    class Int64Field: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewI64Field(" + name + ");";
            return def;
        }
    } int64Type;

    class Int64ArrayField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewI64ArrayField(" + name + ");";
            return def;
        }
    } int64ArrayType;

    class UInt64Field: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewUI64Field(" + name + ");";
            return def;
        }
    } uint64Type;

    class UInt64ArrayField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewUI64ArrayField(" + name + ");";
            return def;
        }
    } uint64ArrayType;

    class IntField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewIntField(" + name + ");";
            return def;
        }
    } intType;

    class IntArrayField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewIntArrayField(" + name + ");";
            return def;
        }
    } intArrayType;

    class BoolField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewBoolField(" + name + ");";
            return def;
        }
    } boolType;

    class BoolArrayField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewBoolArrayField(" + name + ");";
            return def;
        }
    } boolArrayType;

    class UIntField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewUIntField(" + name + ");";
            return def;
        }
    } uintType;

    class UIntArrayField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewUIntArrayField(" + name + ");";
            return def;
        }
    } uintArrayType;

    class DoubleField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewDoubleField(" + name + ");";
            return def;
        }
    } doubleType;

    class DoubleArrayField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewDoubleArrayField(" + name + ");";
            return def;
        }
    } doubleArrayType;

    class StringField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewStringField(" + name + ");";
            return def;
        }
    } stringType;

    class StringArrayField: public IFieldType {
    public:
        virtual string GetDefinition(const std::string& indent, const std::string &name) {
            std::string def =  indent + "NewStringArrayField(" + name + ");";
            return def;
        }
    } stringArrayType;

    typedef std::map<std::string,std::string> Keys;
    Keys keys;
    Keys::iterator current;
    std::string indent;
    std::string nsIndent;
    std::string namespaceName;
    spJSON::GeneratorOptions options;
};


std::string spJSON::Gen(
        const string& className,
        const string& exampleJson,
        spJSON::GeneratorOptions options )
{
    SimpleParsedJSON_Generator gen("", "    ", options);

    rapidjson::StringStream ss(exampleJson.c_str());
    rapidjson::Reader reader;

    reader.Parse(ss,gen);

    return gen.GetCode(className);
}
