/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "DatabaseModule"
#include "DatabaseModule.h"
#include "qcril_db.h"
#include "DbState.h"
#include "framework/Log.h"
#include <modules/db/DatabasePrepareStatementMessage.h>
#include <modules/db/DatabaseExecStatementMessage.h>
#include <sqlite3.h>
#include <unordered_map>
#include <memory>
#include <utility>
#include <queue>


template <typename T>
struct sqlite3_column_traits {
};
template <> struct sqlite3_column_traits<double> {
    static constexpr auto get_function = sqlite3_column_double;
    static constexpr auto bind_function = sqlite3_bind_double;
    static const bool has_length = false;
};
template <> struct sqlite3_column_traits<int> {
    static constexpr auto get_function = sqlite3_column_int;
    static constexpr auto bind_function = sqlite3_bind_int;
    static const bool has_length = false;
};
template <> struct sqlite3_column_traits<int64_t> {
    static constexpr auto get_function = sqlite3_column_int64;
    static constexpr auto bind_function = sqlite3_bind_int64;
    static const bool has_length = false;
};
template <> struct sqlite3_column_traits<const void *> {
    static constexpr auto get_function = sqlite3_column_blob;
    static constexpr auto bind_function = sqlite3_bind_blob;
    static const bool has_length = true;
    static constexpr auto length_fn = sqlite3_column_bytes;
};
template <> struct sqlite3_column_traits<std::basic_string<uint8_t>> {
    static constexpr auto get_function = sqlite3_column_blob;
    static constexpr auto bind_function = sqlite3_bind_blob;
    static const bool has_length = true;
    static constexpr auto length_fn = sqlite3_column_bytes;
};
template <> struct sqlite3_column_traits<std::string> {
    static constexpr auto get_function = sqlite3_column_text;
    static constexpr auto bind_function = sqlite3_bind_text;
    static const bool has_length = true;
    static constexpr auto length_fn = sqlite3_column_bytes;
};
template <> struct sqlite3_column_traits<unsigned char *> {
    static constexpr auto get_function = sqlite3_column_text;
    static constexpr auto bind_function = sqlite3_bind_text;
    static const bool has_length = true;
    static constexpr auto length_fn = sqlite3_column_bytes;
};
template <> struct sqlite3_column_traits<char16_t *> {
    static constexpr auto get_function = sqlite3_column_text16;
    static constexpr auto bind_function = sqlite3_bind_text16;
    static const bool has_length = true;
    static constexpr auto length_fn = sqlite3_column_bytes16;
};

class QtiRilDbRowImpl: public QtiRilDbRow {
    public:
        QtiRilDbRowImpl(std::shared_ptr<QtiRilPreparedStatement> stmt):
                    QtiRilDbRow(),
                    stmt(stmt) { }
        QtiRilDbRow::Result getResult() const;

        QtiRilParameter get(size_t column) const;
        size_t getColumnCount() const;
        const std::string &errorMessage() const;
    private:
        std::shared_ptr<QtiRilPreparedStatement> stmt;
        std::string errmsg;
};

class QtiRilPreparedStatement: public std::enable_shared_from_this<QtiRilPreparedStatement> {
    public:
        using StmtType = sqlite3_stmt *;

        std::string sql;
        StmtType stmt;
        bool persist;
        bool owns_stmt;
        int result;
        std::string errorMsg;

        ~QtiRilPreparedStatement() { finalize(); }

        QtiRilPreparedStatement(): sql(), stmt(nullptr), persist(false), owns_stmt(false) {}

        QtiRilPreparedStatement(std::string sql, bool persist = true):
            sql(sql),
            stmt(nullptr),
            persist(persist),
            owns_stmt(false) { prepare(); }

        QtiRilPreparedStatement(const std::string &sql,
                sqlite3_stmt *stmt,
                bool persist = false,
                bool owns_stmt = true):
            sql(sql),
            stmt(stmt),
            persist(persist),
            owns_stmt(owns_stmt) {}

        QtiRilPreparedStatement(const QtiRilPreparedStatement &o):
            sql(o.sql),
            stmt(nullptr),
            persist(o.persist),
            owns_stmt(true)
        {
            prepare();
        }

        QtiRilPreparedStatement(QtiRilPreparedStatement &&o):
            sql(std::move(o.sql)),
            stmt(std::move(o.stmt)),
            persist(std::move(o.persist)),
            owns_stmt(std::move(o.owns_stmt))
        {}

        QtiRilPreparedStatement &operator= (const QtiRilPreparedStatement &o) {
            finalize();
            sql = o.sql;
            stmt = nullptr;
            persist = o.persist;
            owns_stmt = o.owns_stmt;
            prepare();
            return *this;
        }

