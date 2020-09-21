#include "ParseQueryString.h"
#include <sstream>
#include <stack>
#include "utilities.h"

using namespace std;
using namespace libCZI;

/*static*/std::once_flag CParserUtils::initRegex;
/*static*/CParserUtils::RegexInfo CParserUtils::regExInfo;

/*static*/const char* CParserUtils::VariableName_PhysicalWidth = "Width";
/*static*/const char* CParserUtils::VariableName_PhysicalHeight = "Height";
/*static*/const char* CParserUtils::VariableName_LogicalPosX = "LogPosX";
/*static*/const char* CParserUtils::VariableName_LogicalPosY = "LogPosY";
/*static*/const char* CParserUtils::VariableName_LogicalPosWidth = "LogPosWidth";
/*static*/const char* CParserUtils::VariableName_LogicalPosHeight = "LogPosHeight";
/*static*/const char* CParserUtils::VariableName_IsLayer0 = "IsLayer0";

/*static*/string CParserUtils::GetRegexExpression(const std::string& possibleDimensions)
{
    static const string regexTequals = "[[:blank:]]*(=|>|<|<=|>=|!=)[[:blank:]]*([+-]?[[:digit:]]+)";
    static const string regexTrange = "[[:blank:]]*=[[:blank:]]*\\[[[:blank:]]*([+-]?[[:digit:]]+)[[:blank:]]*,[[:blank:]]*([+-]?[[:digit:]]+)[[:blank:]]*\\]";
    static const string regexTlist = "[[:blank:]]*=[[:blank:]]*\\{([[:blank:]]*[+-]?[[:digit:]]+[[:blank:]]*(?:,[[:blank:]]*[+-]?[[:digit:]]+[[:blank:]]*)*)\\}";

    stringstream ss;
    ss << "(?:";
    for (char c : possibleDimensions)
    {
        ss << c << '|';
    }

    ss << VariableName_PhysicalWidth << '|' << VariableName_PhysicalHeight << '|';
    ss << VariableName_LogicalPosX << '|' << VariableName_LogicalPosY << '|' << VariableName_LogicalPosWidth << '|' << VariableName_LogicalPosHeight;
    ss << '|' << VariableName_IsLayer0;
    ss << ')';

    stringstream ssRegex;
    ssRegex << "^[[:blank:]]*(AND|OR|XOR|NOT|\\(|\\)|" << "((" << ss.str() << ")" << regexTequals << ")|((" << ss.str() << ")" << regexTrange << ")|((" << ss.str() << ")" << regexTlist << "))";

    return ssRegex.str();
}

