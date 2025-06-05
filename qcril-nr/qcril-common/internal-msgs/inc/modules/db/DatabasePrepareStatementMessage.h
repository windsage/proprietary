/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef DATABASE_PREPARE_STATEMENT_MESSAGE_H
#define DATABASE_PREPARE_STATEMENT_MESSAGE_H

#include <modules/db/types.h>
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"


/**
 * Prepare a statement that can be later used with DatabaseExecStatementMessage
 * The statement will exist as long as the caller holds a pointer to the statement
 * and can be reused as many times as required
 */
class DatabasePrepareStatementMessage: public SolicitedMessage<QtiRilPreparedStatement>,
    public add_message_id<DatabasePrepareStatementMessage>
{
    private:
        std::shared_ptr<QtiRilPreparedStatement> stmt;
        std::string sql;
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.db.prepare_statement";
        DatabasePrepareStatementMessage() = delete;
        DatabasePrepareStatementMessage(std::string sql):
            SolicitedMessage(get_class_message_id()),
            stmt(nullptr),
            sql(sql)
        {
            mName = MESSAGE_NAME;
        }
        const std::string &getSql() {
            return sql;
        }
        std::string dump() {
            return std::string("DatabasePrepareStatementMessage") + "sql: " + sql;
        }

};

#endif