        QtiRilPreparedStatement &operator= (QtiRilPreparedStatement &&o) {
            finalize();
            std::swap(sql, o.sql);
            std::swap(stmt, o.stmt);
            std::swap(persist, o.persist);
            std::swap(owns_stmt, o.owns_stmt);
            return *this;
        }


        QtiRilDbRow::Result convertResult(int code) {
            switch(code) {
                case SQLITE_OK:
                case SQLITE_ROW:
                    return QtiRilDbRow::Result::OK;
                    break;
                case SQLITE_DONE:
                    return QtiRilDbRow::Result::DONE;
                case SQLITE_BUSY:
                    return QtiRilDbRow::Result::BUSY;
                default:
                    return QtiRilDbRow::Result::ERROR;
            }
        }

        QtiRilDbRow::Result getResult() {
            return convertResult(result);
        }

        QtiRilDbRow::Result reset() {
            return convertResult(sqlite3_reset(stmt));
        }

        bool hasMore() {
            return (result == SQLITE_ROW || result == SQLITE_BUSY);
        }

        template <typename T>
        T getValue(size_t column) const {
            T ret ( sqlite3_column_traits<T>::get_function(stmt, column) );
            return ret;
        };

        template <typename T>
        int getLength(size_t column) const {
            static_assert(sqlite3_column_traits<T>::has_length, "sqlite3_column_traits::length_fn not defined for this type");
            size_t res = 0;
            if (stmt) {
                res = sqlite3_column_traits<T>::length_fn(stmt, column);
            }
            return res;
        }

        template <typename T>
        int bindValue(const T& value, size_t column);

        int bindNullValue(size_t column) {
            if (stmt) {
                return sqlite3_bind_null(stmt, column);
            }
            return SQLITE_ERROR;
        }

        template <QtiRilParameter::Type Tid>
        void setParameter(const QtiRilParameter& param, size_t column) {
            using Type = typename MapType<Tid>::type;
            int rc = bindValue(param.get<Type>(), column);
            if (rc != SQLITE_OK) {
                QCRIL_LOG_ERROR("Unable to bind value for column %zu: %s", rc, qcril_db_errmsg());
            }
        }
        void setParameterList(const QtiRilParameterList &parameterList);


        QtiRilParameter get(size_t column) const;

        void updateErrorMsg() {
            std::string msg{};
            if (stmt) {
                const char *tmp = qcril_db_errmsg();
                if (tmp) {
                    msg += tmp;
                }
            }
            errorMsg = msg;
        }
        const std::string &errorMessage() const {
            return errorMsg;
        }

        size_t getColumnCount() {
            size_t ret = 0;
            if (stmt) {
                ret = sqlite3_column_count(stmt);
            }
            return ret;
        }

        int step() {
            result = SQLITE_ERROR;
            if (stmt) {
                result = sqlite3_step(stmt);
            }
            return result;
        }
        bool isValid() {
            return stmt != nullptr;
        }

    private:
        void prepare() {
            if (qcril_db_prepare_stmt(sql, persist, stmt) == 0 && stmt != nullptr) {
                owns_stmt = true;
            }
        }
        void finalize() {
            if (owns_stmt) {
                qcril_db_finalize_stmt(stmt);
            }
            stmt = nullptr;
        }
};

template <>
void QtiRilPreparedStatement::setParameter<QtiRilParameter::Type::NULLT>(const QtiRilParameter& , size_t column) {
    int rc = bindNullValue(column);
    if (rc != 0) {
        QCRIL_LOG_ERROR("Error while binding null value: %d", rc);
    }
}
void QtiRilPreparedStatement::setParameterList(const QtiRilParameterList &parameterList) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    size_t column = 0;
    for (auto &param: parameterList) {
        column ++;
        switch(param.getType()) {
            case QtiRilParameter::Type::BLOB:
                setParameter<QtiRilParameter::Type::BLOB>(param, column);
                break;
            case QtiRilParameter::Type::DOUBLE:
                setParameter<QtiRilParameter::Type::DOUBLE>(param, column);
                break;
            case QtiRilParameter::Type::INT:
                setParameter<QtiRilParameter::Type::INT>(param, column);
                break;
            case QtiRilParameter::Type::INT64:
                setParameter<QtiRilParameter::Type::INT64>(param, column);
                break;
            case QtiRilParameter::Type::NULLT:
                setParameter<QtiRilParameter::Type::NULLT>(param, column);
                break;
            case QtiRilParameter::Type::TEXT:
                setParameter<QtiRilParameter::Type::TEXT>(param, column);
                break;
            case QtiRilParameter::Type::TEXT16:
                setParameter<QtiRilParameter::Type::TEXT16>(param, column);
                break;
            default:
                QCRIL_LOG_ERROR("Unsupported type");
                break;
        }
    }
}
template <typename T>
int QtiRilPreparedStatement::bindValue(const T& value, size_t column) {
            int rc = sqlite3_column_traits<T>::bind_function(stmt, column, value);
            return rc;
}
template <>
int QtiRilPreparedStatement::bindValue<std::string>(const std::string &value, size_t column) {
    return sqlite3_column_traits<std::string>::bind_function(stmt,
            column,
            value.c_str(),
            sizeof(typename std::string::value_type) * value.length(),
            nullptr);
}