/*static*/const CParserUtils::RegexInfo& CParserUtils::GetRegex()
{
    std::call_once(CParserUtils::initRegex,
        []()
        {
            CParserUtils::regExInfo.regEx = regex(CParserUtils::GetRegexExpression(CParserUtils::GetAllPossibleDimensions()));
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

/*static*/ std::string CParserUtils::GetAllPossibleDimensions()
{
    string str;
    str.reserve(10);
    for (underlying_type<DimensionIndex>::type i = (underlying_type<DimensionIndex>::type)DimensionIndex::MinDim;
        i <= (underlying_type<DimensionIndex>::type)DimensionIndex::MaxDim;
        ++i)
    {
        str += Utils::DimensionToChar((DimensionIndex)i);
    }

    return str;
}

/*static*/std::vector<TokenItem> CParserUtils::Tokenize(const std::string& str)
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
                    int start, end;
                    if (!Utilities::TryParseInt32(sm[regExInfo.indexRangeStart], &start) ||
                        !Utilities::TryParseInt32(sm[regExInfo.indexRangeEnd], &end))
                    {
                        throw LibCZIQueryParseException("Invalid Query-string", LibCZIQueryParseException::ErrorType::InvalidNumberFormat);
                    }

                    ti.token = Token::Condition;
                    ti.condition.SetRange(start, end);
                    SetVariableFromString(sm[regExInfo.indexRangeDimension], ti.condition);
                }
                else if (sm[regExInfo.indexListStatement].matched)
                {
                    ti.token = Token::Condition;
                    ti.condition.SetList(ParseListOfIntegers(sm[regExInfo.indexListNumberList]));
                    SetVariableFromString(sm[regExInfo.indexListDimension], ti.condition);
                }
                else if (sm[regExInfo.indexRelationStatement].matched)
                {
                    ti.token = Token::Condition;
                    auto relationType = StringToConditionType(sm[regExInfo.indexRelationOp]);
                    int constant = stoi(sm[regExInfo.indexRelationConstant]);
                    ti.condition.SetRelationTypeAndConstant(relationType, constant);
                    SetVariableFromString(sm[regExInfo.indexRelationDimension], ti.condition);
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
                        if (n == offset && c != '(' && c != ')' && ti.token != Token::RightParenthesis && ti.token != Token::LeftParenthesis)
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

    bool mustBeLeftParenthesisOrConditionOrNot = false;
    for (const auto& ti : src)
    {
        if (mustBeLeftParenthesisOrConditionOrNot)
        {
            if (ti.token != Token::LeftParenthesis &&
                ti.token != Token::Condition &&
                !(ti.token == Token::Operator && ti.op == Operator::NOT))
            {
                throw LibCZIQueryParseException("ill-formed expression", LibCZIQueryParseException::ErrorType::IllformedExpression);
            }

            mustBeLeftParenthesisOrConditionOrNot = false;
        }

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
            else
            {
                // "NOT" must be followed by either a left parenthesis, a condition or another "NOT"
                mustBeLeftParenthesisOrConditionOrNot = true;
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

    if (mustBeLeftParenthesisOrConditionOrNot)
    {
        throw LibCZIQueryParseException("ill-formed expression", LibCZIQueryParseException::ErrorType::IllformedExpression);
    }

    for (; !operatorStack.empty();)
    {
        if (operatorStack.top().token == Token::LeftParenthesis || operatorStack.top().token == Token::RightParenthesis)
        {
            throw LibCZIQueryParseException("Mismatched parenthesis encountered", LibCZIQueryParseException::ErrorType::UnbalancedParenthesis);
        }

        output.push_back(operatorStack.top());
        operatorStack.pop();
    }

    const bool isWellFormed = CParserUtils::CheckTokenList(output);
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

/*static*/bool CParserUtils::Evaluate(const std::vector<TokenItem>& tokens, const IEvaluationData* evaluateData)
{
    stack<bool> evalStack;

    for (const auto& t : tokens)
    {
        switch (t.token)
        {
        case Token::Condition:
        {
            bool b = EvaluateCondition(t.condition, evaluateData);
            evalStack.push(b);
            break;
        }
        case Token::Operator:
            switch (t.op)
            {
            case Operator::AND:
            {
                //assert(evalStack.size() > 1);
                bool b1 = evalStack.top();
                evalStack.pop();
                bool b2 = evalStack.top();
                evalStack.pop();
                evalStack.push(b1 & b2);
                break;
            }
            case Operator::OR:
            {
                //assert(evalStack.size() > 1);
                bool b1 = evalStack.top();
                evalStack.pop();
                bool b2 = evalStack.top();
                evalStack.pop();
                evalStack.push(b1 | b2);
                break;
            }
            case Operator::XOR:
            {
                //assert(evalStack.size() > 1);
                bool b1 = evalStack.top();
                evalStack.pop();
                bool b2 = evalStack.top();
                evalStack.pop();
                evalStack.push(b1 ^ b2);
                break;
            }
            case Operator::NOT:
            {
                bool b = evalStack.top();
                evalStack.pop();
                evalStack.push(!b);
                break;
            }
            }

            break;
        }
    }

    return evalStack.top();
}

/*static*/void CParserUtils::SetVariableFromString(const std::string& str, CCondition& condition)
{
    static const struct
    {
        const char* variableName;
        VariableType type;
    } table[] =
    {
        {VariableName_PhysicalWidth, VariableType::PhysicalWidth},
        {VariableName_PhysicalHeight, VariableType::PhysicalHeight},
        {VariableName_LogicalPosX, VariableType::LogicalPositionX},
        {VariableName_LogicalPosY, VariableType::LogicalPositionY},
        {VariableName_LogicalPosWidth, VariableType::LogicalPositionWidth},
        {VariableName_LogicalPosHeight, VariableType::LogicalPositionHeight},
        {VariableName_IsLayer0, VariableType::IsLayer0}
    };

    for (auto i : table)
    {
        if (strcmp(i.variableName, str.c_str()) == 0)
        {
            condition.SetVariableType(i.type);
            return;
        }
    }

    auto d = str[0];
    auto dim = Utils::CharToDimension(d);
    condition.SetDimension(dim);
}

/*static*/std::vector<int> CParserUtils::ParseListOfIntegers(const std::string& str)
{
    vector<int> list;
    string number;
    int value;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (isblank(str[i]))
        {
            continue;
        }

        if (str[i] == ',')
        {
            if (!Utilities::TryParseInt32(number, &value))
            {
                throw LibCZIQueryParseException("Invalid number", LibCZIQueryParseException::ErrorType::InvalidNumberFormat);
            }

            list.emplace_back(value);
            number.clear();
        }
        else
        {
            number += str[i];
        }
    }

    if (!Utilities::TryParseInt32(number, &value))
    {
        throw LibCZIQueryParseException("Invalid number", LibCZIQueryParseException::ErrorType::InvalidNumberFormat);
    }

    list.emplace_back(value);
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

/*static*/bool CParserUtils::EvaluateCondition(const CCondition& cond, const IEvaluationData* evaluateData)
{
    auto type = cond.GetVariableType();
    int val = -1;
    switch (type)
    {
    case VariableType::PhysicalWidth:
    case VariableType::PhysicalHeight:
    case VariableType::LogicalPositionX:
    case VariableType::LogicalPositionY:
    case VariableType::LogicalPositionWidth:
    case VariableType::LogicalPositionHeight:
    case VariableType::IsLayer0:
        val = evaluateData->GetVariable(type);
        break;
    case VariableType::Dimension:
        val = evaluateData->GetCoordinateValue(cond.GetDimension());
        break;
    default:
        throw LibCZIException("unexpected value for condition-type");
    }

    bool b = cond.Evaluate(val);
    return b;
}

//-----------------------------------------------------------------------------

bool CCondition::Evaluate(int value) const
{
    switch (this->condition)
    {
    case ConditionType::Equal:
        return this->conditionConst == value;
    case ConditionType::Unequal:
        return this->conditionConst != value;
    case ConditionType::LessThan:
        return value < this->conditionConst;
    case ConditionType::GreaterThan:
        return value > this->conditionConst;
    case ConditionType::LessThanOrEqual:
        return value <= this->conditionConst;
    case ConditionType::GreaterThanOrEqual:
        return value >= this->conditionConst;
    case ConditionType::InRange:
        return value >= this->rangeStart && value <= this->rangeEnd;
    case ConditionType::InList:
        return find(this->list.cbegin(), this->list.cend(), value) != this->list.cend();
    }

    throw runtime_error("invalid condition");
}