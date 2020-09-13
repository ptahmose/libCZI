#include "pch.h"
#include "inc_libCZI.h"
#include "../libCZI/ParseQueryString.h"

using namespace libCZI;

class EvaluationDataAdapter :public IEvaluationData
{
private:
    std::function<int(char)> func;
public:
    EvaluationDataAdapter(std::function<int(char)> func)
        : func(func)
    {}

    virtual int GetCoordinateValue(char dim) const
    {
        return func(dim);
    }
};

TEST(ParseQuery, Tokenize1)
{
    auto tokens = CParserUtils::Tokenize("T=5 OR T=[1,2] OR T={9,10,23}");

    EXPECT_EQ(tokens.size(), 5) << "Expected 5 tokens";

    EXPECT_TRUE(tokens[0].token == Token::Condition && tokens[0].condition.GetDimension() == 'T');
    EXPECT_TRUE(tokens[1].token == Token::Operator && tokens[1].op == Operator::OR);
    EXPECT_TRUE(tokens[2].token == Token::Condition && tokens[2].condition.GetDimension() == 'T');
    EXPECT_TRUE(tokens[3].token == Token::Operator && tokens[3].op == Operator::OR);
    EXPECT_TRUE(tokens[4].token == Token::Condition && tokens[4].condition.GetDimension() == 'T');
}

TEST(ParseQuery, Tokenize2)
{
    bool exceptionOccurred = false;
    try
    {
        auto tokens = CParserUtils::Tokenize("T=5OR T=[1,2] OR T={9,10,23}");
    }
    catch (LibCZIQueryParseException& ex)
    {
        EXPECT_EQ(ex.GetErrorType(), LibCZIQueryParseException::ErrorType::SyntaxError);
        EXPECT_EQ(ex.GetPositionAfterWhichParserErrorOccurred(), 3);
        exceptionOccurred = true;
    }

    EXPECT_TRUE(exceptionOccurred);
}

TEST(ParseQuery, Tokenize3)
{
    auto tokens = CParserUtils::Tokenize("T=5 OR T=[1,2] OR T={-9,+10,-23}  ");

    EXPECT_EQ(tokens.size(), 5) << "Expected 5 tokens";

    EXPECT_TRUE(tokens[0].token == Token::Condition && tokens[0].condition.GetDimension() == 'T');
    EXPECT_TRUE(tokens[1].token == Token::Operator && tokens[1].op == Operator::OR);
    EXPECT_TRUE(tokens[2].token == Token::Condition && tokens[2].condition.GetDimension() == 'T');
    EXPECT_TRUE(tokens[3].token == Token::Operator && tokens[3].op == Operator::OR);
    EXPECT_TRUE(tokens[4].token == Token::Condition && tokens[4].condition.GetDimension() == 'T');
}

TEST(ParseQuery, Tokenize4)
{
    bool exceptionOccurred = false;
    try
    {
        auto tokens = CParserUtils::Tokenize("T=5 OR T=[1,2] OR T={9,10,23");
    }
    catch (LibCZIQueryParseException& ex)
    {
        EXPECT_EQ(ex.GetErrorType(), LibCZIQueryParseException::ErrorType::SyntaxError);
        EXPECT_EQ(ex.GetPositionAfterWhichParserErrorOccurred(), 18);
        exceptionOccurred = true;
    }

    EXPECT_TRUE(exceptionOccurred);
}

TEST(ParseQuery, GenerateTokenList1)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND Z=1 OR C=0");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EXPECT_EQ(tirpn.size(), 5) << "Expected 5 tokens";
    EXPECT_TRUE(tirpn[0].token == Token::Condition && tirpn[0].condition.GetDimension() == 'T');
    EXPECT_TRUE(tirpn[1].token == Token::Condition && tirpn[1].condition.GetDimension() == 'Z');
    EXPECT_TRUE(tirpn[2].token == Token::Operator && tirpn[2].op == Operator::AND);
    EXPECT_TRUE(tirpn[3].token == Token::Condition && tirpn[3].condition.GetDimension() == 'C');
    EXPECT_TRUE(tirpn[4].token == Token::Operator && tirpn[4].op == Operator::OR);
}


