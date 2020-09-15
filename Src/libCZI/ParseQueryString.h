#pragma once

#include <regex>
#include <mutex>  
#include "libCZI.h"

enum class Operator : std::uint8_t
{
    AND = 1,
    OR = 2,
    XOR = 3,
    NOT = 4,

    LAST = 4
};

enum class Token
{
    Invalid,
    Condition,
    Operator,
    LeftParenthesis,    // it's an "("
    RightParenthesis,    // it's an ")"
};

enum class ConditionType
{
    Invalid,
    Equal,
    Unequal,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,
    InRange,
    InList
};

enum class VariableType
{
    None,
    Dimension,
    PhysicalWidth,
    PhysicalHeight,
    LogicalPositionX,
    LogicalPositionY,
    LogicalPositionWidth,
    LogicalPositionHeight
};

class CCondition
{
private:
    VariableType varType;
    libCZI::DimensionIndex dimension;
    ConditionType condition;
    int conditionConst;
    int rangeStart, rangeEnd;
    std::vector<int> list;
public:
    CCondition() : condition(ConditionType::Invalid), varType(VariableType::None) {};

    libCZI::DimensionIndex GetDimension() const { return this->dimension; }

    bool Evaluate(int value) const;

    void SetDimension(libCZI::DimensionIndex d)
    {
        this->varType = VariableType::Dimension;
        this->dimension = d;
    }

    void SetVariableType(VariableType type)
    {
        this->varType = type;
    }

    VariableType GetVariableType() const
    {
        return this->varType;
    }

    void SetRange(int rangeStart, int rangeEnd)
    {
        this->condition = ConditionType::InRange;
        this->rangeStart = rangeStart;
        this->rangeEnd = rangeEnd;
    }

    void SetList(const std::vector<int>& list)
    {
        this->condition = ConditionType::InList;
        this->list = list;
    }

    void SetRelationTypeAndConstant(ConditionType type, int constant)
    {
        this->condition = type;
        this->conditionConst = constant;
    }

    void SetEqualTo(int equalConst)
    {
        this->condition = ConditionType::Equal;
        this->conditionConst = equalConst;
    }

    void SetUnequalTo(int unequalConst)
    {
        this->condition = ConditionType::Unequal;
        this->conditionConst = unequalConst;
    }

    void SetLessThan(int lessThanConst)
    {
        this->condition = ConditionType::LessThan;
        this->conditionConst = lessThanConst;
    }

    void SetGreaterThan(int greaterThanConst)
    {
        this->condition = ConditionType::GreaterThan;
        this->conditionConst = greaterThanConst;
    }

    void SetLessThanOrEqual(int lessThanOrEqualConst)
    {
        this->condition = ConditionType::LessThanOrEqual;
        this->conditionConst = lessThanOrEqualConst;
    }

    void SetGreaterThanOrEqual(int greaterThanOrEqualConst)
    {
        this->condition = ConditionType::GreaterThanOrEqual;
        this->conditionConst = greaterThanOrEqualConst;
    }
};

class TokenItem
{
public:
    TokenItem() : token(Token::Invalid) {};
    Token token;
    CCondition condition;
    Operator op;
};

class IEvaluationData
{
public:
    virtual int GetCoordinateValue(libCZI::DimensionIndex dim) const = 0;
    virtual int GetVariable(VariableType type) const = 0;
};

class CParserUtils
{
public:
    static std::vector<TokenItem> Tokenize(const std::string& str);
    static std::vector<TokenItem> ConvertToReversePolnish(const std::vector<TokenItem>& src);
    static bool CheckTokenList(const std::vector<TokenItem>& tokens);
    static bool Evaluate(const std::vector<TokenItem>& tokens, const IEvaluationData* evaluateData);
private:
    static const char* VariableName_PhysicalWidth;
    static const char* VariableName_PhysicalHeight;
    static const char* VariableName_LogicalPosX;
    static const char* VariableName_LogicalPosY;
    static const char* VariableName_LogicalPosWidth;
    static const char* VariableName_LogicalPosHeight;

    static std::once_flag initRegex;

    struct RegexInfo
    {
        std::regex regEx;

        /// The capture-group index of "relation statement" (e.g. T >= 4).
        int     indexRelationStatement;
        int     indexRelationDimension;
        /// The capture-group index for "relation statement", this is the relational symbol (e.g. =, >, >=, ...)
        int     indexRelationOp;
        /// The capture-group index for "relation statement", this is the number.
        int     indexRelationConstant;

        /// The capture-group index of "range statement" (e.g. T=[2, 4]).
        int indexRangeStatement;
        int indexRangeDimension;
        /// The capture-group index for "range statement", this is the start number.
        int indexRangeStart;
        /// The capture-group index for "range statement", this is the end number.
        int indexRangeEnd;

        int indexListStatement;
        int indexListDimension;
        int indexListNumberList;
    };

    static const RegexInfo& GetRegex();
    static std::string GetRegexExpression(const std::string& possibleDimensions);
    static void SetVariableFromString(const std::string& str, CCondition& condition);
    static std::vector<int> ParseListOfIntegers(const std::string& str);
    static ConditionType StringToConditionType(const std::string& str);
    static bool IsOperatorPrecedenceHigher(Operator a, Operator b);
    static bool EvaluateCondition(const CCondition& cond, const IEvaluationData* evaluateData);

    static RegexInfo regExInfo;
};