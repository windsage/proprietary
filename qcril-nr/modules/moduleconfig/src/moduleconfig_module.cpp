/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ-MODULECNF"
#include <framework/Module.h>
#include <modules/db/DatabasePrepareStatementMessage.h>
#include <modules/db/DatabaseExecStatementMessage.h>
#include <qtiril-loadable/QtiRilLoadable.h>
#include <interfaces/module_config/ModuleConfig.h>
#include <QtiMutex.h>
#include <framework/QcrilInitMessage.h>
#include <mutex>

using namespace ModuleConfig;
class moduleconfig_module: public Module
{
    private:
        QtiSharedMutex statementMutex;
        std::shared_ptr<QtiRilPreparedStatement> queryCurrentConfig;
        std::shared_ptr<QtiRilPreparedStatement> queryGetAllModulesList;
        std::shared_ptr<QtiRilPreparedStatement> queryGetModuleList;
        std::shared_ptr<QtiRilPreparedStatement> queryAvailableConfigs;
        std::shared_ptr<QtiRilPreparedStatement> queryDeleteCustomConfigModules;
        std::shared_ptr<QtiRilPreparedStatement> querySetCustomConfigModules;
        std::shared_ptr<QtiRilPreparedStatement> querySetCurrentConfig;

        void prepareStatement(std::string sql, std::shared_ptr<QtiRilPreparedStatement> &output);
    public:
        void handleGetModuleListMessage(std::shared_ptr<GetModuleListMessage> msg);
        void handleGetConfigListMessage(std::shared_ptr<GetConfigListMessage> msg);
        void handleGetCurrentConfigMessage(std::shared_ptr<GetCurrentConfigMessage> msg);
        void handleSetCurrentConfigMessage(std::shared_ptr<SetCurrentConfigMessage> msg);
        void handleSetCustomConfigModules(std::shared_ptr<SetCustomConfigModules> msg);
        void handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg);
        moduleconfig_module();
};



moduleconfig_module::moduleconfig_module()
{
    mName = "moduleconfig_module";
    using std::placeholders::_1;

    mMessageHandler = {
        HANDLER(GetModuleListMessage, moduleconfig_module::handleGetModuleListMessage),
        HANDLER(GetConfigListMessage, moduleconfig_module::handleGetConfigListMessage),
        HANDLER(GetCurrentConfigMessage, moduleconfig_module::handleGetCurrentConfigMessage),
        HANDLER(SetCurrentConfigMessage, moduleconfig_module::handleSetCurrentConfigMessage),
        HANDLER(SetCustomConfigModules, moduleconfig_module::handleSetCustomConfigModules),
        HANDLER(QcrilInitMessage, moduleconfig_module::handleQcrilInitMessage),
    };
}