TEST(ParseQuery, Evaluate1)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND Z=1 OR T=0");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](char dim)->int
        {
            switch (dim)
            {
            case 'T':return 3;
            case 'Z':return 1;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);

    EXPECT_EQ(r, true) << "Unexpected result";
}

TEST(ParseQuery, Evaluate2)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND Z=1 OR T=0");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](char dim)->int
        {
            switch (dim)
            {
            case 'T':return 3;
            case 'Z':return 0;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);

    EXPECT_EQ(r, false) << "Unexpected result";
}

TEST(ParseQuery, Evaluate3)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND (Z=1 OR C=0)");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](char dim)->int
        {
            switch (dim)
            {
            case 'T':return 4;
            case 'Z':return 1;
            case 'C':return 0;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);

    EXPECT_EQ(r, false) << "Unexpected result";
}

TEST(ParseQuery, Evaluate4)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND Z=1 OR C=0");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](char dim)->int
        {
            switch (dim)
            {
            case 'T':return 4;
            case 'Z':return 1;
            case 'C':return 0;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);

    EXPECT_EQ(r, true) << "Unexpected result";
}

TEST(ParseQuery, Evaluate5)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND (((((((((Z=1 OR C=0)))))))))");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](char dim)->int
        {
            switch (dim)
            {
            case 'T':return 4;
            case 'Z':return 1;
            case 'C':return 0;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);

    EXPECT_EQ(r, false) << "Unexpected result";
}

TEST(ParseQuery, Evaluate6)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND ((((((((((Z=1 OR C=0)))))))))");
    bool exceptionOccurred = false;
    try
    {
        auto tirpn = CParserUtils::ConvertToReversePolnish(tis);
    }
    catch (LibCZIQueryParseException& ex)
    {
        EXPECT_EQ(ex.GetErrorType(), LibCZIQueryParseException::ErrorType::UnbalancedParenthesis);
        exceptionOccurred = true;
    }

    EXPECT_TRUE(exceptionOccurred);
}

TEST(ParseQuery, EvaluateNot1)
{
    auto tis = CParserUtils::Tokenize("  NOT   T=3   ");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](char dim)->int
        {
            switch (dim)
            {
            case 'T':return 3;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);
    EXPECT_EQ(r, false) << "Unexpected result";
}

TEST(ParseQuery, EvaluateNot2)
{
    auto tis = CParserUtils::Tokenize("  NOT   T=3   ");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](char dim)->int
        {
            switch (dim)
            {
            case 'T':return 5;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);
    EXPECT_EQ(r, true) << "Unexpected result";
}

TEST(ParseQuery, EvaluateNot3)
{
    auto tis = CParserUtils::Tokenize("  NOT   T=3  NOT ");
    bool exceptionOccurred = false;
    try
    {
        auto tirpn = CParserUtils::ConvertToReversePolnish(tis);
    }
    catch (LibCZIQueryParseException& ex)
    {
        EXPECT_EQ(ex.GetErrorType(), LibCZIQueryParseException::ErrorType::IllformedExpression);
        exceptionOccurred = true;
    }

    EXPECT_TRUE(exceptionOccurred);
}

TEST(ParseQuery, EvaluateNot4)
{
    auto tis = CParserUtils::Tokenize("  NOT NOT NOT NOT  NOT T=3   ");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](char dim)->int
        {
            switch (dim)
            {
            case 'T':return 5;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);
    EXPECT_EQ(r, true) << "Unexpected result";
}

TEST(ParseQuery, EvaluateNot5)
{
    auto tis = CParserUtils::Tokenize("  NOT  NOT  NOT ");
    bool exceptionOccurred = false;
    try
    {
        auto tirpn = CParserUtils::ConvertToReversePolnish(tis);
    }
    catch (LibCZIQueryParseException& ex)
    {
        EXPECT_EQ(ex.GetErrorType(), LibCZIQueryParseException::ErrorType::IllformedExpression);
        exceptionOccurred = true;
    }

    EXPECT_TRUE(exceptionOccurred);
}
