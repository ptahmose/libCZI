#pragma once

#include <regex>
#include <mutex>  
#include "libCZI.h"

class IEvaluationData
{
public:
    virtual int GetCoordinateValue(char dim) const = 0;
};

class CParserUtils
{
public:
    static std::vector<libCZI::TokenItem> Tokenize(const std::string& str);
    static std::vector <libCZI::TokenItem > ConvertToReversePolnish(const std::vector<libCZI::TokenItem>& src);
    static bool CheckTokenList(const std::vector<libCZI::TokenItem>& tokens);
    static bool Evaluate(const std::vector<libCZI::TokenItem>& tokens, const IEvaluationData* evaluateData);
private:
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
    static void SetDimensionFromString(const std::string& str, libCZI::CCondition& condition);
    static std::vector<int> ParseListOfIntegers(const std::string& str);
    static libCZI::ConditionType StringToConditionType(const std::string& str);
    static bool IsOperatorPrecedenceHigher(libCZI::Operator a, libCZI::Operator b);
    static bool EvaluateCondition(const libCZI::CCondition& cond, const IEvaluationData* evaluateData);

    static RegexInfo regExInfo;
};