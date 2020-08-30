#include <SimpleJSONFmt.h>

#include <rapidjson/reader.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace {
    class Parser {
    public:
        Parser() : writer(buf) {}

        bool Key(const char* str, rapidjson::SizeType length, bool copy) {
            return writer.Key(str, length, copy);
        }

        bool String(const char* str, rapidjson::SizeType length, bool copy) {
            return writer.String(str, length, copy);
        }

        bool Int(int i) {
            return writer.Int(i);
        }

        bool Uint(unsigned u) {
            return writer.Uint(u);
        }

        bool Double(double d) {
            return writer.Double(d);
        }

        bool StartObject() {
            return writer.StartObject();
        }

        bool EndObject(rapidjson::SizeType memberCount) {
            count = memberCount;
            return writer.EndObject(memberCount);
        }

        bool Null() {
            return writer.Null();
        }

        bool Bool(bool b) {
            return writer.Bool(b);
        }

        bool Int64(int64_t i) {
            return writer.Int64(i);
        }

        bool Uint64(uint64_t u) {
            return writer.Uint64(u);
        }

        bool StartArray() {
            return writer.StartArray();
        }

        bool EndArray(rapidjson::SizeType elementCount) {
            return writer.EndArray(elementCount);
        }

        bool RawNumber(const char* str, size_t len, bool copy) {
            return writer.RawNumber(str, len, copy);
        }

        size_t count = 0;
        rapidjson::StringBuffer buf;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer;
    };

    class IStreamWrap {
    public:
        std::istream& is;
        std::istream::pos_type startPos;
        IStreamWrap(std::istream& is): is(is) {
            startPos = is.tellg();
        }

        using Ch = char;

        //! Read the current character from stream without moving the read cursor.
        Ch Peek() const {
            return is.peek();
        }

        //! Read the current character from stream and moving the read cursor to next character.
        Ch Take() {
            return is.get();
        }

        //! Get the current read cursor.
        //! \return Number of characters read from start.
        size_t Tell() {
            return is.tellg();
        }

        void Revert() {
            is.seekg(startPos);
        }

        struct WriteToReadOnlyStream {};

        Ch* PutBegin() { throw WriteToReadOnlyStream{}; }
        void Put(Ch c) { throw WriteToReadOnlyStream{}; }
        void Flush() { throw WriteToReadOnlyStream{}; }
        size_t PutEnd(Ch* begin) {throw WriteToReadOnlyStream{}; }
    };

    std::string ErrorCode(const rapidjson::ParseErrorCode& code) {
        std::string error = "UNKNOWN";
        switch(code) {
            case rapidjson::kParseErrorNone:
                error = "No Error";
                break;

            case rapidjson::kParseErrorDocumentEmpty:
                error = "Empty Document";
                break;

            case rapidjson::kParseErrorDocumentRootNotSingular:
                error = "Text after end of JSON doc";
                break;

            case rapidjson::kParseErrorValueInvalid:
                error = "Invalid value";
                break;
            case rapidjson::kParseErrorObjectMissName:
                error = "Object has no name!";
                break;
        }

        return error;

    }

    bool TryMakeJSON(IStreamWrap in, std::ostream& out) {
        bool success = false;

        constexpr unsigned  flags =
                rapidjson::kParseTrailingCommasFlag |
                rapidjson::kParseEscapedApostropheFlag |
                rapidjson::kParseStopWhenDoneFlag |
                rapidjson::kParseNanAndInfFlag;
        Parser p;
        rapidjson::Reader reader;
        rapidjson::ParseResult result = reader.Parse<flags>(in,p);
        if (result.IsError() || p.count == 0) {
            in.Revert();
        } else {
            out << p.buf.GetString() << std::endl;
            success = true;
        }

        return success;
    }

    bool ReadSome(std::istream& in, std::string& buf) {
        buf = "";
        bool gotSome = false;

        while (std::isspace(in.peek())) {
            buf += (char) in.get();
            gotSome = true;
        }

        std::string tok;
        if (in >> tok)  {
            gotSome = true;
            buf += tok;
        }

        return gotSome;

    }
}

void FmtJSON::Fmt(std::istream& in, std::ostream& out) {
    std::string buf;

    in >> std::noskipws;

    for (size_t spos = in.tellg(); ReadSome(in, buf); spos = in.tellg()) {
        const size_t pos = buf.find('{');
        if ( pos != std::string::npos) {
            out << buf.substr(0, pos);

            in.seekg(spos + pos);
            if (!TryMakeJSON(in, out)) {
                in.seekg(1, std::ios_base::cur);
                out << "{";
            }
        } else {
            out << buf;
        }
    }
}

