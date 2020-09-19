#pragma once

namespace libCZI
{
    /// This is an opaque object representing a query.
    class IQueryCondition
    {
    protected:
        IQueryCondition() {};
        IQueryCondition(IQueryCondition const&) = delete;
        IQueryCondition& operator=(IQueryCondition const&) = delete;
    public:
        virtual ~IQueryCondition() {};
    };

    /// Methods for parsing queries and querying a subblock-repository are found here.
    class CQueryParser
    {
    public:

        /// Parse the specified query expression and return an object representing the parsed query. In case of an error
        /// during the parsing, an exception is thrown.
        /// \param str The query expression.
        /// \returns An object representing the parsed query.
        static std::shared_ptr<libCZI::IQueryCondition> ParseQueryString(const std::string& str);

        static void EnumSubset(libCZI::ISubBlockRepository* sbRepository, const std::shared_ptr<IQueryCondition>& condition, const std::function<bool(int index, const libCZI::SubBlockInfo& info)>& funcEnum);
        static void EnumSubset(libCZI::ISubBlockRepository* sbRepository, const libCZI::IQueryCondition* condition, const std::function<bool(int index, const libCZI::SubBlockInfo& info)>& funcEnum);

        static std::vector<int> GetSubBlocksMatching(libCZI::ISubBlockRepository* sbRepository, const std::shared_ptr<libCZI::IQueryCondition>& condition, int maxResults);
        static std::vector<int> GetSubBlocksMatching(libCZI::ISubBlockRepository* sbRepository, const libCZI::IQueryCondition* condition, int maxResults);
    };
}