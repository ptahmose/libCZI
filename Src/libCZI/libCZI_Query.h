#pragma once

namespace libCZI
{
    /// This is an opaque object representing a query.
    class IQueryCondition
    {
    protected:
        IQueryCondition() = default;
        IQueryCondition(IQueryCondition const&) = delete;
        IQueryCondition& operator=(IQueryCondition const&) = delete;
    public:
        virtual ~IQueryCondition() = default;
    };

    /// This structure gathers options for the query-construction.
    struct QueryOptions
    {
        /// Values that indicate how to deal with a condition which references
        /// a dimension not existent on a subblock.
        enum class HandlingOfNonExistentDimensions
        {
            EvaluateToTrue,     ///< A condition involving a non-existent dimension evaluates to true.
            EvaluateToFalse,    ///< A condition involving a non-existent dimension evaluates to false.
            Error               ///< A condition involving a non-existent dimension results in an error.
        };

        /// This field describes how "non existent dimensions" are treated in a query expression.
        HandlingOfNonExistentDimensions handlingNonExistentDimensions;

        /// Sets the struct to default values.
        void SetDefault()
        {
            this->handlingNonExistentDimensions = HandlingOfNonExistentDimensions::EvaluateToTrue;
        }
    };

    /// Methods for parsing queries and querying a subblock-repository are found here.
    /// The syntax for the query is as follows: conditions of the form "variable = constant" can
    /// be formulated, and multiple conditions can be combined with boolean operators (AND, OR, XOR, NOT).
    /// The conditions can be grouped with parenthesis.
    /// Variables can be the coordinates of the various dimensions (like T, Z, C), or the following expressions:
    /// string         |  meaning
    /// -------------- | -----------------
    /// Width          | the (physical) width of the subblock in pixels
    /// Height         | the (physical) height of the subblock in pixels
    /// LogPosX        | the x-position of the logical subblock position
    /// LogPosY        | the y-position of the logical subblock position
    /// LogPosWidth    | the width of the logical subblock position
    /// LogPosHeight   | the height of the logical subblock position
    /// IsLayer0       | gives "1" if the subblock is in pyramid layer 0, and "0" otherwise
    class LIBCZI_API CQueryParser
    {
    public:
        /// Parse the specified query expression and return an object representing the parsed query. In
        /// case of an error during the parsing, an exception is thrown.
        /// \param str     The query expression.
        /// \param options (Optional) If non-null, options for controlling the operation.
        /// \returns An object representing the parsed query.
        static std::shared_ptr<libCZI::IQueryCondition> ParseQueryString(const std::string& str, const QueryOptions* options = nullptr);

        /// Enumerate the subblock-repository for subblocks matching the specified condition. For each subblock matching
        /// the condition, the specified callback is called. If the callback returns false, the enumeration is stopped.
        /// \param [in]     sbRepository The subblock repository.
        /// \param          condition    The condition.
        /// \param          funcEnum     The callback function which is called for every subblock matching the condition.
        static void EnumSubset(libCZI::ISubBlockRepository* sbRepository, const std::shared_ptr<IQueryCondition>& condition, const std::function<bool(int index, const libCZI::SubBlockInfo& info)>& funcEnum);

        /// Enumerate the subblock-repository for subblocks matching the specified condition. For each subblock matching
        /// the condition, the specified callback is called. If the callback returns false, the enumeration is stopped.
        /// \param [in]     sbRepository The subblock repository.
        /// \param          condition    The condition.
        /// \param          funcEnum     The callback function which is called for every subblock matching the condition.
        static void EnumSubset(libCZI::ISubBlockRepository* sbRepository, const libCZI::IQueryCondition* condition, const std::function<bool(int index, const libCZI::SubBlockInfo& info)>& funcEnum);

        /// Gets a vector containing the ids of the subblocks matching the specified condition. The maximal number of ids which will be returned
        /// may be specified.
        /// \param [in]     sbRepository The subblock repository.
        /// \param          condition    The condition.
        /// \param          maxResults   The maximum number of results to return. If less or equal to zero, then all ids are retrieved.
        /// \returns The ids of the subblocks matching the specified condition.
        static std::vector<int> GetSubBlocksMatching(libCZI::ISubBlockRepository* sbRepository, const std::shared_ptr<libCZI::IQueryCondition>& condition, int maxResults);

        /// Gets a vector containing the ids of the subblocks matching the specified condition. The maximal number of ids which will be returned
        /// may be specified.
        /// \param [in]     sbRepository The subblock repository.
        /// \param          condition    The condition.
        /// \param          maxResults   The maximum number of results to return. If less or equal to zero, then all ids are retrieved.
        /// \returns The ids of the subblocks matching the specified condition.
        static std::vector<int> GetSubBlocksMatching(libCZI::ISubBlockRepository* sbRepository, const libCZI::IQueryCondition* condition, int maxResults);
    };
}
