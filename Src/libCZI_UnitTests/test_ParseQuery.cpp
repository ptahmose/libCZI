#include "pch.h"
#include <vector>
#include "inc_libCZI.h"
#include "../libCZI/ParseQueryString.h"

using namespace libCZI;
using namespace std;

class EvaluationDataAdapter :public IEvaluationData
{
private:
    std::function<int(DimensionIndex)> func;
public:
    EvaluationDataAdapter(std::function<int(DimensionIndex)> func)
        : func(func)
    {}

    virtual int GetCoordinateValue(DimensionIndex dim) const
    {
        return func(dim);
    }

    virtual int GetVariable(VariableType type) const
    {
        throw runtime_error("not implemented");
    }
};

TEST(ParseQuery, Tokenize1)
{
    auto tokens = CParserUtils::Tokenize("T=5 OR T=[1,2] OR T={9,10,23}");

    EXPECT_EQ(tokens.size(), 5) << "Expected 5 tokens";

    EXPECT_TRUE(tokens[0].token == Token::Condition && tokens[0].condition.GetDimension() == DimensionIndex::T);
    EXPECT_TRUE(tokens[1].token == Token::Operator && tokens[1].op == Operator::OR);
    EXPECT_TRUE(tokens[2].token == Token::Condition && tokens[2].condition.GetDimension() == DimensionIndex::T);
    EXPECT_TRUE(tokens[3].token == Token::Operator && tokens[3].op == Operator::OR);
    EXPECT_TRUE(tokens[4].token == Token::Condition && tokens[4].condition.GetDimension() == DimensionIndex::T);
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

    EXPECT_TRUE(tokens[0].token == Token::Condition && tokens[0].condition.GetDimension() == DimensionIndex::T);
    EXPECT_TRUE(tokens[1].token == Token::Operator && tokens[1].op == Operator::OR);
    EXPECT_TRUE(tokens[2].token == Token::Condition && tokens[2].condition.GetDimension() == DimensionIndex::T);
    EXPECT_TRUE(tokens[3].token == Token::Operator && tokens[3].op == Operator::OR);
    EXPECT_TRUE(tokens[4].token == Token::Condition && tokens[4].condition.GetDimension() == DimensionIndex::T);
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
    EXPECT_TRUE(tirpn[0].token == Token::Condition && tirpn[0].condition.GetDimension() == DimensionIndex::T);
    EXPECT_TRUE(tirpn[1].token == Token::Condition && tirpn[1].condition.GetDimension() == DimensionIndex::Z);
    EXPECT_TRUE(tirpn[2].token == Token::Operator && tirpn[2].op == Operator::AND);
    EXPECT_TRUE(tirpn[3].token == Token::Condition && tirpn[3].condition.GetDimension() == DimensionIndex::C);
    EXPECT_TRUE(tirpn[4].token == Token::Operator && tirpn[4].op == Operator::OR);
}


TEST(ParseQuery, Evaluate1)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND Z=1 OR T=0");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T: return 3;
            case DimensionIndex::Z: return 1;
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

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 3;
            case DimensionIndex::Z:return 0;
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

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 4;
            case DimensionIndex::Z:return 1;
            case DimensionIndex::C:return 0;
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

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 4;
            case DimensionIndex::Z:return 1;
            case DimensionIndex::C:return 0;
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

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 4;
            case DimensionIndex::Z:return 1;
            case DimensionIndex::C:return 0;
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

TEST(ParseQuery, Evaluate7)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND (Z=1 OR Z=2 OR Z=4)");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 3;
            case DimensionIndex::Z:return 2;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);

    EXPECT_EQ(r, true) << "Unexpected result";
}

TEST(ParseQuery, Evaluate8)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND Z=1 OR Z=2 OR Z=4");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 6;
            case DimensionIndex::Z:return 4;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);

    EXPECT_EQ(r, true) << "Unexpected result";
}

TEST(ParseQuery, Evaluate9)
{
    auto tis = CParserUtils::Tokenize("  T=3 AND NOT(Z=1 OR Z=2 OR Z=4)");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 3;
            case DimensionIndex::Z:return 4;
            default:
                throw std::runtime_error("unexpected dimension");
            }
        });

    bool r = CParserUtils::Evaluate(tirpn, &evalData);

    EXPECT_EQ(r, false) << "Unexpected result";
}

TEST(ParseQuery, Evaluate10)
{
    bool exceptionOccurred = false;
    try
    {
        auto tis = CParserUtils::Tokenize("  T={2147483648}  ");
    }
    catch (LibCZIQueryParseException& ex)
    {
        EXPECT_EQ(ex.GetErrorType(), LibCZIQueryParseException::ErrorType::InvalidNumberFormat);
        exceptionOccurred = true;
    }

    EXPECT_TRUE(exceptionOccurred);
}

