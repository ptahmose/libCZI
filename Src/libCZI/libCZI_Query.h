#pragma once

namespace libCZI
{
  //enum class Operator : std::uint8_t
  //{
  //  AND = 1,
  //  OR = 2,
  //  XOR = 3,
  //  NOT = 4,

  //  LAST = 4
  //};

  //enum class Token
  //{
  //  Invalid,
  //  Condition,
  //  Operator,
  //  LeftParenthesis,    // it's an "("
  //  RightParenthesis,    // it's an ")"
  //};

  //enum class ConditionType
  //{
  //  Invalid,
  //  Equal,
  //  Unequal,
  //  LessThan,
  //  GreaterThan,
  //  LessThanOrEqual,
  //  GreaterThanOrEqual,
  //  InRange,
  //  InList
  //};

  //class CCondition
  //{
  //private:
  //  char dimension;
  //  ConditionType condition;
  //  int conditionConst;
  //  int rangeStart, rangeEnd;
  //  std::vector<int> list;
  //public:
  //  CCondition() : condition(ConditionType::Invalid) {};

  //  char GetDimension() const { return this->dimension; }

  //  bool Evaluate(int value) const;

  //  void SetDimension(char d)
  //  {
  //    this->dimension = d;
  //  }

  //  void SetRange(int rangeStart, int rangeEnd)
  //  {
  //    this->condition = ConditionType::InRange;
  //    this->rangeStart = rangeStart;
  //    this->rangeEnd = rangeEnd;
  //  }

  //  void SetList(const std::vector<int>& list)
  //  {
  //    this->condition = ConditionType::InList;
  //    this->list = list;
  //  }

  //  void SetRelationTypeAndConstant(ConditionType type, int constant)
  //  {
  //    this->condition = type;
  //    this->conditionConst = constant;
  //  }

  //  void SetEqualTo(int equalConst)
  //  {
  //    this->condition = ConditionType::Equal;
  //    this->conditionConst = equalConst;
  //  }

  //  void SetUnequalTo(int unequalConst)
  //  {
  //    this->condition = ConditionType::Unequal;
  //    this->conditionConst = unequalConst;
  //  }

  //  void SetLessThan(int lessThanConst)
  //  {
  //    this->condition = ConditionType::LessThan;
  //    this->conditionConst = lessThanConst;
  //  }

  //  void SetGreaterThan(int greaterThanConst)
  //  {
  //    this->condition = ConditionType::GreaterThan;
  //    this->conditionConst = greaterThanConst;
  //  }

  //  void SetLessThanOrEqual(int lessThanOrEqualConst)
  //  {
  //    this->condition = ConditionType::LessThanOrEqual;
  //    this->conditionConst = lessThanOrEqualConst;
  //  }

  //  void SetGreaterThanOrEqual(int greaterThanOrEqualConst)
  //  {
  //    this->condition = ConditionType::GreaterThanOrEqual;
  //    this->conditionConst = greaterThanOrEqualConst;
  //  }
  //};

  //class TokenItem
  //{
  //public:
  //  TokenItem() : token(Token::Invalid) {};
  //  Token token;
  //  CCondition condition;
  //  Operator op;
  //};

  class IQueryCondition
  {
  protected:
    IQueryCondition() {};
    IQueryCondition(IQueryCondition const&) = delete;
    IQueryCondition& operator=(IQueryCondition const&) = delete;
  public:
    virtual ~IQueryCondition() {};
  };

  class CQueryParser
  {
  public:
    static std::shared_ptr<IQueryCondition> ParseQueryString(const std::string& str);

    static void EnumSubset(ISubBlockRepository* sbRepository, const std::shared_ptr<IQueryCondition>& condition, const std::function<bool(int index, const SubBlockInfo& info)>& funcEnum);
    static void EnumSubset(ISubBlockRepository* sbRepository, const IQueryCondition* condition, const std::function<bool(int index, const SubBlockInfo& info)>& funcEnum);

    static std::vector<int> GetSubBlocksMatching(ISubBlockRepository* sbRepository, const std::shared_ptr<IQueryCondition>& condition, int maxResults);
    static std::vector<int> GetSubBlocksMatching(ISubBlockRepository* sbRepository, const IQueryCondition* condition, int maxResults);
  };
}