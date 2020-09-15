#include "libCZI.h"
#include "ParseQueryString.h"

using namespace std;
using namespace libCZI;

class CQueryCondition : public libCZI::IQueryCondition
{
private:
    vector<TokenItem> tokenlist;
public:
    CQueryCondition(vector<TokenItem>&& list) : tokenlist(move(list))
    {}

    const vector<TokenItem> GetTokenList() const { return this->tokenlist; }
};

class CEvaluationWrapper : public IEvaluationData
{
private:
    const SubBlockInfo* infoRef;
public:
    void SetSubblockInfo(const SubBlockInfo& info)
    {
        this->infoRef = &info;
    }

    virtual int GetCoordinateValue(DimensionIndex dim) const
    {
        int value = std::numeric_limits<int>::min();
        bool b = this->infoRef->coordinate.TryGetPosition(dim, &value);
        // TODO: how to deal with "false" here?
        return value;
    }

    virtual int GetVariable(VariableType type) const
    {
        switch (type)
        {
        case VariableType::PhysicalWidth:
            return this->infoRef->physicalSize.w;
        case VariableType::PhysicalHeight:
            return this->infoRef->physicalSize.h;
        case VariableType::LogicalPositionX:
            return this->infoRef->logicalRect.x;
        case VariableType::LogicalPositionY:
            return this->infoRef->logicalRect.y;
        case VariableType::LogicalPositionWidth:
            return this->infoRef->logicalRect.w;
        case VariableType::LogicalPositionHeight:
            return this->infoRef->logicalRect.h;
        default:
            throw runtime_error("illegal type");
        }
    }
};

/*static*/std::shared_ptr<libCZI::IQueryCondition> CQueryParser::ParseQueryString(const std::string& str)
{
    auto l1 = CParserUtils::Tokenize(str);
    auto l2 = CParserUtils::ConvertToReversePolnish(l1);

    auto r = make_shared<CQueryCondition>(move(l2));
    return r;
}

/*static*/void CQueryParser::EnumSubset(ISubBlockRepository* sbRepository, const std::shared_ptr<IQueryCondition>& condition, const std::function<bool(int index, const SubBlockInfo& info)>& funcEnum)
{
    CQueryParser::EnumSubset(sbRepository, condition.get(), funcEnum);
}

/*static*/void CQueryParser::EnumSubset(ISubBlockRepository* sbRepository, const IQueryCondition* condition, const std::function<bool(int index, const SubBlockInfo& info)>& funcEnum)
{
    const auto* queryCondition = dynamic_cast<const CQueryCondition*>(condition);
    if (queryCondition == nullptr)
    {
        throw invalid_argument("The query-condition argument is invalid.");
    }

    CEvaluationWrapper wrapper;
    sbRepository->EnumerateSubBlocks(
        [&](int idx, const SubBlockInfo& info)->bool
        {
            wrapper.SetSubblockInfo(info);
            bool b = CParserUtils::Evaluate(queryCondition->GetTokenList(), &wrapper);
            if (b == true)
            {
                return funcEnum(idx, info);
            }

            return true;
        });
}

/*static*/std::vector<int> CQueryParser::GetSubBlocksMatching(ISubBlockRepository* sbRepository, const std::shared_ptr<IQueryCondition>& condition, int maxResults)
{
    return CQueryParser::GetSubBlocksMatching(sbRepository, condition.get(), maxResults);
}

/*static*/std::vector<int> CQueryParser::GetSubBlocksMatching(ISubBlockRepository* sbRepository, const IQueryCondition* condition, int maxResults)
{
    std::vector<int> list;
    CQueryParser::EnumSubset(
        sbRepository,
        condition,
        [&](int idx, const SubBlockInfo& info)->bool
        {
            list.push_back(idx);
            if (maxResults > 0 && list.size() > maxResults)
            {
                return false;
            }

            return true;
        });

    return list;
}