void moduleconfig_module::prepareStatement(std::string sql, std::shared_ptr<QtiRilPreparedStatement> &output) {
    auto prepMsg = std::make_shared<DatabasePrepareStatementMessage>(sql);
    if (prepMsg) {
        prepMsg->dispatch(
                [this, &output] (std::shared_ptr<Message>,
                    Message::Callback::Status status,
                    std::shared_ptr<QtiRilPreparedStatement> response
                    ) {
                    if (status == Message::Callback::Status::SUCCESS && response) {
                        std::unique_lock<QtiSharedMutex> lock(statementMutex);
                        output = response;
                    } else {
                        QCRIL_LOG_ERROR("Unable to prepare statement");
                    }
                }
        );
    }
}
void moduleconfig_module::handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg) {
   QCRIL_LOG_INFO("Preparing CurrentConfig statement");
   prepareStatement(
            "SELECT c.rowid, c.name, c.description "
            "FROM configs c INNER JOIN "
            "( SELECT value FROM qcril_properties_table "
            "   WHERE property = 'qcril-current-config' ) AS p "
            "ON c.rowid = p.value;",
            queryCurrentConfig
    );

   QCRIL_LOG_INFO("Preparing GetAllModulesList statement");
    prepareStatement(
            "SELECT '-', m.module, m.mandatory, m.internal, m.prio "
            "FROM modules m;",
            queryGetAllModulesList
    );

    QCRIL_LOG_INFO("Preparing GetModuleList statement");
    prepareStatement(
            "SELECT c.name, m.module, m.mandatory, m.internal, m.prio "
            "FROM modules m "
            "INNER JOIN config_modules cm ON m.rowid = cm.module_id "
            "INNER JOIN configs c ON cm.config_id = c.rowid "
            "WHERE c.rowid IN ( "
            "    SELECT rowid "
            "    FROM configs "
            "    WHERE configs.name = ? "
            "    );",
            queryGetModuleList
    );

    QCRIL_LOG_INFO("Preparing AvailableConfigs statement");
    prepareStatement(
            "SELECT c.name "
            "FROM configs c;",
            queryAvailableConfigs
            );

    QCRIL_LOG_INFO("Preparing DeleteConfigModules statement");
    prepareStatement(
            "DELETE from config_modules "
            "WHERE config_id IN ( "
            "    SELECT rowid "
            "    FROM configs "
            "    WHERE configs.name = 'Custom' "
            "    );",
            queryDeleteCustomConfigModules
    );

    QCRIL_LOG_INFO("Preparing SetCustomConfigModules statement");
    prepareStatement(
            "INSERT INTO config_modules (config_id, module_id) "
            "SELECT c.rowid, m.rowid from configs c, modules m WHERE c.name = 'Custom' AND m.module = ?;",
            querySetCustomConfigModules
    );

    QCRIL_LOG_INFO("Preparing SetCurrentConfig statement");
    prepareStatement(
            "INSERT OR REPLACE INTO qcril_properties_table (property,value) "
            "SELECT 'qcril-current-config', c.rowid "
            "FROM configs c WHERE c.name = ?; ",
            querySetCurrentConfig
    );
}
void moduleconfig_module::handleGetModuleListMessage(std::shared_ptr<GetModuleListMessage> request)
{

    QCRIL_LOG_FUNC_ENTRY();
    if(!request) {
        QCRIL_LOG_ERROR("Received a null request");
        return;
    }

    QtiRilParameterList parameterList;

    std::shared_ptr<QtiRilPreparedStatement> query;
    std::string configName = "All";
    if (request->hasConfigName() && queryGetModuleList) {
        configName = request->getConfigName();
        query = queryGetModuleList;
        parameterList.addValue(configName);
    }
    if (!query) {
        query = queryGetAllModulesList;
    }
    if (!query) {
        QCRIL_LOG_ERROR("No valid prepared query!");
        request->sendResponse(request, Message::Callback::Status::FAILURE, nullptr);
        return;
    }
    auto aggregator = std::make_shared<ModuleConfig::ModuleList>(configName);
    if (!aggregator) {
        QCRIL_LOG_ERROR("Unable to allocate aggregator!");
        request->sendResponse(request, Message::Callback::Status::FAILURE, nullptr);
        return;
    }
    auto queryMsg = std::make_shared<DatabaseExecStatementMessage>(
            query,
            parameterList,
            [request, aggregator] (const QtiRilDbRow &row) -> bool {
                if (row.getResult() == QtiRilDbRow::Result::OK) {
                    aggregator->emplace_back(row[1].get<std::string>());
                }
                return true;
            }
    );

    if (queryMsg) {
        queryMsg->dispatch(
                [aggregator, request] (std::shared_ptr<Message> msg,
                    Message::Callback::Status status,
                    std::shared_ptr<void>
                   ) {
                    request->sendResponse(request,
                            status,
                            aggregator);
                }
        );
    }
    QCRIL_LOG_FUNC_RETURN();
}

void moduleconfig_module::handleGetConfigListMessage(std::shared_ptr<GetConfigListMessage> request)
{

    QCRIL_LOG_FUNC_ENTRY();
    if(!request) {
        QCRIL_LOG_ERROR("Received a null request");
        return;
    }


    std::shared_ptr<QtiRilPreparedStatement> query;
    query = queryAvailableConfigs;
    if (!query) {
        QCRIL_LOG_ERROR("No valid prepared query!");
        request->sendResponse(request, Message::Callback::Status::FAILURE, nullptr);
        return;
    }
    auto aggregator = std::make_shared<std::vector<std::string>>();
    auto queryMsg = std::make_shared<DatabaseExecStatementMessage>(
            query,
            [request, aggregator] (const QtiRilDbRow &row) -> bool {
                if (row.getResult() == QtiRilDbRow::Result::OK) {
                    if (aggregator) {
                        aggregator->emplace_back(row[0].get<std::string>());
                    }
                }
                return true;
            }
    );

    if (queryMsg) {
        queryMsg->dispatch(
                [aggregator, request] (std::shared_ptr<Message> msg,
                    Message::Callback::Status status,
                    std::shared_ptr<void>
                   ) {
                    request->sendResponse(request,
                            status,
                            aggregator);
                }
        );
    }
    QCRIL_LOG_FUNC_RETURN();
}

