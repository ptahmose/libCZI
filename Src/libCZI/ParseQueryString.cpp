#include "ParseQueryString.h"
#include <sstream>

using namespace std;
using namespace libCZI;

/*static*/std::once_flag CParserUtils::initRegex;
/*static*/CParserUtils::RegexInfo CParserUtils::regExInfo;

/*static*/string CParserUtils::GetRegexExpression(const std::string& possibleDimensions)
{
	static const string regexTequals = "[[:blank:]]*(=|>|<|<=|>=|!=)[[:blank:]]*([+-]?[[:digit:]]+)";
	static const string regexTrange = "[[:blank:]]*=[[:blank:]]*\\[[[:blank:]]*([+-]?[[:digit:]]+)[[:blank:]]*,[[:blank:]]*([+-]?[[:digit:]]+)[[:blank:]]*\\]";
	static const string regexTlist = "[[:blank:]]*=[[:blank:]]*\\{([[:blank:]]*[+-]?[[:digit:]]+[[:blank:]]*(?:,[[:blank:]]*[+-]?[[:digit:]]+[[:blank:]]*)*)\\}";

	stringstream ss;
	ss << "^[[:blank:]]*(AND|OR|XOR|NOT|\\(|\\)|" << "(([" << possibleDimensions << "])" << regexTequals << ")|(([" << possibleDimensions << "])" << regexTrange << ")|(([" << possibleDimensions << "])" << regexTlist << "))";

	return ss.str();
}

/*static*/const CParserUtils::RegexInfo& CParserUtils::GetRegex()
{
	std::call_once(CParserUtils::initRegex,
		[]()
		{
			CParserUtils::regExInfo.regEx = regex(CParserUtils::GetRegexExpression("TZCR"));
			CParserUtils::regExInfo.indexRelationStatement = 2;
			CParserUtils::regExInfo.indexRelationDimension = 3;
			CParserUtils::regExInfo.indexRelationOp = 4;
			CParserUtils::regExInfo.indexRelationConstant = 5;

			CParserUtils::regExInfo.indexRangeStatement = 6;
			CParserUtils::regExInfo.indexRangeDimension = 7;
			CParserUtils::regExInfo.indexRangeStart = 8;
			CParserUtils::regExInfo.indexRangeEnd = 9;

			CParserUtils::regExInfo.indexListStatement = 10;
			CParserUtils::regExInfo.indexListDimension = 11;
			CParserUtils::regExInfo.indexListNumberList = 12;
		});

	return CParserUtils::regExInfo;
}

/*static*/std::vector<libCZI::TokenItem> CParserUtils::Tokenize(const std::string& str)
{
    std::vector<TokenItem> tokens;
    const auto& regExInfo = CParserUtils::GetRegex();
    for (size_t offset = 0;;)
    {
        smatch sm;    // same as std::match_results<string::const_iterator> sm;
        regex_search(str.begin() + offset, str.end(), sm, regExInfo.regEx);

        if (sm.size() >= 1)
        {
            const auto& g = sm[1];
            if (g.matched)
            {
                const string matchedstr = g;
                TokenItem ti;
                if (matchedstr == "AND")
                {
                    ti.token = Token::Operator;
                    ti.op = Operator::AND;
                }
                else if (matchedstr == "OR")
                {
                    ti.token = Token::Operator;
                    ti.op = Operator::OR;
                }
                else if (matchedstr == "XOR")
                {
                    ti.token = Token::Operator;
                    ti.op = Operator::XOR;
                }
                else if (matchedstr == "NOT")
                {
                    ti.token = Token::Operator;
                    ti.op = Operator::NOT;
                }
                else if (matchedstr == "(")
                {
                    ti.token = Token::LeftParenthesis;
                }
                else if (matchedstr == ")")
                {
                    ti.token = Token::RightParenthesis;
                }
                else if (sm[regExInfo.indexRangeStatement].matched)
                {
                    int start = stoi(sm[regExInfo.indexRangeStart]);
                    int end = stoi(sm[regExInfo.indexRangeEnd]);
                    ti.token = Token::Condition;
                    ti.condition.SetRange(start, end);
                    SetDimensionFromString(sm[regExInfo.indexRangeDimension], ti.condition);
                }
                else if (sm[regExInfo.indexListStatement].matched)
                {
                    ti.token = Token::Condition;
                    ti.condition.SetList(ParseListOfIntegers(sm[regExInfo.indexListNumberList]));
                    SetDimensionFromString(sm[regExInfo.indexListDimension], ti.condition);
                }
                else if (sm[regExInfo.indexRelationStatement].matched)
                {
                    ti.token = Token::Condition;
                    auto relationType = StringToConditionType(sm[regExInfo.indexRelationOp]);
                    int constant = stoi(sm[regExInfo.indexRelationConstant]);
                    ti.condition.SetRelationTypeAndConstant(relationType, constant);
                    SetDimensionFromString(sm[regExInfo.indexRelationDimension], ti.condition);
                }

                if (ti.token == Token::Invalid)
                {
                    throw LibCZIQueryParseException("Invalid Query-string", LibCZIQueryParseException::ErrorType::SyntaxError, offset);
                }

                tokens.emplace_back(ti);
                offset += sm[0].length();

                // Now skip all whitespaces, and if we reach the end of the string, we're done.
                // Note that we require to have a least one whitespace (before the next token)
                for (size_t n=offset;; ++n)
                {
                    char c = str[n];
                    if (c == '\0')
                    {
                        return tokens;
                    }

                    if (!isspace(c))
                    {
                        if (n == offset)
                        {
                            throw LibCZIQueryParseException("Invalid Query-string", LibCZIQueryParseException::ErrorType::SyntaxError, offset);
                        }

                        offset = n;
                        break;
                    }
                }
            }
        }
        else
        {
            throw LibCZIQueryParseException("Invalid Query-string", LibCZIQueryParseException::ErrorType::SyntaxError, offset);
        }
    }
}

/*static*/void CParserUtils::SetDimensionFromString(const std::string& str, CCondition& condition)
{
    auto d = str[0];
    condition.SetDimension(d);
}

/*static*/std::vector<int> CParserUtils::ParseListOfIntegers(const std::string& str)
{
    vector<int> list;
    string number;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (isblank(str[i]))
        {
            continue;
        }

        if (str[i] == ',')
        {
            int i = stoi(number);
            list.emplace_back(i);
            number.clear();
        }
        else
        {
            number += str[i];
        }
    }

    int i = stoi(number);
    list.emplace_back(i);
    return list;
}

/*static*/ConditionType CParserUtils::StringToConditionType(const std::string& str)
{
    static const struct
    {
        const char* conditionString;
        ConditionType type;
    } list[] =
    {
        {">", ConditionType::GreaterThan },
        {"<", ConditionType::LessThan },
        {"=", ConditionType::Equal },
        {"<=", ConditionType::LessThanOrEqual},
        {">=", ConditionType::GreaterThanOrEqual},
        {"!=", ConditionType::Unequal}
    };

    for (size_t i = 0; i < sizeof(list) / sizeof(list[0]); ++i)
    {
        if (str.compare(list[i].conditionString) == 0)
        {
            return list[i].type;
        }
    }

    return ConditionType::Invalid;
}