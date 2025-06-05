/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <memory>
#include <modules/db/DatabaseExecStatementMessage.h>
#include <gtest/gtest.h>
#include <DatabaseModule.h>
#define TAG "TestDatabase"
#include <framework/Log.h>
#include <chrono>

using namespace std::chrono_literals;
enum class State {
    INITIAL = 0,
    REQ_DISPATCHED,
    RECEIVED_ROW,
    RECEIVED_RESPONSE,
};

std::string toString(State s) {
    switch (s) {
        case State::INITIAL:
            return "INITIAL";
            break;
        case State::REQ_DISPATCHED:
            return "REQ_DISPATCHED";
            break;
        case State::RECEIVED_ROW:
            return "RECEIVED_ROW";
            break;
        case State::RECEIVED_RESPONSE:
            return "RECEIVED_RESPONSE";
            break;
        default:
            break;
    }
    return "<Unknown>";
}

TEST(Database, Init) {
    DatabaseModule::createDataBaseModule(0);
}

TEST(Database, SendQuery) {
    std::mutex mutex;
    std::condition_variable cv;
    State state = State::INITIAL;
    size_t processed_rows = 0;
    std::string sql = "select value from qcril_properties_table";
    std::shared_ptr<DatabaseExecStatementMessage> execStmt = std::make_shared<DatabaseExecStatementMessage>(sql,
            [&cv, &mutex, &state, &processed_rows] (const QtiRilDbRow &row) -> bool {
                bool ret = false;
                std::lock_guard<std::mutex> lock(mutex);
                if (state == State::REQ_DISPATCHED ||
                        state == State::RECEIVED_ROW) {
                    state = State::RECEIVED_ROW;
                    processed_rows ++;
                    ret = true;
                    cv.notify_all();
                }
                return ret;
            },
            0
            );
    ASSERT_NE(execStmt,nullptr);

    GenericCallback<void> cb(
            [&cv, &mutex, &state ](std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<void> resp) -> void {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (state == State::REQ_DISPATCHED ||
                            state == State::RECEIVED_ROW ) {
                        state = State::RECEIVED_RESPONSE;
                        cv.notify_all();
                    }
            }
    );
    execStmt->setCallback(&cb);
    {
        std::unique_lock<std::mutex> lock(mutex);
        state = State::REQ_DISPATCHED;
        execStmt->dispatch();
        cv.wait_for(lock, 5s, [&state] () -> bool { return state == State::RECEIVED_RESPONSE; });
        ASSERT_GE(processed_rows, 1);
    }
}

std::shared_ptr<QtiRilPreparedStatement> stmt = nullptr;
std::mutex stmt_mutex;
std::condition_variable stmt_cv;

TEST(Database, Prepare) {

    bool done = false;
    std::shared_ptr<DatabasePrepareStatementMessage> msg =
        std::make_shared<DatabasePrepareStatementMessage>("select value from qcril_properties_table;");

    ASSERT_NE(msg, nullptr);

    {
        std::unique_lock<std::mutex> lock(stmt_mutex);
        msg->dispatch(
            [&done] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<QtiRilPreparedStatement> resp) -> void {
                std::unique_lock<std::mutex> lock(stmt_mutex);
                stmt = resp;
                done = true;
                stmt_cv.notify_all();
            }
        );
        stmt_cv.wait_for(lock, 5s, [&done] () -> bool { return done; });
        ASSERT_NE(stmt, nullptr);
    }
}

TEST(Database, ExecPrepared) {

    std::mutex count_mutex;
    std::condition_variable count_cv;
    size_t row_count  = 0;
    bool done = false;

    std::shared_ptr<DatabaseExecStatementMessage> msg =
        std::make_shared<DatabaseExecStatementMessage>(stmt,
                [&count_mutex, &row_count] (const QtiRilDbRow &row) -> bool {
                    if(row.getResult() == QtiRilDbRow::Result::OK) {
                        std::unique_lock<std::mutex> lock(count_mutex);
                        row_count ++;
                    }
                    return true;
                }
                );

    ASSERT_NE(msg, nullptr);

    {
        std::unique_lock<std::mutex> lock(count_mutex);
        msg->dispatch(
            [&count_mutex, &count_cv, &done] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<void> resp) -> void {
                std::unique_lock<std::mutex> lock(count_mutex);
                done = true;
                count_cv.notify_all();
            }
        );
        count_cv.wait_for(lock, 5s, [&done] () -> bool { return done; });
        ASSERT_GE(row_count, 1);
    }
}