void moduleconfig_module::handleGetCurrentConfigMessage(std::shared_ptr<GetCurrentConfigMessage> request)
{

    QCRIL_LOG_FUNC_ENTRY();

    if(!request) {
        QCRIL_LOG_ERROR("Null message received!");
        return;
    }

    std::shared_ptr<QtiRilPreparedStatement> query = queryCurrentConfig;

    if (!query) {
        QCRIL_LOG_ERROR("No valid prepared query!");
        request->sendResponse(request, Message::Callback::Status::FAILURE, nullptr);
        return;
    }

    using Aggregator = std::optional<std::pair<int, std::string>>;
    auto aggregator = std::make_shared<Aggregator>();
    auto queryMsg = std::make_shared<DatabaseExecStatementMessage>(
            query,
            [aggregator] (const QtiRilDbRow &row) -> bool {
                if (row.getResult() == QtiRilDbRow::Result::OK) {
                    if (aggregator && !aggregator->has_value()) {
                        *aggregator = std::pair(row[0].get<int64_t>(), row[1].get<std::string>());
                    }
                }
                return true;
            }
    );

    if (queryMsg) {
        queryMsg->dispatch(
                [request, aggregator] (std::shared_ptr<Message>,
                    Message::Callback::Status status,
                    std::shared_ptr<void> response
                   ) {
                    if (status == Message::Callback::Status::SUCCESS && aggregator && *aggregator) {
                        auto [id, name] = *(*aggregator);
                        auto response = std::make_shared<std::string>(name);
                        request->sendResponse(request, Message::Callback::Status::SUCCESS, response);
                    }
                }
        );
    }
    QCRIL_LOG_FUNC_RETURN();
}
void moduleconfig_module::handleSetCustomConfigModules(std::shared_ptr<SetCustomConfigModules> request)
{
    QCRIL_LOG_FUNC_ENTRY();
    if (!request) {
        QCRIL_LOG_ERROR("Null message received!!");
        return;
    }
    auto delQuery = queryDeleteCustomConfigModules;
    auto query = querySetCustomConfigModules;

    if (!delQuery || !query) {
        QCRIL_LOG_ERROR("No valid prepared query!");
        request->sendResponse(request, Message::Callback::Status::FAILURE, nullptr);
        return;
    }

    auto deleteRequest = std::make_shared<DatabaseExecStatementMessage>(
            delQuery,
            [] ( const QtiRilDbRow &row) -> bool { return true; }

    );
    if (deleteRequest) {
        deleteRequest->dispatch(
                [this, request] (std::shared_ptr<Message>,
                    Message::Callback::Status status,
                    std::shared_ptr<void> response
                   ) {
                    if (status == Message::Callback::Status::SUCCESS) {
                        std::shared_ptr<QtiRilPreparedStatement> query =
                            querySetCustomConfigModules;
                        std::vector<QtiRilParameterList> parameterLists{};

                        for(std::string module: *request) {
                            QtiRilParameterList &parameterList = parameterLists.emplace_back();
                            parameterList.addValue(module);
                        }
                        auto insertRequest = std::make_shared<DatabaseExecStatementMessage>(
                                query,
                                parameterLists,
                                [] (const QtiRilDbRow &row) -> bool { return true; }
                        );
                        if (insertRequest) {
                            insertRequest->dispatch(
                                    [request] (std::shared_ptr<Message>,
                                        Message::Callback::Status status,
                                        std::shared_ptr<void> response
                                       ) {
                                        request->sendResponse(
                                                request,
                                                status,
                                                nullptr);
                                        }
                            );
                        }
                    } else {
                        request->sendResponse(
                                request,
                                Message::Callback::Status::FAILURE,
                                nullptr);
                    }
                }
        );
    }


    QCRIL_LOG_FUNC_RETURN();
}

void moduleconfig_module::handleSetCurrentConfigMessage(std::shared_ptr<SetCurrentConfigMessage> request)
{
    QCRIL_LOG_FUNC_ENTRY();

    if (!request) {
        QCRIL_LOG_ERROR("Null message received!!");
        return;
    }

    auto query = querySetCurrentConfig;

    QtiRilParameterList paramList;
    paramList.addValue(request->getConfig());
    auto setMsg = std::make_shared<DatabaseExecStatementMessage>(
            query,
            paramList,
            [] ( const QtiRilDbRow &row) -> bool { return true; }
    );

    if ( setMsg ) {
        setMsg->dispatch(
                [request] (std::shared_ptr<Message> msg,
                    Message::Callback::Status status,
                    std::shared_ptr<void>
                    ) {
                    request->sendResponse(request,
                            status,
                            nullptr);
                }
        );
    }
    QCRIL_LOG_FUNC_RETURN();
}

#if defined(QTIRIL_STATIC_MODULES) || defined(QMI_RIL_UTF)
static load_module<moduleconfig_module> the_module;
#else
static struct LoadableHeader *&getHeader() {
    static struct LoadableHeader *theHeader = nullptr;
    return theHeader;
}

static QtiSharedMutex &getHeaderMutex() {
    static QtiSharedMutex theHeaderMutex;
    return theHeaderMutex;
}

extern "C"
struct LoadableHeader *loadable_init() {
    std::unique_lock<QtiSharedMutex> lock(getHeaderMutex());
    if (getHeader() == nullptr) {
        std::shared_ptr<moduleconfig_module> theModule = std::make_shared<moduleconfig_module>();
        if (theModule) {
            theModule->init();
        }
        struct LoadableHeader *header = new LoadableHeader{"moduleconfig_module", std::static_pointer_cast<Module>(theModule)};
        getHeader() = header;
    }
    return getHeader();
}

extern "C"
int loadable_deinit() {
    std::unique_lock<QtiSharedMutex> lock(getHeaderMutex());
    if (getHeader() != nullptr) {
        delete getHeader();
        getHeader() = nullptr;
    }
    return 0;
}
#endif