template <>
int QtiRilPreparedStatement::bindValue<std::basic_string<uint8_t>>(const std::basic_string<uint8_t> &value, size_t column) {
    return sqlite3_column_traits<std::basic_string<uint8_t>>::bind_function(stmt,
            column,
            value.c_str(),
            sizeof(typename std::basic_string<uint8_t>::value_type) * value.length(),
            nullptr);
}

DatabaseModule& DatabaseModule::getInstance() {
    static DatabaseModule sInstance;
    return sInstance;
}

void DatabaseModule::createDataBaseModule(int id)
{
    getInstance().setDatabaseInstanceId(id);
    getInstance().init();
}


/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
DatabaseModule::DatabaseModule() {
  mName = "DatabaseModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(DatabaseUpdateNotifyMessage, DatabaseModule::handleDatabaseUpdateNotification),
    HANDLER(DatabasePrepareStatementMessage, DatabaseModule::handleDatabasePrepareStatement),
    HANDLER(DatabaseExecStatementMessage, DatabaseModule::handleDatabaseExecStatementMessage),
  };
}

/* Follow RAII.
*/
DatabaseModule::~DatabaseModule() {}

/*
 * Module specific initialization that does not belong to RAII .
 */
void DatabaseModule::init() {
  Module::init();

  (void)DbState::createDbState(mId == 0);
  qcril_db_init(mId);
  QCRIL_LOG_INFO("Preparing transaction statements");
  sqlite3_stmt *begin_stmt = nullptr;
  sqlite3_stmt *commit_stmt = nullptr;
  sqlite3_stmt *rollback_stmt = nullptr;

  const char *begin_sql = "BEGIN;";
  int res = qcril_db_prepare_stmt(begin_sql, true, begin_stmt);
  if (res != SQLITE_OK) {
      QCRIL_LOG_ERROR("Unable to prepare begin statement");
      return;
  }

  const char *commit_sql = "COMMIT;";
  res = qcril_db_prepare_stmt(commit_sql, true, commit_stmt);
  if (res != SQLITE_OK) {
      QCRIL_LOG_ERROR("Unable to prepare commit statement");
      qcril_db_finalize_stmt(begin_stmt);
      return;
  }

  const char *rollback_sql = "ROLLBACK;";
  res = qcril_db_prepare_stmt(rollback_sql, true, rollback_stmt);
  if (res != SQLITE_OK) {
      QCRIL_LOG_ERROR("Unable to prepare rollback statement");
      qcril_db_finalize_stmt(begin_stmt);
      qcril_db_finalize_stmt(commit_stmt);
      return;
  }

  beginStmt = std::make_shared<QtiRilPreparedStatement>(begin_sql, begin_stmt, true);
  commitStmt = std::make_shared<QtiRilPreparedStatement>(commit_sql, commit_stmt, true);
  rollbackStmt = std::make_shared<QtiRilPreparedStatement>(commit_sql, rollback_stmt, true);
}

