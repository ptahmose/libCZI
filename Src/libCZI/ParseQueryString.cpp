#include "ParseQueryString.h"
#include <sstream>
#include <stack>

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
                for (size_t n = offset;; ++n)
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

/*static*/std::vector<TokenItem> CParserUtils::ConvertToReversePolnish(const std::vector<TokenItem>& src)
{
    // see https://en.wikipedia.org/wiki/Shunting-yard_algorithm
    vector<TokenItem> output;
    stack<TokenItem> operatorStack;

    for (const auto& ti : src)
    {
        switch (ti.token)
        {
        case Token::Condition:
            output.push_back(ti);
            break;
        case Token::Operator:
            if (ti.op != Operator::NOT)
            {
                while (!operatorStack.empty() &&
                    operatorStack.top().op != Operator::NOT &&
                    CParserUtils::IsOperatorPrecedenceHigher(operatorStack.top().op, ti.op) &&
                    operatorStack.top().token != Token::LeftParenthesis)
                {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();
                }
            }

            operatorStack.push(ti);
            break;
        case Token::LeftParenthesis:
            operatorStack.push(ti);
            break;
        case Token::RightParenthesis:
        {
            bool foundLeftParenthesis = false;
            for (; !operatorStack.empty();)
            {
                if (operatorStack.top().token == Token::LeftParenthesis)
                {
                    operatorStack.pop();
                    foundLeftParenthesis = true;
                    break;
                }

                output.push_back(operatorStack.top());
                operatorStack.pop();
            }

            if (!foundLeftParenthesis)
            {
                throw LibCZIQueryParseException("Mismatched parenthesis encountered", LibCZIQueryParseException::ErrorType::UnbalancedParenthesis);
            }

            break;
        }
        }
    }

    for (; !operatorStack.empty();)
    {
        if (operatorStack.top().token == Token::LeftParenthesis || operatorStack.top().token == Token::RightParenthesis)
        {
            throw LibCZIQueryParseException("Mismatched parenthesis encountered", LibCZIQueryParseException::ErrorType::UnbalancedParenthesis);
        }

        if (operatorStack.top().token == Token::Operator && operatorStack.top().op == Operator::NOT)
        {
            throw LibCZIQueryParseException("Illformed statement", LibCZIQueryParseException::ErrorType::IllformedExpression);
        }

        output.push_back(operatorStack.top());
        operatorStack.pop();
    }

    bool isWellFormed = CParserUtils::CheckTokenList(output);
    if (!isWellFormed)
    {
        throw LibCZIQueryParseException("ill-formed expression", LibCZIQueryParseException::ErrorType::IllformedExpression);
    }

    return output;
}

/*static*/bool CParserUtils::CheckTokenList(const vector<TokenItem>& tokens)
{
    uint32_t stackSize = 0;
    for (const auto& ti : tokens)
    {
        switch (ti.token)
        {
        case Token::Condition:
            ++stackSize;
            break;
        case Token::Operator:
            switch (ti.op)
            {
            case Operator::AND:
                if (stackSize < 2) return false;
                // we pop 2 elements from the stack, and put one onto it -> so, in total, we remove one
                --stackSize;
                break;
            case Operator::OR:
                if (stackSize < 2) return false;
                // we pop 2 elements from the stack, and put one onto it -> so, in total, we remove one
                --stackSize;
                break;
            case Operator::XOR:
                if (stackSize < 2) return false;
                // we pop 2 elements from the stack, and put one onto it -> so, in total, we remove one
                --stackSize;
                break;
            case Operator::NOT:
                if (stackSize < 1) return false;
                // stack size remains unchanged
                break;
            default:
                return false;
            }

            break;
        default:
            return false;
        }
    }

    return stackSize == 1;
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

/*static*/bool CParserUtils::IsOperatorPrecedenceHigher(Operator a, Operator b)
{
    if (a == b)
    {
        return false;
    }

    if (a == Operator::AND)
    {
        return true;
    }

    return false;
}