TEST(Database, ExecPrepared2) {
    std::mutex count_mutex;
    std::condition_variable count_cv;
    size_t row_count  = 0;
    bool done = false;

    std::shared_ptr<DatabaseExecStatementMessage> msg =
        std::make_shared<DatabaseExecStatementMessage>(stmt,
                [&count_mutex, &row_count] (const QtiRilDbRow &row) -> bool {
                    if(row.getResult() == QtiRilDbRow::Result::OK) {
                        std::unique_lock<std::mutex> lock(count_mutex);
                        row_count ++;
                    }
                    return true;
                }
                );

    ASSERT_NE(msg, nullptr);

    GenericCallback<void> cb(
            [&count_mutex, &count_cv, &done] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<void> resp) -> void {
                std::unique_lock<std::mutex> lock(count_mutex);
                done = true;
                count_cv.notify_all();
            }
    );
    msg->setCallback(&cb);
    {
        std::unique_lock<std::mutex> lock(count_mutex);
        msg->dispatch();
        count_cv.wait_for(lock, 5s, [&done] () -> bool { return done; });
        ASSERT_GE(row_count, 1);
    }
}

TEST(Database, ExecPreparedArguments) {

    // Prepare statement
    std::shared_ptr<DatabasePrepareStatementMessage> msg =
        std::make_shared<DatabasePrepareStatementMessage>("select value from qcril_properties_table where property = ?;");
    std::shared_ptr<QtiRilPreparedStatement> stmt = nullptr;
    std::mutex stmt_mutex;
    bool done = false;
    std::condition_variable stmt_cv;

    ASSERT_NE(msg, nullptr);

    {
        std::unique_lock<std::mutex> lock(stmt_mutex);
        msg->dispatch(
            [&stmt, &stmt_mutex, &stmt_cv, &done] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<QtiRilPreparedStatement> resp) -> void {
                std::unique_lock<std::mutex> lock(stmt_mutex);
                stmt = resp;
                done = true;
                stmt_cv.notify_all();
            }
        );
        stmt_cv.wait_for(lock, 5s, [&done] () -> bool { return done; });
        ASSERT_NE(stmt, nullptr);
    }

    std::mutex count_mutex;
    std::condition_variable count_cv;
    size_t row_count  = 0;
    done = false;
    QtiRilParameterList list;
    QtiRilParameter::Type type;

    list.addValue("qcril-current-config");

    std::shared_ptr<DatabaseExecStatementMessage> execMsg =
        std::make_shared<DatabaseExecStatementMessage>(stmt, list,
                [&count_mutex, &row_count, &type] (const QtiRilDbRow &row) -> bool {
                    if(row.getResult() == QtiRilDbRow::Result::OK) {
                        std::unique_lock<std::mutex> lock(count_mutex);
                        QtiRilParameter p = row[0];
                        type = p.getType();
                        if (p.getType() == QtiRilParameter::Type::TEXT) {
                        }
                        row_count ++;
                    }
                    return true;
                }
                );

    ASSERT_NE(execMsg, nullptr);

    {
        std::unique_lock<std::mutex> lock(count_mutex);
        execMsg->dispatch(
            [&count_mutex, &count_cv, &done] (std::shared_ptr<Message> execMsg, Message::Callback::Status status,
                std::shared_ptr<void> resp) -> void {
                std::unique_lock<std::mutex> lock(count_mutex);
                done = true;
                count_cv.notify_all();
            }
                );
        count_cv.wait_for(lock, 5s, [&done] () -> bool { return done; });
        ASSERT_EQ(row_count, 1);
        ASSERT_EQ(type, QtiRilParameter::Type::TEXT);
    }
}