void DatabaseModule::setDatabaseInstanceId(int id) {
  mId = id;
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void DatabaseModule::handleDatabaseUpdateNotification(std::shared_ptr<DatabaseUpdateNotifyMessage> msg) {

    Log::getInstance().d("[DatabaseModule]: Handling msg = " + msg->dump());
}

void DatabaseModule::handleDatabasePrepareStatement(std::shared_ptr<DatabasePrepareStatementMessage> msg) {
    if (msg) {
        sqlite3_stmt *out = nullptr;
        std::shared_ptr<QtiRilPreparedStatement> response = nullptr;
        int res = qcril_db_prepare_stmt(msg->getSql(), true, out);
        if (res == SQLITE_OK) {
            response = std::make_shared<QtiRilPreparedStatement>(msg->getSql(), out, true);
        }
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, response);
    }
}
static inline int doExecStatementMessage(
        std::shared_ptr<DatabaseExecStatementMessage> msg,
        std::shared_ptr<QtiRilPreparedStatement> stmt,
        const std::optional<QtiRilParameterList> &parameterList = std::nullopt) {
    if (parameterList) {
        stmt->setParameterList(*parameterList);
    }
    DatabaseExecStatementMessage::RowCb cb;
    stmt->reset();
    int rc = stmt->step();
    QtiRilDbRowImpl row(stmt);
    if (msg) {
        cb = msg->getRowCb();
    }
    bool cont = true;
    while (rc == SQLITE_ROW && cont) {
        if (cb) {
            cont = cb(row);
        }
        if (cont) {
            rc = stmt->step();
        }
    }
    if (!cont) {
        rc = SQLITE_DONE;
    }
    return rc;
}
void DatabaseModule::handleDatabaseExecStatementMessage(std::shared_ptr<DatabaseExecStatementMessage> msg) {
    if (msg) {
        std::shared_ptr<QtiRilPreparedStatement> stmt = msg->getPreparedStatement();
        if (!stmt) {
            stmt = std::make_shared<QtiRilPreparedStatement>( msg->getSql(), false);
            if (!stmt) {
                QCRIL_LOG_ERROR("Unable to allocate prepared statement for query: %s", msg->getSql().c_str());
                return;
            }
        }
        auto parameterLists = msg->getParameterLists();
        if (parameterLists ) {
            doExecStatementMessage(msg, beginStmt);
            int rc = 0;
            for(auto &parameterList: *parameterLists) {
                rc = doExecStatementMessage(msg, stmt, parameterList);
                if (rc != SQLITE_OK && rc != SQLITE_DONE) {
                    break;
                }
            }
            if (rc == SQLITE_OK || rc == SQLITE_DONE) {
                doExecStatementMessage(msg, commitStmt);
            } else {
                doExecStatementMessage(msg, rollbackStmt);
            }
            stmt->reset();
        } else {
            doExecStatementMessage(msg, stmt);
        }
        std::shared_ptr<void> rsp = nullptr;
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, rsp);
    }
}

QtiRilDbRow::Result QtiRilDbRowImpl::getResult() const {
    QtiRilDbRow::Result result = QtiRilDbRow::Result::UNKNOWN;
    if (stmt) {
        result = stmt->getResult();
    }
    return result;
}

const std::string &QtiRilDbRowImpl::errorMessage() const {
    static std::string ret{};
    if (stmt) {
        return stmt->errorMessage();
    }
    return ret;
};

QtiRilParameter QtiRilDbRowImpl::get(size_t column) const {
    QtiRilParameter ret(nullptr);
    QCRIL_LOG_DEBUG("Initialized ret to nullptr");
    if (stmt) {
        QCRIL_LOG_DEBUG("stmt is not null");
        return stmt->get(column);
    }
    QCRIL_LOG_ERROR("Returning a null parameter");
    return ret;
}

size_t QtiRilDbRowImpl::getColumnCount() const {
    size_t ret = 0;
    if (stmt) {
        ret = stmt->getColumnCount();
    }
    return ret;
}

template <>
std::string QtiRilPreparedStatement::getValue<std::string>(size_t column) const {
    std::string ret ( reinterpret_cast<const char *>(
                sqlite3_column_traits<std::string>::get_function(stmt, column)));
    return ret;
}

QtiRilParameter QtiRilPreparedStatement::get(size_t column) const {
    QtiRilParameter ret (nullptr);
    int sqltype = sqlite3_column_type(stmt, column);
    switch (sqltype) {
        case SQLITE_INTEGER:
            {
                int64_t value = getValue<int64_t>(column);
                return QtiRilParameter( value );
            }
            break;
        case SQLITE_FLOAT:
            {
                double value = getValue<double>(column);
                return QtiRilParameter( value );
            }
            break;
        case SQLITE_TEXT:
            {
                std::string value = getValue<std::string>(column);
                return QtiRilParameter( value );
            }
            break;
        case SQLITE_BLOB:
            {
                int tlen = getLength<const void *>(column);
                const uint8_t *data = static_cast<const uint8_t*>(
                        getValue<const void *>(column));
                size_t len = 0;
                if (tlen >= 0) {
                    len = (size_t) len;
                }
                std::basic_string<uint8_t> value {
                    data ? data : reinterpret_cast<const uint8_t *>(""),
                        len};
                return QtiRilParameter( value );
            }
            break;
    }
    return ret;
}