TEST(ParseQuery, Evaluate11)
{
    bool exceptionOccurred = false;
    try
    {
        auto tis = CParserUtils::Tokenize("  T=[-2147483649  , 0]");
    }
    catch (LibCZIQueryParseException& ex)
    {
        EXPECT_EQ(ex.GetErrorType(), LibCZIQueryParseException::ErrorType::InvalidNumberFormat);
        exceptionOccurred = true;
    }

    EXPECT_TRUE(exceptionOccurred);
}


TEST(ParseQuery, EvaluateNot1)
{
    auto tis = CParserUtils::Tokenize("  NOT   T=3   ");
    auto tirpn = CParserUtils::ConvertToReversePolnish(tis);

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 3;
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

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 5;
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

    EvaluationDataAdapter evalData([](DimensionIndex dim)->int
        {
            switch (dim)
            {
            case DimensionIndex::T:return 5;
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

//-----------------------------------------------------------------------------

class CSubBlockRepositoryMock : public libCZI::ISubBlockRepository
{
private:
    vector<SubBlockInfo> sbBlks;
public:
    void AddSubBlock(SubBlockInfo& sbi)
    {
        this->sbBlks.push_back(sbi);
    }

    virtual void EnumerateSubBlocks(std::function<bool(int index, const SubBlockInfo& info)> funcEnum)
    {
        for (size_t i = 0; i < sbBlks.size(); ++i)
        {
            bool b = funcEnum((int)i, sbBlks[i]);
            if (!b)
            {
                break;
            }
        }
    }
    virtual void EnumSubset(const IDimCoordinate* planeCoordinate, const IntRect* roi, bool onlyLayer0, std::function<bool(int index, const SubBlockInfo& info)> funcEnum)
    {
        throw std::runtime_error("not implemented");
    }
    virtual std::shared_ptr<ISubBlock> ReadSubBlock(int index)
    {
        throw std::runtime_error("not implemented");
    }
    virtual bool TryGetSubBlockInfoOfArbitrarySubBlockInChannel(int channelIndex, SubBlockInfo& info)
    {
        throw std::runtime_error("not implemented");
    }
    virtual SubBlockStatistics GetStatistics()
    {
        throw std::runtime_error("not implemented");
    }
    virtual PyramidStatistics GetPyramidStatistics()
    {
        throw std::runtime_error("not implemented");
    }
};

TEST(ParseQuery, QueryParser1Test)
{
    CSubBlockRepositoryMock sbRepoMock;
    for (int z = 0; z < 50; ++z)
    {
        SubBlockInfo sbInfo;
        sbInfo.coordinate.Set(DimensionIndex::Z, z);
        sbRepoMock.AddSubBlock(sbInfo);
    }

    auto list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("Z > 24"), -1);

    EXPECT_EQ(list.size(), 25);
}

TEST(ParseQuery, QueryParser2Test)
{
    CSubBlockRepositoryMock sbRepoMock;
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    int count4 = 0;
    int count5 = 0;
    for (int z = 0; z < 10; ++z)
    {
        for (int t = 0; t < 10; ++t)
        {
            SubBlockInfo sbInfo;
            sbInfo.coordinate.Set(DimensionIndex::Z, z);
            sbInfo.coordinate.Set(DimensionIndex::T, t);
            sbRepoMock.AddSubBlock(sbInfo);

            if (z > 4 && t > 4) ++count1;
            if (z > 4 || t > 4) ++count2;
            if (!(z > 4 || t > 4)) ++count3;
            if ((!(z > 4)) || t > 4) ++count4;
            if ((z > 4) ^ (t > 4)) ++count5;
        }
    }

    auto list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("Z > 4 AND T>4"), -1);
    EXPECT_EQ(list.size(), count1);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("Z > 4 OR T>4"), -1);
    EXPECT_EQ(list.size(), count2);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("NOT (Z > 4 OR T>4)"), -1);
    EXPECT_EQ(list.size(), count3);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("(NOT Z > 4) OR T > 4"), -1);
    EXPECT_EQ(list.size(), count4);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("Z > 4 XOR T>4"), -1);
    EXPECT_EQ(list.size(), count5);
}

TEST(ParseQuery, QueryParserWidthVariable1Test)
{
    CSubBlockRepositoryMock sbRepoMock;
    for (int z = 0; z < 50; ++z)
    {
        SubBlockInfo sbInfo;
        sbInfo.physicalSize.w = 10 * (z + 1);
        sbInfo.physicalSize.h = 10 * (z + 1);
        sbInfo.coordinate.Set(DimensionIndex::Z, z);
        sbRepoMock.AddSubBlock(sbInfo);
    }

    auto list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("Width > 480"), -1);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list[0], 48);
    EXPECT_EQ(list[1], 49);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("Width >= 480"), -1);

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list[0], 47);
    EXPECT_EQ(list[1], 48);
    EXPECT_EQ(list[2], 49);
} 

