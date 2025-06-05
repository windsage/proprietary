/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "DatabaseUpdateNotifyMessage.h"
#include <modules/db/DatabasePrepareStatementMessage.h>
#include <modules/db/DatabaseExecStatementMessage.h>

class DatabaseModule : public Module {
  public:
    static void createDataBaseModule(int id);
    static DatabaseModule& getInstance();
    DatabaseModule();
    ~DatabaseModule();
    void setDatabaseInstanceId(int id);
    void init();

  private:
    int mId;
    void handleDatabaseUpdateNotification(std::shared_ptr<DatabaseUpdateNotifyMessage> msg);
    void handleDatabasePrepareStatement(std::shared_ptr<DatabasePrepareStatementMessage> msg);
    void handleDatabaseExecStatementMessage(std::shared_ptr<DatabaseExecStatementMessage> msg);
    std::shared_ptr<QtiRilPreparedStatement> beginStmt;
    std::shared_ptr<QtiRilPreparedStatement> commitStmt;
    std::shared_ptr<QtiRilPreparedStatement> rollbackStmt;
};

DatabaseModule& getDatabaseModule();
