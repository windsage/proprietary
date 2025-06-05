/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef DATABASE_EXEC_STATEMENT_MESSAGE_H
#define DATABASE_EXEC_STATEMENT_MESSAGE_H

#include <modules/db/types.h>
#include "framework/SolicitedMessage.h"
#include <framework/add_message_id.h>
#include <functional>
#include <optional>
#include <limits>

/**
 * DatabaseExecStatementMessage.
 *
 * This message allows a module to access the qcril database by executing an SQL
 * command in the proper context.
 *
 * A callback must be provided, which will be called for every row in the results.
 * Once all rows have been processed (or the processing has been aborted by the callback),
 * the response callback for the message will be invoked
 *
 */
class DatabaseExecStatementMessage: public SolicitedMessage<void>,
    public add_message_id<DatabaseExecStatementMessage>
{
    public:
        using RowCb = std::function<bool(const QtiRilDbRow &)>;
    private:
        std::shared_ptr<QtiRilPreparedStatement> stmt;
        std::string sql;
        RowCb cb;
        size_t batchSize;
        std::optional<std::vector<QtiRilParameterList>> parameterLists;
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.db.exec_statement";
        DatabaseExecStatementMessage() = delete;

        /**
         * Execute the prepared statement a number of times within a transaction.
         * This is useful, e.g., to perform multiple inserts at the same time
         * @param stmt: A shared_ptr to a prepared statement previously obtained by sending
         * DatabasePrepareStatementMessage.
         * @param parameterLists: A vector of parameter lists of actual parameters to pass to the prepared statement.
         * @param cb: A callback invoked for every row in the result. Note that this callback
         * is invoked in the context of the database module. The callback implementation should
         * not block for a long time to prevent starving other users of the database module.
         */
        DatabaseExecStatementMessage(std::shared_ptr<QtiRilPreparedStatement> stmt,
                std::vector<QtiRilParameterList> parameterLists,
                RowCb cb):
            SolicitedMessage(get_class_message_id()),
            stmt(stmt),
            cb(cb),
            batchSize(std::numeric_limits<size_t>::max()),
            parameterLists(parameterLists)
        {
            mName = MESSAGE_NAME;
        }

        /**
         * @param stmt: A shared_ptr to a prepared statement previously obtained by sending
         * DatabasePrepareStatementMessage.
         * @param parameterList: A list of actual parameters to pass to the prepared statement.
         * @param cb: A callback invoked for every row in the result. Note that this callback
         * is invoked in the context of the database module. The callback implementation should
         * not block for a long time to prevent starving other users of the database module.
         * @param batchSize: A hint specifying how many rows should be processed at once. The
         * implementation may choose to ignore this, but when it is considered, it is expected
         * that the higher this number, the better the throughput will be, but the higher the
         * latency of the database module.
         * The implementation may also ignore values larger or smaller than a specific value
         */
        DatabaseExecStatementMessage(std::shared_ptr<QtiRilPreparedStatement> stmt,
                QtiRilParameterList parameterList,
                RowCb cb,
                size_t batchSize = std::numeric_limits<size_t>::max()):
            SolicitedMessage(get_class_message_id()),
            stmt(stmt),
            cb(cb),
            batchSize(batchSize),
            parameterLists({parameterList})
        {
            mName = MESSAGE_NAME;
        }

        /**
         * @param stmt: A shared_ptr to a prepared statement previously obtained by sending
         * DatabasePrepareStatementMessage.
         * @param cb: A callback invoked for every row in the result. Note that this callback
         * is invoked in the context of the database module. The callback implementation should
         * not block for a long time to prevent starving other users of the database module.
         * @param batchSize: A hint specifying how many rows should be processed at once. The
         * implementation may choose to ignore this, but when it is considered, it is expected
         * that the higher this number, the better the throughput will be, but the higher the
         * latency of the database module.
         * The implementation may also ignore values larger or smaller than a specific value
         */
        DatabaseExecStatementMessage(std::shared_ptr<QtiRilPreparedStatement> stmt,
                RowCb cb,
                size_t batchSize = std::numeric_limits<size_t>::max()):
            SolicitedMessage(get_class_message_id()),
            stmt(stmt),
            cb(cb),
            batchSize(batchSize),
            parameterLists()
        {
            mName = MESSAGE_NAME;
        }
        /**
         * @param sql: The SQL command to be executed
         * @param cb: A callback invoked for every row in the result. Note that this callback
         * is invoked in the context of the database module. The callback implementation should
         * not block for a long time to prevent starving other users of the database module.
         * @param batchSize: A hint specifying how many rows should be processed at once. The
         * implementation may choose to ignore this, but when it is considered, it is expected
         * that the higher this number, the better the throughput will be, but the higher the
         * latency of the database module.
         * The implementation may also ignore values larger or smaller than a specific value
         */
        DatabaseExecStatementMessage(std::string sql,
                RowCb cb,
                size_t batchSize = std::numeric_limits<size_t>::max()):
            SolicitedMessage(get_class_message_id()),
            sql(sql),
            cb(cb),
            batchSize(batchSize),
            parameterLists()
        {
            mName = MESSAGE_NAME;
        }
        std::shared_ptr<QtiRilPreparedStatement> getPreparedStatement() {
            return stmt;
        }
        const std::string &getSql() const {
            return sql;
        }
        std::string dump() {
            return std::string("DatabaseExecStatementMessage") + "sql: " + sql;
        }

        const std::optional<std::vector<QtiRilParameterList>> &getParameterLists() const {
            return parameterLists;
        }

        size_t getBatchSize() {
            return batchSize;
        }
        RowCb getRowCb() {
            return cb;
        }
};

#endif