TEST(ParseQuery, QueryParserHeightVariable1Test)
{
    CSubBlockRepositoryMock sbRepoMock;
    for (int z = 0; z < 50; ++z)
    {
        SubBlockInfo sbInfo;
        sbInfo.physicalSize.w = 10 * (z + 1);
        sbInfo.physicalSize.h = 10 * (z + 1);
        sbInfo.coordinate.Set(DimensionIndex::Z, z);
        sbRepoMock.AddSubBlock(sbInfo);
    }

    auto list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("Height > 480"), -1);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list[0], 48);
    EXPECT_EQ(list[1], 49);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("Height >= 480"), -1);

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list[0], 47);
    EXPECT_EQ(list[1], 48);
    EXPECT_EQ(list[2], 49);
}

TEST(ParseQuery, QueryParserLogPosXVariable1Test)
{
    CSubBlockRepositoryMock sbRepoMock;
    for (int z = 0; z < 50; ++z)
    {
        SubBlockInfo sbInfo;
        sbInfo.physicalSize.w = 10 * (z + 1);
        sbInfo.physicalSize.h = 10 * (z + 1);
        sbInfo.logicalRect.x = 1 * (z + 1);
        sbInfo.logicalRect.y = 10 * (z + 1);
        sbInfo.logicalRect.w = 100 * (z + 1);
        sbInfo.logicalRect.h = 1000 * (z + 1);
        sbInfo.coordinate.Set(DimensionIndex::Z, z);
        sbRepoMock.AddSubBlock(sbInfo);
    }

    auto list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("LogPosX > 48"), -1);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list[0], 48);
    EXPECT_EQ(list[1], 49);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("LogPosX >= 48"), -1);

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list[0], 47);
    EXPECT_EQ(list[1], 48);
    EXPECT_EQ(list[2], 49);
}

TEST(ParseQuery, QueryParserLogPosYVariable1Test)
{
    CSubBlockRepositoryMock sbRepoMock;
    for (int z = 0; z < 50; ++z)
    {
        SubBlockInfo sbInfo;
        sbInfo.physicalSize.w = 10 * (z + 1);
        sbInfo.physicalSize.h = 10 * (z + 1);
        sbInfo.logicalRect.x = 1 * (z + 1);
        sbInfo.logicalRect.y = 10 * (z + 1);
        sbInfo.logicalRect.w = 100 * (z + 1);
        sbInfo.logicalRect.h = 1000 * (z + 1);
        sbInfo.coordinate.Set(DimensionIndex::Z, z);
        sbRepoMock.AddSubBlock(sbInfo);
    }

    auto list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("LogPosY > 480"), -1);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list[0], 48);
    EXPECT_EQ(list[1], 49);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("LogPosY >= 480"), -1);

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list[0], 47);
    EXPECT_EQ(list[1], 48);
    EXPECT_EQ(list[2], 49);
}

TEST(ParseQuery, QueryParserLogPosWVariable1Test)
{
    CSubBlockRepositoryMock sbRepoMock;
    for (int z = 0; z < 50; ++z)
    {
        SubBlockInfo sbInfo;
        sbInfo.physicalSize.w = 10 * (z + 1);
        sbInfo.physicalSize.h = 10 * (z + 1);
        sbInfo.logicalRect.x = 1 * (z + 1);
        sbInfo.logicalRect.y = 10 * (z + 1);
        sbInfo.logicalRect.w = 100 * (z + 1);
        sbInfo.logicalRect.h = 1000 * (z + 1);
        sbInfo.coordinate.Set(DimensionIndex::Z, z);
        sbRepoMock.AddSubBlock(sbInfo);
    }

    auto list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("LogPosWidth > 4800"), -1);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list[0], 48);
    EXPECT_EQ(list[1], 49);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("LogPosWidth >= 4800"), -1);

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list[0], 47);
    EXPECT_EQ(list[1], 48);
    EXPECT_EQ(list[2], 49);
}

TEST(ParseQuery, QueryParserLogPosHVariable1Test)
{
    CSubBlockRepositoryMock sbRepoMock;
    for (int z = 0; z < 50; ++z)
    {
        SubBlockInfo sbInfo;
        sbInfo.physicalSize.w = 10 * (z + 1);
        sbInfo.physicalSize.h = 10 * (z + 1);
        sbInfo.logicalRect.x = 1 * (z + 1);
        sbInfo.logicalRect.y = 10 * (z + 1);
        sbInfo.logicalRect.w = 100 * (z + 1);
        sbInfo.logicalRect.h = 1000 * (z + 1);
        sbInfo.coordinate.Set(DimensionIndex::Z, z);
        sbRepoMock.AddSubBlock(sbInfo);
    }

    auto list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("LogPosHeight > 48000"), -1);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list[0], 48);
    EXPECT_EQ(list[1], 49);

    list = CQueryParser::GetSubBlocksMatching(&sbRepoMock, CQueryParser::ParseQueryString("LogPosHeight >= 48000"), -1);

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list[0], 47);
    EXPECT_EQ(list[1], 48);
    EXPECT_EQ(list[2], 49);
}