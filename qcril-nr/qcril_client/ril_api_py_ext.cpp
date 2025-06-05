/******************************************************************************
#  Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <Marshal.h>
#include <RilApiSession.hpp>
#include <string.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <filesystem>

static PyObject* translate(const RIL_UUS_Info* uusInfo);

static PyObject* translate(const RIL_GW_SignalStrength& signalStrength);
static PyObject* translate(const RIL_WCDMA_SignalStrength& signalStrength);
static PyObject* translate(const RIL_CDMA_SignalStrength& signalStrength);
static PyObject* translate(const RIL_EVDO_SignalStrength& signalStrength);
static PyObject* translate(const RIL_LTE_SignalStrength_v8& signalStrength);
static PyObject* translate(const RIL_TD_SCDMA_SignalStrength& signalStrength);
static PyObject* translate(const RIL_NR_SignalStrength& signalStrength);

static PyObject* translate(const RIL_CellIdentityGsm_v12& cellIdentity);
static PyObject* translate(const RIL_CellIdentityWcdma_v12& cellIdentity);
static PyObject* translate(const RIL_CellIdentityLte_v12& cellIdentity);
static PyObject* translate(const RIL_CellIdentityTdscdma& cellIdentity);
static PyObject* translate(const RIL_CellIdentityCdma& cellIdentity);
static PyObject* translate(const RIL_CellIdentityNr& cellIdentity);
static PyObject* translate(const RIL_CellIdentity_v16& cellIdentity);

static PyObject* RilApiError;
static RilApiSession rilSession("127.0.0.1", 50000);
std::fstream ofs;
std::fstream ifs; // For writing i/p data

std::string saveValuesToTempLocation()
{
    CHAR czTempPath[MAX_PATH] = {0};
    GetTempPathA(MAX_PATH, czTempPath);
    std::string sPath = czTempPath;
    sPath += "values.txt";
    return sPath;
}

std::string saveInputValuesToTempLocation()
{
    CHAR czTempPath[MAX_PATH] = {0};
    GetTempPathA(MAX_PATH, czTempPath);
    std::string sPath = czTempPath;
    sPath += "input.txt";
    return sPath;
}

/*---------------------< RIL AFL Testcase Generation APIs >--------------*/
#ifdef QCRIL_BUILD_AFL_TESTCASE_GEN
static PyObject* setTestcaseOutputDir(PyObject *self, PyObject* args) {
  char *path;

  if (!PyArg_ParseTuple(args, "s", &path)) {
    return NULL;
  }
  std::string path_str(path);
  Status s = rilSession.setTestcaseOutputDir(path);
  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "Failed to set testcase output directory.");
    return NULL;
  }

  return PyLong_FromLong(static_cast<long>(s));

}

static PyObject* getTestcaseOutputDir(PyObject* self, PyObject* args) {
  std::string path = rilSession.getTestcaseOutputDir();
  return PyBytes_FromString(path.c_str());

}

static PyObject* setSaveTestcases(PyObject* self, PyObject* args) {
  int saveTestcases;
  if (!PyArg_ParseTuple(args, "p", &saveTestcases)) {
    PyErr_SetString(RilApiError, "Failed to parse saveTestcases arg.");
    return NULL;
  }

  Status s = rilSession.setSaveTestcases((bool)saveTestcases);
  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "Failed to  enable/disable testcase generation.");
    return NULL;
  }

  return PyLong_FromLong(static_cast<long>(s));

}

static PyObject* getSaveTestcases(PyObject* self, PyObject* args) {
  bool saveTestcases = rilSession.getSaveTestcases();
  return PyBool_FromLong((long)saveTestcases);

}
#endif // QCRIL_BUILD_AFL_TESTCASE_GEN

// Base case of below recursive variadic function Template
void printInput()
{
  ifs << std::endl;
  ifs.close();
}

// Variadic function template that takes variable number of arguments and prints all of them.
template <typename T, typename... Types>
void printInput(T var1, Types... var2)
{
  if (!ifs.is_open()) {
    ifs.open(saveInputValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  }

  ifs << var1 << std::endl;
  printInput(var2...);  // Recursive Call to Print the Next input
}

/*---------------------< RIL Request for Set Preferred Network type Request >--------------*/
static PyObject* setPreferredNetworkType(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int nwType;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &nwType, &callback)) {
        return NULL;
    }
    printInput("setPreferredNetworkType", nwType);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.setPreferredNetworkType(nwType,[callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Set preferred Network Type: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "RIL request for set Preferred Network Type");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Successfully sent set preferred network type" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*---------------------< RIL Request for Set Facility Lock Request >--------------*/
static PyObject* simSetFacilityLockReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    const char* set_fci_lock[5] ={};
    const char* facility;
    const char* lock_status;
    const char* password;
    const char* service_class;
    const char* aid;
    std::vector<std::string> userInput;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sssssO", &facility, &lock_status, &password, &service_class, &aid, &callback)) {
        return NULL;
    }

    printInput("simSetFacilityLockReq", facility, lock_status, password, service_class, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    set_fci_lock[0] = const_cast<char*>(facility);
    set_fci_lock[1] = const_cast<char*>(lock_status);
    set_fci_lock[2] = const_cast<char*>(password);
    set_fci_lock[3] = const_cast<char*>(service_class);
    set_fci_lock[4] = const_cast<char*>(aid);

    userInput.push_back(facility);
    userInput.push_back(lock_status);
    userInput.push_back(password);
    userInput.push_back(service_class);
    userInput.push_back(aid);

    Status s = rilSession.simSetFacilityLockReq(set_fci_lock,
        [callback] (RIL_Errno e, int32_t* pNumRetries) -> void {
            int32_t numRetries = -1;
            if (pNumRetries != nullptr) {
                numRetries = *pNumRetries;
            }

            PyObject* cbArgs = Py_BuildValue("(ii)", e, numRetries);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);

            std::cout << "Got response for set facility lock:  " << e << std::endl;
            ofs <<  "Callback error value: " << e << std::endl;

            if (pNumRetries != NULL) {
                std::cout << "Response for set facility req: " << std::to_string(*pNumRetries) << std::endl;
                ofs << "Response for set facility req: " << std::to_string(*pNumRetries) << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "RIL request for set facility lock request failed");
        return NULL;
    } else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for set facility lock" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/*---------------------< RIL Request for Sim Query Lock Request >--------------*/
static PyObject* simQueryFacilityLockReq(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    const char* quert_fci_lock[4] ={};
    const char* facility;
    const char* password;
    const char* service_class;
    const char* aid;
    PyObject* callback;
    std::vector<std::string> userInput;

    if (!PyArg_ParseTuple(args, "ssssO", &facility, &password, &service_class, &aid, &callback)) {
        return NULL;
    }

    printInput("simQueryFacilityLockReq", facility, password, service_class, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    quert_fci_lock[0] = const_cast<char*>(facility);
    quert_fci_lock[1] = const_cast<char*>(password);
    quert_fci_lock[2] = const_cast<char*>(service_class);
    quert_fci_lock[3] = const_cast<char*>(aid);

    userInput.push_back(facility);
    userInput.push_back(password);
    userInput.push_back(service_class);
    userInput.push_back(aid);

    Status s = rilSession.simQueryFacilityLockReq(quert_fci_lock,
        [callback] (RIL_Errno e, int32_t* pLockStatus) -> void {
            int32_t lockStatus = -1;
            if (pLockStatus != nullptr) {
                lockStatus = *pLockStatus;
            }

            PyObject* cbArgs = Py_BuildValue("(ii)", e, lockStatus);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);

            std::cout << "Got response for Query Fdn lock req: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (pLockStatus != NULL) {
                std::cout << "Response for queryFdn lock: "<< std::to_string(*pLockStatus) << std::endl;
                ofs << "Response for queryFdn lock: "<< std::to_string(*pLockStatus) << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "RIL request for sim Request for facility lock request failed");
        return NULL;
    } else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "RIL request for sim Request for facility lock sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*---------------------< RIL Request for cancel USSD >--------------*/
static PyObject* cancelUssd(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("cancelUssd");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.cancelUssd([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for CancelUSSD request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send CancelUSSD");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "CancelUSSD request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*---------------------< RIL Request for UDUB >--------------*/
static PyObject* udub(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("udub");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.udub([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for udub: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "RIL request for UDUB failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "UDUB request sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*---------------------< RIL Request for Conference Calls >--------------*/
static PyObject* conference(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("conference");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.conference([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Conference request." << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send Conference");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Conference request sent succesfully" <<  std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*---------------------< RIL Request for switch waiting or holding active >--------------*/
static PyObject* switchWaitingOrHoldingAndActive(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    // TODO: get new parameters and call new API
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("switchWaitingOrHoldingAndActive");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.switchWaitingOrHoldingAndActive([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for switch holding and active: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "RIL request for switch waiting or holding or active failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "RIL request for switch waiting ot active holding success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*---------------------< RIL Request for converged switch waiting or holding active >--------------*/
static PyObject* convergedSwitchWaitingOrHoldingAndActive(PyObject* self, PyObject* args)
{
  PyObject* callback;
  RIL_SwitchWaitingOrHoldingAndActive reqData;
  int hascalltype;
  int calltype;

  if (!PyArg_ParseTuple(args, "iiO", &hascalltype, &calltype, &callback)) {
    return NULL;
  }
  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
    return NULL;
  }
  reqData.hasCallType = static_cast<uint8_t>(hascalltype);
  std::cout << "hasCallType: " << hascalltype << std::endl;
  reqData.callType = static_cast<RIL_CallType>(calltype);
  std::cout << "calltype: " << calltype << std::endl;

  Py_XINCREF(callback);
  Status s = rilSession.switchWaitingOrHoldingAndActive(reqData, [callback](RIL_Errno e) -> void {
    PyObject* cbArgs = Py_BuildValue("(i)", e);
    if (cbArgs == NULL) {
      return;
    }

    PyObject* result = PyObject_CallObject(callback, cbArgs);

    Py_XDECREF(cbArgs);
    Py_XDECREF(result);
    Py_XDECREF(callback);
    std::cout << "Got response for converged switch holding and active: " << e << std::endl;
  });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError,
                    "RIL request for converged switch waiting or holding or active failed");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "RIL request for converged switch waiting ot active holding success" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }
  return PyLong_FromLong(static_cast<long>(s));
}
/*---------------------< RIL Request for hangup foreground and resume background >--------------*/
static PyObject* hangupForegroundResumeBackground(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("hangupForegroundResumeBackground");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.hangupForegroundResumeBackground([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for hangupForegroundResumeBackground request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send hangupForegroundResumeBackground");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "hangupForegroundResumeBackground request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*-------------------< RIL Request Hang up waiting or background >---------------------------*/
static PyObject* hangupWaitingOrBackground(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("hangupWaitingOrBackground");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.hangupWaitingOrBackground([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for hangupWaitingOrBackground request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send hangupWaitingOrBackground");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "hangupWaitingOrBackground request sent successfuly" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*-------------------< RIL request to Enter Sim Pin Request >--------------------------------*/
static PyObject* simEnterPinReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* unblock_pin[2]= {};
    const char* pin;
    const char* aid;
    PyObject* callback;
    std::vector<std::string> userInput;

    if (!PyArg_ParseTuple(args, "ssO", &pin, &aid, &callback)) {
        std::cout << "Here"<< std::endl;
        return NULL;
    }
    printInput("simEnterPinReq", pin, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    unblock_pin[0] = const_cast<char*>(pin);
    unblock_pin[1] = const_cast<char*>(aid);
    userInput.push_back(pin);
    userInput.push_back(aid);
    Py_XINCREF(callback);
    Status s = rilSession.simEnterPinReq(unblock_pin, [callback] (RIL_Errno e, int * retries) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Enter pin: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (retries != NULL)
            {
                std::cout << "Response for Enter Pin: "<< std::to_string(*retries)<<std::endl;
                ofs << "Response for Enter Pin: "<< std::to_string(*retries)<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for Enter_pin");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for enter_pin" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

const std::string getCallStateString(RIL_CallState state){
    std::string ret = "NONE";
    switch (state){
        case 0:
            ret = "CALL_ACTIVE";
            break;
        case 1:
            ret = "CALL_HOLDING";
            break;
        case 2:
            ret = "CALL_DIALING";
            break;
        case 3:
            ret = "CALL_ALERTING";
            break;
        case 4:
            ret = "CALL_INCOMING";
            break;
        case 5:
            ret = "CALL_WAITING";
            break;
        case 6:
        ret = "CALL_END";
        break;
     }
    return ret;
    }

void printCall(const RIL_Call* call,std::fstream &ofs){
    std::cout << "Call Index: "<< call->index << std::endl
              << "Call State: " << getCallStateString(call->state) << std::endl
              << "Call Direction: " << ((call->isMT) ? ("Mobile Terminated") : ("Mobile Originated"))
              << std::endl
              << "Phone Number: " << call->number << std::endl;
        ofs  << "Call Index: "<< call->index << std::endl
              << "Call State: " << getCallStateString(call->state) << std::endl
              << "Call Direction: " << ((call->isMT) ? ("Mobile Terminated") : ("Mobile Originated"))
              << std::endl
              << "Phone Number: " << call->number << std::endl;
}

static PyObject* translate(const RIL_Call** calls, const size_t numCalls) {
    PyObject* lCalls = PyList_New(0);
    if (lCalls == nullptr) {
        return nullptr;
    }

    if (calls == nullptr) {
        return lCalls;
    }

    for (size_t i = 0; i < numCalls; i++) {
        if (calls[i] == nullptr) {
            continue;
        }

        PyObject* dCall = PyDict_New();
        if (dCall == nullptr) {
            return nullptr;
        }

        PyDict_SetItemString(dCall, "state", PyLong_FromLong(calls[i]->state));
        PyDict_SetItemString(dCall, "index", PyLong_FromLong(calls[i]->index));
        PyDict_SetItemString(dCall, "toa", PyLong_FromLong(calls[i]->toa));
        PyDict_SetItemString(dCall, "isMpty", PyLong_FromLong(calls[i]->isMpty));
        PyDict_SetItemString(dCall, "isMT", PyLong_FromLong(calls[i]->isMT));
        PyDict_SetItemString(dCall, "als", PyLong_FromLong(calls[i]->als));
        PyDict_SetItemString(dCall, "isVoice", PyLong_FromLong(calls[i]->isVoice));
        PyDict_SetItemString(dCall, "isVoicePrivacy", PyLong_FromLong(calls[i]->isVoicePrivacy));

        if (calls[i]->number != nullptr) {
            PyDict_SetItemString(dCall, "number", PyUnicode_FromString(calls[i]->number));
        }

        PyDict_SetItemString(dCall, "numberPresentation", PyLong_FromLong(calls[i]->numberPresentation));

        if (calls[i]->name != nullptr) {
            PyDict_SetItemString(dCall, "name", PyUnicode_FromString(calls[i]->name));
        }

        PyDict_SetItemString(dCall, "namePresentation", PyLong_FromLong(calls[i]->namePresentation));

        if (calls[i]->uusInfo != nullptr) {
            PyObject* dUusInfo = translate(calls[i]->uusInfo);
            if (dUusInfo == nullptr) {
                return nullptr;
            }
            PyDict_SetItemString(dCall, "uusInfo", dUusInfo);
        }

        PyDict_SetItemString(dCall, "audioQuality", PyLong_FromLong(calls[i]->audioQuality));

        if (calls[i]->redirNum != nullptr) {
            PyDict_SetItemString(dCall, "redirNum", PyUnicode_FromString(calls[i]->redirNum));
        }

        PyDict_SetItemString(dCall, "redirNumPresentation", PyLong_FromLong(calls[i]->redirNumPresentation));

        PyList_Append(lCalls, dCall);
    }

    return lCalls;
}

/*-------------------< RIL Request Get Current Calls >----------------------------------------*/
static PyObject* getCurrentCalls(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getCurrentCalls");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.getCurrentCalls( [callback] (RIL_Errno e, const size_t numCalls, const RIL_Call ** calls)-> void {
            PyObject* lCalls = translate(calls, numCalls);
            if (lCalls == nullptr) {
                std::cout << "Failed to translate getCurrentCalls response." << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, lCalls);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);

            std::cout << "Got response for Current Calls request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            for (int i = 0; i < numCalls; i++){
                std::cout << "Call: " << i + 1 << std::endl;
                ofs << "Call: " << i + 1 << std::endl;
                printCall(calls[i], ofs);

            }
            ofs.close();
    } );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Get Current calls request Failed");
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Current calls request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

void printCallInfo(const RIL_CallInfo call)
{
  std::cout << "Call Index: " << call.index << std::endl
            << "Call State: " << getCallStateString(call.callState) << std::endl
            << "Call Direction: " << ((call.isMt) ? ("Mobile Terminated") : ("Mobile Originated"))
            << std::endl
            << "Phone Number: " << call.number << std::endl;
}

static PyObject* translate(const std::vector<RIL_CallInfo> callList, const size_t numCalls)
{
  PyObject* lCalls = PyList_New(0);
  if (lCalls == nullptr) {
    return nullptr;
  }

  if (callList.empty()) {
    return lCalls;
  }

  for (size_t i = 0; i < numCalls; i++) {
    if (callList.size() < i) {
      continue;
    }

    PyObject* dCall = PyDict_New();
    if (dCall == nullptr) {
      return nullptr;
    }

    PyDict_SetItemString(dCall, "state", PyLong_FromLong(callList[i].callState));
    PyDict_SetItemString(dCall, "index", PyLong_FromLong(callList[i].index));
    PyDict_SetItemString(dCall, "toa", PyLong_FromLong(callList[i].toa));
    PyDict_SetItemString(dCall, "isMpty", PyLong_FromLong(callList[i].isMpty));
    PyDict_SetItemString(dCall, "isMT", PyLong_FromLong(callList[i].isMt));
    PyDict_SetItemString(dCall, "als", PyLong_FromLong(callList[i].als));
    PyDict_SetItemString(dCall, "isVoice", PyLong_FromLong(callList[i].isVoice));
    PyDict_SetItemString(dCall, "isVoicePrivacy", PyLong_FromLong(callList[i].isVoicePrivacy));

    if (callList[i].number != nullptr) {
      PyDict_SetItemString(dCall, "number", PyUnicode_FromString(callList[i].number));
    }

    PyDict_SetItemString(
        dCall, "numberPresentation", PyLong_FromLong(callList[i].numberPresentation));

    if (callList[i].name != nullptr) {
      PyDict_SetItemString(dCall, "name", PyUnicode_FromString(callList[i].name));
    }

    PyDict_SetItemString(dCall, "namePresentation", PyLong_FromLong(callList[i].namePresentation));

    if (callList[i].uusInfo != nullptr) {
      PyObject* dUusInfo = translate(callList[i].uusInfo);
      if (dUusInfo == nullptr) {
        return nullptr;
      }
      PyDict_SetItemString(dCall, "uusInfo", dUusInfo);
    }

    if (callList[i].redirNum != nullptr) {
      PyDict_SetItemString(dCall, "redirNum", PyUnicode_FromString(callList[i].redirNum));
    }

    PyDict_SetItemString(
        dCall, "redirNumPresentation", PyLong_FromLong(callList[i].redirNumPresentation));

    PyList_Append(lCalls, dCall);
  }

  return lCalls;
}

/*-------------------< RIL Request Converged Get Current Calls >----------------------------------------*/
static PyObject* convergedGetCurrentCalls(PyObject* self, PyObject* args)
{
  PyObject* callback;
  if (!PyArg_ParseTuple(args, "O", &callback)) {
    return NULL;
  }
  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
    return NULL;
  }
  Py_XINCREF(callback);
  Status s = rilSession.getCurrentCalls(
      [callback](RIL_Errno e, const std::vector<RIL_CallInfo> callList) -> void {
        int numCalls = callList.size();
        PyObject* lCalls = translate(callList, numCalls);
        if (lCalls == nullptr) {
          std::cout << "Failed to translate convergedGetCurrentCalls response." << std::endl;
          return;
        }

        PyObject* cbArgs = Py_BuildValue("(iN)", e, lCalls);
        if (cbArgs == nullptr) {
          return;
        }

        PyObject* result = PyObject_CallObject(callback, cbArgs);
        Py_XDECREF(cbArgs);
        Py_XDECREF(result);
        Py_XDECREF(callback);

        std::cout << "Got response for converged Current Calls request: " << e << std::endl;
        for (int i = 0; i < numCalls; i++) {
          std::cout << "Call: " << i + 1 << std::endl;
          printCallInfo(callList[i]);
        }
      });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "Get Converged Current calls request Failed");
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Converged Current calls request sent succesfully" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }

  return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translate(const RIL_CardStatus_v6* cardStatusInfo) {
    PyObject* cardInfo = PyDict_New();
    if (cardInfo == nullptr) {
        return nullptr;
    }

    // return an empty dictionary if cardStatusInfo is NULL
    if (cardStatusInfo == nullptr) {
        return cardInfo;
    }

    PyDict_SetItemString(cardInfo, "card_state", PyLong_FromLong(cardStatusInfo->card_state));
    PyDict_SetItemString(cardInfo, "physical_slot_id", PyLong_FromLong(cardStatusInfo->physical_slot_id));

    if (cardStatusInfo->atr != nullptr) {
        PyDict_SetItemString(cardInfo, "atr", PyUnicode_FromString(cardStatusInfo->atr));
    }

    if (cardStatusInfo->iccid != nullptr) {
        PyDict_SetItemString(cardInfo, "iccid", PyUnicode_FromString(cardStatusInfo->iccid));
    }

    PyDict_SetItemString(cardInfo, "universal_pin_state", PyLong_FromLong(cardStatusInfo->universal_pin_state));
    PyDict_SetItemString(cardInfo, "gsm_umts_subscription_app_index", PyLong_FromLong(cardStatusInfo->gsm_umts_subscription_app_index));
    PyDict_SetItemString(cardInfo, "cdma_subscription_app_index", PyLong_FromLong(cardStatusInfo->cdma_subscription_app_index));
    PyDict_SetItemString(cardInfo, "ims_subscription_app_index", PyLong_FromLong(cardStatusInfo->ims_subscription_app_index));

    if (cardStatusInfo->eid != nullptr) {
        PyDict_SetItemString(cardInfo, "eid", PyUnicode_FromString(cardStatusInfo->eid));
    }

    PyObject* applications = PyList_New(0);
    if (applications == nullptr) {
        return nullptr;
    }

    for (uint32_t i = 0; i < cardStatusInfo->num_applications; i++) {
        PyObject* application = PyDict_New();
        if (application == nullptr) {
            return nullptr;
        }

        PyDict_SetItemString(application, "app_type", PyLong_FromLong(cardStatusInfo->applications[i].app_type));
        PyDict_SetItemString(application, "app_state", PyLong_FromLong(cardStatusInfo->applications[i].app_state));
        PyDict_SetItemString(application, "perso_substate", PyLong_FromLong(cardStatusInfo->applications[i].perso_substate));

        if (cardStatusInfo->applications[i].aid_ptr != nullptr) {
            PyDict_SetItemString(application, "aid_ptr", PyUnicode_FromString(cardStatusInfo->applications[i].aid_ptr));
        }

        if (cardStatusInfo->applications[i].app_label_ptr != nullptr) {
            PyDict_SetItemString(application, "app_label_ptr", PyUnicode_FromString(cardStatusInfo->applications[i].app_label_ptr));
        }

        PyDict_SetItemString(application, "pin1_replaced", PyLong_FromLong(cardStatusInfo->applications[i].pin1_replaced));
        PyDict_SetItemString(application, "pin1", PyLong_FromLong(cardStatusInfo->applications[i].pin1));
        PyDict_SetItemString(application, "pin2", PyLong_FromLong(cardStatusInfo->applications[i].pin2));
        PyList_Append(applications, application);
    }

    PyDict_SetItemString(cardInfo, "applications", applications);

    return cardInfo;
}

/*-------------------< RIl Request Get Sim request and response >-----------------------------*/
static PyObject* simGetSimStatusReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("simGetSimStatusReq");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.simGetSimStatusReq( [callback] (RIL_Errno e, const RIL_CardStatus_v6* cardStatusInfo) -> void {
            PyObject* cardInfo = translate(cardStatusInfo);
            if (cardInfo == nullptr) {
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, cardInfo);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout <<"Got response for get card state: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (cardStatusInfo != nullptr)
            {
                std::cout << "cardStatusInfo.card_state: " << cardStatusInfo->card_state << std::endl;
                std::cout << "cardStatusInfo.physical_slot_id: "<< cardStatusInfo->physical_slot_id << std::endl;
                std::cout << "cardStatusInfo.atr: " << (cardStatusInfo->atr ? cardStatusInfo->atr : "NULL") << std::endl;
                std::cout << "cardStatusInfo.iccid: " << (cardStatusInfo->iccid ? cardStatusInfo->iccid : "NULL") << std::endl;
                std::cout << "cardStatusInfo.universal_pin_state: " << cardStatusInfo->universal_pin_state << std::endl;
                std::cout << "cardStatusInfo.gsm_umts_subscription_app_index: " << cardStatusInfo->gsm_umts_subscription_app_index << std::endl;
                std::cout << "cardStatusInfo.cdma_subscription_app_index: "<<cardStatusInfo->cdma_subscription_app_index <<std::endl;
                std::cout << "cardStatusInfo.ims_subscription_app_index: "<<cardStatusInfo->ims_subscription_app_index <<std::endl;
                std::cout << "cardStatusInfo.eid: "<< (cardStatusInfo->eid ? cardStatusInfo->eid : "NULL")  <<std::endl;
                std::cout << "cardStatusInfo.num_applications: "<<cardStatusInfo->num_applications <<std::endl;

                ofs << "cardStatusInfo.card_state: " << cardStatusInfo->card_state << std::endl;
                ofs << "cardStatusInfo.physical_slot_id: "<< cardStatusInfo->physical_slot_id << std::endl;
                ofs << "cardStatusInfo.atr: " << (cardStatusInfo->atr ? cardStatusInfo->atr : "NULL") << std::endl;
                ofs << "cardStatusInfo.iccid: " << (cardStatusInfo->iccid ? cardStatusInfo->iccid : "NULL") << std::endl;
                ofs << "cardStatusInfo.universal_pin_state: " << cardStatusInfo->universal_pin_state << std::endl;
                ofs << "cardStatusInfo.gsm_umts_subscription_app_index: " << cardStatusInfo->gsm_umts_subscription_app_index << std::endl;
                ofs << "cardStatusInfo.cdma_subscription_app_index: "<<cardStatusInfo->cdma_subscription_app_index <<std::endl;
                ofs << "cardStatusInfo.ims_subscription_app_index: "<<cardStatusInfo->ims_subscription_app_index <<std::endl;
                ofs << "cardStatusInfo.eid: "<< (cardStatusInfo->eid ? cardStatusInfo->eid : "NULL")  <<std::endl;
                ofs << "cardStatusInfo.num_applications: "<<cardStatusInfo->num_applications <<std::endl;
                for(uint32_t cnt=0 ;cnt< cardStatusInfo->num_applications;cnt++)
                {
                    std::cout << "cardStatusInfo.applications[cnt].app_type: "<<cardStatusInfo->applications[cnt].app_type <<std::endl;
                    std::cout << "cardStatusInfo.applications[cnt].app_state: "<<cardStatusInfo->applications[cnt].app_state <<std::endl;
                    std::cout << "cardStatusInfo.applications[cnt].perso_substate: "<<cardStatusInfo->applications[cnt].perso_substate <<std::endl;
                    std::cout << "cardStatusInfo.applications[cnt].aid_ptr: "
                        << (cardStatusInfo->applications[cnt].aid_ptr ? cardStatusInfo->applications[cnt].aid_ptr : "NULL") << std::endl;
                    std::cout << "cardStatusInfo.applications[cnt].app_label_ptr: "
                        << (cardStatusInfo->applications[cnt].app_label_ptr ? cardStatusInfo->applications[cnt].app_label_ptr : "NULL") << std::endl;
                    std::cout << "cardStatusInfo.applications[cnt].pin1_replaced: "<<cardStatusInfo->applications[cnt].pin1_replaced <<std::endl;
                    std::cout << "cardStatusInfo.applications[cnt].pin1: "<<cardStatusInfo->applications[cnt].pin1 <<std::endl;
                    std::cout << "cardStatusInfo.applications[cnt].pin2: "<<cardStatusInfo->applications[cnt].pin2<<std::endl;

                    ofs << "cardStatusInfo.applications[cnt].app_type: "<<cardStatusInfo->applications[cnt].app_type <<std::endl;
                    ofs << "cardStatusInfo.applications[cnt].app_state: "<<cardStatusInfo->applications[cnt].app_state <<std::endl;
                    ofs << "cardStatusInfo.applications[cnt].perso_substate: "<<cardStatusInfo->applications[cnt].perso_substate <<std::endl;
                    ofs << "cardStatusInfo.applications[cnt].aid_ptr: "
                        << (cardStatusInfo->applications[cnt].aid_ptr ? cardStatusInfo->applications[cnt].aid_ptr : "NULL") <<std::endl;
                    ofs << "cardStatusInfo.applications[cnt].app_label_ptr: "
                        << (cardStatusInfo->applications[cnt].app_label_ptr ? cardStatusInfo->applications[cnt].app_label_ptr : "NULL") << std::endl;
                    ofs << "cardStatusInfo.applications[cnt].pin1_replaced: "<<cardStatusInfo->applications[cnt].pin1_replaced <<std::endl;
                    ofs << "cardStatusInfo.applications[cnt].pin1: "<<cardStatusInfo->applications[cnt].pin1 <<std::endl;
                    ofs << "cardStatusInfo.applications[cnt].pin2: "<<cardStatusInfo->applications[cnt].pin2<<std::endl;
                }
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Sim status response recieved with error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received sim status response successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
void printCellIdentity(RIL_CellIdentityGsm_v12 cell, std::fstream& ofs) {
  std::cout << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;

    ofs << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellIdentity(RIL_CellIdentityCdma cell, std::fstream& ofs) {
  std::cout << "Network Id: " << cell.networkId << std::endl
            << "Operator Id: " << cell.basestationId << std::endl
            << "LAT: " << cell.latitude << std::endl
            << "LONG: " << cell.longitude << std::endl
            << "Base Station Id: " << cell.basestationId << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;

    ofs << "Network Id: " << cell.networkId << std::endl
            << "Operator Id: " << cell.basestationId << std::endl
            << "LAT: " << cell.latitude << std::endl
            << "LONG: " << cell.longitude << std::endl
            << "Base Station Id: " << cell.basestationId << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellIdentity(RIL_CellIdentityLte_v12 cell, std::fstream& ofs) {
  std::cout << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.bandwidth << std::endl
            << "Bandwidth: " << cell.ci << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;

    ofs << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.bandwidth << std::endl
            << "Bandwidth: " << cell.ci << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellIdentity(RIL_CellIdentityWcdma_v12 cell, std::fstream& ofs) {
  std::cout << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;

        ofs <<  "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellIdentity(RIL_CellIdentityTdscdma cell, std::fstream& ofs) {
  std::cout << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;

        ofs << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}
const std::string getRegistrationStateFromValue (RIL_RegState state) {
    std::string ret;
    switch (state) {
        case RIL_NOT_REG_AND_NOT_SEARCHING:
            ret = "RIL_NOT_REG_AND_NOT_SEARCHING";
            break;
        case RIL_REG_HOME:
            ret = "RIL_REG_HOME";
            break;
        case RIL_NOT_REG_AND_SEARCHING:
            ret = "RIL_NOT_REG_AND_SEARCHING";
            break;
        case RIL_REG_DENIED:
            ret = "RIL_REG_DENIED";
            break;
        case RIL_UNKNOWN:
            ret = "RIL_UNKNOWN";
            break;
        case RIL_REG_ROAMING:
            ret = "RIL_REG_ROAMING";
            break;
        case RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING:
            ret = "RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING";
            break;
        case RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING:
            ret = "RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING";
            break;
        case RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE:
            ret = "RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE";
            break;
        case RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE:
            ret = "RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE";
            break;
        default:
            ret = "NONE";
    }
    return ret;
}
const std::string getRatFromValue(RIL_RadioTechnology rat) {
  std::string ret;
  switch (rat) {
    case RADIO_TECH_UNKNOWN:
      ret = "RADIO_TECH_UNKNOWN";
      break;
    case RADIO_TECH_GPRS:
      ret = "RADIO_TECH_GPRS";
      break;
    case RADIO_TECH_EDGE:
      ret = "RADIO_TECH_EDGE";
      break;
    case RADIO_TECH_UMTS:
      ret = "RADIO_TECH_UMTS";
      break;
    case RADIO_TECH_IS95A:
      ret = "RADIO_TECH_IS95A";
      break;
    case RADIO_TECH_IS95B:
      ret = "RADIO_TECH_IS95B";
      break;
    case RADIO_TECH_1xRTT:
      ret = "RADIO_TECH_1xRTT";
      break;
    case RADIO_TECH_EVDO_0:
      ret = "RADIO_TECH_EVDO_0";
      break;
    case RADIO_TECH_EVDO_A:
      ret = "RADIO_TECH_EVDO_A";
      break;
    case RADIO_TECH_HSDPA:
      ret = "RADIO_TECH_HSDPA";
      break;
    case RADIO_TECH_HSUPA:
      ret = "RADIO_TECH_HSUPA";
      break;
    case RADIO_TECH_HSPA:
      ret = "RADIO_TECH_HSPA";
      break;
    case RADIO_TECH_EVDO_B:
      ret = "RADIO_TECH_EVDO_B";
      break;
    case RADIO_TECH_EHRPD:
      ret = "RADIO_TECH_EHRPD";
      break;
    case RADIO_TECH_LTE:
      ret = "RADIO_TECH_LTE";
      break;
    case RADIO_TECH_HSPAP:
      ret = "RADIO_TECH_HSPAP";
      break;
    case RADIO_TECH_GSM:
      ret = "RADIO_TECH_GSM";
      break;
    case RADIO_TECH_TD_SCDMA:
      ret = "RADIO_TECH_TD_SCDMA";
      break;
    case RADIO_TECH_IWLAN:
      ret = "RADIO_TECH_IWLAN";
      break;
    case RADIO_TECH_LTE_CA:
      ret = "RADIO_TECH_LTE_CA";
      break;
    case RADIO_TECH_5G:
      ret = "RADIO_TECH_5G";
      break;
    case RADIO_TECH_WIFI:
      ret = "RADIO_TECH_WIFI";
      break;
    case RADIO_TECH_ANY:
      ret = "RADIO_TECH_ANY";
      break;
    case RADIO_TECH_AUTO:
      ret = "RADIO_TECH_AUTO";
      break;
    default:
      ret = "NONE";
  }
  return ret;
}
const std::string getTypeStringFromVal(RIL_CellInfoType type) {
  std::string ret;
  switch (type) {
    case RIL_CELL_INFO_TYPE_NONE:
      ret = "NONE";
      break;
    case RIL_CELL_INFO_TYPE_GSM:
      ret = "GSM";
      break;
    case RIL_CELL_INFO_TYPE_CDMA:
      ret = "CDMA";
      break;
    case RIL_CELL_INFO_TYPE_LTE:
      ret = "LTE";
      break;
    case RIL_CELL_INFO_TYPE_WCDMA:
      ret = "WCDMA";
      break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
      ret = "TD_SCDMA";
      break;
    default:
      ret = "NONE";
  }
  return ret;
}
void printCellInfo(RIL_CellIdentity_v16 cell, std::fstream& ofs) {
  std::cout << "Cell Identity for: " << getTypeStringFromVal(cell.cellInfoType) << std::endl;
  ofs << "Cell Identity for: " << getTypeStringFromVal(cell.cellInfoType) << std::endl;
  switch (cell.cellInfoType) {
    case RIL_CELL_INFO_TYPE_NONE:;
      break;
    case RIL_CELL_INFO_TYPE_GSM:
      printCellIdentity(cell.cellIdentityGsm, ofs);
      break;
    case RIL_CELL_INFO_TYPE_CDMA:
      printCellIdentity(cell.cellIdentityCdma, ofs);
      break;
    case RIL_CELL_INFO_TYPE_LTE:
      printCellIdentity(cell.cellIdentityLte, ofs);
      break;
    case RIL_CELL_INFO_TYPE_WCDMA:
      printCellIdentity(cell.cellIdentityWcdma, ofs);
      break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
      printCellIdentity(cell.cellIdentityTdscdma, ofs);
      break;
  }
}

static PyObject* translate(const RIL_VoiceRegistrationStateResponse& voiceRegState) {
    PyObject* dVoiceRegState = PyDict_New();
    if (dVoiceRegState == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dVoiceRegState, "regState", PyLong_FromLong(voiceRegState.regState));
    PyDict_SetItemString(dVoiceRegState, "rat", PyLong_FromLong(voiceRegState.rat));
    PyDict_SetItemString(dVoiceRegState, "cssSupported", PyLong_FromLong(voiceRegState.cssSupported));
    PyDict_SetItemString(dVoiceRegState, "roamingIndicator", PyLong_FromLong(voiceRegState.roamingIndicator));
    PyDict_SetItemString(dVoiceRegState, "systemIsInPrl", PyLong_FromLong(voiceRegState.systemIsInPrl));
    PyDict_SetItemString(dVoiceRegState, "defaultRoamingIndicator", PyLong_FromLong(voiceRegState.defaultRoamingIndicator));
    PyDict_SetItemString(dVoiceRegState, "reasonForDenial", PyLong_FromLong(voiceRegState.reasonForDenial));
    PyDict_SetItemString(dVoiceRegState, "geranDtmSupported", PyLong_FromLong(voiceRegState.geranDtmSupported));

    PyObject* dCellIdentity = translate(voiceRegState.cellIdentity);
    if (dCellIdentity != nullptr) {
        PyDict_SetItemString(dVoiceRegState, "cellIdentity", dCellIdentity);
    }

    return dVoiceRegState;
}

/*-------------------< Voice Registration request and resonse >------------------------------*/
static PyObject* getVoiceRegStatus(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getVoiceRegStatus");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.getVoiceRegStatus( [callback] (RIL_Errno e,  RIL_VoiceRegistrationStateResponse reg) mutable-> void {
            PyObject* dVoiceRegState = translate(reg);
            if (dVoiceRegState == nullptr) {
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dVoiceRegState);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Voice Reg request: " << e << std::endl;
            std::cout << "VOICE REGISTRATION:" << std::endl;
            std::cout << "Registration State: " << getRegistrationStateFromValue(reg.regState) << std::endl;
            std::cout << "Radio Access Technology: " << getRatFromValue(reg.rat) << std::endl;
            std::cout << "Roaming Indicator: " << reg.roamingIndicator << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "Registration State: " << getRegistrationStateFromValue(reg.regState) << std::endl;
            ofs << "Radio Access Technology: " << getRatFromValue(reg.rat) << std::endl;
            ofs << "Roaming Indicator: " << reg.roamingIndicator << std::endl;
            printCellInfo(reg.cellIdentity, ofs);
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Get Voice Registration Status Failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Succesfully sent get Voice registration" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translate(const RIL_GW_SignalStrength& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "signalStrength", PyLong_FromLong(signalStrength.signalStrength));
    PyDict_SetItemString(dSignalStrength, "bitErrorRate", PyLong_FromLong(signalStrength.bitErrorRate));

    return dSignalStrength;
}

static PyObject* translate(const RIL_WCDMA_SignalStrength& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "signalStrength", PyLong_FromLong(signalStrength.signalStrength));
    PyDict_SetItemString(dSignalStrength, "bitErrorRate", PyLong_FromLong(signalStrength.bitErrorRate));
    PyDict_SetItemString(dSignalStrength, "rscp", PyLong_FromLong(signalStrength.rscp));
    PyDict_SetItemString(dSignalStrength, "ecio", PyLong_FromLong(signalStrength.ecio));

    return dSignalStrength;
}

static PyObject* translate(const RIL_CDMA_SignalStrength& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "dbm", PyLong_FromLong(signalStrength.dbm));
    PyDict_SetItemString(dSignalStrength, "ecio", PyLong_FromLong(signalStrength.ecio));

    return dSignalStrength;
}

static PyObject* translate(const RIL_EVDO_SignalStrength& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "dbm", PyLong_FromLong(signalStrength.dbm));
    PyDict_SetItemString(dSignalStrength, "ecio", PyLong_FromLong(signalStrength.ecio));
    PyDict_SetItemString(dSignalStrength, "signalNoiseRatio", PyLong_FromLong(signalStrength.signalNoiseRatio));

    return dSignalStrength;
}

static PyObject* translate(const RIL_LTE_SignalStrength_v8& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "signalStrength", PyLong_FromLong(signalStrength.signalStrength));
    PyDict_SetItemString(dSignalStrength, "rsrp", PyLong_FromLong(signalStrength.rsrp));
    PyDict_SetItemString(dSignalStrength, "rsrq", PyLong_FromLong(signalStrength.rsrq));
    PyDict_SetItemString(dSignalStrength, "rssnr", PyLong_FromLong(signalStrength.rssnr));
    PyDict_SetItemString(dSignalStrength, "cqi", PyLong_FromLong(signalStrength.cqi));
    PyDict_SetItemString(dSignalStrength, "timingAdvance", PyLong_FromLong(signalStrength.timingAdvance));

    return dSignalStrength;
}

static PyObject* translate(const RIL_TD_SCDMA_SignalStrength& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "signalStrength", PyLong_FromLong(signalStrength.signalStrength));
    PyDict_SetItemString(dSignalStrength, "bitErrorRate", PyLong_FromLong(signalStrength.bitErrorRate));
    PyDict_SetItemString(dSignalStrength, "rscp", PyLong_FromLong(signalStrength.rscp));

    return dSignalStrength;
}

static PyObject* translate(const RIL_NR_SignalStrength& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "ssRsrp", PyLong_FromLong(signalStrength.ssRsrp));
    PyDict_SetItemString(dSignalStrength, "ssRsrq", PyLong_FromLong(signalStrength.ssRsrq));
    PyDict_SetItemString(dSignalStrength, "ssSinr", PyLong_FromLong(signalStrength.ssSinr));
    PyDict_SetItemString(dSignalStrength, "csiRsrp", PyLong_FromLong(signalStrength.csiRsrp));
    PyDict_SetItemString(dSignalStrength, "csiRsrq", PyLong_FromLong(signalStrength.csiRsrq));
    PyDict_SetItemString(dSignalStrength, "csiSinr", PyLong_FromLong(signalStrength.csiSinr));

    return dSignalStrength;
}

static PyObject* translate(const RIL_SignalStrength& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyObject* dGsmSignalStrength = translate(signalStrength.GW_SignalStrength);
    if (dGsmSignalStrength != nullptr) {
        PyDict_SetItemString(dSignalStrength, "GW_SignalStrength", dGsmSignalStrength);
    }

    PyObject* dWcdmaSignalStrength = translate(signalStrength.WCDMA_SignalStrength);
    if (dWcdmaSignalStrength != nullptr) {
        PyDict_SetItemString(dSignalStrength, "WCDMA_SignalStrength", dWcdmaSignalStrength);
    }

    PyObject* dCdmaSignalStrength = translate(signalStrength.CDMA_SignalStrength);
    if (dCdmaSignalStrength != nullptr) {
        PyDict_SetItemString(dSignalStrength, "CDMA_SignalStrength", dCdmaSignalStrength);
    }

    PyObject* dEvdoSignalStrength = translate(signalStrength.EVDO_SignalStrength);
    if (dEvdoSignalStrength != nullptr) {
        PyDict_SetItemString(dSignalStrength, "EVDO_SignalStrength", dEvdoSignalStrength);
    }

    PyObject* dLteSignalStrength = translate(signalStrength.LTE_SignalStrength);
    if (dLteSignalStrength != nullptr) {
        PyDict_SetItemString(dSignalStrength, "LTE_SignalStrength", dLteSignalStrength);
    }

    PyObject* dTdscdmaSignalStrength = translate(signalStrength.TD_SCDMA_SignalStrength);
    if (dTdscdmaSignalStrength != nullptr) {
        PyDict_SetItemString(dSignalStrength, "TD_SCDMA_SignalStrength", dTdscdmaSignalStrength);
    }

    PyObject* dNrSignalStrength = translate(signalStrength.NR_SignalStrength);
    if (dNrSignalStrength != nullptr) {
        PyDict_SetItemString(dSignalStrength, "NR_SignalStrength", dNrSignalStrength);
    }

    return dSignalStrength;
}

static PyObject* translate(const RIL_CellIdentityOperatorNames& operatorNames) {
    PyObject* dOperatorNames = PyDict_New();
    if (dOperatorNames == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dOperatorNames, "alphaShort", PyUnicode_FromString(operatorNames.alphaShort));
    PyDict_SetItemString(dOperatorNames, "alphaLong", PyUnicode_FromString(operatorNames.alphaLong));

    return dOperatorNames;
}

static PyObject* translate(const RIL_CellIdentityGsm_v12& cellIdentity) {
    PyObject* dCellIdentity = PyDict_New();
    if (dCellIdentity == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dCellIdentity, "mcc", PyUnicode_FromString(cellIdentity.mcc));
    PyDict_SetItemString(dCellIdentity, "mnc", PyUnicode_FromString(cellIdentity.mnc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mcc", PyUnicode_FromString(cellIdentity.reg_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mnc", PyUnicode_FromString(cellIdentity.reg_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mcc", PyUnicode_FromString(cellIdentity.primary_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mnc", PyUnicode_FromString(cellIdentity.primary_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "lac", PyLong_FromLong(cellIdentity.lac));
    PyDict_SetItemString(dCellIdentity, "cid", PyLong_FromLong(cellIdentity.cid));
    PyDict_SetItemString(dCellIdentity, "arfcn", PyLong_FromLong(cellIdentity.arfcn));
    PyDict_SetItemString(dCellIdentity, "bsic", PyLong_FromLong(cellIdentity.bsic));

    PyObject* dOperatorNames = translate(cellIdentity.operatorNames);
    if (dOperatorNames != nullptr) {
        PyDict_SetItemString(dCellIdentity, "operatorNames", dOperatorNames);
    }

    return dCellIdentity;
}

static PyObject* translate(const RIL_CellIdentityWcdma_v12& cellIdentity) {
    PyObject* dCellIdentity = PyDict_New();
    if (dCellIdentity == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dCellIdentity, "mcc", PyUnicode_FromString(cellIdentity.mcc));
    PyDict_SetItemString(dCellIdentity, "mnc", PyUnicode_FromString(cellIdentity.mnc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mcc", PyUnicode_FromString(cellIdentity.reg_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mnc", PyUnicode_FromString(cellIdentity.reg_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mcc", PyUnicode_FromString(cellIdentity.primary_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mnc", PyUnicode_FromString(cellIdentity.primary_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "lac", PyLong_FromLong(cellIdentity.lac));
    PyDict_SetItemString(dCellIdentity, "cid", PyLong_FromLong(cellIdentity.cid));
    PyDict_SetItemString(dCellIdentity, "psc", PyLong_FromLong(cellIdentity.psc));
    PyDict_SetItemString(dCellIdentity, "uarfcn", PyLong_FromLong(cellIdentity.uarfcn));

    PyObject* dOperatorNames = translate(cellIdentity.operatorNames);
    if (dOperatorNames != nullptr) {
        PyDict_SetItemString(dCellIdentity, "operatorNames", dOperatorNames);
    }

    return dCellIdentity;
}

static PyObject* translate(const RIL_CellIdentityLte_v12& cellIdentity) {
    PyObject* dCellIdentity = PyDict_New();
    if (dCellIdentity == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dCellIdentity, "mcc", PyUnicode_FromString(cellIdentity.mcc));
    PyDict_SetItemString(dCellIdentity, "mnc", PyUnicode_FromString(cellIdentity.mnc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mcc", PyUnicode_FromString(cellIdentity.reg_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mnc", PyUnicode_FromString(cellIdentity.reg_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mcc", PyUnicode_FromString(cellIdentity.primary_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mnc", PyUnicode_FromString(cellIdentity.primary_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "ci", PyLong_FromLong(cellIdentity.ci));
    PyDict_SetItemString(dCellIdentity, "pci", PyLong_FromLong(cellIdentity.pci));
    PyDict_SetItemString(dCellIdentity, "tac", PyLong_FromLong(cellIdentity.tac));
    PyDict_SetItemString(dCellIdentity, "earfcn", PyLong_FromLong(cellIdentity.earfcn));
    PyDict_SetItemString(dCellIdentity, "bandwidth", PyLong_FromLong(cellIdentity.bandwidth));
    PyDict_SetItemString(dCellIdentity, "band", PyLong_FromLong(cellIdentity.band));

    PyObject* dOperatorNames = translate(cellIdentity.operatorNames);
    if (dOperatorNames != nullptr) {
        PyDict_SetItemString(dCellIdentity, "operatorNames", dOperatorNames);
    }

    return dCellIdentity;
}

static PyObject* translate(const RIL_CellIdentityTdscdma& cellIdentity) {
    PyObject* dCellIdentity = PyDict_New();
    if (dCellIdentity == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dCellIdentity, "mcc", PyUnicode_FromString(cellIdentity.mcc));
    PyDict_SetItemString(dCellIdentity, "mnc", PyUnicode_FromString(cellIdentity.mnc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mcc", PyUnicode_FromString(cellIdentity.reg_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mnc", PyUnicode_FromString(cellIdentity.reg_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mcc", PyUnicode_FromString(cellIdentity.primary_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mnc", PyUnicode_FromString(cellIdentity.primary_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "lac", PyLong_FromLong(cellIdentity.lac));
    PyDict_SetItemString(dCellIdentity, "cid", PyLong_FromLong(cellIdentity.cid));
    PyDict_SetItemString(dCellIdentity, "cpid", PyLong_FromLong(cellIdentity.cpid));
    PyDict_SetItemString(dCellIdentity, "uarfcn", PyLong_FromLong(cellIdentity.uarfcn));

    PyObject* dOperatorNames = translate(cellIdentity.operatorNames);
    if (dOperatorNames != nullptr) {
        PyDict_SetItemString(dCellIdentity, "operatorNames", dOperatorNames);
    }

    return dCellIdentity;
}

static PyObject* translate(const RIL_CellIdentityCdma& cellIdentity) {
    PyObject* dCellIdentity = PyDict_New();
    if (dCellIdentity == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dCellIdentity, "networkId", PyLong_FromLong(cellIdentity.networkId));
    PyDict_SetItemString(dCellIdentity, "systemId", PyLong_FromLong(cellIdentity.systemId));
    PyDict_SetItemString(dCellIdentity, "basestationId", PyLong_FromLong(cellIdentity.basestationId));
    PyDict_SetItemString(dCellIdentity, "longitude", PyLong_FromLong(cellIdentity.longitude));
    PyDict_SetItemString(dCellIdentity, "latitude", PyLong_FromLong(cellIdentity.latitude));

    PyObject* dOperatorNames = translate(cellIdentity.operatorNames);
    if (dOperatorNames != nullptr) {
        PyDict_SetItemString(dCellIdentity, "operatorNames", dOperatorNames);
    }

    return dCellIdentity;
}

static PyObject* translate(const RIL_CellIdentityNr& cellIdentity) {
    PyObject* dCellIdentity = PyDict_New();
    if (dCellIdentity == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dCellIdentity, "mcc", PyUnicode_FromString(cellIdentity.mcc));
    PyDict_SetItemString(dCellIdentity, "mnc", PyUnicode_FromString(cellIdentity.mnc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mcc", PyUnicode_FromString(cellIdentity.reg_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "reg_plmn_mnc", PyUnicode_FromString(cellIdentity.reg_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mcc", PyUnicode_FromString(cellIdentity.primary_plmn_mcc));
    PyDict_SetItemString(dCellIdentity, "primary_plmn_mnc", PyUnicode_FromString(cellIdentity.primary_plmn_mnc));
    PyDict_SetItemString(dCellIdentity, "nci", PyLong_FromLong(cellIdentity.nci));
    PyDict_SetItemString(dCellIdentity, "pci", PyLong_FromLong(cellIdentity.pci));
    PyDict_SetItemString(dCellIdentity, "tac", PyLong_FromLong(cellIdentity.tac));
    PyDict_SetItemString(dCellIdentity, "nrarfcn", PyLong_FromLong(cellIdentity.nrarfcn));
    PyDict_SetItemString(dCellIdentity, "bandwidth", PyLong_FromLong(cellIdentity.bandwidth));
    PyDict_SetItemString(dCellIdentity, "band", PyLong_FromLong(cellIdentity.band));

    PyObject* dOperatorNames = translate(cellIdentity.operatorNames);
    if (dOperatorNames != nullptr) {
        PyDict_SetItemString(dCellIdentity, "operatorNames", dOperatorNames);
    }

    return dCellIdentity;
}

static PyObject* translate(const RIL_CellIdentity_v16& cellIdentity) {
    PyObject* dCellIdentity = PyDict_New();
    if (dCellIdentity == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dCellIdentity, "cellInfoType", PyLong_FromLong(cellIdentity.cellInfoType));

    switch (cellIdentity.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            {
                PyObject* dCellIdentityGsm = translate(cellIdentity.cellIdentityGsm);
                if (dCellIdentityGsm != nullptr) {
                    PyDict_SetItemString(dCellIdentity, "cellIdentityGsm", dCellIdentityGsm);
                }
                break;
            }
        case RIL_CELL_INFO_TYPE_CDMA:
            {
                PyObject* dCellIdentityCdma = translate(cellIdentity.cellIdentityCdma);
                if (dCellIdentityCdma != nullptr) {
                    PyDict_SetItemString(dCellIdentity, "cellIdentityCdma", dCellIdentityCdma);
                }
                break;
            }
        case RIL_CELL_INFO_TYPE_LTE:
            {
                PyObject* dCellIdentityLte = translate(cellIdentity.cellIdentityLte);
                if (dCellIdentityLte != nullptr) {
                    PyDict_SetItemString(dCellIdentity, "cellIdentityLte", dCellIdentityLte);
                }
                break;
            }
        case RIL_CELL_INFO_TYPE_WCDMA:
            {
                PyObject* dCellIdentityWcdma = translate(cellIdentity.cellIdentityWcdma);
                if (dCellIdentityWcdma != nullptr) {
                    PyDict_SetItemString(dCellIdentity, "cellIdentityWcdma", dCellIdentityWcdma);
                }
                break;
            }
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            {
                PyObject* dCellIdentityTdscdma = translate(cellIdentity.cellIdentityTdscdma);
                if (dCellIdentityTdscdma != nullptr) {
                    PyDict_SetItemString(dCellIdentity, "cellIdentityTdscdma", dCellIdentityTdscdma);
                }
                break;
            }
        case RIL_CELL_INFO_TYPE_NR:
            {
                PyObject* dCellIdentityNr = translate(cellIdentity.cellIdentityNr);
                if (dCellIdentityNr != nullptr) {
                    PyDict_SetItemString(dCellIdentity, "cellIdentityNr", dCellIdentityNr);
                }
                break;
            }
        default:
            break;
    }

    return dCellIdentity;
}

static PyObject* translate(const LteVopsInfo& vopsInfo) {
    PyObject* dVopsInfo = PyDict_New();
    if (dVopsInfo == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dVopsInfo, "isVopsSupported", PyLong_FromLong(vopsInfo.isVopsSupported));
    PyDict_SetItemString(dVopsInfo, "isEmcBearerSupported", PyLong_FromLong(vopsInfo.isEmcBearerSupported));

    return dVopsInfo;
}

static PyObject* translate(const NrVopsInfo& vopsInfo) {
    PyObject* dVopsInfo = PyDict_New();
    if (dVopsInfo == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dVopsInfo, "isVopsSupported", PyLong_FromLong(vopsInfo.isVopsSupported));
    PyDict_SetItemString(dVopsInfo, "isEmcSupported", PyLong_FromLong(vopsInfo.isEmcSupported));
    PyDict_SetItemString(dVopsInfo, "isEmfSupported", PyLong_FromLong(vopsInfo.isEmfSupported));

    return dVopsInfo;
}

static PyObject* translate(const NrIndicators& nrIndicators) {
    PyObject* dNrIndicators = PyDict_New();
    if (dNrIndicators == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dNrIndicators, "isEndcAvailable", PyLong_FromLong(nrIndicators.isEndcAvailable));
    PyDict_SetItemString(dNrIndicators, "isDcNrRestricted", PyLong_FromLong(nrIndicators.isDcNrRestricted));
    PyDict_SetItemString(dNrIndicators, "plmnInfoListR15Available", PyLong_FromLong(nrIndicators.plmnInfoListR15Available));

    return dNrIndicators;
}

static PyObject* translate(const RIL_DataRegistrationStateResponse& dataRegState) {
    PyObject* dDataRegState = PyDict_New();
    if (dDataRegState == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dDataRegState, "regState", PyLong_FromLong(dataRegState.regState));
    PyDict_SetItemString(dDataRegState, "rat", PyLong_FromLong(dataRegState.rat));
    PyDict_SetItemString(dDataRegState, "reasonDataDenied", PyLong_FromLong(dataRegState.reasonDataDenied));
    PyDict_SetItemString(dDataRegState, "maxDataCalls", PyLong_FromLong(dataRegState.maxDataCalls));

    PyObject* dCellIdentity = translate(dataRegState.cellIdentity);
    if (dCellIdentity != nullptr) {
        PyDict_SetItemString(dDataRegState, "cellIdentity", dCellIdentity);
    }

    if (dataRegState.lteVopsInfoValid) {
        PyObject* dLteVopsInfo = translate(dataRegState.lteVopsInfo);
        if (dLteVopsInfo != nullptr) {
            PyDict_SetItemString(dDataRegState, "lteVopsInfo", dLteVopsInfo);
        }
    }

    if (dataRegState.nrVopsInfoValid) {
        PyObject* dNrVopsInfo = translate(dataRegState.nrVopsInfo);
        if (dNrVopsInfo != nullptr) {
            PyDict_SetItemString(dDataRegState, "nrVopsInfo", dNrVopsInfo);
        }
    }

    if (dataRegState.nrIndicatorsValid) {
        PyObject* dNrIndicators = translate(dataRegState.nrIndicators);
        if (dNrIndicators != nullptr) {
            PyDict_SetItemString(dDataRegState, "nrIndicators", dNrIndicators);
        }
    }

    return dDataRegState;
}

/*-------------------< Data Registration request and response >------------------------------*/
static PyObject* getDataRegStatus(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.getDataRegStatus( [callback] (RIL_Errno e,  RIL_DataRegistrationStateResponse reg) -> void {
            PyObject* dDataRegState = translate(reg);
            if (dDataRegState == nullptr) {
                std::cout << "Failed to translate getDataRegStatus response." << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dDataRegState);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Data Reg request: " << e << std::endl;
            std::cout << "DATA REGISTRATION:" << std::endl;
            std::cout << "Registration State: " << getRegistrationStateFromValue(reg.regState) << std::endl;
            std::cout << "Radio Access Technology: " << getRatFromValue(reg.rat) << std::endl;
            std::cout << "Max Data Calls: " << reg.maxDataCalls << std::endl;
            if (reg.lteVopsInfoValid) {
                std::cout << "LTE VoPS: Supported: " << static_cast<bool>(reg.lteVopsInfo.isVopsSupported)
                << " and EMC Bearer Supported " << reg.lteVopsInfo.isEmcBearerSupported << std::endl;
        }
        if (reg.nrIndicatorsValid) {
            std::cout << "NR Indicators: " << std::endl
                      << "Endc Available: " << static_cast<bool>(reg.nrIndicators.isEndcAvailable) << std::endl
                      << "PLMN Info List R15 Available: " << static_cast<bool>(reg.nrIndicators.plmnInfoListR15Available)
                      << std::endl;
        }
            ofs << "Callback error value: " << e << std::endl;
            ofs << "Registration State: " << getRegistrationStateFromValue(reg.regState) << std::endl;
            ofs << "Radio Access Technology: " << getRatFromValue(reg.rat) << std::endl;
            ofs << "Max Data Calls: " << reg.maxDataCalls << std::endl;
            if (reg.lteVopsInfoValid) {
                ofs << "LTE VoPS: Supported: " << static_cast<bool>(reg.lteVopsInfo.isVopsSupported) <<  std::endl;
                ofs << "EMC Bearer Supported :" << reg.lteVopsInfo.isEmcBearerSupported << std::endl;
        }
        if (reg.nrIndicatorsValid) {
                      ofs << "Endc Available: " << static_cast<bool>(reg.nrIndicators.isEndcAvailable) << std::endl
                      << "PLMN Info List R15 Available: " << static_cast<bool>(reg.nrIndicators.plmnInfoListR15Available)
                      << std::endl;
        }
        ofs.close();
    });

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send getDataRegistration");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Successfully sent data registration state" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*----------------------------< Radio Power NEW request and response >---------------------------*/
static PyObject* radioPower(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int state;
    int forEmergencyCall;
    int preferredForEmergencyCall;

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiiO", &state, &forEmergencyCall, &preferredForEmergencyCall, &callback)) {
        return NULL;
    }
    printInput("radioPower", state, forEmergencyCall, preferredForEmergencyCall);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    RIL_RadioPowerReq req {};
    req.state = state;
    req.forEmergencyCall = forEmergencyCall;
    req.preferredForEmergencyCall = preferredForEmergencyCall;

    //Status s = rilSession.radioPower(state,
    Status s = rilSession.radioPower(req,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Radio Power request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send setRadioPower");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Set Radio Power request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator <<(std::ostream& out, RIL_CdmaRoamingPreference roamingPref) {
  switch (roamingPref) {
    case CDMA_ROAMING_PREFERENCE_HOME_NETWORK:
      out << "Home Network";
      break;
    case CDMA_ROAMING_PREFERENCE_AFFILIATED_ROAM:
      out << "Affiliated Roam";
      break;
    case CDMA_ROAMING_PREFERENCE_ANY_ROAM:
      out << "Any Roam";
      break;
    default:
      out << "Invalid Roaming Preference";
      break;
  }

  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_TimeStampType timeStampType) {
  out << "Time Stamp Type: ";
  switch (timeStampType) {
    case RIL_TimeStampType::RIL_TIMESTAMP_TYPE_ANTENNA:
      out << "Antenna";
      break;
    case RIL_TimeStampType::RIL_TIMESTAMP_TYPE_MODEM:
      out << "Modem";
      break;
    case RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL:
      out << "OEM RIL";
      break;
    case RIL_TimeStampType::RIL_TIMESTAMP_TYPE_JAVA_RIL:
      out << "Java RIL";
      break;
    default:
      out << "Unknown";
      break;
  }
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellConnectionStatus connStatus) {
  out << "Cell Connection Status: ";
  switch (connStatus) {
    case RIL_CellConnectionStatus::RIL_CELL_CONNECTION_STATUS_PRIMARY:
      out << "Primary";
      break;
    case RIL_CellConnectionStatus::RIL_CELL_CONNECTION_STATUS_SECONDARY:
      out << "Secondary";
      break;
    default:
      out << "Unknown";
      break;
  }
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityOperatorNames& operatorNames) {
  out << "Operator Short Name: " << std::string(operatorNames.alphaShort,
    strnlen(operatorNames.alphaShort, sizeof(operatorNames.alphaShort))) << std::endl;
  out << "Operator Long Name: " << std::string(operatorNames.alphaLong,
    strnlen(operatorNames.alphaLong, sizeof(operatorNames.alphaLong)));
  return out;
}

template <typename T>
std::ostream& outputMccMnc(std::ostream& out, const T& cellIdentity) {
  out << "MCC: " << std::string(cellIdentity.mcc,
    strnlen(cellIdentity.mcc, sizeof(cellIdentity.mcc))) << std::endl;

  out << "MNC: " << std::string(cellIdentity.mnc,
    strnlen(cellIdentity.mnc, sizeof(cellIdentity.mnc))) << std::endl;

  out << "Registered PLMN MCC: " << std::string(cellIdentity.reg_plmn_mcc,
    strnlen(cellIdentity.reg_plmn_mcc, sizeof(cellIdentity.reg_plmn_mcc))) << std::endl;

  out << "Registered PLMN MNC: " << std::string(cellIdentity.reg_plmn_mnc,
    strnlen(cellIdentity.reg_plmn_mnc, sizeof(cellIdentity.reg_plmn_mnc))) << std::endl;

  out << "Primary PLMN MCC: " << std::string(cellIdentity.primary_plmn_mcc,
    strnlen(cellIdentity.primary_plmn_mcc, sizeof(cellIdentity.primary_plmn_mcc))) << std::endl;

  out << "Primary PLMN MNC: " << std::string(cellIdentity.primary_plmn_mnc,
    strnlen(cellIdentity.primary_plmn_mnc, sizeof(cellIdentity.primary_plmn_mnc)));

  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityGsm_v12& cellIdentityGsm) {
  out << "GSM Cell Identity : " << std::endl;
  outputMccMnc(out, cellIdentityGsm) << std::endl;
  out << "LAC: " << cellIdentityGsm.lac << std::endl;
  out << "CID: " << cellIdentityGsm.cid << std::endl;
  out << "ARFCN: " << cellIdentityGsm.arfcn << std::endl;
  out << "BSIC: " << cellIdentityGsm.bsic << std::endl;
  out << cellIdentityGsm.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_GSM_SignalStrength_v12& signalStrengthGsm) {
  out << "GSM Signal Strength: " << std::endl;
  out << "Signal Strength: " << signalStrengthGsm.signalStrength << std::endl;
  out << "Bit Error Rate: " << signalStrengthGsm.bitErrorRate << std::endl;
  out << "Timing Advance: " << signalStrengthGsm.timingAdvance;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoGsm_v12& cellInfoGsm) {
  out << "GSM Cell Info: " << std::endl;
  out << cellInfoGsm.cellIdentityGsm << std::endl;
  out << cellInfoGsm.signalStrengthGsm;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityCdma& cellIdentityCdma) {
  out << "CDMA Cell Identity: " << std::endl;
  out << "Network ID: " << cellIdentityCdma.networkId << std::endl;
  out << "System ID: " << cellIdentityCdma.systemId << std::endl;
  out << "Base Station ID: " << cellIdentityCdma.basestationId << std::endl;
  out << "Longitude: " << cellIdentityCdma.longitude << std::endl;
  out << "Latitude: " << cellIdentityCdma.latitude << std::endl;
  out << cellIdentityCdma.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CDMA_SignalStrength& signalStrengthCdma) {
  out << "CDMA Signal Strength: " << std::endl;
  out << "RSSI (DBM): " << signalStrengthCdma.dbm << std::endl;
  out << "Ec/Io: " << signalStrengthCdma.ecio;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_EVDO_SignalStrength& signalStrengthEvdo) {
  out << "EVDO Signal Strength: " << std::endl;
  out << "RSSI (DBM): " << signalStrengthEvdo.dbm << std::endl;
  out << "Ec/Io: " << signalStrengthEvdo.ecio << std::endl;
  out << "SNR: " << signalStrengthEvdo.signalNoiseRatio;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoCdma& cellInfoCdma) {
  out << "CDMA Cell Info : " << std::endl;
  out << cellInfoCdma.cellIdentityCdma << std::endl;
  out << cellInfoCdma.signalStrengthCdma << std::endl;
  out << cellInfoCdma.signalStrengthEvdo;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityLte_v12& cellIdentityLte) {
  out << "LTE Cell Identity: " << std::endl;
  outputMccMnc(out, cellIdentityLte) << std::endl;
  out << "Cell ID: " << cellIdentityLte.ci << std::endl;
  out << "Physical Cell ID: " << cellIdentityLte.pci << std::endl;
  out << "TAC: " << cellIdentityLte.tac << std::endl;
  out << "E-ARFCN: " << cellIdentityLte.earfcn << std::endl;
  out << "Bandwidth: " << cellIdentityLte.bandwidth << std::endl;
  out << "Band: " << cellIdentityLte.band << std::endl;
  out << cellIdentityLte.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_LTE_SignalStrength_v8& signalStrengthLte) {
  out << "LTE Signal Strength: " << std::endl;
  out << "Signal Strength: " << signalStrengthLte.signalStrength << std::endl;
  out << "RSRP: " << signalStrengthLte.rsrp << std::endl;
  out << "RSRQ: " << signalStrengthLte.rsrq << std::endl;
  out << "RSSNR: " << signalStrengthLte.rssnr << std::endl;
  out << "CQI: " << signalStrengthLte.cqi << std::endl;
  out << "Timing Advance: " << signalStrengthLte.timingAdvance;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellConfigLte& cellConfigLte) {
  out << "LTE Cell Config: " << std::endl;
  out << "ENDC Available: " << static_cast<bool>(cellConfigLte.isEndcAvailable);
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoLte_v12& cellInfoLte) {
  out << "LTE Cell Info: " << std::endl;
  out << cellInfoLte.cellIdentityLte << std::endl;
  out << cellInfoLte.signalStrengthLte << std::endl;
  out << cellInfoLte.cellConfig;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityWcdma_v12& cellIdentityWcdma) {
  out << "WCDMA Cell Identity: " << std::endl;
  outputMccMnc(out, cellIdentityWcdma) << std::endl;
  out << "LAC: " << cellIdentityWcdma.lac << std::endl;
  out << "Cell ID: " << cellIdentityWcdma.cid << std::endl;
  out << "PSC: " << cellIdentityWcdma.psc << std::endl;
  out << "U-ARFCN: " << cellIdentityWcdma.uarfcn << std::endl;
  out << cellIdentityWcdma.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_SignalStrengthWcdma& signalStrengthWcdma) {
  out << "WCDMA Signal Strength: " << std::endl;
  out << "Signal Strength: " << signalStrengthWcdma.signalStrength << std::endl;
  out << "Bit Error Rate: " << signalStrengthWcdma.bitErrorRate;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoWcdma_v12& cellInfoWcdma) {
  out << "WCDMA Cell Info: " << std::endl;
  out << cellInfoWcdma.cellIdentityWcdma << std::endl;
  out << cellInfoWcdma.signalStrengthWcdma;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityTdscdma& cellIdentityTdscdma) {
  out << "TDSCDMA Cell Identity: " << std::endl;
  outputMccMnc(out, cellIdentityTdscdma) << std::endl;
  out << "LAC: " << cellIdentityTdscdma.lac << std::endl;
  out << "Cell ID: " << cellIdentityTdscdma.cid << std::endl;
  out << "CPID: " << cellIdentityTdscdma.cpid << std::endl;
  out << "U-ARFCN: " << cellIdentityTdscdma.uarfcn << std::endl;
  out << cellIdentityTdscdma.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_TD_SCDMA_SignalStrength& signalStrengthTdscdma) {
  out << "TDSCDMA Signal Strength: " << std::endl;
  out << "RSSI: " << signalStrengthTdscdma.signalStrength << std::endl;
  out << "Bit Error Rate: " << signalStrengthTdscdma.bitErrorRate << std::endl;
  out << "RSCP: " << signalStrengthTdscdma.rscp;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoTdscdma& cellInfoTdscdma) {
  out << "TDSCDMA Cell Info: " << std::endl;
  out << cellInfoTdscdma.cellIdentityTdscdma << std::endl;
  out << cellInfoTdscdma.signalStrengthTdscdma;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityNr& cellIdentityNr) {
  out << "5G NR Cell Identity: " << std::endl;
  outputMccMnc(out, cellIdentityNr) << std::endl;
  out << "NR Cell ID: " << cellIdentityNr.nci << std::endl;
  out << "Physical Cell ID: " << cellIdentityNr.pci << std::endl;
  out << "TAC: " << cellIdentityNr.tac << std::endl;
  out << "NR-ARFCN: " << cellIdentityNr.nrarfcn << std::endl;
  out << "Bandwidth: " << cellIdentityNr.bandwidth << std::endl;
  out << "Band: " << cellIdentityNr.band << std::endl;
  out << cellIdentityNr.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_NR_SignalStrength& signalStrengthNr) {
  out << "5G NR Signal Strength: " << std::endl;
  out << "SS-RSRP: " << signalStrengthNr.ssRsrp << std::endl;
  out << "SS-RSRQ: " << signalStrengthNr.ssRsrq << std::endl;
  out << "SS-SINR: " << signalStrengthNr.ssSinr << std::endl;
  out << "CSI-RSRP: " << signalStrengthNr.csiRsrp << std::endl;
  out << "CSI-RSRQ: " << signalStrengthNr.csiRsrq << std::endl;
  out << "CSI-SINR: " << signalStrengthNr.csiSinr;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoNr& cellInfoNr) {
  out << "5G NR Cell Info: " << std::endl;
  out << cellInfoNr.cellIdentityNr << std::endl;
  out << cellInfoNr.signalStrengthNr;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfo_v12& cellInfo) {
  out << "Cell Info: " << std::endl;
  out << "Registered: " << cellInfo.registered << std::endl;
  out << cellInfo.timeStampType << std::endl;
  out << "Time Stamp: " << cellInfo.timeStamp << std::endl;
  out << cellInfo.connStatus << std::endl;

  switch (cellInfo.cellInfoType) {
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM:
      out << cellInfo.CellInfo.gsm << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA:
      out << cellInfo.CellInfo.cdma << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE:
      out << cellInfo.CellInfo.lte << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA:
      out << cellInfo.CellInfo.wcdma << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA:
      out << cellInfo.CellInfo.tdscdma << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_NR:
      out << cellInfo.CellInfo.nr << std::endl;
      break;
    default:
      break;
  }

  return out;
}

static PyObject* translate(const RIL_GSM_SignalStrength_v12& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "signalStrength", PyLong_FromLong(signalStrength.signalStrength));
    PyDict_SetItemString(dSignalStrength, "bitErrorRate", PyLong_FromLong(signalStrength.bitErrorRate));
    PyDict_SetItemString(dSignalStrength, "timingAdvance", PyLong_FromLong(signalStrength.timingAdvance));

    return dSignalStrength;
}

static PyObject* translate(const RIL_CellInfoGsm_v12& cellInfo) {
    PyObject* dCellInfo = PyDict_New();
    if (dCellInfo == nullptr) {
        return nullptr;
    }

    PyObject* dCellIdentity = translate(cellInfo.cellIdentityGsm);
    if (dCellIdentity != nullptr) {
        PyDict_SetItemString(dCellInfo, "cellIdentityGsm", dCellIdentity);
    }

    PyObject* dSignalStrength = translate(cellInfo.signalStrengthGsm);
    if (dSignalStrength != nullptr) {
        PyDict_SetItemString(dCellInfo, "signalStrengthGsm", dSignalStrength);
    }

    return dCellInfo;
}

static PyObject* translate(const RIL_CellInfoCdma& cellInfo) {
    PyObject* dCellInfo = PyDict_New();
    if (dCellInfo == nullptr) {
        return nullptr;
    }

    PyObject* dCellIdentity = translate(cellInfo.cellIdentityCdma);
    if (dCellIdentity != nullptr) {
        PyDict_SetItemString(dCellInfo, "cellIdentityCdma", dCellIdentity);
    }

    PyObject* dCdmaSignalStrength = translate(cellInfo.signalStrengthCdma);
    if (dCdmaSignalStrength != nullptr) {
        PyDict_SetItemString(dCellInfo, "signalStrengthCdma", dCdmaSignalStrength);
    }

    PyObject* dEvdoSignalStrength = translate(cellInfo.signalStrengthEvdo);
    if (dEvdoSignalStrength != nullptr) {
        PyDict_SetItemString(dCellInfo, "signalStrengthEvdo", dEvdoSignalStrength);
    }

    return dCellInfo;
}

static PyObject* translate(const RIL_CellConfigLte& lteCellConfig) {
    PyObject* dLteCellConfig = PyDict_New();
    if (dLteCellConfig == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dLteCellConfig, "isEndcAvailable", PyLong_FromLong(lteCellConfig.isEndcAvailable));

    return dLteCellConfig;
}

static PyObject* translate(const RIL_CellInfoLte_v12& cellInfo) {
    PyObject* dCellInfo = PyDict_New();
    if (dCellInfo == nullptr) {
        return nullptr;
    }

    PyObject* dCellIdentity = translate(cellInfo.cellIdentityLte);
    if (dCellIdentity != nullptr) {
        PyDict_SetItemString(dCellInfo, "cellIdentityLte", dCellIdentity);
    }

    PyObject* dSignalStrength = translate(cellInfo.signalStrengthLte);
    if (dSignalStrength != nullptr) {
        PyDict_SetItemString(dCellInfo, "signalStrengthLte", dSignalStrength);
    }

    PyObject* dCellConfig = translate(cellInfo.cellConfig);
    if (dCellConfig != nullptr) {
        PyDict_SetItemString(dCellInfo, "cellConfig", dCellConfig);
    }

    return dCellInfo;
}

static PyObject* translate(const RIL_SignalStrengthWcdma& signalStrength) {
    PyObject* dSignalStrength = PyDict_New();
    if (dSignalStrength == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSignalStrength, "signalStrength", PyLong_FromLong(signalStrength.signalStrength));
    PyDict_SetItemString(dSignalStrength, "bitErrorRate", PyLong_FromLong(signalStrength.bitErrorRate));

    return dSignalStrength;
}

static PyObject* translate(const RIL_CellInfoWcdma_v12& cellInfo) {
    PyObject* dCellInfo = PyDict_New();
    if (dCellInfo == nullptr) {
        return nullptr;
    }

    PyObject* dCellIdentity = translate(cellInfo.cellIdentityWcdma);
    if (dCellIdentity != nullptr) {
        PyDict_SetItemString(dCellInfo, "cellIdentityWcdma", dCellIdentity);
    }

    PyObject* dSignalStrength = translate(cellInfo.signalStrengthWcdma);
    if (dSignalStrength != nullptr) {
        PyDict_SetItemString(dCellInfo, "signalStrengthWcdma", dSignalStrength);
    }

    return dCellInfo;
}

static PyObject* translate(const RIL_CellInfoTdscdma& cellInfo) {
    PyObject* dCellInfo = PyDict_New();
    if (dCellInfo == nullptr) {
        return nullptr;
    }

    PyObject* dCellIdentity = translate(cellInfo.cellIdentityTdscdma);
    if (dCellIdentity != nullptr) {
        PyDict_SetItemString(dCellInfo, "cellIdentityTdscdma", dCellIdentity);
    }

    PyObject* dSignalStrength = translate(cellInfo.signalStrengthTdscdma);
    if (dSignalStrength != nullptr) {
        PyDict_SetItemString(dCellInfo, "signalStrengthTdscdma", dSignalStrength);
    }

    return dCellInfo;
}

static PyObject* translate(const RIL_CellInfoNr& cellInfo) {
    PyObject* dCellInfo = PyDict_New();
    if (dCellInfo == nullptr) {
        return nullptr;
    }

    PyObject* dCellIdentity = translate(cellInfo.cellIdentityNr);
    if (dCellIdentity != nullptr) {
        PyDict_SetItemString(dCellInfo, "cellIdentityNr", dCellIdentity);
    }

    PyObject* dSignalStrength = translate(cellInfo.signalStrengthNr);
    if (dSignalStrength != nullptr) {
        PyDict_SetItemString(dCellInfo, "signalStrengthNr", dSignalStrength);
    }

    return dCellInfo;
}

static PyObject* translate(const RIL_CellInfo_v12& cellInfo) {
    PyObject* dCellInfo = PyDict_New();
    if (dCellInfo == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dCellInfo, "registered", PyLong_FromLong(cellInfo.registered));
    PyDict_SetItemString(dCellInfo, "timeStampType", PyLong_FromLong(cellInfo.timeStampType));
    PyDict_SetItemString(dCellInfo, "timeStamp", PyLong_FromLong(cellInfo.timeStamp));
    PyDict_SetItemString(dCellInfo, "connStatus", PyLong_FromLong(cellInfo.connStatus));
    PyDict_SetItemString(dCellInfo, "cellInfoType", PyLong_FromLong(cellInfo.cellInfoType));

    switch (cellInfo.cellInfoType) {
        case RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM:
            {
                PyObject* dCellInfoGsm = translate(cellInfo.CellInfo.gsm);
                if (dCellInfoGsm != nullptr) {
                    PyDict_SetItemString(dCellInfo, "CellInfo.gsm", dCellInfoGsm);
                }
            }
            break;
        case RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA:
            {
                PyObject* dCellInfoCdma = translate(cellInfo.CellInfo.cdma);
                if (dCellInfoCdma != nullptr) {
                    PyDict_SetItemString(dCellInfo, "CellInfo.cdma", dCellInfoCdma);
                }
            }
            break;
        case RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE:
            {
                PyObject* dCellInfoLte = translate(cellInfo.CellInfo.lte);
                if (dCellInfoLte != nullptr) {
                    PyDict_SetItemString(dCellInfo, "CellInfo.lte", dCellInfoLte);
                }
            }
            break;
        case RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA:
            {
                PyObject* dCellInfoWcdma = translate(cellInfo.CellInfo.wcdma);
                if (dCellInfoWcdma != nullptr) {
                    PyDict_SetItemString(dCellInfo, "CellInfo.wcdma", dCellInfoWcdma);
                }
            }
            break;
        case RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA:
            {
                PyObject* dCellInfoTdscdma = translate(cellInfo.CellInfo.tdscdma);
                if (dCellInfoTdscdma != nullptr) {
                    PyDict_SetItemString(dCellInfo, "CellInfo.tdscdma", dCellInfoTdscdma);
                }
            }
            break;
        case RIL_CellInfoType::RIL_CELL_INFO_TYPE_NR:
            {
                PyObject* dCellInfoNr = translate(cellInfo.CellInfo.nr);
                if (dCellInfoNr != nullptr) {
                    PyDict_SetItemString(dCellInfo, "CellInfo.nr", dCellInfoNr);
                }
            }
            break;
        default:
            break;
    }

    return dCellInfo;
}

static PyObject* translate(const RIL_CellInfo_v12 cellInfoList[], const size_t cellInfoListLen) {
    PyObject* lCellInfoList = PyList_New(0);
    if (lCellInfoList == nullptr) {
        return nullptr;
    }

    if (cellInfoList == nullptr) {
        return lCellInfoList;
    }

    for (size_t i = 0; i < cellInfoListLen; i++) {
        PyObject* dCellInfo = translate(cellInfoList[i]);
        if (dCellInfo != nullptr) {
            PyList_Append(lCellInfoList, dCellInfo);
        }
    }

    return lCellInfoList;
}

/*----------------------------< Cell Info Request and Response >----------------------------------*/
static PyObject* getCellInfo(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getCellInfo");
    if (!PyCallable_Check(callback)){
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.getCellInfo(
        [callback] (RIL_Errno e, const RIL_CellInfo_v12 cellInfoList[], const size_t cellInfoListLen) -> void {
            PyObject* lCellInfoList = translate(cellInfoList, cellInfoListLen);
            if (lCellInfoList == nullptr) {
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, lCellInfoList);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << std::endl
                << (e == RIL_Errno::RIL_E_SUCCESS ? "Successfully received" : "Failed to receive")
                << " information about serving/neighboring cells." << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            std::for_each(
                cellInfoList,
                cellInfoList + cellInfoListLen,
                [] (const RIL_CellInfo_v12& cellInfo) {
                std::cout << cellInfo << std::endl;
                ofs << cellInfo << std::endl;
        }
      );

      ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to issue request to fetch information about serving/neighboring cells.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Get Call info request sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
};

/*----------------------------< get CDMA SMS Broadcast Config >----------------------------------*/
static PyObject* getCdmaSmsBroadcastConfig(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getCdmaSmsBroadcastConfig");
    if (!PyCallable_Check(callback)){
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.getCdmaSmsBroadcastConfig(
        [callback] (RIL_Errno e, const RIL_CDMA_BroadcastSmsConfigInfo* configInfo, size_t configSize) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for get cdma sms broadcast config: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (configInfo != nullptr && configSize) {
                for (int i = 0; i < configSize; i++) {
                    std::cout << "config " << i << ":" << std::endl
                    << " - service_category: "<< configInfo[i].service_category << std::endl
                    << " - language: " << configInfo[i].language << std::endl
                    << " - selected: " << configInfo[i].selected << std::endl;

                    ofs << "config: " << i << std::endl
                    << "service_category: "<< configInfo[i].service_category << std::endl
                    << "language: " << configInfo[i].language << std::endl
                    << "selected: " << configInfo[i].selected << std::endl;
                }
            }
            ofs.close();
        }
      );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send get CDMA sms broadcast config req");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Succeeded to send get CDMA sms broadcast config req" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
};

void printSignalInfo(RIL_GW_SignalStrength info, std::fstream &ofs){
    std::cout << "GW Signal Strength Information: " << std::endl;
    std::cout << "signalStrength = " << info.signalStrength << std::endl;
    std::cout << "bitErrorRate = " << info.bitErrorRate << std::endl;
    std::cout << std::endl;
    ofs << "GW Signal Strength Information: " << std::endl;
    ofs << "signalStrength: " << info.signalStrength << std::endl;
    ofs << "bitErrorRate: " << info.bitErrorRate << std::endl;
}
void printSignalInfo(RIL_WCDMA_SignalStrength info, std::fstream &ofs){
    if (!info.valid) return;
    std::cout << "WCDMA Signal Strength Information: " << std::endl;
    std::cout << "SignalStrength = " << info.signalStrength << std::endl;
    std::cout << "bitErrorRate = " << info.bitErrorRate << std::endl;
    std::cout << "rscp = " << info.rscp << std::endl;
    std::cout << "ecio = " << info.ecio << std::endl;
    std::cout << "valid = " << info.valid << std::endl;
    std::cout << std::endl;
    ofs << "WCDMA Signal Strength Information: " << std::endl;
    ofs << "SignalStrength: " << info.signalStrength << std::endl;
    ofs << "bitErrorRate: " << info.bitErrorRate << std::endl;
    ofs << "rscp: " << info.rscp << std::endl;
    ofs << "ecio: " << info.ecio << std::endl;
    ofs << "valid: " << info.valid << std::endl;
}
void printSignalInfo(RIL_CDMA_SignalStrength info, std::fstream &ofs){
    std::cout << "CDMA Signal Strength Information: " << std::endl;
    std::cout << "dbm = " << info.dbm << std::endl;
    std::cout << "ecio = " << info.ecio << std::endl;
    std::cout << std::endl;
    ofs <<  "CDMA Signal Strength Information: " << std::endl;
    ofs << "dbm: " << info.dbm << std::endl;
    ofs << "ecio: " << info.ecio << std::endl;
}
void printSignalInfo(RIL_EVDO_SignalStrength info, std::fstream &ofs){
    std::cout << "EVDO Signal Strength Information: " << std::endl;
    std::cout << "dbm = " << info.dbm << std::endl;
    std::cout << "ecio = " << info.ecio << std::endl;
    std::cout << "signal Noise Ratio = " << info.signalNoiseRatio << std::endl;
    std::cout << std::endl;
    ofs << "EVDO Signal Strength Information: " << std::endl;
    ofs << "dbm: " << info.dbm << std::endl;
    ofs << "ecio: " << info.ecio << std::endl;
    ofs << "signal Noise Ratio: " << info.signalNoiseRatio << std::endl;
}
void printSignalInfo(RIL_LTE_SignalStrength_v8 info, std::fstream &ofs)
{
    std::cout << "LTE Signal Strength Information: "<< std::endl;
    std::cout << "signalStrength = " << info.signalStrength << std::endl;
    std::cout << "rsrp = " << info.rsrp << std::endl;
    std::cout << "rsrq = "<< info.rsrq <<std::endl;
    std::cout << "rssnr = " << info.rssnr << std::endl;
    std::cout << "cqi = "<< info.cqi << std::endl;
    std::cout << "timingAdvance = " << info.timingAdvance << std::endl;
    std::cout << std::endl;
    ofs << "LTE Signal Strength Information: "<< std::endl;
    ofs << "signalStrength: " << info.signalStrength << std::endl;
    ofs << "rsrp: " << info.rsrp << std::endl;
    ofs << "rsrq: "<< info.rsrq <<std::endl;
    ofs << "rssnr: " << info.rssnr << std::endl;
    ofs << "cqi: "<< info.cqi << std::endl;
    ofs << "timingAdvance: " << info.timingAdvance << std::endl;
}
void printSignalInfo(RIL_TD_SCDMA_SignalStrength info, std::fstream &ofs)
{
    std::cout << "EVDO Signal Strength Information: " << std::endl;
    std::cout << "bitErrorRate = " << info.bitErrorRate << std::endl;
    std::cout << "rscp = " << info.rscp << std::endl;
    std::cout << "signalStrength = " << info.signalStrength << std::endl;
    std::cout << std::endl;
    ofs << "EVDO Signal Strength Information: " << std::endl;
    ofs << "bitErrorRate: " << info.bitErrorRate << std::endl;
    ofs << "rscp: " << info.rscp << std::endl;
    ofs << "signalStrength: " << info.signalStrength << std::endl;
}
void printSignalInfo(RIL_NR_SignalStrength info, std::fstream &ofs){
    std::cout << "NR Signal Strength Information: " << std::endl;
    std::cout << "ssRsrp = " << info.ssRsrp << std::endl;
    std::cout << "ssRsrq = " << info.ssRsrq << std::endl;
    std::cout << "ssSinr = " << info.ssSinr << std::endl;
    std::cout << "csiRsrp = " << info.csiRsrp << std::endl;
    std::cout << "csiRsrq = " << info.csiRsrq << std::endl;
    std::cout << "csiSinr = " << info.csiSinr << std::endl;
    std::cout << std::endl;
    ofs <<  "NR Signal Strength Information: " << std::endl;
    ofs << "ssRsrp: " << info.ssRsrp << std::endl;
    ofs << "ssRsrq: " << info.ssRsrq << std::endl;
    ofs << "ssSinr: " << info.ssSinr << std::endl;
    ofs << "csiRsrp: " << info.csiRsrp << std::endl;
    ofs << "csiRsrq: " << info.csiRsrq << std::endl;
    ofs << "csiSinr: " << info.csiSinr << std::endl;
}
std::ostream& operator<<(std::ostream& os, const RIL_ActivityStatsInfo info) {
  os << "Modem Activity Info:" << std::endl;
  os << "sleep_mode_time_ms = " << info.sleep_mode_time_ms << std::endl;
  os << "idle_mode_time_ms = " << info.idle_mode_time_ms << std::endl;
  for(int i=0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
    os << "tx_mode_time_ms[" << i << "] = " << info.tx_mode_time_ms[i] << std::endl;
  }
  os << "rx_mode_time_ms = " << info.rx_mode_time_ms<< std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_CdmaSubscriptionSource source) {
    switch(source) {
        case CDMA_SUBSCRIPTION_SOURCE_INVALID:
            os << "INVALID";
        break;
        case CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM:
            os << "RUIM_SIM";
        break;
        case CDMA_SUBSCRIPTION_SOURCE_NV:
            os << "NV";
        break;
        default:
        break;
    }
    return os;
}
/*----------------------------< Signal Strength Request and Response >----------------------------*/
static PyObject* getSignalStrength(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getSignalStrength");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.getSignalStrength( [callback] (RIL_Errno e, const RIL_SignalStrength* signalInfo) -> void {
            PyObject* dSignalStrength = nullptr;
            if (signalInfo == nullptr) {
                dSignalStrength = PyDict_New();
            } else {
                dSignalStrength = translate(*signalInfo);
            }

            if (dSignalStrength == nullptr) {
                std::cout << "Failed to translate getSignalStrength response." << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dSignalStrength);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Signal Strength request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (signalInfo){
                printSignalInfo(signalInfo->GW_SignalStrength, ofs);
                printSignalInfo(signalInfo->WCDMA_SignalStrength, ofs);
                printSignalInfo(signalInfo->CDMA_SignalStrength, ofs);
                printSignalInfo(signalInfo->EVDO_SignalStrength, ofs);
                printSignalInfo(signalInfo->LTE_SignalStrength, ofs);
                printSignalInfo(signalInfo->TD_SCDMA_SignalStrength, ofs);
               printSignalInfo(signalInfo->NR_SignalStrength, ofs);
            }
            else {
                std::cout << "No Signal Strength recieved" << std::endl;
                ofs << "No Signal Strength recieved: " << std::endl;
            }

            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send requestSignalStrength");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Signal Strength request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translateOperatorNames(const char** operatorNames) {
    PyObject* dOperatorNames = PyDict_New();
    if (dOperatorNames == nullptr) {
        return nullptr;
    }

    if (operatorNames == nullptr) {
        std::cout << "operatorNames is null" << std::endl;
        return dOperatorNames;
    }

    if (operatorNames[0] != nullptr) {
        PyDict_SetItemString(dOperatorNames, "longName", PyUnicode_FromString(operatorNames[0]));
    }

    if (operatorNames[1] != nullptr) {
        PyDict_SetItemString(dOperatorNames, "shortName", PyUnicode_FromString(operatorNames[1]));
    }

    if (operatorNames[2] != nullptr) {
        PyDict_SetItemString(dOperatorNames, "numeric", PyUnicode_FromString(operatorNames[2]));
    }

    return dOperatorNames;
}

/*--------------------< Operator request and response >------------------------------------*/
static PyObject* operatorName(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("operatorName");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.operatorName([callback] (RIL_Errno e,  const char ** operName) -> void {
            PyObject* dOperatorNames = translateOperatorNames(operName);
            if (dOperatorNames == nullptr) {
                std::cout << "dOperatorNames is null, translation failed" << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dOperatorNames);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Operator Name request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (operName) {
                 if (operName[0]) {
                     std::cout << "longName : " << operName[0] << std::endl;
                     ofs << "longName: " << operName[0] << std::endl;
                 }
                 if (operName[1]) {
                     std::cout << "shortName : " << operName[1] << std::endl;
                     ofs << "shortName: " << operName[1] << std::endl;
                 }
                 if (operName[2]) {
                     std::cout << "numeric : " << operName[2] << std::endl;
                     ofs << "numeric: " << operName[2] << std::endl;
                 }

        } else {
          std::cout << "No Operator Name recieved" << std::endl;
          ofs << "No Operator Name recieved: " << std::endl;
        }
        ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send requestOperatorName");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Successfully sent request Opertator name request" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translate(const RIL_LastCallFailCauseInfo& cause) {
    PyObject* dLastCallFailCauseInfo = PyDict_New();
    if (dLastCallFailCauseInfo == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dLastCallFailCauseInfo, "cause_code", PyLong_FromLong(cause.cause_code));
    PyDict_SetItemString(dLastCallFailCauseInfo, "vendor_cause", PyUnicode_FromString(cause.vendor_cause));

    return dLastCallFailCauseInfo;
}

/*----------------------------< Last Call Fail Request and Response >----------------------------*/
static PyObject* lastCallFailCause(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("lastCallFailCause");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.lastCallFailCause( [callback] (RIL_Errno e, const RIL_LastCallFailCauseInfo& cause) -> void {
            PyObject* dLastCallFailCauseInfo = translate(cause);
            if (dLastCallFailCauseInfo == nullptr) {
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dLastCallFailCauseInfo);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Last call fail cause request: " << e << std::endl;
            std::cout << "Cause code: " << cause.cause_code << std::endl;
            if (cause.vendor_cause) {
                std::cout << "Vendor cause: " << cause.vendor_cause << std::endl;
            }
            ofs << "Callback error value: " << e << std::endl;
            ofs << "Cause code: " << cause.cause_code << std::endl;
            if (cause.vendor_cause) {
                ofs << "Vendor cause: " << cause.vendor_cause << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send request");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request sent succesfully " << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/* -------------------------------------< DTMF Request and Response >------------------------------------*/
static PyObject* dtmf(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* dtmf = nullptr;
    PyObject* callback = nullptr;

    if (!PyArg_ParseTuple(args, "sO", &dtmf, &callback)) {
        return NULL;
    }
    printInput("dtmf", dtmf);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.dtmf(
        dtmf[0],
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(iO)", e, Py_None);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for DTMF request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send DTMF");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "DTMF request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< simIOReq Request and Response >------------------------------------*/
static PyObject* simIOReq(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string command;
    std::string fileid;
    std::string path;
    std::string p1;
    std::string p2;
    std::string p3;
    std::string data;
    std::string pin2;
    std::string aidPtr;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sssssssssO",&command, &fileid, &path, &p1, &p2, &p3, &data, &pin2, &aidPtr, &callback)) {
        return NULL;
    }
    printInput("simIOReq", command, fileid, path, p1, p2, p3, data, pin2, aidPtr);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_SIM_IO_v6 reqparams {};
    reqparams.command = std::stoi(command, NULL, 16);
    reqparams.fileid  = std::stoi(fileid,NULL,16);
    reqparams.path    = const_cast<char *>(path.c_str());
    reqparams.p1      = std::stoi(p1);
    reqparams.p2      = std::stoi(p2);
    reqparams.p3      = std::stoi(p3);
    reqparams.data    = const_cast<char *>(data.c_str());
    reqparams.pin2 = const_cast<char*>(pin2.c_str());
    reqparams.aidPtr = const_cast<char*>(aidPtr.c_str());
    Status s = rilSession.simIOReq(
        reqparams,
        [callback] (RIL_Errno e, const RIL_SIM_IO_Response *res) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for get simIO request: " << e <<std::endl;
            std::cout<< "sw1: " << res->sw1 <<std::endl;
            std::cout<< "sw2: " << res->sw2 <<std::endl;
            ofs << "Callback error value: " << e <<std::endl;
            ofs << "sw1: " << res->sw1 <<std::endl;
            ofs << "sw2: " << res->sw2 <<std::endl;
            if(res->simResponse != nullptr)
            {
                std::cout<<"simresponse: "<<res->simResponse<<std::endl;
                ofs <<"simresponse: "<<res->simResponse<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for SimIo request");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for SimIo request" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}
std::string dump(const RIL_Qos& qos) {
    std::stringstream os;
    os<<"qosType="<<qos.qosType;
    if (qos.qosType == QOS_TYPE_EPS) {
        os<<" qci="<<(int)qos.eps.qci;
        os<<" dl={max="<<qos.eps.downlink.maxBitrateKbps
            <<","<<qos.eps.downlink.guaranteedBitrateKbps<<"}";
        os<<" ul={max="<<qos.eps.uplink.maxBitrateKbps
            <<","<<qos.eps.uplink.guaranteedBitrateKbps<<"}"<<std::endl;
    } else if (qos.qosType == QOS_TYPE_NR) {
        os<<" fiveQi="<<(int)qos.nr.fiveQi;
        os<<" dl={max="<<qos.nr.downlink.maxBitrateKbps
            <<","<<qos.nr.downlink.guaranteedBitrateKbps<<"}";
        os<<" ul={max="<<qos.nr.uplink.maxBitrateKbps
            <<","<<qos.nr.uplink.guaranteedBitrateKbps<<"}";
        os<<" qfi="<<(int)qos.nr.qfi;
        os<<" averagingWindowMs="<<(int)qos.nr.averagingWindowMs<<std::endl;
    }
    return os.str();
}

std::string dump(const RIL_QosFilter& qosFilter, std::string padding) {
    std::stringstream os;
    os << padding << "local={";
    for (int i = 0; i < qosFilter.localAddressesLength; i++) {
        os << qosFilter.localAddresses[i] << ",";
    }
    os << "}";
    os << " remote={";
    for (int i = 0; i < qosFilter.remoteAddressesLength; i++) {
        os << qosFilter.remoteAddresses[i] << ",";
    }
    os << "}";
    if (qosFilter.localPortValid) {
        os << " localPort={" << qosFilter.localPort.start
            << "," << qosFilter.localPort.end << "}";
    }
    if (qosFilter.remotePortValid) {
        os << " remotePort={" << qosFilter.remotePort.start
            << "," << qosFilter.remotePort.end << "}";
    }
    os << " protocol=" << (int)qosFilter.protocol;
    if (qosFilter.tosValid) {
        os << " tos=" << (int)qosFilter.tos;
    }
    if (qosFilter.flowLabelValid) {
        os << " flowLabel=" << qosFilter.flowLabel;
    }
    if (qosFilter.spiValid) {
        os << " spi=" << qosFilter.spi;
    }
    os << " dir=" << (int)qosFilter.direction;
    os << " precedence=" << (int)qosFilter.precedence << std::endl;
    return os.str();
}

std::string dump(const RIL_QosSession& qosSession, std::string padding) {
    std::stringstream os;
    os << padding << "id=" << qosSession.qosSessionId << " ";
    dump(qosSession.qos);
    for (int i = 0; i < qosSession.qosFiltersLength; i++) {
        dump(qosSession.qosFilters[i], padding + "  ");
    }
    return os.str();
}

static PyObject* translate(const RIL_TrafficDescriptor* trafficDescriptors,
        const size_t numTrafficDescriptors) {
    PyObject* lTrafficDescriptors = PyList_New(0);
    if (lTrafficDescriptors == nullptr) {
        return nullptr;
    }

    if (trafficDescriptors == nullptr) {
        return lTrafficDescriptors;
    }

    for (size_t i = 0; i < numTrafficDescriptors; i++) {
        PyObject* dTrafficDescriptor = PyDict_New();
        if (dTrafficDescriptor == nullptr) {
            return nullptr;
        }

        if (trafficDescriptors[i].dnnValid && trafficDescriptors[i].dnn != nullptr) {
            PyDict_SetItemString(dTrafficDescriptor, "dnn", PyUnicode_FromString(trafficDescriptors[i].dnn));
        }

        if (trafficDescriptors[i].osAppIdValid && trafficDescriptors[i].osAppIdLength > 0) {
            PyDict_SetItemString(dTrafficDescriptor, "osAppId",
                PyUnicode_FromStringAndSize(reinterpret_cast<const char*>(trafficDescriptors[i].osAppId),
                    trafficDescriptors[i].osAppIdLength));
        }

        PyList_Append(lTrafficDescriptors, dTrafficDescriptor);
    }

    return lTrafficDescriptors;
}

static PyObject* translate(const RIL_SliceInfo& sliceInfo) {
    PyObject* dSliceInfo = PyDict_New();
    if (dSliceInfo == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSliceInfo, "sst", PyLong_FromLong(sliceInfo.sst));
    PyDict_SetItemString(dSliceInfo, "sliceDifferentiator", PyLong_FromLong(sliceInfo.sliceDifferentiator));
    PyDict_SetItemString(dSliceInfo, "mappedHplmnSst", PyLong_FromLong(sliceInfo.mappedHplmnSst));
    PyDict_SetItemString(dSliceInfo, "mappedHplmnSD", PyLong_FromLong(sliceInfo.mappedHplmnSD));
    PyDict_SetItemString(dSliceInfo, "status", PyLong_FromLong(sliceInfo.status));

    return dSliceInfo;
}

static PyObject* translate(const RIL_PortRange& portRange) {
    PyObject* dPortRange = PyDict_New();
    if (dPortRange == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dPortRange, "start", PyLong_FromLong(portRange.start));
    PyDict_SetItemString(dPortRange, "end", PyLong_FromLong(portRange.end));

    return dPortRange;
}

static PyObject* translate(const RIL_QosFilter* qosFilters, const size_t numQosFilters) {
    PyObject* lQosFilters = PyList_New(0);
    if (lQosFilters == nullptr) {
        return nullptr;
    }

    if (qosFilters == nullptr) {
        return lQosFilters;
    }

    for (size_t i = 0; i < numQosFilters; i++) {
        PyObject* dQosFilter = PyDict_New();
        if (dQosFilter == nullptr) {
            return nullptr;
        }

        if (qosFilters[i].localAddressesLength > 0) {
            PyObject* lLocalAddresses = PyList_New(0);
            if (lLocalAddresses == nullptr) {
                return nullptr;
            }

            for (size_t j = 0; j < qosFilters[i].localAddressesLength; j++) {
                if (qosFilters[i].localAddresses[j] != nullptr) {
                    PyList_Append(lLocalAddresses,
                        PyUnicode_FromString(qosFilters[i].localAddresses[j]));
                }
            }

            PyDict_SetItemString(dQosFilter, "localAddresses", lLocalAddresses);
        }

        if (qosFilters[i].remoteAddressesLength > 0) {
            PyObject* lRemoteAddresses = PyList_New(0);
            if (lRemoteAddresses == nullptr) {
                return nullptr;
            }

            for (size_t j = 0; j < qosFilters[i].remoteAddressesLength; j++) {
                if (qosFilters[i].remoteAddresses[j] != nullptr) {
                    PyList_Append(lRemoteAddresses,
                        PyUnicode_FromString(qosFilters[i].remoteAddresses[j]));
                }
            }

            PyDict_SetItemString(dQosFilter, "remoteAddresses", lRemoteAddresses);
        }

        if (qosFilters[i].localPortValid) {
            PyObject* dLocalPort = translate(qosFilters[i].localPort);
            if (dLocalPort != nullptr) {
                PyDict_SetItemString(dQosFilter, "localPort", dLocalPort);
            }
        }

        if (qosFilters[i].remotePortValid) {
            PyObject* dRemotePort = translate(qosFilters[i].remotePort);
            if (dRemotePort != nullptr) {
                PyDict_SetItemString(dQosFilter, "remotePort", dRemotePort);
            }
        }

        PyDict_SetItemString(dQosFilter, "protocol", PyLong_FromLong(qosFilters[i].protocol));

        if (qosFilters[i].tosValid) {
            PyDict_SetItemString(dQosFilter, "tos", PyLong_FromLong(qosFilters[i].tos));
        }

        if (qosFilters[i].flowLabelValid) {
            PyDict_SetItemString(dQosFilter, "flowLabel", PyLong_FromLong(qosFilters[i].flowLabel));
        }

        if (qosFilters[i].spiValid) {
            PyDict_SetItemString(dQosFilter, "spi", PyLong_FromLong(qosFilters[i].spi));
        }

        PyDict_SetItemString(dQosFilter, "direction", PyLong_FromLong(qosFilters[i].direction));
        PyDict_SetItemString(dQosFilter, "precedence", PyLong_FromLong(qosFilters[i].precedence));

        PyList_Append(lQosFilters, dQosFilter);
    }

    return lQosFilters;
}

static PyObject* translate(const RIL_QosBandwidth& bandwidth) {
    PyObject* dBandwidth = PyDict_New();
    if (dBandwidth == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dBandwidth, "maxBitrateKbps",
        PyLong_FromLong(bandwidth.maxBitrateKbps));
    PyDict_SetItemString(dBandwidth, "guaranteedBitrateKbps",
        PyLong_FromLong(bandwidth.guaranteedBitrateKbps));

    return dBandwidth;
}

static PyObject* translate(const RIL_EpsQos& epsQos) {
    PyObject* dEpsQos = PyDict_New();
    if (dEpsQos == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dEpsQos, "qci", PyLong_FromLong(epsQos.qci));

    PyObject* dDownlink = translate(epsQos.downlink);
    if (dDownlink != nullptr) {
        PyDict_SetItemString(dEpsQos, "downlink", dDownlink);
    }

    PyObject* dUplink = translate(epsQos.uplink);
    if (dUplink != nullptr) {
        PyDict_SetItemString(dEpsQos, "uplink", dUplink);
    }

    return dEpsQos;
}

static PyObject* translate(const RIL_NrQos& nrQos) {
    PyObject* dNrQos = PyDict_New();
    if (dNrQos == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dNrQos, "fiveQi", PyLong_FromLong(nrQos.fiveQi));

    PyObject* dDownlink = translate(nrQos.downlink);
    if (dDownlink != nullptr) {
        PyDict_SetItemString(dNrQos, "downlink", dDownlink);
    }

    PyObject* dUplink = translate(nrQos.uplink);
    if (dUplink != nullptr) {
        PyDict_SetItemString(dNrQos, "uplink", dUplink);
    }

    PyDict_SetItemString(dNrQos, "qfi", PyLong_FromLong(nrQos.qfi));
    PyDict_SetItemString(dNrQos, "averagingWindowMs",
        PyLong_FromLong(nrQos.averagingWindowMs));

    return dNrQos;
}

static PyObject* translate(const RIL_Qos& qos) {
    if (qos.qosType == RIL_QosType::QOS_TYPE_NOINIT) {
        return nullptr;
    }

    PyObject* dQos = PyDict_New();
    if (dQos == nullptr) {
        return nullptr;
    }

    switch (qos.qosType) {
        case RIL_QosType::QOS_TYPE_EPS:
            {
                PyObject* dEpsQos = translate(qos.eps);
                if (dEpsQos != nullptr) {
                    PyDict_SetItemString(dQos, "eps", dEpsQos);
                }
                break;
            }
        case RIL_QosType::QOS_TYPE_NR:
            {
                PyObject* dNrQos = translate(qos.nr);
                if (dNrQos != nullptr) {
                    PyDict_SetItemString(dQos, "nr", dNrQos);
                }
                break;
            }
        default:
            break;
    }

    return dQos;
}

static PyObject* translate(const RIL_QosSession* qosSessions, const size_t numQosSessions) {
    PyObject* lQosSessions = PyList_New(0);
    if (lQosSessions == nullptr) {
        return nullptr;
    }

    if (qosSessions == nullptr) {
        return lQosSessions;
    }

    for (size_t i = 0; i < numQosSessions; i++) {
        PyObject* dQosSession = PyDict_New();
        if (dQosSession == nullptr) {
            return nullptr;
        }

        PyDict_SetItemString(dQosSession, "qosSessionId",
            PyLong_FromLong(qosSessions[i].qosSessionId));

        PyObject* dQos = translate(qosSessions[i].qos);
        if (dQos != nullptr) {
            PyDict_SetItemString(dQosSession, "qos", dQos);
        }

        PyObject* lQosFilters = translate(qosSessions[i].qosFilters,
            qosSessions[i].qosFiltersLength);
        if (lQosFilters != nullptr) {
            PyDict_SetItemString(dQosSession, "qosFilters", lQosFilters);
        }

        PyList_Append(lQosSessions, dQosSession);
    }

    return lQosSessions;
}

static PyObject* translate(const RIL_LinkAddress* linkAddresses, const size_t numLinkAddresses) {
    PyObject* lLinkAddresses = PyList_New(0);
    if (lLinkAddresses == nullptr) {
        return nullptr;
    }

    if (linkAddresses == nullptr) {
        return lLinkAddresses;
    }

    for (size_t i = 0; i < numLinkAddresses; i++) {
        PyObject* dLinkAddress = PyDict_New();
        if (dLinkAddress == nullptr) {
            return nullptr;
        }

        if (linkAddresses[i].address != nullptr) {
            PyDict_SetItemString(dLinkAddress, "address", PyUnicode_FromString(linkAddresses[i].address));
        }

        PyDict_SetItemString(dLinkAddress, "properties", PyLong_FromLong(linkAddresses[i].properties));
        PyDict_SetItemString(dLinkAddress, "deprecationTime", PyLong_FromLong(linkAddresses[i].deprecationTime));
        PyDict_SetItemString(dLinkAddress, "expirationTime", PyLong_FromLong(linkAddresses[i].expirationTime));

        PyList_Append(lLinkAddresses, dLinkAddress);
    }

    return lLinkAddresses;
}

static PyObject* translate(const RIL_Data_Call_Response_v11& response) {
    PyObject* dDataCallResponse = PyDict_New();
    if (dDataCallResponse == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dDataCallResponse, "status", PyLong_FromLong(response.status));
    PyDict_SetItemString(dDataCallResponse, "suggestedRetryTime", PyLong_FromLong(response.suggestedRetryTime));
    PyDict_SetItemString(dDataCallResponse, "cid", PyLong_FromLong(response.cid));
    PyDict_SetItemString(dDataCallResponse, "active", PyLong_FromLong(response.active));

    if (response.type != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "type", PyUnicode_FromString(response.type));
    }

    if (response.ifname != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "ifname", PyUnicode_FromString(response.ifname));
    }

    /*if (response.addresses != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "addresses", PyUnicode_FromString(response.addresses));
    }*/

    if (response.dnses != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "dnses", PyUnicode_FromString(response.dnses));
    }

    if (response.gateways != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "gateways", PyUnicode_FromString(response.gateways));
    }

    if (response.pcscf != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "pcscf", PyUnicode_FromString(response.pcscf));
    }

    PyDict_SetItemString(dDataCallResponse, "mtu", PyLong_FromLong(response.mtu));
    PyDict_SetItemString(dDataCallResponse, "mtuV4", PyLong_FromLong(response.mtuV4));
    PyDict_SetItemString(dDataCallResponse, "mtuV6", PyLong_FromLong(response.mtuV6));

    if (response.linkAddressesLength > 0) {
        PyObject* lLinkAddresses = translate(
            response.linkAddresses, response.linkAddressesLength);
        if (lLinkAddresses != nullptr) {
            PyDict_SetItemString(dDataCallResponse, "linkAddresses", lLinkAddresses);
        }
    }

    // TODO: Needed until marshalling of RIL_Data_Call_Response_v11 is fixed
    if (response.linkAddressesLength == 0) {
        if (response.addresses != nullptr) {
            PyDict_SetItemString(dDataCallResponse, "addresses", PyUnicode_FromString(response.addresses));
        }
    }

    PyObject* dDefaultQos = translate(response.defaultQos);
    if (dDefaultQos != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "defaultQos", dDefaultQos);
    }

    if (response.qosSessionsLength > 0) {
        PyObject* lQosSessions = translate(
            response.qosSessions, response.qosSessionsLength);
        if (lQosSessions != nullptr) {
            PyDict_SetItemString(dDataCallResponse, "qosSessions", lQosSessions);
        }
    }

    PyDict_SetItemString(dDataCallResponse, "handoverFailureMode",
        PyLong_FromLong(response.handoverFailureMode));

    if (response.sliceInfoValid) {
        PyObject* dSliceInfo = translate(response.sliceInfo);
        if (dSliceInfo != nullptr) {
            PyDict_SetItemString(dDataCallResponse, "sliceInfo", dSliceInfo);
        }
    }

    if (response.trafficDescriptorsLength > 0) {
        PyObject* lTrafficDescriptors = translate(
            response.trafficDescriptors, response.trafficDescriptorsLength);
        if (lTrafficDescriptors != nullptr) {
            PyDict_SetItemString(dDataCallResponse, "trafficDescriptors", lTrafficDescriptors);
        }
    }

    return dDataCallResponse;
}

static PyObject* translate(const RIL_Data_Call_Response_v11* dataCalls, size_t numDataCalls) {
    PyObject* lDataCalls = PyList_New(0);
    if (lDataCalls == nullptr) {
        return nullptr;
    }

    if (dataCalls == nullptr) {
        return lDataCalls;
    }

    std::cout << "numDataCalls: " << numDataCalls << std::endl;
    for (size_t i = 0; i < numDataCalls; i++) {
        PyObject* dDataCall = translate(dataCalls[i]);
        if (dDataCall != nullptr) {
            PyList_Append(lDataCalls, dDataCall);
        }
    }

    return lDataCalls;
}

static PyObject* translate(const RIL_Data_Call_Response_v11* response) {
    PyObject* dDataCallResponse = PyDict_New();
    if (dDataCallResponse == nullptr) {
        return nullptr;
    }

    if (response == nullptr) {
        std::cout << "response is null" << std::endl;
        return dDataCallResponse;
    }

    PyDict_SetItemString(dDataCallResponse, "status", PyLong_FromLong(response->status));
    PyDict_SetItemString(dDataCallResponse, "suggestedRetryTime", PyLong_FromLong(response->suggestedRetryTime));
    PyDict_SetItemString(dDataCallResponse, "cid", PyLong_FromLong(response->cid));
    PyDict_SetItemString(dDataCallResponse, "active", PyLong_FromLong(response->active));

    if (response->type != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "type", PyUnicode_FromString(response->type));
    }

    if (response->ifname != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "ifname", PyUnicode_FromString(response->ifname));
    }
    if (response->linkAddressesLength > 0) {
        std::stringstream ss;
        for (int j = 0; j < response->linkAddressesLength; j++) {
            ss << std::string(response->linkAddresses[j].address)
               <<" properties="<<std::to_string(response->linkAddresses[j].properties)
               <<" deprecation="<<std::to_string(response->linkAddresses[j].deprecationTime)
               <<" expiration="<<std::to_string(response->linkAddresses[j].expirationTime)
               << std::endl;
        }
        PyDict_SetItemString(dDataCallResponse, "addresses", PyUnicode_FromString(ss.str().c_str()));
    } else if (response->addresses) {
        PyDict_SetItemString(dDataCallResponse, "addresses", PyUnicode_FromString(response->addresses));
    }

    if (response->dnses != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "dnses", PyUnicode_FromString(response->dnses));
    }

    if (response->gateways != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "gateways", PyUnicode_FromString(response->gateways));
    }

    if (response->pcscf != nullptr) {
        PyDict_SetItemString(dDataCallResponse, "pcscf", PyUnicode_FromString(response->pcscf));
    }

    PyDict_SetItemString(dDataCallResponse, "mtu", PyLong_FromLong(response->mtu));
    PyDict_SetItemString(dDataCallResponse, "mtuv4", PyLong_FromLong(response->mtuV4));
    PyDict_SetItemString(dDataCallResponse, "mtuv6", PyLong_FromLong(response->mtuV6));

    PyDict_SetItemString(dDataCallResponse, "defaultqos", PyUnicode_FromString(dump(response->defaultQos).c_str()));
    PyDict_SetItemString(dDataCallResponse, "qosSessionsLength", PyLong_FromLong(response->qosSessionsLength));
    for (int i = 0; i < response->qosSessionsLength; i++) {
        std::string key = "qosSessions [" + std::to_string(i) + "] ";
        PyDict_SetItemString(dDataCallResponse, key.c_str(), PyUnicode_FromString(dump(response->qosSessions[i], "  ").c_str()));
    }
    PyDict_SetItemString(dDataCallResponse, "trafficDescriptorsLength", PyLong_FromLong(response->trafficDescriptorsLength));
    for (int i = 0; i < response->trafficDescriptorsLength; i++) {
        std::string key = "trafficDescriptors [" + std::to_string(i) + "].dnnValid";
        PyDict_SetItemString(dDataCallResponse, key.c_str(), PyLong_FromLong(response->trafficDescriptors[i].dnnValid));
        if (response->trafficDescriptors[i].dnnValid) {
            key = "trafficDescriptors [" + std::to_string(i) + "].dnn";
            PyDict_SetItemString(dDataCallResponse, key.c_str(), PyUnicode_FromString(response->trafficDescriptors[i].dnn));
        }
        key = "trafficDescriptors [" + std::to_string(i) + "].osAppIdValid";
        PyDict_SetItemString(dDataCallResponse, key.c_str(), PyLong_FromLong(response->trafficDescriptors[i].osAppIdValid));
        if (response->trafficDescriptors[i].osAppIdValid) {
            key = "trafficDescriptors [" + std::to_string(i) + "].osAppIdLength";
            PyDict_SetItemString(dDataCallResponse, key.c_str(), PyLong_FromLong(response->trafficDescriptors[i].osAppIdLength));
            //first 17 bytes are already known (16 byte UUID and 1 byte length)
            if (response->trafficDescriptors[i].osAppIdLength > 17) {
                std::string appId = "";
                for (size_t j = 17; j < response->trafficDescriptors[i].osAppIdLength; j++) {
                    appId += std::to_string(response->trafficDescriptors[i].osAppId[j]) + " ";
                }
                key = "trafficDescriptors [" + std::to_string(i) + "].osAppId";
                PyDict_SetItemString(dDataCallResponse, key.c_str(), PyUnicode_FromString(appId.c_str()));
            }
        }
    }
    return dDataCallResponse;
}

/* -------------------------------------< Set Up Data Call 1_6 Request and Response >------------------------------------*/

static PyObject* setupDataCall_1_6(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* protocoltype;
    const char* roamingProtocoltype;
    const char* apn;
    unsigned int accessNetwork;
    unsigned int bearerBitmask;
    unsigned int TrafficDescriptorvalid;
    unsigned int dnnValid;
    const char* dnn;
    unsigned int osAppIdValid;
    unsigned int osAppIdLength;
    // 16 bytes for UUID "97a498e3-fc92-5c94-8986-0333d06e4e47"
    std::vector<uint8_t> osAppId = {0x97, 0xa4, 0x98, 0xe3, 0xfc, 0x92, 0x5c, 0x94,
                                    0x89, 0x86, 0x03, 0x33, 0xd0, 0x6e, 0x4e, 0x47};
    int matchallRuleAllowed;
    PyObject* osAppIdObj;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "zzzIIIIzIIOiO", &protocoltype, &roamingProtocoltype, &apn, &accessNetwork, &bearerBitmask,
                                           &TrafficDescriptorvalid, &dnnValid, &dnn, &osAppIdValid, &osAppIdLength, &osAppIdObj,
                                           &matchallRuleAllowed, &callback)) {
        return NULL;
    }
    printInput("setupDataCall_1_6",
               protocoltype,
               roamingProtocoltype,
               apn,
               accessNetwork,
               bearerBitmask,
               TrafficDescriptorvalid,
               dnnValid,
               dnn,
               osAppIdValid,
               osAppIdLength,
               osAppIdObj,
               matchallRuleAllowed);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    if (osAppIdValid) {
        //1 byte for length
        osAppId.push_back(static_cast<uint8_t> (osAppIdLength));
        PyObject *iter = PyObject_GetIter(osAppIdObj);
        if (iter) {
            while(true) {
                PyObject *next = PyIter_Next(iter);
                if (!next) {
                    break;
                }
                if (PyLong_Check(next)) {
                    osAppId.push_back(static_cast<uint8_t> (PyLong_AsUnsignedLong(next)));
                    std::cout << static_cast<uint8_t> (PyLong_AsUnsignedLong(next)) << " ,";
                    ofs << static_cast<uint8_t> (PyLong_AsUnsignedLong(next)) << " ,";
                }
                else {
                    break;
                }
            }
        }
    }

    std::string username("xyz");
    std::string password("xyz");

    RIL_SetUpDataCallParams_Ursp datacallparams = {};

    datacallparams.accessNetwork = static_cast<RIL_RadioAccessNetworks>(accessNetwork);

    datacallparams.profileInfo.profileId = 1;
    datacallparams.profileInfo.protocol = const_cast<char*>(protocoltype);
    datacallparams.profileInfo.roamingProtocol = const_cast<char*>(roamingProtocoltype);
    datacallparams.profileInfo.apn = const_cast<char*>(apn);
    datacallparams.profileInfo.authType = 0;
    datacallparams.profileInfo.user = const_cast<char*>(username.c_str());
    datacallparams.profileInfo.password = const_cast<char*>(password.c_str());
    datacallparams.profileInfo.type = 1;
    datacallparams.profileInfo.maxConnsTime = 0;
    datacallparams.profileInfo.maxConns = 0;
    datacallparams.profileInfo.waitTime = 1;
    datacallparams.profileInfo.enabled = 1;
    datacallparams.profileInfo.supportedTypesBitmask = 0X1;
    datacallparams.profileInfo.bearerBitmask = static_cast<RIL_RadioAccessFamily>(bearerBitmask);
    datacallparams.profileInfo.mtuV4 = 1400;
    datacallparams.profileInfo.mtuV6 = 1500;
    datacallparams.profileInfo.preferred = 0 ;
    datacallparams.profileInfo.persistent = 1;
    datacallparams.roamingAllowed = true;
    datacallparams.reason = static_cast<RIL_RadioDataRequestReasons>(0x01);
    datacallparams.addresses = (char*) "";
    datacallparams.dnses = (char*) "";

    datacallparams.profileInfo.trafficDescriptorValid = TrafficDescriptorvalid;
    if (datacallparams.profileInfo.trafficDescriptorValid) {
        datacallparams.profileInfo.trafficDescriptor.dnnValid = dnnValid;
        if (datacallparams.profileInfo.trafficDescriptor.dnnValid) {
            datacallparams.profileInfo.trafficDescriptor.dnn = const_cast<char*>(dnn);
            std::cout << "dnn :" << std::string(datacallparams.profileInfo.trafficDescriptor.dnn) << std::endl;
            ofs << "dnn :" << std::string(datacallparams.profileInfo.trafficDescriptor.dnn) << std::endl;
        }
        datacallparams.profileInfo.trafficDescriptor.osAppIdValid = osAppIdValid;
        if (datacallparams.profileInfo.trafficDescriptor.osAppIdValid) {
            datacallparams.profileInfo.trafficDescriptor.osAppIdLength = std::min(static_cast<int>(osAppId.size()), MAX_OS_APP_ID_LENGTH);
            memcpy(datacallparams.profileInfo.trafficDescriptor.osAppId, osAppId.data(), datacallparams.profileInfo.trafficDescriptor.osAppIdLength);
            for (int i = 0; i < datacallparams.profileInfo.trafficDescriptor.osAppIdLength; i++) {
                std::cout << "osAppID[" << i << "]" << datacallparams.profileInfo.trafficDescriptor.osAppId[i] << " ,";
                ofs << "osAppID[" << i << "]" << datacallparams.profileInfo.trafficDescriptor.osAppId[i] << " ,";
            }
        }
    }

    Status s = rilSession.setupDataCall(
        datacallparams,
        [callback] (RIL_Errno e, const Status status, const RIL_Data_Call_Response_v11* response) -> void {
            std::cout << "Got response for setupDatacall 1.6 request: " + std::to_string(e) << std::endl;

            PyObject* dDataCallResponse = translate(response);
            if (dDataCallResponse == nullptr) {
                std::cout << "DataCallResponse is null, translation failed" << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dDataCallResponse);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);

            ofs << "Callback error value: " + std::to_string(e) << std::endl;

            if(response) {
                std::cout << "[StartDataCall 1.6]: status :" + std::to_string(response->status) << std::endl;
                std::cout << "[StartDataCall 1.6]: suggestedRetryTime :" + std::to_string(response->suggestedRetryTime) << std::endl;
                std::cout << "[StartDataCall 1.6]: cid :" + std::to_string(response->cid) << std::endl;
                std::cout << "[StartDataCall 1.6]: active :" + std::to_string(response->active) << std::endl;

                ofs << "status: " + std::to_string(response->status) << std::endl;
                ofs << "suggestedRetryTime: " + std::to_string(response->suggestedRetryTime) << std::endl;
                ofs << "cid: " + std::to_string(response->cid) << std::endl;
                ofs << "active: " + std::to_string(response->active) << std::endl;

                if(response->type) {
                    std::string str(response->type);
                    std::cout << "[StartDataCall 1.6]:: type :" + str << std::endl;
                    ofs << "type: " + str  << std::endl;
                }

                if(response->ifname) {
                    std::string str(response->ifname);
                    std::cout << "[StartDataCall 1.6]:: ifname :" + str << std::endl;
                    ofs << "ifname: " + str << std::endl;
                }

                if(response->addresses) {
                    std::string str(response->addresses);
                    std::cout << "[StartDataCall 1.6]:: addresses :" + str << std::endl;
                    ofs << "addresses: " + str << std::endl;
                }

                if(response->dnses) {
                    std::string str(response->dnses);
                    std::cout << "[StartDataCall 1.6]:: dnses :" + str << std::endl;
                    ofs << "dnses: " + str << std::endl;
                }

                if(response->gateways) {
                    std::string str(response->gateways);
                    std::cout << "[StartDataCall 1.6]:: gateways :" + str << std::endl;
                    ofs << "gateways: " + str << std::endl;
                }

                if(response->pcscf) {
                    std::string str(response->pcscf);
                    std::cout << "[StartDataCall 1.6]:: pcscf: " + str << std::endl;
                    ofs << "pcscf: " + str << std::endl;
                }

                std::cout<<"[StartDataCall 1.6]:: mtuV4 :"+std::to_string(response->mtuV4)<<std::endl;
                std::cout<<"[StartDataCall 1.6]:: mtuV6 :"+std::to_string(response->mtuV6)<<std::endl;

                ofs<<"mtuV4 :"+std::to_string(response->mtuV4)<<std::endl;
                ofs<<"mtuV6 :"+std::to_string(response->mtuV6)<<std::endl;
                std::cout << dump(response->defaultQos) << std:: endl;
                ofs << dump(response->defaultQos) << std::endl;

                std::cout<<"[StartDataCall 1.6]:: qosSessions :"<<std::endl;
                ofs<<"qosSessions :"<<std::endl;
                for (int i = 0; i < response->qosSessionsLength; i++) {
                    std::cout << dump(response->qosSessions[i], "  ") << std::endl;
                    ofs<< dump(response->qosSessions[i], "  ") << std::endl;
                }

                std::cout<<"[StartDataCall 1.6]:: trafficDescriptors :";
                for (int i = 0; i < response->trafficDescriptorsLength; i++) {
                    std::cout << "{";
                    ofs << "{";
                    if (response->trafficDescriptors[i].dnnValid) {
                        std::string str(response->trafficDescriptors[i].dnn);
                        std::cout << str;
                        ofs << str;
                    }
                    std::cout << ",";
                    ofs << ",";
                    if (response->trafficDescriptors[i].osAppIdValid) {
                        if (response->trafficDescriptors[i].osAppIdLength > 17) {
                            for (size_t j = 17; j < response->trafficDescriptors[i].osAppIdLength; j++) {
                                std::cout << (char)response->trafficDescriptors[i].osAppId[j];
                                ofs << (char)response->trafficDescriptors[i].osAppId[j];
                            }
                        }
                    }
                    std::cout << "},";
                    ofs << "},";
                }
                std::cout<< std::endl;
                ofs << std::endl;
            } else {
                std::cout << "[StartDataCall 1.6]:: response is NULL!!!" << std::endl;
                ofs << "response: NULL"<<std::endl;
            }

            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Data Call request for 1_6 failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Data Call request for 1_6 sent successfully " << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< Set Up Data Call Request and Response >------------------------------------*/
static PyObject* setupDataCall(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* protocoltype;
    const char* roamingProtocoltype;
    const char* apn;
    unsigned int accessNetwork;
    unsigned int bearerBitmask;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "zzzIIO", &protocoltype, &roamingProtocoltype, &apn, &accessNetwork, &bearerBitmask, &callback)) {
        return NULL;
    }
    printInput(
        "setupDataCall", protocoltype, roamingProtocoltype, apn, accessNetwork, bearerBitmask);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    std::string username("xyz");
    std::string password("xyz");

    RIL_SetUpDataCallParams datacallparams = {};

    datacallparams.accessNetwork = static_cast<RIL_RadioAccessNetworks>(accessNetwork);

    datacallparams.profileInfo.profileId = 1;
    datacallparams.profileInfo.protocol = const_cast<char*>(protocoltype);
    datacallparams.profileInfo.roamingProtocol = const_cast<char*>(roamingProtocoltype);
    datacallparams.profileInfo.apn = const_cast<char*>(apn);
    datacallparams.profileInfo.authType = 0;
    datacallparams.profileInfo.user = const_cast<char*>(username.c_str());
    datacallparams.profileInfo.password = const_cast<char*>(password.c_str());
    datacallparams.profileInfo.type = 1;
    datacallparams.profileInfo.maxConnsTime = 0;
    datacallparams.profileInfo.maxConns = 0;
    datacallparams.profileInfo.waitTime = 1;
    datacallparams.profileInfo.enabled = 1;
    datacallparams.profileInfo.supportedTypesBitmask = 0X1;
    datacallparams.profileInfo.bearerBitmask = static_cast<RIL_RadioAccessFamily>(bearerBitmask);
    datacallparams.profileInfo.mtu = 1400;
    datacallparams.profileInfo.preferred = 0 ;
    datacallparams.profileInfo.persistent = 1;

    datacallparams.roamingAllowed = true;
    datacallparams.reason = static_cast<RIL_RadioDataRequestReasons>(0x01);
    datacallparams.addresses = (char*) "";
    datacallparams.dnses = (char*) "";

    Status s = rilSession.setupDataCall(
        datacallparams,
        [callback] (RIL_Errno e, const Status status, const RIL_Data_Call_Response_v11* response) -> void {
            std::cout << "Got response for setupDatacall request: " + std::to_string(e) << std::endl;

            PyObject* dDataCallResponse = nullptr;
            if (response == nullptr) {
                dDataCallResponse = PyDict_New();
            } else {
                dDataCallResponse = translate(response);
            }

            if (dDataCallResponse == nullptr) {
                std::cout << "DataCallResponse is null, translation failed" << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dDataCallResponse);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);

            ofs << "Callback error value: " + std::to_string(e) << std::endl;

            if(response) {
                std::cout << "[StartDataCall]: status :" + std::to_string(response->status) << std::endl;
                std::cout << "[StartDataCall]: suggestedRetryTime :" + std::to_string(response->suggestedRetryTime) << std::endl;
                std::cout << "[StartDataCall]: cid :" + std::to_string(response->cid) << std::endl;
                std::cout << "[StartDataCall]: active :" + std::to_string(response->active) << std::endl;

                ofs << "status: " + std::to_string(response->status) << std::endl;
                ofs << "suggestedRetryTime: " + std::to_string(response->suggestedRetryTime) << std::endl;
                ofs << "cid: " + std::to_string(response->cid) << std::endl;
                ofs << "active: " + std::to_string(response->active) << std::endl;

                if(response->type) {
                    std::string str(response->type);
                    std::cout << "[StartDataCall]:: type :" + str << std::endl;
                    ofs << "type: " + str  << std::endl;
                }

                if(response->ifname) {
                    std::string str(response->ifname);
                    std::cout << "[StartDataCall]:: ifname :" + str << std::endl;
                    ofs << "ifname: " + str << std::endl;
                }

                // TODO: Needed until marshalling of RIL_Data_Call_Response_v11 is fixed
                if (response->linkAddressesLength == 0) {
                    if(response->addresses) {
                        std::string str(response->addresses);
                        std::cout << "[StartDataCall]:: addresses :" + str << std::endl;
                        ofs << "addresses: " + str << std::endl;
                    }
                }

                if(response->dnses) {
                    std::string str(response->dnses);
                    std::cout << "[StartDataCall]:: dnses :" + str << std::endl;
                    ofs << "dnses: " + str << std::endl;
                }

                if(response->gateways) {
                    std::string str(response->gateways);
                    std::cout << "[StartDataCall]:: gateways :" + str << std::endl;
                    ofs << "gateways: " + str << std::endl;
                }

                if(response->pcscf) {
                    std::string str(response->pcscf);
                    std::cout << "[StartDataCall]:: pcscf: " + str << std::endl;
                    ofs << "pcscf: " + str << std::endl;
                }

                std::cout << "[StartDataCall]:: mtu :" + std::to_string(response->mtu) << std::endl;
                ofs << "mtu: " + std::to_string(response->mtu) << std::endl;
            } else {
                std::cout << "[StartDataCall]:: response is NULL!!!" << std::endl;
                ofs << "response: NULL"<<std::endl;
            }

            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Data Call request failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Data Call request sent successfully " << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator << (std::ostream &os, const RIL_CDMA_SMS_Address &addr) {
  os << "Digit Mode: " << addr.digit_mode << std::endl;
  if (addr.digit_mode == RIL_CDMA_SMS_DIGIT_MODE_8_BIT) {
    os << "Number Mode: " << addr.number_mode << std::endl;
    os << "Number Type: " << addr.number_type << std::endl;
    os << "Number Plan: " << addr.number_plan << std::endl;
  }
  os << "Number of Digits: " << addr.number_of_digits;
  if (addr.number_of_digits > 0) {
    os << std::endl << "Digits: ";
    uint32_t size = (addr.number_of_digits > RIL_CDMA_SMS_ADDRESS_MAX) ?
            RIL_CDMA_SMS_ADDRESS_MAX : addr.number_of_digits;
    for (uint32_t i = 0; i < size; i++) {
        os << addr.digits[i] << " ";
    }
  }
  return os;
}

/* -------------------------------------< Send CDMA SMS Request and Response >------------------------------------*/

static PyObject* sendCdmaSms(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int uTeleserviceID;
    unsigned char bIsServicePresent;
    int uServicecategory;
    uint32_t len;
    uint32_t digit_mode;
    int number_mode;
    int number_type;
    int number_plan;
    int number_of_digits;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "ibiiiiiiiO", &uTeleserviceID, &bIsServicePresent,&uServicecategory, &len, &digit_mode, &number_mode, &number_type, &number_plan, &number_of_digits, &callback)) {
        return NULL;
    }
    printInput("sendCdmaSms",
               uTeleserviceID,
               bIsServicePresent,
               uServicecategory,
               len,
               digit_mode,
               number_mode,
               number_type,
               number_plan,
               number_of_digits);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_CDMA_SMS_Address addr {};
    addr.digit_mode = static_cast<RIL_CDMA_SMS_DigitMode>(digit_mode);
    if (addr.digit_mode == RIL_CDMA_SMS_DIGIT_MODE_8_BIT)
    {
        addr.number_mode = static_cast<RIL_CDMA_SMS_NumberMode>(number_mode);
        addr.number_type = static_cast<RIL_CDMA_SMS_NumberType>(number_type);
        addr.number_plan = static_cast<RIL_CDMA_SMS_NumberPlan>(number_plan);
    }
    addr.number_of_digits = static_cast<unsigned char>(number_of_digits);

    RIL_CDMA_SMS_Message cdmaSms{};
    cdmaSms.uTeleserviceID = uTeleserviceID;
    cdmaSms.bIsServicePresent = bIsServicePresent;
    cdmaSms.uServicecategory = uServicecategory;
    cdmaSms.sAddress = addr;
    cdmaSms.uBearerDataLen = len;

    cdmaSms.expectMore= 0;
    Status s = rilSession.sendCdmaSms(
        cdmaSms,
        [ callback] (RIL_Errno e, const RIL_SMS_Response& resp) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for send CDMA SMS: " << e << std::endl;
             std::cout << "message reference: " << resp.messageRef << std::endl;
             std::cout << "error code: " << resp.errorCode << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs << "message reference: " << resp.messageRef << std::endl;
             ofs << "error code: " << resp.errorCode << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "sendCdmaSms failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "sendCdmaSms request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}
/* -------------------------------------< Dial Request and Response >------------------------------------*/
static PyObject* dial(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* address;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO", &address, &callback)) {
        return NULL;
    }
    printInput("dial", address);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    RIL_Dial dialParams {};
    dialParams.address = const_cast<char*>(address);

    Status s = rilSession.dial(
        dialParams,
        [callback] (RIL_Errno e)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for dial request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Dial failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Dialed request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< convergedDial Request and Response
 * >------------------------------------*/
static PyObject* convergedDial(PyObject* self, PyObject* args)
{
  const char* address;
  int domain;
  PyObject* callback;

  if (!PyArg_ParseTuple(args, "siO", &address, &domain, &callback)) {
    return NULL;
  }

  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
    return NULL;
  }
  Py_XINCREF(callback);
  RIL_DialParams dialParams = {};
  std::cout << "addrress as string: " << address << std::endl;
  dialParams.address = const_cast<char*>(address);
  std::cout << "address: " << dialParams.address << std::endl;
  std::cout << "domain " << domain << std::endl;
  dialParams.callDetails.callDomain = static_cast<RIL_CallDomain>(domain);
  std::cout << "domain after typecast: " << dialParams.callDetails.callDomain << std::endl;
  Status s = rilSession.dial(dialParams, [callback](RIL_Errno e) -> void {
    PyObject* cbArgs = Py_BuildValue("(i)", e);
    if (cbArgs == NULL) {
      return;
    }
    PyObject* result = PyObject_CallObject(callback, cbArgs);
    Py_XDECREF(cbArgs);
    Py_XDECREF(result);
    Py_XDECREF(callback);
    std::cout << "Got response for new dial request: " << e << std::endl;
  });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "Failed to send convergedDial.");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "convergedDial request sent successfully" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }

  return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< deactivate Data Call Request and Response >------------------------------------*/
static PyObject* deactivateDataCall(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int32_t cid;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &cid, &callback)) {
        return NULL;
    }
    printInput("deactivateDataCall", cid);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    bool reason = true;
    Status s = rilSession.deactivateDataCall(
        cid, reason,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for deactivateDataCall request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "deactivateDataCall failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "deactivateDataCall request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}


/*-------------------< RIL Request and Response for Hang Up >-------------------------*/
static PyObject* hangup(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int connid;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &connid, &callback)) {
        return NULL;
    }
    printInput("hangup", connid);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.hangup(
        connid,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for Hangup request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "HangUp Of Call failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Hangup request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

const std::string convertToPrefType(int type) {
  std::string ret;
  switch (type) {
    case 0:
      ret = "GSM WCDMA";
      break;
    case 1:
      ret = "GSM ONLY";
      break;
    case 2:
      ret = "WCDMA";
      break;
    case 3:
      ret = "GSM/WCDMA AUTO";
      break;
    case 4:
      ret = "CDMA/EVDO AUTO";
      break;
    case 5:
      ret = "CDMA ONLY";
      break;
    case 6:
      ret = "EVDO ONLY";
      break;
    case 7:
      ret = "GSM/WCDMA CDMA EVDO AUTO";
      break;
    case 8:
      ret = "LTE CDMA EVDO";
      break;
    case 9:
      ret = "LTE GSM WCDMA";
      break;
    case 10:
      ret = "LTE CMDA EVDO GSM WCDMA";
      break;
    case 11:
      ret = "LTE ONLY";
      break;
    case 12:
      ret = "LTE WCDMA";
      break;
    case 13:
      ret = "TD-SCDMA ONLY";
      break;
    case 14:
      ret = "TD-SCDMA WCDMA";
      break;
    case 15:
      ret = "TD-SCDMA LTE";
      break;
    case 16:
      ret = "TD-SCDMA GSM";
      break;
    case 17:
      ret = "TD-SCDMA GSM LTE";
      break;
    case 18:
      ret = "TD-SCDMA GSM WCDMA";
      break;
    case 19:
      ret = "TD-SCDMA WCDMA LTE";
      break;
    case 20:
      ret = "TD-SCDMA GSM WCDMA LTE";
      break;
    case 21:
      ret = "TD-SCDMA GSM WCDMA CDMA EVDO AUTO";
      break;
    case 22:
      ret = "TD-SCDMA LTE CDMA EVDO GSM WCDMA";
      break;
    case 23:
      ret = "NR5G Only";
      break;
    case 24:
      ret = "NR5G TD-SCDMA LTE CDMA EVDO GSM WCDMA";
      break;
    case 25:
      ret = "GSM WCDMA LTE NR5G";
      break;
    case 26:
      ret = "GSM WCDMA NR5G";
      break;
    case 27:
      ret = "GSM LTE NR5G";
      break;
    case 28:
      ret = "WCDMA LTE NR5G";
      break;
    case 29:
      ret = "GSM NR5G";
      break;
    case 30:
      ret = "WCDMA NR5G";
      break;
    case 31:
      ret = "LTE NR5G";
      break;
    default:
      ret = "NONE";
  }
  return ret;
}
/*-------------------< RIL Request and Response for get Network type >-------------------------*/
static PyObject* getPreferredNetworkType(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getPreferredNetworkType");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.getPreferredNetworkType(
        [callback] (RIL_Errno e, int prefMode) -> void {
            PyObject* cbArgs = Py_BuildValue("(ii)", e, prefMode);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for Preferred request." << e << std::endl;
            std::cout << "Preferred Network Type: " << convertToPrefType(prefMode) << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "Preferred Network Type: " << convertToPrefType(prefMode) << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send getPreferredNetworks");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "send get preferred network type success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}
/*-------------------< RIL Request and Response for set Network Selection Automatic >-------------------------*/
static PyObject* setNetworkSelectionAutomatic(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("setNetworkSelectionAutomatic");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.setNetworkSelectionAutomatic(
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for Set Network Selection Mode Automatic: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Set Network type selection automatic failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request for set network type selection automatic success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}
/*-------------------< RIL Request and Response for Query Network Selection Mode >-------------------------*/
static PyObject* QueryNetworkSelectionMode(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("QueryNetworkSelectionMode");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.QueryNetworkSelectionMode(
        [callback] (RIL_Errno e, int mode) -> void {
            PyObject* cbArgs = Py_BuildValue("(ii)", e, mode);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for Network Selection Mode : " << e << std::endl;
             std::cout << "Network Selection Mode : " << (mode ? "MANUAL" : "AUTOMATIC") << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs << "Network Selection Mode: " << (mode ? "MANUAL" : "AUTOMATIC") << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Query Network Selection Mode failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request for query network selection mode success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}
/* -------------------------------------< DTMF start Request and Response >------------------------------------*/
static PyObject* dtmfStart(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* dtmf = nullptr;
    PyObject* callback = nullptr;

    if (!PyArg_ParseTuple(args, "sO", &dtmf, &callback)) {
        return NULL;
    }
    printInput("dtmfStart", dtmf);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.dtmfStart(
        dtmf[0],
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(iO)", e, Py_None);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for StartDTMF request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send StartDTMF");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "StartDTMF request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/* -------------------------------------< DTMF stop Request and Response >------------------------------------*/
static PyObject* dtmfStop(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("dtmfStop");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.dtmfStop(
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(iO)", e, Py_None);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for StopDTMF request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Dtmf Stop failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "StopDTMF request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/* -------------------------------------< Base Band version Request and Response >------------------------------------*/
static PyObject* basebandVersion(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("basebandVersion");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.basebandVersion(
        [callback] (RIL_Errno e, const char * version) -> void {
            PyObject* cbArgs = Py_BuildValue("(is)", e, version);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for Baseband Version request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
              if (version) {
                  std::cout << "Baseband Version : " << version << std::endl;
                  ofs << "Baseband Version: " << version << std::endl;
              }
              else
              {
                  std::cout << "No Baseband Version recieved" << std::endl;
                  ofs << "No Baseband Version recieved: " << std::endl;
              }
              ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Base Band Version failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Success for request base band version" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}
/*-------------------< RIL Request and Response for Seperate Connection >-------------------------*/
static PyObject* separateConnection(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int callId;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &callId, &callback)) {
        return NULL;
    }
    printInput("separateConnection", callId);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.separateConnection(
        callId,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for separateConnection request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Request for seperate connection failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request for seperate connection success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< Set Mute request and response >---------------------------*/
static PyObject* setMute(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    bool mute = false;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "pO",&mute, &callback)) {
        return NULL;
    }
    printInput("setMute", mute);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

        Status s = rilSession.setMute(mute, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for SetMute request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Set Mute Failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Set mute request sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< Get Mute request and response >---------------------------*/
static PyObject* getMute(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getMute");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

        Status s = rilSession.getMute([callback] (RIL_Errno e, int mute) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for GetMute request:" << e << std::endl;
            std::cout << "mute: " << mute << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "mute: " << mute << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send getMute");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout  << "send mute request success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*----------------------------< Get Data Nr Icon Request and Response >---------------------------*/
static PyObject* GetDataNrIconType(PyObject* self, PyObject* args){
    PyObject* callback;
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("GetDataNrIconType");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.GetDataNrIconType([callback] (RIL_Errno e, const Status status, const five_g_icon_type* response) -> void {
            PyObject* Response = PyDict_New();
            std::cout <<"Callback error value: " + std::to_string(e)<< std::endl;
            ofs <<"Callback error value: " + std::to_string(e)<< std::endl;
            if (response) {
                std::cout << "GetDataNrIconType : Nr Icon Type is ="
                + std::to_string(*response) << std::endl;
                ofs << "GetDataNrIconType : Nr Icon Type is ="
                + std::to_string(*response) << std::endl;
                PyDict_SetItemString(Response, "NrIcon", PyLong_FromLong(static_cast<int>(*response)));
            }
            PyObject* cbArgs = Py_BuildValue("(iN)", e, Response);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            ofs.close();
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
    });

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Get Data Nr Icon Failed");
        return NULL;
    }
    else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Get Data Nr Icon sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< Get Slicing Config Request and Response >---------------------------*/
static PyObject* GetSlicingConfig(PyObject* self, PyObject* args){
    PyObject* callback;
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("GetSlicingConfig");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.GetSlicingConfig([callback] (RIL_Errno e, const Status status, const rilSlicingConfig* response) -> void {
            PyObject* Response = PyDict_New();
            std::cout <<"Callback error value: " + std::to_string(e)<< std::endl;
            ofs <<"Callback error value: " + std::to_string(e)<< std::endl;
            if (response) {
                const rilSlicingConfig config = *response;
                std::cout << "GetSlicingConfig :UrspRuleSize =" << config.UsrpRuleSize << std::endl;
                ofs << "GetSlicingConfig :UrspRuleSize =" << config.UsrpRuleSize << std::endl;
                PyDict_SetItemString(Response, "UrspRuleSize", PyLong_FromLong(config.UsrpRuleSize));
                for (int i = 0; i < config.UsrpRuleSize; i++) {
                    std::cout << "[GetSlicingConfig]:  UrspRuleList: " << i << std::endl;
                    ofs << "[GetSlicingConfig]:  UrspRuleList: " << i << std::endl;
                    std::cout << "[GetSlicingConfig]:  tdSize: " << config.ursprules[i].tdSize << std::endl;
                    ofs << "[GetSlicingConfig]:  tdSize: " << config.ursprules[i].tdSize << std::endl;
                    std::string key = "UrspList [ " + std::to_string(i) + " ]";
                    std::string tdListsize = key + "TdListsize";
                    PyDict_SetItemString(Response, tdListsize.c_str(), PyLong_FromLong(config.ursprules[i].tdSize));
                    for (int j = 0; j < config.ursprules[i].tdSize; j++) {
                        std::cout << "[GetSlicingConfig]: SlicingConfigChanged TdList: " << j << std::endl;
                        ofs << "[GetSlicingConfig]: SlicingConfigChanged TdList: " << j << std::endl;
                        std::string td = key + "TD [" + std::to_string(j) + "]";
                        std::stringstream ss;
                        RIL_TrafficDescriptor trafficDescriptor = config.ursprules[i].tdList[j];
                        ss << "{";
                        if (trafficDescriptor.dnnValid) {
                            std::string str(trafficDescriptor.dnn);
                            ss << str;
                        }
                        ss << ",";
                        if (trafficDescriptor.osAppIdValid) {
                            if (trafficDescriptor.osAppIdLength > 17) {
                                for (size_t k = 17; k < trafficDescriptor.osAppIdLength; k++) {
                                    ss << (char)trafficDescriptor.osAppId[k];
                                }
                            }
                        }
                        ss << "},";
                        std::cout << ss.str();
                        ofs << ss.str();
                        PyDict_SetItemString(Response, td.c_str(), PyUnicode_FromString(ss.str().c_str()));
                    }
                }
            }
            PyObject* cbArgs = Py_BuildValue("(iN)", e, Response);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            ofs.close();
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
    });

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Get Slicing Config Failed");
        return NULL;
    }
    else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Get Slicing Config sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< StartKeepAlive Request and Response >---------------------------*/
static PyObject* StartKeepAlive(PyObject* self, PyObject* args){
    PyObject* callback;
    const char* sourceAddress;
    const char* destinationAddress;
    unsigned int sourcePort;
    unsigned int destinationPort;
    unsigned int maxKeepaliveIntervalMillis;
    unsigned int cid;
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    if (!PyArg_ParseTuple(args, "zIzIIIO", &sourceAddress, &sourcePort, &destinationAddress, &destinationPort,
                          &maxKeepaliveIntervalMillis, &cid, &callback)) {
        return NULL;
    }
    printInput("StartKeepAlive",
               sourceAddress,
               sourcePort,
               destinationAddress,
               destinationPort,
               maxKeepaliveIntervalMillis,
               cid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    RIL_KeepaliveRequest request = {};
    request.type = RIL_KeepaliveType::NATT_IPV4;
    memcpy(request.sourceAddress, sourceAddress, (sizeof(char) * (strlen(sourceAddress)+1)));
    request.sourcePort = sourcePort;
    memcpy(request.destinationAddress, destinationAddress, (sizeof(char) * (strlen(destinationAddress)+1)));
    request.destinationPort = destinationPort;
    request.maxKeepaliveIntervalMillis = maxKeepaliveIntervalMillis;
    request.cid = cid;
    std::cout << request.sourceAddress << std::endl;
    std::cout << request.destinationAddress << std::endl;
    std::cout << request.sourcePort << std::endl;
    std::cout << request.destinationPort << std::endl;
    std::cout << request.maxKeepaliveIntervalMillis << std::endl;
    std::cout << request.cid << std::endl;
    Status s = rilSession.StartKeepAlive(request, [callback] (RIL_Errno e, const Status status, const RIL_ResponseError* response,
                                         const uint32_t* handle, const RIL_KeepaliveStatus_t* statuscode) -> void {
            PyObject* Response = PyDict_New();
            std::cout <<"Callback error value: " + std::to_string(e)<< std::endl;
            ofs <<"Callback error value: " + std::to_string(e)<< std::endl;
            if (response) {
                std::cout << "StartKeepAlive : response is ="
                + std::to_string(*response) << std::endl;
                ofs << "StartKeepAlive : response is ="
                + std::to_string(*response) << std::endl;
                PyDict_SetItemString(Response, "Response", PyLong_FromLong(static_cast<int>(*response)));
            }
            if (handle) {
                std::cout << " StartKeepAlive : Handle is ="
                + std::to_string(*handle) << std::endl;
                ofs << " StartKeepAlive : Handle is ="
                + std::to_string(*handle) << std::endl;
                PyDict_SetItemString(Response, "Handle", PyLong_FromLong(static_cast<int>(*handle)));
            }
            if (statuscode) {
                std::cout << "StartKeepAlive : Status is ="
                + std::to_string(*statuscode) << std::endl;
                ofs << "StartKeepAlive : Status is ="
                + std::to_string(*statuscode) << std::endl;
                PyDict_SetItemString(Response, "Status", PyLong_FromLong(static_cast<int>(*statuscode)));
            }
            PyObject* cbArgs = Py_BuildValue("(iN)", e, Response);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            ofs.close();
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
    });

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Start KeepAlive Failed");
        return NULL;
    }
    else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Start KeepAlive sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*----------------------------< StopKeepAlive Request and Response >---------------------------*/
static PyObject* StopKeepAlive(PyObject* self, PyObject* args){
    PyObject* callback;
    int handle;
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    if (!PyArg_ParseTuple(args, "iO", &handle, &callback)) {
        return NULL;
    }
    printInput("StopKeepAlive", handle);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.StopKeepAlive(handle, [callback] (RIL_Errno e, const Status status, const RIL_ResponseError* response) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            std::cout <<"Callback error value: " + std::to_string(e)<< std::endl;
            ofs <<"Callback error value: " + std::to_string(e)<< std::endl;
            ofs.close();
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
    });

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Stop Keep Alive Failed");
        return NULL;
    }
    else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Stop Keep Alive sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*----------------------------< SetDataThrottling Request and Response >---------------------------*/
static PyObject* SetDataThrottling(PyObject* self, PyObject* args){
    PyObject* callback;
    int action;
    long duration;
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    if (!PyArg_ParseTuple(args, "ilO", &action, &duration, &callback)) {
        return NULL;
    }
    printInput("SetDataThrottling", action, duration);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.setDataThrottling(static_cast<RIL_DataThrottlingAction>(action), duration, [callback] (const RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            std::cout <<"Callback error value: " + std::to_string(e)<< std::endl;
            ofs <<"Callback error value: " + std::to_string(e)<< std::endl;
            ofs.close();
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
    });

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Set Data Throttling Failed");
        return NULL;
    }
    else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Set Data Throttling sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< Get Data Call List  request and response >---------------------------*/
static PyObject* getDataCallList(PyObject* self, PyObject* args){
    PyObject* callback;
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getDataCallList");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.getDataCallList([callback] (const RIL_Errno e, const RIL_Data_Call_Response_v11* response, const unsigned long sz)-> void {

        PyObject* dDataCallResponse = PyDict_New();
        std::cout << "[GetDataCallList]: testGetDataCallList status: " + std::to_string(e)<< std::endl;
        ofs <<"Callback error value: " + std::to_string(e)<< std::endl;
        if(!response) {
            std::cout << "[GetDataCallList]: testGetDataCallList response NULL " << std::endl;
            ofs << "testGetDataCallList response: NULL "<< std::endl;
            PyDict_SetItemString(dDataCallResponse, "DataCallListSize", PyLong_FromLong(0));
            PyObject* cbArgs = Py_BuildValue("(iN)", RIL_E_SUCCESS, dDataCallResponse);
            if (cbArgs == nullptr) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            return;
        }
        std::cout << "[GetDataCallList]: testGetDataCallList  received Data Call size = "+std::to_string(sz) << std::endl;
        ofs << "testGetDataCallList  received Data Call size = "+std::to_string(sz) << std::endl;

        PyDict_SetItemString(dDataCallResponse, "DataCallListSize", PyLong_FromLong(static_cast<int>(sz)));
        std::cout << "[GetDataCallList]: testGetDataCallList reading entry " << std::endl;
        for (int i = 0; (i < sz); i++) {
            std::cout << "[GetDataCallList]: status :"+std::to_string(response[i].status) << std::endl;
            std::cout << "[GetDataCallList]: suggestedRetryTime :"+std::to_string(response[i].suggestedRetryTime) << std::endl;
            std::cout << "[GetDataCallList]: cid :"+std::to_string(response[i].cid) << std::endl;
            std::cout << "[GetDataCallList]: active :"+std::to_string(response[i].active) << std::endl;

            ofs << " status :"+std::to_string(response[i].status) << std::endl;
            ofs << " suggestedRetryTime :"+std::to_string(response[i].suggestedRetryTime) << std::endl;
            ofs << " cid :"+std::to_string(response[i].cid) << std::endl;
            ofs << " active :"+std::to_string(response[i].active) << std::endl;
            if(response[i].type) {
                std::string str(response[i].type);
                std::cout << "[GetDataCallList]: type :"+str << std::endl;
                ofs << "type :" + str << std::endl;
            }
            if(response[i].ifname) {
                std::string str(response[i].ifname);
                std::cout << "[GetDataCallList]: ifname :"+str << std::endl;
                ofs << "ifname :" + str << std::endl;
            }
            if (response[i].linkAddressesLength > 0) {
                for (int j = 0; j < std::min(static_cast<size_t>(MAX_LINK_ADDRESSES), response[i].linkAddressesLength); j++) {
                    if (response[i].linkAddresses[j].address) {
                        std::stringstream ss;
                        ss  <<std::string(response[i].linkAddresses[j].address)
                            <<" properties="<<std::to_string(response[i].linkAddresses[j].properties)
                            <<" deprecation="<<std::to_string(response[i].linkAddresses[j].deprecationTime)
                            <<" expiration="<<std::to_string(response[i].linkAddresses[j].expirationTime)<<std::endl;
                        std::cout << ss.str();
                        ofs << ss.str();
                    }
                }
            }
            else if(response[i].addresses) {
                std::string str(response[i].addresses);
                std::cout << "[GetDataCallList]: addresses :"+str << std::endl;
                ofs << "addresses :" + str << std::endl;
            }
            if(response[i].dnses) {
                std::string str(response[i].dnses);
                std::cout << "[GetDataCallList]: dnses :"+str << std::endl;
                ofs << "dnses :" + str << std::endl;
            }
            if(response[i].gateways) {
                std::string str(response[i].gateways);
                std::cout << "[GetDataCallList]: gateways :"+str << std::endl;
                ofs << "gateways :" + str << std::endl;
            }
            if(response[i].pcscf) {
                std::string str(response[i].pcscf);
                std::cout << "[GetDataCallList]: pcscf :"+str <<std::endl;
                ofs << "pcscf :" + str << std::endl;
            }
            std::cout << "[GetDataCallList]: mtu :"+std::to_string(response[i].mtu) << std::endl;
            ofs << "mtu :"+std::to_string(response[i].mtu) << std::endl;
            std::cout<<"[GetDataCallList ]:: mtuV4 :"+std::to_string(response[i].mtuV4)<<std::endl;
            std::cout<<"[GetDataCallList ]:: mtuV6 :"+std::to_string(response[i].mtuV6)<<std::endl;

            ofs<<"mtuV4 :"+std::to_string(response[i].mtuV4)<<std::endl;
            ofs<<"mtuV6 :"+std::to_string(response[i].mtuV6)<<std::endl;
            std::cout<<"[GetDataCallList]:: defaultQos :";
            ofs<<"defaultQos :"<<std::endl;
            std::cout << dump(response[i].defaultQos) << std:: endl;
            ofs << dump(response[i].defaultQos) << std::endl;

            std::cout<<"[GetDataCallList]:: qosSessions :"<<std::endl;
            ofs<<"qosSessions :"<<std::endl;
            for (int j = 0; j < response[i].qosSessionsLength; j++) {
                std::cout << dump(response[i].qosSessions[j], "  ") << std::endl;
                ofs<< dump(response[i].qosSessions[j], "  ") << std::endl;
            }

            std::cout<<"[GetDataCallList]:: trafficDescriptors :";
            for (int k = 0; k < response[i].trafficDescriptorsLength; k++) {
                std::cout << "{";
                ofs << "{";
                if (response[i].trafficDescriptors[k].dnnValid) {
                    std::string str(response[i].trafficDescriptors[k].dnn);
                    std::cout << str;
                    ofs << str;
                }
                std::cout << ",";
                ofs << ",";
                if (response[i].trafficDescriptors[k].osAppIdValid) {
                    if (response[i].trafficDescriptors[k].osAppIdLength > 17) {
                        for (size_t j = 17; j < response[i].trafficDescriptors[k].osAppIdLength; j++) {
                            std::cout << (char)response[i].trafficDescriptors[k].osAppId[j];
                            ofs << (char)response[i].trafficDescriptors[k].osAppId[j];
                        }
                    }
                }
                std::cout << "},";
                ofs << "},";
            }
            std::cout<< std::endl;
            ofs << std::endl;

            PyObject* DataCallResponse_t = translate(&response[i]);
            if (DataCallResponse_t == nullptr) {
                std::cout << "DataCallResponse is null, translation failed" << std::endl;
                continue;
            }

            std::string key = "Call [" + std::to_string(i) + "]";
            PyDict_SetItemString(dDataCallResponse, key.c_str(), DataCallResponse_t);
        }

        PyObject* cbArgs = Py_BuildValue("(iN)", RIL_E_SUCCESS, dDataCallResponse);
        if (cbArgs == nullptr) {
            return;
        }
        PyObject* result = PyObject_CallObject(callback, cbArgs);
        Py_XDECREF(cbArgs);
        Py_XDECREF(result);
        Py_XDECREF(callback);
        ofs.close();
    });

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Get Data Call List Failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "request for Get Data Call List sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*----------------------------< OEM HOOK RAW request and response >---------------------------*/
static PyObject* oemhookRaw(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    char* data;
    int dataLength;

    if (!PyArg_ParseTuple(args, "siO", &data, &dataLength, &callback)) {
        return NULL;
    }
    printInput("oemhookRaw", data, dataLength);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

        Status s = rilSession.oemhookRaw(data, dataLength, [callback] (RIL_Errno e, const char * oem, size_t val) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response fot OEM hook raw: " << e << std::endl;
            std::cout << "oem: " << oem << std::endl;
            std::cout << "value: " << std::to_string(val) << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "oem: " << oem << std::endl;
            ofs << "value: " << std::to_string(val) << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "OEM HOOK RAW Failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "request for OEM HOOK RAW sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< QUERY AVAILABLE BAND MODE request and response >---------------------------*/
static PyObject* queryAvailableBandMode(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("queryAvailableBandMode");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

        Status s = rilSession.queryAvailableBandMode([callback] (RIL_Errno e, const int * bandMode) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for queryAvailableBandMode: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (bandMode) {
                size_t size = bandMode[0];
                std::cout << "Size:" << size << std::endl;
                for(int i=1; i<=size; i++) {
                    std::cout << "bandMode"<< std::to_string(i) <<": "<< static_cast<RIL_RadioBandMode>(bandMode[i])<< std::endl;
                    ofs << "bandMode"<< std::to_string(i) <<": "<< static_cast<RIL_RadioBandMode>(bandMode[i]) << std::endl;
                }
        }
        ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send queryAvailableBandMode");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "request for queryAvailableBandMode sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< Explicit call transfer request and response >---------------------------*/
static PyObject* explicitCallTransfer(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("explicitCallTransfer");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

        Status s = rilSession.explicitCallTransfer([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for ExplicitCallTransfer request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send explicitCallTransfer");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request to Explicit call transfer success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*----------------------------< RIL request for GSM SMS broadcast activation >---------------------------*/
static PyObject* GsmSmsBroadcastActivation(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    uint32_t disable;

    if (!PyArg_ParseTuple(args, "iO", &disable, &callback)) {
        return NULL;
    }
    printInput("GsmSmsBroadcastActivation", disable);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

        Status s = rilSession.GsmSmsBroadcastActivation(disable,[callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for GsmSmsBroadcastActivation: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Request for GSM SMS broadcast activation Failed");
        return NULL;
    }
     else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request GSM SMS broadcast activation sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for device identity activation >---------------------------*/
static PyObject* deviceIdentity(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("deviceIdentity");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

        Status s = rilSession.deviceIdentity([callback] (RIL_Errno e, const char ** deviceInfo) -> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);

            std::cout << "Got response for Device Identity request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (deviceInfo) {
                if (deviceInfo[0]){
                    std::cout << "imei : " << deviceInfo[0] << std::endl;
                    ofs << "imei: " << deviceInfo[0] << std::endl;
                }
                if (deviceInfo[1]){
                    std::cout << "imeisv : " << deviceInfo[1] << std::endl;
                    ofs << "imeisv: " << deviceInfo[1] << std::endl;
                }
                if (deviceInfo[2]){
                    std::cout << "esn : " << deviceInfo[2] << std::endl;
                    ofs << "esn: " << deviceInfo[2] << std::endl;
                }
                if (deviceInfo[3]){
                    std::cout << "meid : " << deviceInfo[3] << std::endl;
                    ofs << "meid: " << deviceInfo[3] << std::endl;
                }
                } else {
                    std::cout << "No Device Info recieved" << std::endl;
                    ofs << "No Device Info recieved: " << std::endl;
                }
                ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send requestDeviceIdentity");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request Device Identity sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for get SMSC address >---------------------------*/
static PyObject* getSmscAddress(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getSmscAddress");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

        Status s = rilSession.getSmscAddress([callback] (RIL_Errno e, const char * addr) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Get response for get SMSC address, err code: "<< e  <<std::endl;
            std::cout << "address: " << (addr ? addr : "null") << std::endl;
            ofs << "Callback error value: "<< e  <<std::endl;
            ofs << "address: " << (addr ? addr : "null") << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send get SMSC address req");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Succeeded to send get SMSC address req" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for answer>---------------------------*/
static PyObject* answer(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("answer");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

        Status s = rilSession.answer([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Answer request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send Answer");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Answer request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for set SMSC address >---------------------------*/
static PyObject* setSmscAddress(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    char* Smscaddress;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO",&Smscaddress, &callback)) {
        return NULL;
    }
    printInput("setSmscAddress", Smscaddress);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    std::string address(Smscaddress);

    std::cout << "address: " << address << std::endl;
        Status s = rilSession.setSmscAddress(address,[callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for set SMSC address: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send set SMSC address req");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Succeeded to send set SMSC address req" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for sim isim authentication >---------------------------*/
static PyObject* simIsimAuthenticationReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string reqParams;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO",&reqParams, &callback)) {
        return NULL;
    }
    printInput("simIsimAuthenticationReq", reqParams);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    const char *command = const_cast<char *>(reqParams.c_str());
    Py_XINCREF(callback);
        Status s = rilSession.simIsimAuthenticationReq(command,[callback] (RIL_Errno e, const char * res) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for IsimAuthenticate req: " << e << std::endl;
            std::cout << "simresponse: " << res << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "simresponse: " << res << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Isimauthticate with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout <<"Received  Isimauthenticate  response succesfully"<< std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for set authentication >---------------------------*/
static PyObject* simAuthenticationReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string authContext;
    std::string authData;
    std::string aid;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sssO",&authContext, &authData, &aid, &callback)) {
        return NULL;
    }
    printInput("simAuthenticationReq", authContext, authData, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    RIL_SimAuthentication reqParams {};
    reqParams.authData = const_cast<char *>(authData.c_str());
    reqParams.authContext = std::stoi(authContext,NULL,16);
    reqParams.aid = const_cast<char *>(aid.c_str());;

        Status s = rilSession.simAuthenticationReq(reqParams,[callback] (RIL_Errno e, const RIL_SIM_IO_Response *res) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for simAuthenticate req: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (res != nullptr)
            {
                std::cout<<"StatusWord1: "<<res->sw1;
                std::cout<<"Statusword2: "<<res->sw2;
                ofs << "StatusWord1: "<<res->sw1;
                ofs << "Statusword2: "<<res->sw2;

            if(res->simResponse != nullptr)
            {
                std::cout<<"simresponse: "<< res->simResponse<<std::endl;
                ofs << "simresponse: "<< res->simResponse<<std::endl;
            }
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received simauthticate with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received  simauthenticate  response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for sim stk send envolope with status req >---------------------------*/
static PyObject* simStkSendEnvelopeWithStatusReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string reqParams;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO",&reqParams, &callback)) {
        return NULL;
    }
    printInput("simStkSendEnvelopeWithStatusReq", reqParams);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    const char *aid = const_cast<char *>(reqParams.c_str());
        Status s = rilSession.simStkSendEnvelopeWithStatusReq(aid,[callback] (RIL_Errno e, const RIL_SIM_IO_Response* res) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for envelope status request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if( res != nullptr)
            {
                std::cout<<"StatusWord1: "<<res->sw1;
                std::cout<<"Statusword2: "<<res->sw2;
                ofs << "StatusWord1: "<<res->sw1;
                ofs << "Statusword2: "<<res->sw2;

            if(res->simResponse != nullptr)
            {
                std::cout<<"simresponse: "<< std::string(res->simResponse)<<std::endl;
                ofs << "simresponse: "<< std::string(res->simResponse)<<std::endl;
            }
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for envelope status request");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for envelope status request"<< std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for voice radio tech >---------------------------*/
static PyObject* VoiceRadioTech(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("VoiceRadioTech");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
        Status s = rilSession.VoiceRadioTech([callback] (RIL_Errno e, int tech) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for voice radio tech request: " << e << std::endl;
            std::cout << "Voice Radio Tech: " << tech << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "Voice Radio Tech: " << tech << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send requestVoiceRadioTech");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "request voice radio tech sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for SET UNSOL CELL INFO LIST RATE >---------------------------*/
static PyObject* setUnsolCellInfoListRate(PyObject* self, PyObject* args){
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  int32_t mRate;
  PyObject* callback;

  if (!PyArg_ParseTuple(args, "sO", &mRate, &callback)) {
    return NULL;
    }
    printInput("setUnsolCellInfoListRate", mRate);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
        Status s = rilSession.setUnsolCellInfoListRate(mRate,[callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setUnsolCellInfoListRate request." << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send requestSetUnsolCellInfoListRate");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "request for set Unsol Cell Info List rate sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for SET initial attatch apn >---------------------------*/
static PyObject* setInitialAttachApn(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int profileId;
    const char* apn;
    const char* protocol;
    const char* roamingProtocol;
    const char* user;
    const char* password;
    int profiletype;
    int authtype;
    int supportedTypesBitmask;
    int mode;
    int preferred;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "izzzzziiiiiO", &profileId, &apn, &protocol, &roamingProtocol, &user, &password,
                         &profiletype, &authtype, &supportedTypesBitmask, &mode, &preferred, &callback)) {
        return NULL;
    }
    printInput("setInitialAttachApn",
               profileId,
               apn,
               protocol,
               roamingProtocol,
               user,
               password,
               profiletype,
               authtype,
               supportedTypesBitmask,
               mode,
               preferred);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    RIL_DataProfileInfo dataProfile = {};
    dataProfile.profileId = profileId;
    dataProfile.apn = const_cast<char*>(apn);
    dataProfile.protocol = const_cast<char*>(protocol);
    dataProfile.roamingProtocol = const_cast<char*>(roamingProtocol);
    dataProfile.authType = authtype;
    dataProfile.user = const_cast<char*>(user);
    dataProfile.password = const_cast<char*>(password);
    dataProfile.type = profiletype;
    if (dataProfile.type < 0 || dataProfile.type > 2) {
        std::cout << "Invalid option selected. Defaulting to 3GPP" << std::endl;
        ofs << "Invalid option selected. Defaulting to 3GPP" << std::endl;
        dataProfile.type = 1;
    }
    dataProfile.maxConnsTime = 0;
    dataProfile.maxConns = 0;
    dataProfile.waitTime = 1;
    dataProfile.enabled = 1;
    dataProfile.supportedTypesBitmask = supportedTypesBitmask;
    switch (mode) {
        case 1 :
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
            break;
        case 2 :
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE_CA;
            break;
        case 3 :
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_5G;
            break;
        default:
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
            break;
    }
    dataProfile.mtu = 1400;
    if (dataProfile.preferred != 0 && dataProfile.preferred != 1) {
        std::cout << "Invalid value. Defaulting to non-preferred" << std::endl;
        dataProfile.preferred = 0;
    }
    dataProfile.persistent = 1;
    Py_XINCREF(callback);
    Status s = rilSession.setInitialAttachApn(dataProfile, [callback](RIL_Errno e) -> void {
      PyObject* cbArgs = Py_BuildValue("(i)", e);
      if (cbArgs == NULL) {
        return;
      }

      PyObject* result = PyObject_CallObject(callback, cbArgs);

      Py_XDECREF(cbArgs);
      Py_XDECREF(result);
      Py_XDECREF(callback);
      std::cout << "Got response for setInitialAttachApn request: " + std::to_string(e) << std::endl;
      ofs << "Callback error value: " + std::to_string(e) << std::endl;
      ofs.close();
    });

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Request for set initial apn Failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request for set Initial attach apn sent success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for sim Transmit Apdu Basic request >---------------------------*/
static PyObject* simTransmitApduBasicReq(PyObject* self, PyObject* args){
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  int sessionid;
  int cla;
  int instruction;
  int p1;
  int p2;
  int p3;
  std::string data;
  PyObject* callback;

  if (!PyArg_ParseTuple(
          args, "iiiiiisO", &sessionid, &cla, &instruction, &p1, &p2, &p3, &data, &callback)) {
    return NULL;
    }
    printInput("simTransmitApduBasicReq", sessionid, cla, instruction, p1, p2, p3, data);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    RIL_SIM_APDU reqParams;
    reqParams.sessionid = sessionid;
    reqParams.cla = cla;
    reqParams.instruction = instruction;
    reqParams.data = const_cast<char *>(data.c_str());
    reqParams.p1 = p1;
    reqParams.p2 = p2;
    reqParams.p3 = p3;
    Py_XINCREF(callback);
        Status s = rilSession.simTransmitApduBasicReq(reqParams,[callback] (RIL_Errno e, const RIL_SIM_IO_Response *res) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for basic transmit apdu: " << e << std::endl;
            std::cout<< "sw1: " << res->sw1 << std::endl;
            std::cout << "sw2: " << res->sw2 << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "sw1: " << res->sw1 << std::endl;
            ofs << "sw2: " << res->sw2 << std::endl;
            if(res->simResponse != nullptr)
            {
                std::cout<<"simresponse: "<<res->simResponse<<std::endl;
                ofs <<"simresponse: "<<res->simResponse<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for transmit apdu");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received basic transmit apdu response success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*----------------------------< RIL request for sim Open Channel Request >---------------------------*/
static PyObject* simOpenChannelReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string aidPtr;
    std::string p2;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "ssO", &aidPtr, &p2, &callback)) {
        return NULL;
    }
    printInput("simOpenChannelReq", aidPtr, p2);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    RIL_OpenChannelParams reqParams;
    reqParams.aidPtr = const_cast<char *>(aidPtr.c_str());
    reqParams.p2 = std::stoi(p2);
    Py_XINCREF(callback);
        Status s = rilSession.simOpenChannelReq(reqParams,[callback] (RIL_Errno e,const int *aid, size_t len) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for open channel: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if(aid != nullptr)
            {
                std::cout << "Response Aid: " << aid << std::endl;
                std::cout << "Response len: " << len << std::endl;
                ofs << "Response Aid: " << aid << std::endl;
                ofs << "Response len: " << len << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "open channel response failed  with error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "open channel response success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*------------------------< RIL Request for Get IMSI Request >-------------------------------*/
static PyObject* simGetImsiReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* aid;
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "sO", &aid, &callback)) {
        return NULL;
    }
    printInput("simGetImsiReq", aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.simGetImsiReq(aid, [callback] (RIL_Errno e, const char * res) -> void {
            PyObject* sImsi = Py_None;

            if (res != nullptr) {
                sImsi = PyUnicode_FromString(res);
                if (sImsi == nullptr) {
                    std::cout << "sImsi is null, translation failed" << std::endl;
                    return;
                }
            }

            PyObject* cbArgs = Py_BuildValue("(iO)", e, sImsi);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for simGetImsiReq request : " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (res != NULL)
            {
                std::cout << "IMSI value: " << std::string(res)<<std::endl;
                ofs << "IMSI value: " << std::string(res)<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for simGetImsiReq");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for simGetImsiReq" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*------------------------< RIL Request for Sim close channel >-------------------------------*/
static PyObject* simCloseChannelReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int sessionId;
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "iO", &sessionId, &callback)) {
        return NULL;
    }
    printInput("simCloseChannelReq", sessionId);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.simCloseChannelReq(sessionId, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for close channel: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for close channel");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Success response for close channel" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*------------------------< RIL Request for CDMA SMS broadcast activation >-------------------------------*/
static PyObject* setCdmaSmsBroadcastActivation(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int value;
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "iO", &value, &callback)) {
        return NULL;
    }
    printInput("setCdmaSmsBroadcastActivation", value);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.setCdmaSmsBroadcastActivation(value, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for activating cdma sms broadcast: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();

        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send set CDMA sms broadcast activation req");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Succeeded to send set CDMA sms broadcast activation req" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*------------------------< RIL Request for delete SMS on SIM >-------------------------------*/
static PyObject* deleteSmsOnSim(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int recordIndex;
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "iO", &recordIndex, &callback)) {
        return NULL;
    }
    printInput("deleteSmsOnSim", recordIndex);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.deleteSmsOnSim(recordIndex, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for delete SMS from SIM: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            std::cout << ((e == RIL_E_SUCCESS) ? "Successfully deleted SMS from SIM: " : "Failed to delete SMS from SIM: ")
                     << std::endl;

            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to issue request to delete SMS on SIM.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Delete SMS from SIM request success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}

/*static bool translate(RIL_DataProfileInfo_IRadio_1_4* dataProfile, PyObject* dDataProfile) {
    if (dataProfile == nullptr || dDataProfile == nullptr) {
        return false;
    }

    if (!PyDict_Check(dDataProfile)) {
        return false;
    }

    // profileId
    PyObject* lProfileId = PyDict_GetItemString(dDataProfile, "profileId");
    if (lProfileId == nullptr || !PyLong_Check(lProfileId)) {
        return false;
    }

    dataProfile->profileId = PyLong_AsLong(lProfileId);

    // apn

}

static const RIL_DataProfileInfo_IRadio_1_4* translate(PyObject* lDataProfiles) {
    if (lDataProfiles == nullptr) {
        return nullptr;
    }


}*/

/*------------------------< RIL Request for set Data Profile >-------------------------------*/
static PyObject* setDataProfile(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int profileId;
    const char* apn;
    const char* protocol;
    const char* roamingProtocol;
    const char* user;
    const char* password;
    int profiletype;
    int authtype;
    int supportedTypesBitmask;
    int mode;
    int preferred;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "izzzzziiiiiO", &profileId, &apn, &protocol, &roamingProtocol, &user, &password,
                         &profiletype, &authtype, &supportedTypesBitmask, &mode, &preferred, &callback)) {
        return NULL;
    }
    printInput("setDataProfile",
               profileId,
               apn,
               protocol,
               roamingProtocol,
               user,
               password,
               profiletype,
               authtype,
               supportedTypesBitmask,
               mode,
               preferred);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    RIL_DataProfileInfo *dataProfilePtr = nullptr;
    dataProfilePtr = new RIL_DataProfileInfo[1] {};
    dataProfilePtr[0].profileId = profileId;
    dataProfilePtr[0].apn = const_cast<char*>(apn);
    dataProfilePtr[0].protocol = const_cast<char*>(protocol);
    dataProfilePtr[0].roamingProtocol = const_cast<char*>(roamingProtocol);
    dataProfilePtr[0].authType = authtype;
    dataProfilePtr[0].user = const_cast<char*>(user);
    dataProfilePtr[0].password = const_cast<char*>(password);
    dataProfilePtr[0].type = profiletype;
    dataProfilePtr[0].maxConnsTime = 0;
    dataProfilePtr[0].maxConns = 0;
    dataProfilePtr[0].waitTime = 1;
    dataProfilePtr[0].enabled = 1;
    dataProfilePtr[0].supportedTypesBitmask = supportedTypesBitmask;
    dataProfilePtr[0].preferred = preferred;
    switch (mode) {
        case 1 :
            dataProfilePtr[0].bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
            break;
        case 2 :
            dataProfilePtr[0].bearerBitmask = RIL_RadioAccessFamily::RAF_LTE_CA;
            break;
        case 3 :
            dataProfilePtr[0].bearerBitmask = RIL_RadioAccessFamily::RAF_5G;
            break;
        default:
            dataProfilePtr[0].bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
            break;
    }
    dataProfilePtr[0].mtu = 1400;
    if (dataProfilePtr[0].preferred != 0 && dataProfilePtr[0].preferred != 1) {
        std::cout << "Invalid value. Defaulting to non-preferred" << std::endl;
        dataProfilePtr[0].preferred = 0;
    }
    dataProfilePtr[0].persistent = 1;

    if(dataProfilePtr) {
       std::cout << " Sending SetDataProfile for 1 profile " << std::endl;
    }
    Py_XINCREF(callback);
    Status s = rilSession.setDataProfile(dataProfilePtr, 1, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setDataProfile request: " + std::to_string(e) << std::endl;
            ofs << "Callback error value: " + std::to_string(e) << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Request for set Data Profile Failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request for set Data Profile success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*------------------------< RIL Request for sim Transmit apdu channel >-------------------------------*/
static PyObject* simTransmitApduChannelReq(PyObject* self, PyObject* args){
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  int sessionid;
  int cla;
  int instruction;
  int p1;
  int p2;
  int p3;
  std::string data;
  PyObject* callback;
  if (!PyArg_ParseTuple(
          args, "iiiiiisO", &sessionid, &cla, &instruction, &p1, &p2, &p3, &data, &callback)) {
    return NULL;
    }
    printInput("simTransmitApduChannelReq", sessionid, cla, instruction, p1, p2, p3, data);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    RIL_SIM_APDU reqParams{};
    reqParams.sessionid = sessionid;
    reqParams.cla = cla;
    reqParams.instruction = instruction;
    reqParams.data = const_cast<char *>(data.c_str());
    reqParams.p1 = p1;
    reqParams.p2 = p2;
    reqParams.p3 = p3;
    Py_XINCREF(callback);
    Status s = rilSession.simTransmitApduChannelReq(reqParams, [callback] (RIL_Errno e, const RIL_SIM_IO_Response *res) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Transmit apdu channel: " << e << std::endl;
            std::cout<< "sw1: " << res->sw1 <<std::endl;
            std::cout<< "sw2: " << res->sw2 << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "sw1: " << res->sw1 <<std::endl;
            ofs << "sw2: " << res->sw2 << std::endl;
            if(res->simResponse != nullptr)
            {
                std::cout<<"simresponse: "<<res->simResponse<<std::endl;
                ofs << "simresponse: "<<res->simResponse<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Transmit apdu response failed");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Transmit apdu response success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}

/*------------------------< RIL Request GSM set Broadcast sms config >-------------------------------*/
static PyObject* GsmSetBroadcastSMSConfig(PyObject* self, PyObject* args){
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  int fromServiceId;
  int toServiceId;
  int fromCodeScheme;
  int toCodeScheme;
  char selected;
  int len;
  PyObject* callback;

  if (!PyArg_ParseTuple(args,
                        "iiiibiO",
                        &fromServiceId,
                        &toServiceId,
                        &fromCodeScheme,
                        &toCodeScheme,
                        &selected,
                        &len,
                        &callback)) {
    return NULL;
    }

    printInput("GsmSetBroadcastSMSConfig",
               fromServiceId,
               toServiceId,
               fromCodeScheme,
               toCodeScheme,
               selected,
               len);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    RIL_GSM_BroadcastSmsConfigInfo config {};
    config.fromServiceId = fromServiceId;
    config.toServiceId = toServiceId;
    config.fromCodeScheme = fromCodeScheme;
    config.toCodeScheme = toCodeScheme;
    config.selected = selected;

    Py_XINCREF(callback);

    Status s = rilSession.GsmSetBroadcastSMSConfig(&config, 1, /*len,*/
        [callback] (RIL_Errno e) -> void {
            std::cout << "Got response for GsmSetBroadcastSMSConfig: " << e << std::endl;

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Request for GSM set broadcast sms config failed");
        return NULL;
    } else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request for GSM set broadcast sns config success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translate(const RIL_GSM_BroadcastSmsConfigInfo& config) {
    PyObject* dGsmBroadcastSmsConfig = PyDict_New();
    if (dGsmBroadcastSmsConfig == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dGsmBroadcastSmsConfig, "fromServiceId", PyLong_FromLong(config.fromServiceId));
    PyDict_SetItemString(dGsmBroadcastSmsConfig, "toServiceId", PyLong_FromLong(config.toServiceId));
    PyDict_SetItemString(dGsmBroadcastSmsConfig, "fromCodeScheme", PyLong_FromLong(config.fromCodeScheme));
    PyDict_SetItemString(dGsmBroadcastSmsConfig, "toCodeScheme", PyLong_FromLong(config.toCodeScheme));
    PyDict_SetItemString(dGsmBroadcastSmsConfig, "selected", PyLong_FromLong(config.selected));

    return dGsmBroadcastSmsConfig;
}

static PyObject* translate(const RIL_GSM_BroadcastSmsConfigInfo* config, size_t configLen) {
    PyObject* lGsmBroadcastSmsConfigs = PyList_New(0);
    if (lGsmBroadcastSmsConfigs == nullptr) {
        return nullptr;
    }

    if (config == nullptr || configLen == 0) {
        return lGsmBroadcastSmsConfigs;
    }

    for (size_t i = 0; i < configLen; i++) {
        PyObject* dGsmBroadcastSmsConfig = translate(config[i]);
        if (dGsmBroadcastSmsConfig != nullptr) {
            PyList_Append(lGsmBroadcastSmsConfigs, dGsmBroadcastSmsConfig);
        }
    }

    return lGsmBroadcastSmsConfigs;
}

static PyObject* GsmGetBroadcastSmsConfig(PyObject* self, PyObject* args){
    PyObject* callback = nullptr;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return nullptr;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
        return nullptr;
    }

    Py_XINCREF(callback);

    Status s = rilSession.GsmGetBroadcastSmsConfig(
        [callback] (RIL_Errno e, const RIL_GSM_BroadcastSmsConfigInfo* config, size_t configLen) -> void {
            std::cout << "Got response for GsmGetBroadcastSmsConfig: " << e << std::endl;

            PyObject* lGsmBroadcastSmsConfigs = translate(config, configLen);
            if (lGsmBroadcastSmsConfigs == nullptr) {
                std::cout << "Failed to translate GSM broadcast SMS config." << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, lGsmBroadcastSmsConfigs);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Request for GSM get broadcast sms config failed");
        return nullptr;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request for GSM get broadcast sms config success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/*------------------------< RIL Request for write sms to sim >-------------------------------*/
static PyObject* writeSmsToSim(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("writeSmsToSim");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.writeSmsToSim("", "", 0,[callback] (RIL_Errno e, int32_t recordIndex) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            if (e == RIL_E_SUCCESS)
            {
                std::cout << "Got response for writeSmsToSim: " << e << std::endl
                          << "Message Record Index : " << recordIndex << std::endl;
                ofs << "Callback error value: " << e << std::endl
                          << "Message Record Index : " << recordIndex << std::endl;
            }
            else
            {
                std::cout << "Got response for writeSmsToSim: " << e <<  std::endl
                          << "Message Record Index : " << recordIndex << std::endl;
                ofs << "Callback error value: " << e <<  std::endl
                        << "Message Record Index : " << recordIndex << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to issue request to write SMS to SIM.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Request to write SMS to SIM success" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*-------------------< RIL request to Enter Sim puk >--------------------------------*/
static PyObject* simEnterPukReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* new_puk[3]= {};
    const char* pukCode;
    const char* pin;
    const char* aid;
    std::vector<std::string> userInput;

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sssO", &pukCode, &pin, &aid, &callback)) {
        return NULL;
    }
    printInput("simEnterPukReq", pukCode, pin, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    new_puk[0] = const_cast<char*>(pukCode);
    new_puk[1] = const_cast<char*>(pin);
    new_puk[2] = const_cast<char*>(aid);
    userInput.push_back(pukCode);
    userInput.push_back(pin);
    userInput.push_back(aid);
    Py_XINCREF(callback);
    Status s = rilSession.simEnterPukReq(new_puk, [callback] (RIL_Errno e, int * retries) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Enter Puk: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (retries != NULL)
            {
                std::cout << "Response for Enter Puk: "<< std::to_string(*retries)<<std::endl;
                ofs << "Response for Enter Puk: "<< std::to_string(*retries)<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Enter Puk response received with error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Enter Puk response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*-------------------< RIL request to Enter Sim pin2 >--------------------------------*/
static PyObject* simEnterPin2Req(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* unblock_pin2[2]= {};
    const char* pin;
    const char* aid;
    PyObject* callback;
    std::vector<std::string> userInput;

    if (!PyArg_ParseTuple(args, "ssO", &pin, &aid, &callback)) {
        return NULL;
    }
    printInput("simEnterPin2Req", pin, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    unblock_pin2[0] = const_cast<char*>(pin);
    unblock_pin2[1] = const_cast<char*>(aid);
    userInput.push_back(pin);
    userInput.push_back(aid);
    Py_XINCREF(callback);
    Status s = rilSession.simEnterPin2Req(unblock_pin2, [callback] (RIL_Errno e, int * retries) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Enter pin2: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (retries != NULL)
            {
                std::cout << "Response for Enter Pin 2: "<< std::to_string(*retries)<<std::endl;
                ofs << "Response for Enter Pin 2: "<< std::to_string(*retries)<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for Enter_pin2");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for enter_pin2" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/*-------------------< RIL request to Enter Sim puk 2 >--------------------------------*/
static PyObject* simEnterPuk2Req(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* new_puk[3]= {};
    const char* pukCode;
    const char* pin;
    const char* aid;
    std::vector<std::string> userInput;

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sssO", &pukCode, &pin, &aid, &callback)) {
        return NULL;
    }
    printInput("simEnterPuk2Req", pukCode, pin, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    new_puk[0] = const_cast<char*>(pukCode);
    new_puk[1] = const_cast<char*>(pin);
    new_puk[2] = const_cast<char*>(aid);
    userInput.push_back(pukCode);
    userInput.push_back(pin);
    userInput.push_back(aid);
    Py_XINCREF(callback);
    Status s = rilSession.simEnterPuk2Req(new_puk, [callback] (RIL_Errno e, int * retries) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Enter Puk 2: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (retries != NULL)
            {
                std::cout << "Response for Enter Puk 2: "<< std::to_string(*retries)<<std::endl;
                ofs << "Response for Enter Puk 2: "<< std::to_string(*retries)<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Enter Puk 2  response received with error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Enter Puk 2  response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}
/*-------------------< RIL request to change Sim pim >--------------------------------*/
static PyObject* simChangePinReq(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* reqParams[3]= {};
    const char* newPin ;
    const char* oldPin ;
    const char* aid;
    std::vector<std::string> userInput;

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sssO", &oldPin, &newPin, &aid, &callback)) {
        return NULL;
    }
    printInput("simChangePinReq", newPin, oldPin, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    reqParams[0] = const_cast<char*>(oldPin);
    reqParams[1] = const_cast<char*>(newPin);
    reqParams[2] = const_cast<char*>(aid);
    userInput.push_back(newPin);
    userInput.push_back(oldPin);
    userInput.push_back(aid);
    Py_XINCREF(callback);
    Status s = rilSession.simChangePinReq(reqParams, [callback] (RIL_Errno e, int * retries) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for change pin : " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (retries != NULL)
            {
                std::cout << "Response for Change Pin: "<< std::to_string(*retries)<<std::endl;
                ofs << "Response for Change Pin: "<< std::to_string(*retries)<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "change pin response received with error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received change pin response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}

/*-------------------< RIL request to change Sim pin 2 >--------------------------------*/
static PyObject* simChangePin2Req(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* reqParams[3]= {};
    const char* newPin;
    const char* oldPin;
    const char* aid;
    std::vector<std::string> userInput;

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sssO", &oldPin, &newPin, &aid, &callback)) {
        return NULL;
    }
    printInput("simChangePin2Req", newPin, oldPin, aid);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    reqParams[0] = const_cast<char*>(oldPin);
    reqParams[1] = const_cast<char*>(newPin);
    reqParams[2] = const_cast<char*>(aid);
    userInput.push_back(newPin);
    userInput.push_back(oldPin);
    userInput.push_back(aid);
    Py_XINCREF(callback);
    Status s = rilSession.simChangePin2Req(reqParams, [callback] (RIL_Errno e, int * retries) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for change pin2: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (retries != NULL)
            {
                std::cout << "Response for Change Pin2: "<< std::to_string(*retries)<<std::endl;
                ofs << "Response for Change Pin2: "<< std::to_string(*retries)<<std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "change pin2 response received with error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received change pin2 response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
std::ostream& operator <<(std::ostream& out, const RIL_SMS_Response& resp) {
  out << "Error Code: " << resp.errorCode << std::endl;
  out << "Message Ref: " << resp.messageRef << std::endl;
  out << "Ack PDU:" << std::endl;

  if (resp.ackPDU != nullptr) {
    out << std::hex;
    for (char* c = resp.ackPDU; *c; c++) {
      out << *c << " ";
    }
    out << std::dec;
  }

  return out;
}

static PyObject* translate(const RIL_SMS_Response& smsResponse) {
    PyObject* dSmsResponse = PyDict_New();
    if (dSmsResponse == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dSmsResponse, "messageRef", PyLong_FromLong(smsResponse.messageRef));
    PyDict_SetItemString(dSmsResponse, "errorCode", PyLong_FromLong(smsResponse.errorCode));
    if (smsResponse.ackPDU != nullptr) {
        PyDict_SetItemString(dSmsResponse, "ackPDU", PyBytes_FromString(smsResponse.ackPDU));
    }

    return dSmsResponse;
}

/*-------------------< RIL request to send SMS>--------------------------------*/
static PyObject* sendSms(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    char *payload = nullptr;
    char *smsc = nullptr;
    bool expectMore = false;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sspO", &payload, &smsc, &expectMore, &callback)) {
        return NULL;
    }

    printInput("sendSms", smsc, expectMore);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    if (payload == nullptr) {
        std::cout << "sendSms payload is NULL" << std::endl;
        return NULL;
    }

    std::cout << " sendSms : payload = " << payload << std::endl;
    std::cout << " sendSms : Smsc = " << smsc << std::endl;
    std::cout << " sendSms : expectMore = " << expectMore << std::endl;
    Py_XINCREF(callback);
    Status s = rilSession.sendSms(payload, smsc, expectMore, [callback] (RIL_Errno e, const RIL_SMS_Response& resp) -> void {
            PyObject* dSmsResponse = translate(resp);
            if (dSmsResponse == nullptr) {
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dSmsResponse);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for send SMS : " << e << std::endl;
            std::cout << "resp: " << resp << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "resp: " << resp << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Send SMS response received with error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Successfully sent SMS" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set CDMA SMS broadcast config >----------------*/
static PyObject* setCdmaSmsBroadcastConfig(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    uint32_t input_size;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &input_size, &callback)) {
        return NULL;
    }
    printInput("setCdmaSmsBroadcastConfig", input_size);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    size_t size = input_size;
    auto cfgParams = new RIL_CDMA_BroadcastSmsConfigInfo[size];
    Py_XINCREF(callback);
    Status s = rilSession.setCdmaSmsBroadcastConfig(cfgParams, size, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for set cdma sms broadcast config: " << e << std::endl;
            ofs <<  "Callback error value: " << e << std::endl;
            ofs.close();

        }
    );
    delete []cfgParams;

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send set CDMA sms broadcast config req");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Succeeded to send set CDMA sms broadcast config req" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for delete CDMA SMS on RUIM >----------------*/
static PyObject* deleteCdmaSmsOnRuim(PyObject* self, PyObject* args){
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  int32_t index;
  PyObject* callback;

  if (!PyArg_ParseTuple(args, "iO", &index, &callback)) {
    return NULL;
    }
    printInput("deleteCdmaSmsOnRuim", index);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.deleteCdmaSmsOnRuim(index, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout <<  "Got response for deleting cdma sms on Ruim: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send delete CDMA sms to Ruim req");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Succeeded to send delete CDMA sms to Ruim req" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for get CDMA subscription >----------------*/
static PyObject* getCdmaSubscription(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getCdmaSubscription");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.getCdmaSubscription([callback] (RIL_Errno e, const char* info[5])mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout <<  "Got response for getCdmaSubscription request: " << e << std::endl
            << "mdn: " << (info[0] ? info[0] : "null") << std::endl
            << "hSid: " << (info[1] ? info[1] : "null") << std::endl
            << "hNid: " << (info[2] ? info[2] : "null") << std::endl
            << "min: " << (info[3] ? info[3] : "null") << std::endl
            << "prl: " << (info[4] ? info[4] : "null") << std::endl;

            ofs << "Callback error value: " << e << std::endl
            << "mdn: " << (info[0] ? info[0] : "null") << std::endl
            << "hSid: " << (info[1] ? info[1] : "null") << std::endl
            << "hNid: " << (info[2] ? info[2] : "null") << std::endl
            << "min: " << (info[3] ? info[3] : "null") << std::endl
            << "prl: " << (info[4] ? info[4] : "null") << std::endl;
            ofs.close();
        }

    );


    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "failed to send getCdmaSubscription reqeust");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "getCdmaSubscription request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for get CDMA write SMS to RUIM >----------------*/
static PyObject* writeCdmaSmsToRuim(PyObject* self, PyObject* args){
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  int uTeleserviceID;
  unsigned char bIsServicePresent;
  int uServicecategory;
  uint32_t len;
  uint32_t digit_mode;
  int number_mode;
  int number_type;
  int number_plan;
  int number_of_digits;
  int status;
  PyObject* callback;

  if (!PyArg_ParseTuple(args,
                        "ibiiiiiiiiO",
                        &uTeleserviceID,
                        &bIsServicePresent,
                        &uServicecategory,
                        &len,
                        &digit_mode,
                        &number_mode,
                        &number_type,
                        &number_plan,
                        &number_of_digits,
                        &status,
                        &callback)) {
    return NULL;
    }
    printInput("writeCdmaSmsToRuim",
               uTeleserviceID,
               bIsServicePresent,
               uServicecategory,
               len,
               digit_mode,
               number_mode,
               number_type,
               number_plan,
               number_of_digits,
               status);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_CDMA_SMS_Address addr {};
    addr.digit_mode = static_cast<RIL_CDMA_SMS_DigitMode>(digit_mode);
    if (addr.digit_mode == RIL_CDMA_SMS_DIGIT_MODE_8_BIT)
    {
        addr.number_mode = static_cast<RIL_CDMA_SMS_NumberMode>(number_mode);
        addr.number_type = static_cast<RIL_CDMA_SMS_NumberType>(number_type);
        addr.number_plan = static_cast<RIL_CDMA_SMS_NumberPlan>(number_plan);
    }
    addr.number_of_digits = static_cast<unsigned char>(number_of_digits);

    RIL_CDMA_SMS_Message cdmaSms{};
    cdmaSms.uTeleserviceID = uTeleserviceID;
    cdmaSms.bIsServicePresent = bIsServicePresent;
    cdmaSms.uServicecategory = uServicecategory;
    cdmaSms.sAddress = addr;
    cdmaSms.uBearerDataLen = len;

    cdmaSms.expectMore= 0;

    Py_XINCREF(callback);

    RIL_CDMA_SMS_WriteArgs arg{};
    arg.status = status;
    arg.message = cdmaSms;
    Status s = rilSession.writeCdmaSmsToRuim(arg, [callback] (RIL_Errno e, int32_t recordNumber) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout <<  "Got response for writeCdmaSmsToRuim request: " << e << std::endl;
            std::cout << "record number: " << recordNumber << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "record number: " << recordNumber << std::endl;
            ofs.close();
        }

    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "failed to send writeCdmaSmsToRuim reqeust");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "writeCdmaSmsToRuim request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for report SMS Memory Status >----------------*/
static PyObject* reportSmsMemoryStatus(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    bool storageSpaceAvailable;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "pO", &storageSpaceAvailable, &callback)) {
        return NULL;
    }
    printInput("reportSmsMemoryStatus", storageSpaceAvailable);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.reportSmsMemoryStatus(storageSpaceAvailable, [callback] (RIL_Errno e)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
           if(e == RIL_E_SUCCESS)
           {
               std::cout << "Successfully reported SMS storage space availability Error:  "<< e << std::endl;
               ofs <<"Callback error value: "<< e << std::endl;
           }else{
               std::cout << "Failed to report SMS storage space availability Error:  "<< e << std::endl;
               ofs << "Callback error value: "<< e << std::endl;
           }
           ofs.close();
        }
    );


    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to issue request to report SMS storage space availability.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Succeeded to send request to report SMS storage space availability." << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for send USSD >----------------*/
static PyObject* sendUssd(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string ussd;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO", &ussd, &callback)) {
        return NULL;
    }
    printInput("sendUssd", ussd);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.sendUssd(ussd, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for SendUSSD request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send SendUSSD.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "SendUSSD request sent succesfully." << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for report SMS Memory Status >----------------*/
static PyObject* getClir(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getClir");
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.getClir([callback] (RIL_Errno e, int m, int n)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for GetClir request." << e << std::endl;
            std::cout << "m: " << m << std::endl;
            std::cout << "n: " << n << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            std::cout << "m: " << m << std::endl;
            std::cout << "n: " << n << std::endl;
            ofs.close();
        }
    );


    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send GetClir");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "GetClir request sent succesfully. " << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for send USSD >----------------*/
static PyObject* setClir(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int type;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &type, &callback)) {
        return NULL;
    }
    printInput("setClir", type);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.setClir(&type, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for SetClir request. " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send SetClir");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "SetClir request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator <<(std::ostream& os, const RIL_CallForwardInfo &arg )
{
    os << "RIL_CallForwardInfo: " << std::endl;
    os << "status: " << arg.status << std::endl;
    os << "reason: " << arg.reason << std::endl;
    os << "serviceClass: " << arg.serviceClass << std::endl;
    os << "toa: " << arg.toa << std::endl;
    if (arg.number) {
        os << "number: " << arg.number << std::endl;
    }
    os << "timeSeconds: " << arg.timeSeconds << std::endl;
    return os;
}


/*------------< RIL request for query call forward status >----------------*/
static PyObject* queryCallForwardStatus(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int status;
    int reason;
    int serviceClass;
    int toa;
    char* number;
    int timeSeconds;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiiisiO", &status, &reason, &serviceClass, &toa, &number, &timeSeconds, &callback)) {
        return NULL;
    }
    printInput("queryCallForwardStatus", status, reason, serviceClass, toa, number, timeSeconds);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    size_t numSsInfos = 1;
    RIL_CallForwardInfo callFwdInfo{};
    callFwdInfo.status = status;
    callFwdInfo.reason = reason;
    callFwdInfo.serviceClass = serviceClass;
    callFwdInfo.toa = toa;
    callFwdInfo.number = number;
    callFwdInfo.timeSeconds = timeSeconds;
    Status s = rilSession.queryCallForwardStatus(callFwdInfo, [callback] (RIL_Errno e, const size_t numCallFwdInfos, const RIL_CallForwardInfo** callFwdInfos)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for queryCallForwardStatus request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            std::cout << "numCallFwdInfos: " << numCallFwdInfos << std::endl;
            ofs << "numCallFwdInfos: " << numCallFwdInfos << std::endl;
            if (callFwdInfos) {
                for (int i = 0; i < numCallFwdInfos; i++) {
                    if (callFwdInfos[i]) {
                        std::cout << *(callFwdInfos[i]);
                        ofs << *(callFwdInfos[i]) << std::endl;
                    }
                }
            }
            ofs.close();
        }
    );


    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send queryCallForwardStatus");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "queryCallForwardStatus request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set Call Forward >----------------*/
static PyObject* setCallForward(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int status;
    int reason;
    int serviceClass;
    int toa;
    char* number;
    int timeSeconds;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiiisiO", &status, &reason, &serviceClass, &toa, &number, &timeSeconds, &callback)) {
        return NULL;
    }
    printInput("setCallForward", status, reason, serviceClass, toa, number, timeSeconds);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    RIL_CallForwardInfo callFwdInfo{};
    callFwdInfo.status = status;
    callFwdInfo.reason = reason;
    callFwdInfo.serviceClass = serviceClass;
    callFwdInfo.toa = toa;
    callFwdInfo.number = number;
    callFwdInfo.timeSeconds = timeSeconds;
    Status s = rilSession.setCallForward(callFwdInfo, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setCallForward request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send setCallForward");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setCallForward request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for query call waiting >----------------*/
static PyObject* queryCallWaiting(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int serviceClass;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &serviceClass, &callback)) {
        return NULL;
    }
    printInput("queryCallWaiting", serviceClass);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.queryCallWaiting(serviceClass, [callback] (RIL_Errno e,  const int enabled, const int serviceClass)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for queryCallWaiting request: " << e << std::endl;
            std::cout << "serviceClass: " << serviceClass << std::endl;
            std::cout << "enabled: " << enabled << std::endl;

            ofs << "Callback error value: " << e << std::endl;
            ofs << "serviceClass: " << serviceClass << std::endl;
            ofs << "enabled: " << enabled << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send queryCallWaiting");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "queryCallWaiting request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set Call Waiting >----------------*/
static PyObject* setCallWaiting(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int serviceClass;
    int enabled;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiO", &serviceClass, &enabled, &callback)) {
        return NULL;
    }
    printInput("setCallWaiting", serviceClass, enabled);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.setCallWaiting(enabled, serviceClass, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setCallWaiting request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send setCallWaiting");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setCallWaiting request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for change Barring Password >----------------*/
static PyObject* changeBarringPassword(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string facility;
    std::string oldPassword;
    std::string newPassword;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sssO", &facility, &oldPassword, &newPassword, &callback)) {
        return NULL;
    }
    printInput("changeBarringPassword", facility, oldPassword, newPassword);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.changeBarringPassword(facility, oldPassword, newPassword, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for changeBarringPassword request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send changeBarringPassword");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "changeBarringPassword request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for query Clip >----------------*/
static PyObject* queryClip(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("queryClip");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.queryClip([callback] (RIL_Errno e, const int status)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for queryClip request: " << e << std::endl;
            std::cout << "status: " << status << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "status: " << status << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send queryClip ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "queryClip request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set Supp Svc Notification >----------------*/
static PyObject* setSuppSvcNotification(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int enabled;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &enabled, &callback)) {
        return NULL;
    }
    printInput("setSuppSvcNotification", enabled);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.setSuppSvcNotification(enabled, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setSuppSvcNotification request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send setSuppSvcNotification ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setSuppSvcNotification  request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set tty mode >----------------*/
static PyObject* setTtyMode(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int mode;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &mode, &callback)) {
        return NULL;
    }
    printInput("setTtyMode", mode);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.setTtyMode(mode, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setTtyMode request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send setTtyMode");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setTtyMode request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for query Clip >----------------*/
static PyObject* getTtyMode(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getTtyMode");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.getTtyMode([callback] (RIL_Errno e, int mode)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for getTtyMode  request: " << e << std::endl;
            std::cout << "mode: " << mode << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "mode: " << mode << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send getTtyMode ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "getTtyMode request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set Cdma voice pref mode >----------------*/
static PyObject* setCdmaVoicePrefMode(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int mode;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &mode, &callback)) {
        return NULL;
    }
    printInput("setCdmaVoicePrefMode", mode);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.setCdmaVoicePrefMode(mode, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setCdmaVoicePrefMode request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send setCdmaVoicePrefMode");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setCdmaVoicePrefMode request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for get Cdma Voice Pref Mode >----------------*/
static PyObject* getCdmaVoicePrefMode (PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getCdmaVoicePrefMode");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.getCdmaVoicePrefMode ([callback] (RIL_Errno e, int mode)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for getCdmaVoicePrefMode  request: " << e << std::endl;
            std::cout << "mode: " << mode << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "mode: " << mode << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send getCdmaVoicePrefMode ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "getCdmaVoicePrefMode request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for send Cdma flash >----------------*/
static PyObject* sendCdmaFlash(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string flash;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO", &flash, &callback)) {
        return NULL;
    }
    printInput("sendCdmaFlash", flash);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.sendCdmaFlash(flash, [callback] (RIL_Errno e)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for sendCdmaFlash request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send sendCdmaFlash ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "sendCdmaFlash  request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}


/*------------< RIL request for set Radio Capability >----------------*/
static PyObject* setRadioCapability(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int version;
    int phase;
    int rat;
    int status;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiiiO", &version, &phase, &rat, &status, &callback)) {
        return NULL;
    }
    printInput("setRadioCapability", version, phase, rat, status);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    RIL_RadioCapability radioCap {};
    radioCap.version = version;
    radioCap.phase = phase;
    radioCap.rat = rat;
    radioCap.status = status;
    Status s = rilSession.setRadioCapability(radioCap, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setRadioCapability request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send setRadioCapability ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setRadioCapability request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}


/*------------< RIL request for set Radio Capability >----------------*/
static PyObject* getRadioCapability(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getRadioCapability");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.getRadioCapability([callback] (RIL_Errno e , const RIL_RadioCapability radio_capability) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for getRadioCapability request: " << e << std::endl;
            std::cout << "The Version of structure is: " << radio_capability.version << std::endl;
            std::cout << "The Unique session value defined by framework: " << radio_capability.session << std::endl;
            std::cout << "The phase is: " << radio_capability.phase << std::endl;
            std::cout << "The rat is: " << radio_capability.rat << std::endl;
            std::cout << "The status is: " << radio_capability.status << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "The Version of structure is: " << radio_capability.version << std::endl;
            ofs << "The Unique session value defined by framework: " << radio_capability.session << std::endl;
            ofs << "The phase is: " << radio_capability.phase << std::endl;
            ofs << "The rat is: " << radio_capability.rat << std::endl;
            ofs << "The status is: " << radio_capability.status << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send getRadioCapability ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "getRadioCapability request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for send Cdma Burst Dtmf >----------------*/
static PyObject* sendCdmaBurstDtmf(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string dtmfString;
    int on;
    int off;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "siiO", &dtmfString, &on, &off, &callback)) {
        return NULL;
    }
    printInput("sendCdmaBurstDtmf", dtmfString, on, off);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.sendCdmaBurstDtmf(dtmfString, on, off,  [callback] (RIL_Errno e)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for sendCdmaBurstDtmf request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send sendCdmaBurstDtmf ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "sendCdmaBurstDtmf request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for get IMS Reg State >----------------*/
static PyObject* getImsRegState(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getImsRegState");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.getImsRegState([callback] (RIL_Errno e, const bool& registered, const RIL_RadioTechnologyFamily& rat)mutable -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for getImsRegState request: " << e << std::endl;
            std::cout << "registered: " << registered << std::endl;
            std::cout << "rat: " << rat << std::endl;

            ofs << "Callback error value: " << e << std::endl;
            ofs << "registered: " << registered << std::endl;
            ofs << "rat: " << rat << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send getImsRegState ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "getImsRegState request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set Network Selection Mannual >----------------*/
static PyObject* setNetworkSelectionManual(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* mcc = nullptr;
    const char* mnc = nullptr;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "ssO", &mcc, &mnc, &callback)) {
        return NULL;
    }
    printInput("setNetworkSelectionManual", mcc, mnc);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.setNetworkSelectionManual(mcc, mnc, [mcc, mnc, callback] (RIL_Errno e)mutable-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for setNetworkSelectionManual request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (e == RIL_Errno::RIL_E_SUCCESS)
            {
                std::cout << "Successfully requested to select network with MCC: " << mcc << std::endl;
                std::cout << "Successfully requested to select network with MNC: " << mnc << std::endl;
                ofs << "Successfully requested to select network with MCC: " << mcc << std::endl;
                ofs << "Successfully requested to select network with MNC: " << mnc << std::endl;
            }
            else
            {
                std::cout << "Failed to request to select network with MCC: " << mcc << std::endl;
                std::cout << "Failed to request to select network with MNC: " << mnc << std::endl;
                ofs << "Failed to request to select network with MCC: " << mcc << std::endl;
                ofs << "Failed to request to select network with MNC: " << mnc << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send setNetworkSelectionManual ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setNetworkSelectionManual request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set Network Selection Mannual >----------------*/
static PyObject* getCdmaSubscriptionSource(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getCdmaSubscriptionSource");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.getCdmaSubscriptionSource([callback] (RIL_Errno e, RIL_CdmaSubscriptionSource subSource)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for CDMA subscription source: " << e << std::endl;
            std::cout << "CDMA Subscription Source: " << subSource << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "CDMA Subscription Source: " << subSource << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to issue request to get the CDMA subscription source ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "CDMA subscription source request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translate(const RIL_OperatorInfo& operatorInfo) {
    PyObject* dOperatorInfo = PyDict_New();
    if (dOperatorInfo == nullptr) {
        return nullptr;
    }

    if (operatorInfo.alphaLong != nullptr) {
        PyDict_SetItemString(dOperatorInfo, "alphaLong", PyUnicode_FromString(operatorInfo.alphaLong));
    }

    if (operatorInfo.alphaShort != nullptr) {
        PyDict_SetItemString(dOperatorInfo, "alphaShort", PyUnicode_FromString(operatorInfo.alphaShort));
    }

    if (operatorInfo.operatorNumeric != nullptr) {
        PyDict_SetItemString(dOperatorInfo, "operatorNumeric", PyUnicode_FromString(operatorInfo.operatorNumeric));
    }

    PyDict_SetItemString(dOperatorInfo, "status", PyLong_FromLong(operatorInfo.status));

    return dOperatorInfo;
}

static PyObject* translate(const std::vector<RIL_OperatorInfo>& operatorInfos) {
    PyObject* lOperatorInfos = PyList_New(0);
    if (lOperatorInfos == nullptr) {
        return nullptr;
    }

    for (const RIL_OperatorInfo& operatorInfo : operatorInfos) {
        PyObject* dOperatorInfo = translate(operatorInfo);
        if (dOperatorInfo == nullptr) {
            return nullptr;
        }
        PyList_Append(lOperatorInfos, dOperatorInfo);
    }

    return lOperatorInfos;
}

/*------------< RIL request for set Network Selection Mannual >----------------*/
static PyObject* queryAvailableNetworks(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("queryAvailableNetworks");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.queryAvailableNetworks([callback] (RIL_Errno e, const std::vector<RIL_OperatorInfo>& nw_list)-> void {
            PyObject* lOperatorInfos = translate(nw_list);
            if (lOperatorInfos == nullptr) {
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, lOperatorInfos);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for query available networks request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            for(auto i = 0; i < nw_list.size(); i++)
            {
                std::cout << "alphaLong: " << nw_list[i].alphaLong << std::endl;
                std::cout << "alphaShort: " << nw_list[i].alphaShort << std::endl;
                std::cout << "operatorNumeric: " << nw_list[i].operatorNumeric << std::endl;
                std::cout << "status: " << nw_list[i].status << std::endl;

                ofs << "alphaLong: " << nw_list[i].alphaLong << std::endl;
                ofs << "alphaShort: " << nw_list[i].alphaShort << std::endl;
                ofs << "operatorNumeric: " << nw_list[i].operatorNumeric << std::endl;
                ofs << "status: " << nw_list[i].status << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send queryAvailableNetworks ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "queryAvailableNetworks request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for get CDMA Roaming preference >----------------*/
static PyObject* getCdmaRoamingPreference(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("getCdmaRoamingPreference");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);

    Status s = rilSession.getCdmaRoamingPreference([callback] (RIL_Errno e, RIL_CdmaRoamingPreference roamingPref)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            if (e == RIL_Errno::RIL_E_SUCCESS) {
                std::cout << "Got response for CDMA roaming preference: " << e << std::endl;
                ofs << "Callback error value: " << e << std::endl;
                std::cout << "CDMA Roaming Preference: " << roamingPref << "." << std::endl;
                ofs << "CDMA Roaming Preference: " << roamingPref << "." << std::endl;
            }
            else
            {
                std::cout << "Got response for CDMA roaming preference failure: " << e << std::endl;
                ofs << "Callback error value: " << e << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to issue request to get the CDMA roaming preference");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "CDMA roaming preference request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for shut down >----------------*/
static PyObject* shutDown(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("shutDown");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.shutDown([callback] (RIL_Errno e)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Shut Down request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send shutDown");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Shut down request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for enter network depersonalization >----------------*/
static PyObject* EnterNetworkDepersonalization(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* command[1]= {};
    const char* cmd ;
    std::vector<std::string> userInput;

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO", &cmd, &callback)) {
        return NULL;
    }
    printInput("EnterNetworkDepersonalization", cmd);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    command[0] = const_cast<char *>(cmd);
    userInput.push_back(cmd);
    Py_XINCREF(callback);
    Status s = rilSession.EnterNetworkDepersonalization(command, [callback] (RIL_Errno e, int32_t *res) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Network Deperso req: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (res != NULL)
            {
                std::cout << "Response for Network Personalization: " << std::to_string(*res)<< std::endl;
                ofs << "Response for Network Personalization: " << std::to_string(*res)<< std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Networkdeperso with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received  Networkdeperso response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));

}

/*------------< RIL request for Stk Send Envolope command>----------------*/
static PyObject* StkSendEnvelopeCommand(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string input;
    int len;

    PyObject* callback;
    if (!PyArg_ParseTuple(args, "siO", &input, &len, &callback)) {
        return NULL;
    }
    printInput("StkSendEnvelopeCommand", input, len);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    const char *command = const_cast<char *>(input.c_str());
    len = strlen(command) + 1;
    Status s = rilSession.StkSendEnvelopeCommand(command, len, [callback] (RIL_Errno e, const char *res,size_t size)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for envelope command request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if((res != NULL)&&(size > 0))
            {
                std::cout<<"Response data :"<< std::string(res)<< std::endl;
                std::cout<<"Length of the response"<< size<< std::endl;

                ofs << "Response data :"<< std::string(res)<< std::endl;
                ofs <<"Length of the response"<< size<< std::endl;

            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for envelope command request");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for envelope command request" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}


/*------------< RIL request for Stk Send Terminal Response>----------------*/
static PyObject* StkSendTerminalResponse(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string input;
    int len;

    PyObject* callback;
    if (!PyArg_ParseTuple(args, "siO", &input, &len, &callback)) {
        return NULL;
    }
    printInput("StkSendTerminalResponse", input, len);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    const char *command = const_cast<char *>(input.c_str());
    len = strlen(command) + 1;
    Status s = rilSession.StkSendTerminalResponse(command, len, [callback] (RIL_Errno e)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Terminal response command request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for Terminal command request");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for Terminal Response command request" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for Stk handle call set up requested from sim>----------------*/
static PyObject* StkHandleCallSetupRequestedFromSim(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string userInput;

    PyObject* callback;
    if (!PyArg_ParseTuple(args, "sO", &userInput, &callback)) {
        return NULL;
    }
    printInput("StkHandleCallSetupRequestedFromSim", userInput);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    int command = std::stoi(userInput,NULL,16);
    Status s = rilSession.StkHandleCallSetupRequestedFromSim(command, [callback] (RIL_Errno e)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for callsetuprequestedrequest: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Failure response for Callsetuprequestedfromsim request");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Sucess response for CallSetupRequested from sim request" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for is Stk service running>----------------*/
static PyObject* ReportStkServiceIsRunning(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("ReportStkServiceIsRunning");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.ReportStkServiceIsRunning([callback] (RIL_Errno e)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Stk service running req: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Stk service running with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Stk service running  response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for start LCE data>----------------*/
static PyObject* StartLceData(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int32_t interval;
    int32_t mode;
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "iiO",&interval, &mode, &callback)) {
        return NULL;
    }
    printInput("StartLceData", interval, mode);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.StartLceData(interval, mode, [callback] (RIL_Errno e, const RIL_LceStatusInfo* response)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for StartLCEData request: " + std::to_string(e) << std::endl;
            ofs << "Callback error value: " + std::to_string(e) << std::endl;
            if (response) {
                std::cout << "Start LCE -> LCE status: " + std::to_string(response->lce_status) << std::endl;
                std::cout << "Start LCE -> Actual Interval in ms: " + std::to_string(response->actual_interval_ms) << std::endl;

                ofs << "Start LCE -> LCE status: " + std::to_string(response->lce_status) << std::endl;
                ofs << "Start LCE -> Actual Interval in ms: " + std::to_string(response->actual_interval_ms) << std::endl;
        }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received start LCE data with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Start LCE data response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for stop LCE data>----------------*/
static PyObject* StopLceData(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("StopLceData");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.StopLceData([callback] (RIL_Errno e, const RIL_LceStatusInfo* response)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for StopLCEData request: " + std::to_string(e) << std::endl;
            ofs << "Callback error value: " + std::to_string(e) << std::endl;
            if (response) {
                std::cout << "Stop LCE -> LCE status: " + std::to_string(response->lce_status) << std::endl;
                std::cout << "Stop LCE -> Actual Interval in ms: " + std::to_string(response->actual_interval_ms) << std::endl;

                ofs << "Stop LCE -> LCE status: " + std::to_string(response->lce_status) << std::endl;
                ofs << "Stop LCE -> Actual Interval in ms: " + std::to_string(response->actual_interval_ms) << std::endl;
        }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Stop LCE data with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Stop LCE data response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}


/*------------< RIL request for Pull Ice Data>----------------*/
static PyObject* PullLceData(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("PullLceData");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.PullLceData([callback] (RIL_Errno e, const RIL_LceDataInfo* response)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for PullLCEData request: " + std::to_string(e) << std::endl;
            ofs << "Callback error value: " + std::to_string(e) << std::endl;
            if (response) {
                std::cout << "Pull LCE -> Last hop capacity in kbps: " + std::to_string(response->last_hop_capacity_kbps) << std::endl;
                std::cout << "Pull LCE -> Confidence level: " + std::to_string(response->confidence_level) << std::endl;
                std::cout << "Pull LCE -> LCE suspended status: " + std::to_string(response->lce_suspended) << std::endl;

                ofs << "Pull LCE -> Last hop capacity in kbps: " + std::to_string(response->last_hop_capacity_kbps) << std::endl;
                ofs << "Pull LCE -> Confidence level: " + std::to_string(response->confidence_level) << std::endl;
                ofs << "Pull LCE -> LCE suspended status: " + std::to_string(response->lce_suspended) << std::endl;
        }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received Pull LCE data with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received Pull LCE data response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set Link Cap Filter>----------------*/
static PyObject* SetLinkCapFilter(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int enable;
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "iO", &enable, &callback)) {
        return NULL;
    }
    printInput("SetLinkCapFilter", enable);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    RIL_ReportFilter enable_bit = RIL_ReportFilter::RIL_REPORT_FILTER_ENABLE;
    switch (enable)
    {
        case 0:
            enable_bit = RIL_ReportFilter::RIL_REPORT_FILTER_DISABLE;
            break;
        default:
            enable_bit = RIL_ReportFilter::RIL_REPORT_FILTER_ENABLE;
            break;
    }
    Status s = rilSession.SetLinkCapFilter(enable_bit, [callback] (RIL_Errno e, const int* response)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for SetLinkCapFilterMessage request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if(response)
            {
                std::cout << "SetLinkCapFilter response is: " << std::to_string(*response) << std::endl;
                ofs << "SetLinkCapFilter response is: " << std::to_string(*response) << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received SetLinkCapFilter with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received SetLinkCapFilter response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set Link Cap Filter>----------------*/
static PyObject* SetLinkCapRptCriteria(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("SetLinkCapRptCriteria");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    RIL_LinkCapCriteria criteria = {};
    // Default Values provided by telephony
    criteria.hysteresisMs = 3000;
    criteria.hysteresisDlKbps = 50;
    criteria.hysteresisUlKbps = 50;
    criteria.thresholdsDownLength = 11;
    criteria.thresholdsDownlinkKbps = (int*) calloc (criteria.thresholdsDownLength, sizeof(int));
    if (criteria.thresholdsDownlinkKbps) {
        criteria.thresholdsDownlinkKbps[0] = 100;
        criteria.thresholdsDownlinkKbps[1] = 500;
        criteria.thresholdsDownlinkKbps[2] = 1000;
        criteria.thresholdsDownlinkKbps[3] = 5000;
        criteria.thresholdsDownlinkKbps[4] = 10000;
        criteria.thresholdsDownlinkKbps[5] = 20000;
        criteria.thresholdsDownlinkKbps[6] = 50000;
        criteria.thresholdsDownlinkKbps[7] = 100000;
        criteria.thresholdsDownlinkKbps[8] = 200000;
        criteria.thresholdsDownlinkKbps[9] = 500000;
        criteria.thresholdsDownlinkKbps[10] = 1000000;
    }
    criteria.thresholdsUpLength = 9;
    criteria.thresholdsUplinkKbps = (int*) calloc (criteria.thresholdsUpLength, sizeof(int));
    if (criteria.thresholdsUplinkKbps) {
        criteria.thresholdsUplinkKbps[0] = 100;
        criteria.thresholdsUplinkKbps[1] = 500;
        criteria.thresholdsUplinkKbps[2] = 1000;
        criteria.thresholdsUplinkKbps[3] = 5000;
        criteria.thresholdsUplinkKbps[4] = 10000;
        criteria.thresholdsUplinkKbps[5] = 20000;
        criteria.thresholdsUplinkKbps[6] = 50000;
        criteria.thresholdsUplinkKbps[7] = 100000;
        criteria.thresholdsUplinkKbps[8] = 200000;
    }
    criteria.ran = RIL_RAN::LTE_RAN;

    Status s = rilSession.SetLinkCapRptCriteria(criteria, [callback] (RIL_Errno e, const RIL_LinkCapCriteriaResult* response)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for SetLinkCapRptCriteriaMessage request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if(response)
            {
                std::cout << "SetLinkCapRptCriteria response is: " << std::to_string(static_cast<uint8_t>(*response)) << std::endl;
                ofs << "SetLinkCapRptCriteria response is: " << std::to_string(static_cast<uint8_t>(*response)) << std::endl;
            }
            ofs.close();
        }
    );
    if (criteria.thresholdsDownlinkKbps) {
        free(criteria.thresholdsDownlinkKbps);
        criteria.thresholdsDownlinkKbps = NULL;
    }
    if (criteria.thresholdsUplinkKbps) {
        free(criteria.thresholdsUplinkKbps);
        criteria.thresholdsUplinkKbps = NULL;
    }

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received SetLinkCapFilter with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received SetLinkCapFilter response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/*------------< RIL request for set preferred data modem>----------------*/
static PyObject* SetPreferredDataModem(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int modemId;

    PyObject* callback;
    if (!PyArg_ParseTuple(args, "iO", &modemId, &callback)) {
        return NULL;
    }
    printInput("SetPreferredDataModem", modemId);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.SetPreferredDataModem(modemId, [callback] (RIL_Errno e, const Status status, const RIL_ResponseError* response)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for SetPreferredDataModem request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (status == Status::FAILURE)
            {
                std::cout << "SetPreferredDataModem -> Failure in socket data read: " << std::endl;
                ofs << "SetPreferredDataModem -> Failure in socket data read: " << std::endl;
                return;
            }
            if (response)
            {
                std::cout << "SetPreferredDataModem-> Response is: " + std::to_string(*response) << std::endl;
                ofs << "SetPreferredDataModem-> Response is: " + std::to_string(*response) << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Received SetPreferredDataModem with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Received SetPreferredDataModem response succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator<<(std::ostream& os, const RIL_IMS_Registration& imsRegInfo)
{
    os << imsRegInfo.state << std::endl;
    if (imsRegInfo.state == RIL_IMS_REG_STATE_NOT_REGISTERED) {
        os << "Error Code: " << imsRegInfo.errorCode << std::endl;
        if (imsRegInfo.errorMessage) {
            os << "Error Message : " << imsRegInfo.errorMessage << std::endl;
        }
    }
    os << "Radio Technology: " << getRatFromValue(imsRegInfo.radioTech) << std::endl;
    return os;
}
/*------------< RIL request for set preferred data modem>----------------*/
static PyObject* imsGetRegistrationState(PyObject* self, PyObject* args){
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("imsGetRegistrationState");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
    Status s = rilSession.imsGetRegistrationState([callback] (RIL_Errno e, const RIL_IMS_Registration& info)-> void {

            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for imsGetRegistrationState request: " << e << std::endl;
            std::cout << "Info: " << info << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "Info: " << info << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Sent imsGetRegistrationState request with  error");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Sent imsGetRegistrationState request succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< Dial Request and Response >------------------------------------*/
static PyObject* imsDial(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
     const char* address;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO", &address, &callback)) {
        return NULL;
    }
    printInput("imsDial", address);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }
    // std::cout << "addrress as string: " << address <<"address size" << address.size() << std::endl;
    Py_XINCREF(callback);
    RIL_IMS_Dial dialParams = {};
    std::cout << "addrress as string: " << address << std::endl;
    dialParams.address = const_cast<char*>(address);
    std::cout << "address: " << dialParams.address << std::endl;
    Status s = rilSession.imsDial(
        dialParams,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for IMS dial request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send imsDial.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsDial request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* imsAnswer(PyObject* self, PyObject* args){
    int ims_calltype;
    int ims_presentation;
    int ims_rttmode;
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiiO", &ims_calltype, &ims_presentation, &ims_rttmode, &callback)) {
        return NULL;
    }
    printInput("imsAnswer", ims_calltype, ims_presentation, ims_rttmode);
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }
    Py_XINCREF(callback);
        RIL_IMS_Answer answer {};
        answer.callType = static_cast<RIL_IMS_CallType>(ims_calltype);
        answer.presentation = static_cast<RIL_IMS_IpPresentation>(ims_presentation);
        answer.rttMode = static_cast<RIL_IMS_RttMode>(ims_rttmode);
        Status s = rilSession.imsAnswer(answer,[callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                return;
            }
            PyObject* result = PyObject_CallObject(callback, cbArgs);
            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for Answer requese: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send acceptcall");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Answer request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for Converged Answer >-------------------------*/
static PyObject* convergedAnswer(PyObject* self, PyObject* args)
{
  int calltype;
  int presentation;
  int rttmode;
  PyObject* callback;

  if (!PyArg_ParseTuple(args, "iiiO", &calltype, &presentation, &rttmode, &callback)) {
    return NULL;
  }
  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
    return NULL;
  }
  Py_XINCREF(callback);
  RIL_Answer answer{};
  answer.callType = static_cast<RIL_CallType>(calltype);
  answer.presentation = static_cast<RIL_IpPresentation>(presentation);
  answer.rttMode = static_cast<RIL_RttMode>(rttmode);
  Status s = rilSession.answer(answer, [callback](RIL_Errno e) -> void {
    PyObject* cbArgs = Py_BuildValue("(i)", e);
    if (cbArgs == NULL) {
      return;
    }
    PyObject* result = PyObject_CallObject(callback, cbArgs);
    Py_XDECREF(cbArgs);
    Py_XDECREF(result);
    Py_XDECREF(callback);
    std::cout << "Got response for converged Answer request: " << e << std::endl;
  });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "Failed to send converged answer call");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "convergedAnswer request sent succesfully" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }
  return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for ims Hang Up >-------------------------*/
static PyObject* imsHangup(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int connid;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &connid, &callback)) {
        return NULL;
    }
    printInput("imsHangup", connid);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_Hangup hangup {};
    hangup.connIndex = connid;
    Status s = rilSession.imsHangup(
        hangup,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for Hangup request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "HangUp Of Call failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Hangup request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for Converged Hang Up >-------------------------*/
static PyObject* convergedHangup(PyObject* self, PyObject* args)
{
  int connid;
  PyObject* callback;

  if (!PyArg_ParseTuple(args, "iO", &connid, &callback)) {
    return NULL;
  }

  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
    return NULL;
  }

  Py_XINCREF(callback);
  RIL_Hangup hangup{};
  hangup.connIndex = connid;
  Status s = rilSession.hangup(hangup, [callback](RIL_Errno e) -> void {
    PyObject* cbArgs = Py_BuildValue("(i)", e);
    if (cbArgs == NULL) {
      return;
    }
    PyObject* result = PyObject_CallObject(callback, cbArgs);
    Py_XDECREF(cbArgs);
    Py_XDECREF(result);
    Py_XDECREF(callback);
    std::cout << "Got response for convergedHangup request: " << e << std::endl;
  });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "convergedHangUp Of Call failed.");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "convergedHangup request sent successfully" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }
  return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for ims registration change >-------------------------*/
static PyObject* imsRegistrationChange(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int connid;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &connid, &callback)) {
        return NULL;
    }
    printInput("imsRegistrationChange", connid);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_RegistrationState state {};
    state = static_cast<RIL_IMS_RegistrationState>(connid);
    Status s = rilSession.imsRegistrationChange(
        state,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for RegistrationChange request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "RegistrationChange request failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "RegistrationChange request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for ims set service status >-------------------------*/
static PyObject* imsSetServiceStatus(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int callType;
    int status;
    int networkMode;
    int numSsInfos = 1;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiiiO", &callType, &status, &networkMode, &callback)) {
        return NULL;
    }
    printInput("imsSetServiceStatus", callType, status, networkMode);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_ServiceStatusInfo info {};
    info.callType = static_cast<RIL_IMS_CallType>(callType);
    info.accTechStatus.networkMode = static_cast<RIL_RadioTechnology>(networkMode);
    info.accTechStatus.status = static_cast<RIL_IMS_StatusType>(status);

    RIL_IMS_ServiceStatusInfo **ssInfos = nullptr;
    ssInfos = new RIL_IMS_ServiceStatusInfo* [1]();
    if(ssInfos) {
        ssInfos[0] = &info;
    }
    Status s = rilSession.imsSetServiceStatus(
        numSsInfos,(const RIL_IMS_ServiceStatusInfo**)ssInfos,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for ims Set Services Status request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "ims Set Services Status request failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "ims Set Services Status request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    if (ssInfos) {
        delete[] ssInfos;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for ims Query Service Status >-------------------------*/
static PyObject* imsQueryServiceStatus(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O",  &callback)) {
        return NULL;
    }
    printInput("imsQueryServiceStatus");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsQueryServiceStatus( [callback] (RIL_Errno e, const size_t count, const RIL_IMS_ServiceStatusInfo** statusInfo) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for Query ServiceStatus request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            std::cout << "count: " << count << std::endl;
            ofs << "count: " << count << std::endl;
            if (statusInfo)
            {
                for(int i = 0; i < count; i++)
                {
                    std::cout << "statusInfo[" << i <<"]: " << statusInfo[i] << std::endl;
                    ofs << "statusInfo[" << i <<"]: " << statusInfo[i] << std::endl;
                }
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Query ServiceStatus request failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Query ServiceStatus request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator<<(std::ostream& os, const RIL_IMS_SubConfigInfo& info) {
    os << "IMS Sub Config Info : " << std::endl;
    os << "simultStackCount: " << info.simultStackCount << std::endl;
    os << "imsStackEnabledLen: " << info.imsStackEnabledLen << std::endl;
    os << "simultStack: [ ";
    for(int i=0; i < info.imsStackEnabledLen; i++) {
        os << static_cast<bool>(info.imsStackEnabled[i]) << " ";
    }
    os << "]" << std::endl;
    return os;
}

/*-------------------< RIL Request and Response for ims Query Service Status >-------------------------*/
static PyObject* imsGetImsSubConfig(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O",  &callback)) {
        return NULL;
    }
    printInput("imsGetImsSubConfig");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsGetImsSubConfig( [callback] (RIL_Errno e, const RIL_IMS_SubConfigInfo& config) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for Get Ims Sub Config request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            std::cout << "config-> " << config << std::endl;
            ofs << "config: " << config << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Get Ims Sub Config request failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Get Ims Sub Config request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for set indication filter >-------------------------*/
static PyObject* setIndicationFilter(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int32_t filter;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO",&filter, &callback)) {
        return NULL;
    }
    printInput("setIndicationFilter", filter);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = Status::FAILURE;
    s = rilSession.setIndicationFilter(filter, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for setIndicationFilter request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "setIndicationFilter request failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setIndicationFilter request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for set band mode>-------------------------*/
static PyObject* setBandMode(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int bandMode;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO",&bandMode, &callback)) {
        return NULL;
    }
    printInput("setBandMode", bandMode);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = Status::FAILURE;
    s = rilSession.setBandMode(bandMode, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for setBandMode request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "setBandMode request failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "setBandMode request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for exit Emergency Cbmode>-------------------------*/
static PyObject* exitEmergencyCbMode(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("exitEmergencyCbMode");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = Status::FAILURE;
    s = rilSession.exitEmergencyCbMode([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for exitEmergencyCbMode request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "exitEmergencyCbMode request failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "exitEmergencyCbMode request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for Carrier Info for IMSI encryption>-------------------------*/
static PyObject* CarrierInfoForImsiEncryption(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    std::string mcc;
    std::string mnc;
    std::string key;
    std::string identifier;
    int exp;
    int keytype;
    if (!PyArg_ParseTuple(args, "ssssiiO", &mcc, &mnc, &key, &identifier, &exp, &keytype, &callback)) {
        return NULL;
    }
    printInput("CarrierInfoForImsiEncryption", mcc, mnc, key, identifier, exp, keytype);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    RIL_PublicKeyType type = static_cast<RIL_PublicKeyType>(0);
    if (keytype == 1 || keytype == 2)
    {
        type = static_cast<RIL_PublicKeyType>(keytype);
    }
    RIL_CarrierInfoForImsiEncryption imsi = {};
    imsi.mcc = (char*) calloc(mcc.length() + 1, sizeof(char));
    if (imsi.mcc) {
        strncpy_s(imsi.mcc, mcc.length() + 1, mcc.c_str(), _TRUNCATE);
    }
    imsi.mnc = (char*) calloc(mnc.length() + 1, sizeof(char));
    if (imsi.mnc) {
        strncpy_s(imsi.mnc, mnc.length() + 1, mnc.c_str(), _TRUNCATE);
    }
    imsi.carrierKeyLength = key.length();
    imsi.carrierKey = (uint8_t*) calloc(key.length(), sizeof(uint8_t));
    if (imsi.carrierKey) {
        for (int i = 0 ; i < key.length(); i++) {
            imsi.carrierKey[i] = static_cast<uint8_t>(key[i]);
        }
    }
    imsi.keyIdentifier = (char*) calloc(identifier.length() + 1, sizeof(char));
    if (imsi.keyIdentifier) {
        strncpy_s(imsi.keyIdentifier, identifier.length() + 1, identifier.c_str(), _TRUNCATE);
    }
    imsi.expirationTime = exp;
    Py_XINCREF(callback);
    Status s = Status::FAILURE;
    s = rilSession.CarrierInfoForImsiEncryption(imsi, type, [callback] (RIL_Errno e, const Status status, const RIL_Errno* resp) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for Set CarrierInfoForImsiEncryption request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            if (status == Status::FAILURE) {
                std::cout << "Set CarrierInfoForImsiEncryption: Failure in socket data read" << std::endl;
                ofs << "Set CarrierInfoForImsiEncryption: Failure in socket data read" << std::endl;
                return ;
            }
            if (resp) {
                std::cout << "Set CarrierInfoForImsiEncryption Response: " + std::to_string(*resp) << std::endl;
                ofs << "Set CarrierInfoForImsiEncryption Response: " + std::to_string(*resp) << std::endl;
            }
            else{
                std::cout << "Set CarrierInfoForImsiEncryption Response: NULL" << std::endl;
                ofs <<  "Set CarrierInfoForImsiEncryption Response: NULL" << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Set CarrierInfoImsiEncryption request failed.");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Set CarrierInfoImsiEncryption request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for ims Request Conference>-------------------------*/
static PyObject* imsRequestConference(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("imsRequestConference");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = Status::FAILURE;
    s = rilSession.imsRequestConference([callback] (RIL_Errno e, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for imsRequestConference request: " << e << std::endl;
            std::cout << "errorDetails: " << &errorDetails << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs << "errorDetails: " << &errorDetails << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send imsRequestConference");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsRequestConference request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for ims request exit Emergency Cbmode>-------------------------*/
static PyObject* imsRequestExitEmergencyCallbackMode(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("imsRequestExitEmergencyCallbackMode");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = Status::FAILURE;
    s = rilSession.imsRequestExitEmergencyCallbackMode([callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for imsRequestExitEmergencyCallbackMode request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send imsRequestExitEmergencyCallbackMode");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsRequestExitEmergencyCallbackMode request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< RIL Request and Response for ims Explicit call transfer>-------------------------*/
static PyObject* imsExplicitCallTransfer(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    int var;
    uint32_t callId;
    int targetCallId;
    std::string tempString;

    if (!PyArg_ParseTuple(args, "iiisO",&var, &callId, &targetCallId, &tempString, &callback)) {
        return NULL;
    }
    printInput("imsExplicitCallTransfer", var, callId, targetCallId, tempString);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }
    RIL_IMS_EctType arg = static_cast<RIL_IMS_EctType>(var);
    Py_XINCREF(callback);
    RIL_IMS_ExplicitCallTransfer info {};
    info.callId = callId;
    info.ectType = arg;
    info.targetAddress = new char[tempString.size()+1]{};
    tempString.copy(info.targetAddress, tempString.size());
    info.targetCallId = targetCallId;

    Status s = Status::FAILURE;
    s = rilSession.imsExplicitCallTransfer(info, [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }
             PyObject* result = PyObject_CallObject(callback, cbArgs);
             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for ExplicitCallTransfer request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send imsExplicitCallTransfer");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsExplicitCallTransfer request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims DTMF Request and Response >------------------------------------*/
static PyObject* imsDtmf(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* digit;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO", &digit, &callback)) {
        return NULL;
    }
    printInput("imsDtmf", digit);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsDtmf(
        digit[0],
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for imsDtmf request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send imsDtmf");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsDtmf request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims DTMF start Request and Response >------------------------------------*/
static PyObject* imsDtmfStart(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    const char* digit;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO", &digit, &callback)) {
        return NULL;
    }
    printInput("imsDtmfStart", digit);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsDtmfStart(
        digit[0],
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
            std::cout << "Got response for imsDtmfStart  request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send imsDtmfStart ");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsDtmfStart  request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims DTMF stop Request and Response >------------------------------------*/
static PyObject* imsDtmfStop(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("imsDtmfStop");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsDtmfStop(
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for imsDtmfStop request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsDtmfStop send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsDtmfStop request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Modify Call Initiate Request and Response >------------------------------------*/
static PyObject* imsModifyCallInitiate(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    uint32_t callId;
    int calltype;
    int callDomain;
    int rttMode;

    if (!PyArg_ParseTuple(args, "iiiiO",&callId, &calltype, &callDomain, &rttMode, &callback)) {
        return NULL;
    }
    printInput("imsModifyCallInitiate", callId, calltype, callDomain, rttMode);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_CallModifyInfo modifyInfo{};
    modifyInfo.callId = callId;
    modifyInfo.callType = static_cast<RIL_IMS_CallType>(calltype);
    modifyInfo.callDomain = static_cast<RIL_IMS_CallDomain>(callDomain);
    modifyInfo.hasRttMode = 1;
    modifyInfo.rttMode = static_cast<RIL_IMS_RttMode>(rttMode);
    modifyInfo.hasCallModifyFailCause = 0;

    Status s = rilSession.imsModifyCallInitiate(modifyInfo,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for imsModifyCallInitiate request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send imsModifyCallInitiate");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsModifyCallInitiate request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Modify Call Confirm Request and Response >------------------------------------*/
static PyObject* imsModifyCallConfirm(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;
    uint32_t callId;
    int calltype;
    int callDomain;
    int rttMode;

    if (!PyArg_ParseTuple(args, "iiiiO",&callId, &calltype, &callDomain, &rttMode, &callback)) {
        return NULL;
    }
    printInput("imsModifyCallConfirm", callId, calltype, callDomain, rttMode);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_CallModifyInfo modifyInfo{};
    modifyInfo.callId = callId;
    modifyInfo.callType = static_cast<RIL_IMS_CallType>(calltype);
    modifyInfo.callDomain = static_cast<RIL_IMS_CallDomain>(callDomain);
    modifyInfo.hasRttMode = 1;
    modifyInfo.rttMode = static_cast<RIL_IMS_RttMode>(rttMode);
    modifyInfo.hasCallModifyFailCause = 0;

    Status s = rilSession.imsModifyCallConfirm(modifyInfo,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for imsModifyCallConfirm request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to send imsModifyCallConfirm");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsModifyCallConfirm request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims add participants Request and Response >------------------------------------*/
static PyObject* imsAddParticipant(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    char* address;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO",&address, &callback)) {
        return NULL;
    }
    printInput("imsAddParticipant", address);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsAddParticipant(address,
        [callback] (RIL_Errno e, const RIL_IMS_SipErrorInfo SipError) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for imsAddParticipant request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;

             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsAddParticipant send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsAddParticipant request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims hold call Request and Response >------------------------------------*/
static PyObject* imsRequestHoldCall(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    uint32_t callId;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO",&callId, &callback)) {
        return NULL;
    }
    printInput("imsRequestHoldCall", callId);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsRequestHoldCall(callId,
        [callback] (RIL_Errno e, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for imsRequestHoldCall request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             std::cout << "errorDetails: " << &errorDetails;
             ofs << "errorDetails: " << &errorDetails;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsRequestHoldCall send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsRequestHoldCall request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims resume call Request and Response >------------------------------------*/
static PyObject* imsRequestResumeCall(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    uint32_t callId;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO",&callId, &callback)) {
        return NULL;
    }
    printInput("imsRequestResumeCall", callId);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsRequestResumeCall(callId,
        [callback] (RIL_Errno e, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for imsRequestResumeCall request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             std::cout << "errorDetails: " << &errorDetails;
             ofs << "errorDetails: " << &errorDetails;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsRequestResumeCall send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsRequestResumeCall request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Cancel Modify Call Request and Response >------------------------------------*/
static PyObject* imsCancelModifyCall(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    uint32_t callId;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO",&callId, &callback)) {
        return NULL;
    }
    printInput("imsCancelModifyCall", callId);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.imsCancelModifyCall(callId,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for imsCancelModifyCall request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsCancelModifyCall send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsCancelModifyCall request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translate(const RIL_IMS_SendSmsResponse& imsSendSmsResponse) {
    PyObject* dImsSendSmsResponse = PyDict_New();
    if (dImsSendSmsResponse == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dImsSendSmsResponse, "status", PyLong_FromLong(imsSendSmsResponse.status));
    PyDict_SetItemString(dImsSendSmsResponse, "messageRef", PyLong_FromLong(imsSendSmsResponse.messageRef));
    PyDict_SetItemString(dImsSendSmsResponse, "rat", PyLong_FromLong(imsSendSmsResponse.rat));

    return dImsSendSmsResponse;
}

/* -------------------------------------< ims send SMS Request and Response >------------------------------------*/
static PyObject* imsSendSms(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    // Payload will be bytes separated by space with each byte encoded as ascii hex
    char *payload = nullptr;
    int messageRef = 0;
    bool isRetry = false;
    char *smsc = nullptr;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "ssipO", &payload, &smsc, &messageRef, &isRetry, &callback)) {
        return NULL;
    }
    printInput("imsSendSms", smsc, messageRef, isRetry);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    if (payload == nullptr) {
        std::cout << "imsSendSms payload is NULL" << std::endl;
        return NULL;
    }

    std::cout << " imsSendSms : payload = " << payload << std::endl;
    std::cout << " imsSendSms : Smsc = " << smsc << std::endl;
    std::cout << " imsSendSms : messageRef = " << messageRef << std::endl;
    std::cout << " imsSendSms : isRetry = " << isRetry << std::endl;

    std::istringstream pduStream(payload);
    std::basic_string<uint8_t> pdu;
    int pduByte = -1;

    do {
      pduByte = -1;
      pduStream >> std::hex >> pduByte;

      if (pduStream.fail()) {
        std::cout << " imsSendSms : wrong pduStream" << std::endl;
        return NULL;
      } else if (pduByte < 0 || pduByte > 255) {
        std::cout << "Value for byte " << (pdu.length() + 1)
                  << " must be between 0 to 255." << std::endl;
        return NULL;
      }
      pdu += static_cast<uint8_t>(pduByte);
    } while (!pduStream.eof());

    Py_XINCREF(callback);

    RIL_IMS_SmsMessage imsSmsMessage = {};
    imsSmsMessage.pdu = pdu.data();
    imsSmsMessage.pduLength = pdu.length();
    imsSmsMessage.format = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP;
    imsSmsMessage.messageRef = messageRef;
    imsSmsMessage.shallRetry = isRetry;
    imsSmsMessage.smsc = smsc;
    imsSmsMessage.smscLength = smsc ? strlen(smsc) : 0;

    std::cout << "IMS SMS PDU:";
    for (size_t i = 0; i < imsSmsMessage.pduLength; i++) {
        std::cout << " " << static_cast<unsigned int>(imsSmsMessage.pdu[i]);
    }
    std::cout << std::endl;

    Status s = rilSession.imsSendSms(imsSmsMessage,
        [callback] (RIL_Errno e, const RIL_IMS_SendSmsResponse& resp) -> void {
            PyObject* dImsSendSmsResponse = translate(resp);
            if (dImsSendSmsResponse == nullptr) {
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(iN)", e, dImsSendSmsResponse);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
            std::cout << "Got response for imsSendSms request: " << e << std::endl;
            ofs << "Callback error value: " << e << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsSendSms send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsSendSms request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Ack SMS Request and Response >------------------------------------*/
static PyObject* ackSms(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int32_t  result;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &result, &callback)) {
        return NULL;
    }
    printInput("ackSms", result);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
     RIL_GsmSmsAck ack{};
    ack.result = ack.cause;
    Status s = rilSession.ackSms(ack,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for ackSms request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "ackSms send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "ackSms request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ackCdmaSms Request and Response >------------------------------------*/
static PyObject* ackCdmaSms(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int32_t  uErrorClass;
    int uSMSCauseCode;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiO",&uErrorClass, &uSMSCauseCode, &callback)) {
        return NULL;
    }
    printInput("ackCdmaSms", uErrorClass, uSMSCauseCode);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_CDMA_SMS_Ack ack{};
    ack.uErrorClass = static_cast<RIL_CDMA_SMS_ErrorClass>(uErrorClass);
    ack.uSMSCauseCode = uSMSCauseCode;
    Status s = rilSession.ackCdmaSms(ack,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for ackCdmaSms request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "ackCdmaSms send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "ackCdmaSms request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}



/* -------------------------------------< ims Ack SMS Request and Response >------------------------------------*/
static PyObject* imsAckSms(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    uint32_t userInput;
    int deliveryStatus;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiO",&userInput, &deliveryStatus, &callback)) {
        return NULL;
    }
    printInput("imsAckSms", userInput, deliveryStatus);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_SmsAck imsSmsAck = {};
    imsSmsAck.messageRef = userInput;
    imsSmsAck.deliveryStatus= static_cast<RIL_IMS_SmsDeliveryStatus>(deliveryStatus);
    Status s = rilSession.imsAckSms(imsSmsAck,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response for imsAckSms request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsAckSms send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsAckSms request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Get SMS format Request and Response >------------------------------------*/
static PyObject* imsGetSmsFormat(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("imsGetSmsFormat");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.imsGetSmsFormat(
        [callback] (RIL_Errno e, RIL_IMS_SmsFormat smsFormat) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsGetSmsFormat request: " << e << std::endl;
             std::cout << "SMSFormat: " << smsFormat << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs << "SMSFormat: " << smsFormat << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsGetSmsFormat send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsGetSmsFormat request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Query Clip Request and Response >------------------------------------*/
static PyObject* imsQueryClip(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("imsQueryClip");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.imsQueryClip(
        [callback] (RIL_Errno e, const RIL_IMS_ClipInfo & clipInfo) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsQueryClip request: " << e << std::endl;
             std::cout << "clipStatus: " << clipInfo.clipStatus << std::endl;
             std::cout << "errorDetails: " << clipInfo.errorDetails << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs << "clipStatus: " << clipInfo.clipStatus << std::endl;
             ofs << "errorDetails: " << clipInfo.errorDetails << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsQueryClip send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsQueryClip request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Get Clir Request and Response >------------------------------------*/
static PyObject* imsGetClir(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    printInput("imsGetClir");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.imsGetClir(
        [callback] (RIL_Errno e, const RIL_IMS_ClirInfo & clirInfo) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsGetClir request: " << e << std::endl;
             std::cout << "action: " << clirInfo.action << std::endl;
             std::cout << "presentation: " << clirInfo.presentation << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs << "action: " << clirInfo.action << std::endl;
             ofs << "presentation: " << clirInfo.presentation << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsGetClir send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsGetClir request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Get Clir Request and Response >------------------------------------*/
static PyObject* imsSetClir(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    uint32_t action;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO",&action, &callback)) {
        return NULL;
    }
    printInput("imsSetClir", action);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }
    RIL_IMS_SetClirInfo info{};
    info.action = action;
    Py_XINCREF(callback);
    Status s = rilSession.imsSetClir(info,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsGetClir request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsSetClir send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsSetClir request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_CallWaitingSettings& imsCallwaitingSettings) {
    out << "Service Class: " << imsCallwaitingSettings.serviceClass << std::endl;
    out << "State: " << (imsCallwaitingSettings.enabled ? "Enabled" : "Disabled");
    return out;
}

std::ostream& operator<<(std::ostream& os, const RIL_IMS_SipErrorInfo& sipErrorInfo) {
    os << "Error Code: " << sipErrorInfo.errorCode << std::endl;
    os << "Error String: ";
    if (sipErrorInfo.errorString != nullptr) {
         os << sipErrorInfo.errorString;
    }
    return os;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_QueryCallWaitingResponse& imsQueryCallWaitingResp) {
    out << "imsQueryCallWaitingResp.sipError: " << imsQueryCallWaitingResp.sipError << std::endl;
    out << "imsQueryCallWaitingResp.callWaitingSettings: " << imsQueryCallWaitingResp.callWaitingSettings <<  std::endl;
    return out;
}

/* -------------------------------------< ims Query Call Waiting Request and Response >------------------------------------*/
static PyObject* imsQueryCallWaiting(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int32_t _userInput;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO",&_userInput, &callback)) {
        return NULL;
    }
    printInput("imsQueryCallWaiting", _userInput);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }
    if (_userInput == -1) {
        Status s = Status::FAILURE;
        return PyLong_FromLong(static_cast<long>(s));
    }

    uint32_t serviceClass = _userInput;
    Py_XINCREF(callback);
    Status s = rilSession.imsQueryCallWaiting(serviceClass,
        [callback] (RIL_Errno e, const RIL_IMS_QueryCallWaitingResponse& callWaitingResp) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsQueryCallWaiting request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             std::cout  << "callWaitingResp: " << callWaitingResp << std::endl;
             ofs << "callWaitingResp: " << callWaitingResp <<  std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsQueryCallWaiting send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsQueryCallWaiting request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims set Call Waiting Request and Response >------------------------------------*/
static PyObject* imsSetCallWaiting(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int32_t userInput;
    bool enabled;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "ipO",&userInput, &enabled, &callback)) {
        return NULL;
    }
    printInput("imsSetCallWaiting", userInput, enabled);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    RIL_IMS_CallWaitingSettings callWaitingSettings {};
    callWaitingSettings.serviceClass = userInput;
    callWaitingSettings.enabled = enabled;
    Py_XINCREF(callback);
    Status s = rilSession.imsSetCallWaiting(callWaitingSettings,
        [callback] (RIL_Errno e, const RIL_IMS_SipErrorInfo sipErrorInfo) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsSetCallWaiting request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             std::cout  << "sipErrorInfo: " << sipErrorInfo << std::endl;
             ofs << "sipErrorInfo: " << sipErrorInfo <<  std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsSetCallWaiting send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsSetCallWaiting request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims set Supp Svc imsSuppSvcStatusStatus Request and Response >------------------------------------*/
static PyObject* imsSetSuppSvcNotification(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int var;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO",&var, &callback)) {
        return NULL;
    }
    printInput("imsSetSuppSvcNotification", var);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    RIL_IMS_ServiceClassStatus info;
    info = static_cast<RIL_IMS_ServiceClassStatus>(var);
    Py_XINCREF(callback);
    Status s = rilSession.imsSetSuppSvcNotification(info,
        [callback] (RIL_Errno e, const RIL_IMS_ServiceClassStatus & srvStatus) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsSetSuppSvcNotification request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             std::cout  << "srvStatus: " << srvStatus << std::endl;
             ofs << "srvStatus: " << srvStatus << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsSetSuppSvcNotification send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsSetSuppSvcNotification request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims deflect call Request and Response >------------------------------------*/
static PyObject* imsDeflectCall(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int connIndex;
    char *number;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "isO",&connIndex, &number,&callback)) {
        return NULL;
    }
    printInput("imsDeflectCall", connIndex, number);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    RIL_IMS_DeflectCallInfo info {};
    info.connIndex = connIndex;
    info.number = number;
    Py_XINCREF(callback);
    Status s = rilSession.imsDeflectCall(info,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsDeflectCall request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsDeflectCall send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsDeflectCall request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator << (std::ostream& os, const RIL_IMS_ColrInfo& colr) {
  os << colr.status << std::endl;
  os << colr.provisionStatus << std::endl;
  os << colr.presentation << std::endl;
  if (colr.errorDetails)
    os << colr.errorDetails << std::endl;
  return os;
}

/* -------------------------------------< ims get colr Request and Response >------------------------------------*/
static PyObject* imsGetColr(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O",&callback)) {
        return NULL;
    }
    printInput("imsGetColr");

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.imsGetColr(
        [callback] (RIL_Errno e, const RIL_IMS_ColrInfo& colr) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsGetColr request: " << e << std::endl;
             std::cout << "colr: " << colr << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs << "colr: " << colr << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsGetColr send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsGetColr request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims set colr Request and Response >------------------------------------*/
static PyObject* imsSetColr(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int status;
    int provisionstatus;
    int presentation;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiiO", &status, &provisionstatus, &presentation, &callback)) {
        return NULL;
    }
    printInput("imsSetColr", status, provisionstatus, presentation);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_ColrInfo colr{};
    colr.status = static_cast<RIL_IMS_ServiceClassStatus>(status);
    colr.provisionStatus = static_cast<RIL_IMS_ServiceClassProvisionStatus>(provisionstatus);
    colr.presentation = static_cast<RIL_IMS_IpPresentation>(presentation);

    Status s = rilSession.imsSetColr(colr,
        [callback] (RIL_Errno e, const RIL_IMS_SipErrorInfo& errInfo) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsSetColr request: " << e << std::endl;
             std::cout << "errorInfo: " << &errInfo << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs << "errorInfo: " << &errInfo << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsSetColr send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsSetColr request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims send Uity Mode Request and Response >------------------------------------*/
static PyObject* imsSendUiTtyMode(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    int mode;
    int userData;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iiO", &mode, &userData, &callback)) {
        return NULL;
    }
    printInput("imsSendUiTtyMode", mode, userData);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_TtyNotifyInfo ttyInfo {};
    ttyInfo.mode = static_cast<RIL_IMS_TtyModeType>(mode);
    ttyInfo.userData = userData;
    Status s = rilSession.imsSendUiTtyMode(ttyInfo,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsSendUiTtyMode request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsSendUiTtyMode send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsSendUiTtyMode request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims send Geo Location Info Request and Response >------------------------------------*/
static PyObject* imsSendGeolocationInfo(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    double latitude;
    double longitude;
    std::string countryCode;
    std::string country;
    std::string state;
    std::string city;
    std::string postalCode;
    std::string street;
    std::string houseNumber;

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "ddsssssssO", &latitude, &longitude, &countryCode, &country, &state, &city, &postalCode, &street, &houseNumber, &callback)) {
        return NULL;
    }
    printInput("imsSendGeolocationInfo",
               latitude,
               longitude,
               countryCode,
               country,
               state,
               city,
               postalCode,
               street,
               houseNumber);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    RIL_IMS_GeolocationInfo geolocationInfo{};
    geolocationInfo.latitude = latitude;
    geolocationInfo.longitude = longitude;
    geolocationInfo.countryCode = const_cast<char*>(countryCode.c_str());
    geolocationInfo.country = const_cast<char*>(country.c_str());
    geolocationInfo.state = const_cast<char*>(state.c_str());
    geolocationInfo.city = const_cast<char*>(city.c_str());
    geolocationInfo.postalCode = const_cast<char*>(postalCode.c_str());
    geolocationInfo.street = const_cast<char*>(street.c_str());
    geolocationInfo.houseNumber = const_cast<char*>(houseNumber.c_str());

    Status s = rilSession.imsSendGeolocationInfo(geolocationInfo,
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsSendGeolocationInfo request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsSendGeolocationInfo send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsSendGeolocationInfo request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Send Rtt Message Request and Response >------------------------------------*/
static PyObject* imsSendRttMessage(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string msg;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "sO", &msg, &callback)) {
        return NULL;
    }
    printInput("imsSendRttMessage", msg);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.imsSendRttMessage(msg.c_str(), msg.size(),
        [callback] (RIL_Errno e) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsSendRttMessage request: " << e << std::endl;
             ofs << "Callback error value: " << e << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsSendRttMessage send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsSendRttMessage request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* -------------------------------------< ims Query Virtual Line Info Request and Response >------------------------------------*/
static PyObject* imsQueryVirtualLineInfo(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::string input;
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "iO", &input, &callback)) {
        return NULL;
    }
    printInput("imsQueryVirtualLineInfo", input);

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Second parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);
    Status s = rilSession.imsQueryVirtualLineInfo(input.c_str(),
        [callback] (RIL_Errno e, const RIL_IMS_QueryVirtualLineInfoResponse& info) -> void {
            PyObject* cbArgs = Py_BuildValue("(i)", e);
            if (cbArgs == NULL) {
                 return;
             }

             PyObject* result = PyObject_CallObject(callback, cbArgs);

             Py_XDECREF(cbArgs);
             Py_XDECREF(result);
             Py_XDECREF(callback);
             std::cout << "Got response imsQueryVirtualLineInfo request: " << e << std::endl;
             std::cout << "msisdn: " << (info.msisdn ? info.msisdn : "null") << std::endl;
             std::cout << "Number of lines: " << info.numLines << std::endl;
             if (info.virtualLines != nullptr) {
                 for (int i = 0; i < info.numLines; i++) {
                     std::cout << "Line " << i << ": " << (info.virtualLines[i] ?
                                info.virtualLines[i] : "null") << std::endl;
                 }
             }
             ofs << "Callback error value: " << e << std::endl;
             ofs << "msisdn: " << (info.msisdn ? info.msisdn : "null") << std::endl;
             ofs << "Number of lines: " << info.numLines << std::endl;
             if (info.virtualLines != nullptr) {
                 for (int i = 0; i < info.numLines; i++) {
                    ofs << "Line" << i << ": " << (info.virtualLines[i] ?
                                info.virtualLines[i] : "null") << std::endl;
                 }
             }
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "imsQueryVirtualLineInfo send failed.");
        return NULL;
    }
    else
    {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "imsQueryVirtualLineInfo request sent succesfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_SignalInfoRecord &arg)
{
  os << "RIL_CDMA_SignalInfoRecord : " << std::endl;
  os << "isPresent: " << (int)arg.isPresent << std::endl;
  os << "signalType: " << (int)arg.signalType << std::endl;
  os << "alertPitch: " << (int)arg.alertPitch << std::endl;
  os << "signal: " << (int)arg.signal << std::endl;
  return os;
}

/* ---------------------< register New Sms Status Report Indication Handler Indication Handler >------------------------------------*/
static PyObject* registerNewSmsStatusReportIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerNewSmsStatusReportIndicationHandler(
        [] (const char *statusReport)-> void {
            std::cout<<"Received New Sms Status Report Indication"<< std::endl;
            ofs << "Received New Sms Status Report Indication: "<< std::endl;
            if(statusReport) {
                std::cout<<"statusReport: "<< statusReport << std::endl;
                ofs << "statusReport: "<< statusReport << std::endl;
            }
            ofs.close();

        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerNewSmsStatusReportIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerNewSmsStatusReportIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------< register Uicc Sub Status Ind Handler >------------------------------------*/
static PyObject* registerUiccSubStatusIndHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerUiccSubStatusIndHandler(
        [] (int status)-> void {
           std::cout << "Received UiccSubStatusInd, status: ";
           ofs << "Received UiccSubStatusInd, status: " << std::endl;
           if(status) {
                 std::cout << "ENABLED" << std::endl;
                 ofs << "ENABLED: " << std::endl;
            } else {
                std::cout << "DISABLED" << std::endl;
                ofs << "DISABLED: " << std::endl;
            }

            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerUiccSubStatusIndHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerUiccSubStatusIndHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------< register Sim Status Changed indiacation handler>------------------------------------*/
static PyObject* registerSimStatusChanged(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerSimStatusChanged(
        [] ()-> void {
            std::cout<<"Received Sim Status Changed Indication: "<< std::endl;
            ofs << "Received Sim Status Changed Indication: "<< std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerSimStatusChanged failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerSimStatusChanged request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------< register Radio Capability Changed IndHandler>------------------------------------*/
static PyObject* registerRadioCapabilityChangedIndHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerRadioCapabilityChangedIndHandler(
        [] (const RIL_RadioCapability radiocap)-> void {
            std::cout<<"Received register Radio Capability Changed IndHandler: "<< std::endl;
            std::cout << "Version of structure: " << radiocap.version << std::endl;
            std::cout << "Unique session value defined by framework: " << radiocap.session << std::endl;
            std::cout << "Phase: " << radiocap.phase << std::endl;
            std::cout << "RIL_RadioAccessFamily for the radio: " << radiocap.rat << std::endl;
            std::cout << "Status: " << radiocap.status << std::endl;
            ofs <<"Received register Radio Capability Changed IndHandler: "<< std::endl;
            ofs << "Version of structure: " << radiocap.version << std::endl;
            ofs << "Unique session value defined by framework: " << radiocap.session << std::endl;
            ofs << "Phase: " << radiocap.phase << std::endl;
            ofs << "RIL_RadioAccessFamily for the radio: " << radiocap.rat << std::endl;
            ofs << "Status: " << radiocap.status << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerRadioCapabilityChangedIndHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerRadioCapabilityChangedIndHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Call Ring Indication Handler>------------------------------------*/
/*static PyObject* registerCallRingIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerCallRingIndicationHandler(
        [] (const RIL_CDMA_SignalInfoRecord* record)-> void {
            std::cout<<"Received CallRingIndication: "<< std::endl;
            ofs << "Received CallRingIndication: "<< std::endl;
            if (record)
            {
                std::cout << *record << std::endl;
                ofs << *record << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerCallRingIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerCallRingIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}*/

static PyObject* translate(const RIL_CDMA_SignalInfoRecord* signalInfoRecord) {
    PyObject* dSignalInfoRecord = PyDict_New();
    if (dSignalInfoRecord == nullptr) {
        std::cout << "dSignalInfoRecord is NULL" << std::endl;
        return nullptr;
    }

    if (signalInfoRecord == nullptr) {
        return dSignalInfoRecord;
    }

    PyDict_SetItemString(dSignalInfoRecord, "isPresent",
        PyLong_FromLong(static_cast<int>(signalInfoRecord->isPresent)));

    PyDict_SetItemString(dSignalInfoRecord, "signalType",
        PyLong_FromLong(static_cast<int>(signalInfoRecord->signalType)));

    PyDict_SetItemString(dSignalInfoRecord, "alertPitch",
        PyLong_FromLong(static_cast<int>(signalInfoRecord->alertPitch)));

    PyDict_SetItemString(dSignalInfoRecord, "signal",
        PyLong_FromLong(static_cast<int>(signalInfoRecord->signal)));

    return dSignalInfoRecord;
}

static PyObject* registerCallRingIndicationHandler(PyObject* self, PyObject* args) {
    PyObject* callback = nullptr;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.registerCallRingIndicationHandler(
        [callback] (const RIL_CDMA_SignalInfoRecord* signalInfoRecord) -> void {
            std::cout << "Received call ring indication" << std::endl;

            PyObject* dSignalInfoRecord = translate(signalInfoRecord);
            if (dSignalInfoRecord == nullptr) {
                std::cout << "dSignalInfoRecord is NULL" << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(sN)", "RIL_UNSOL_CALL_RING", dSignalInfoRecord);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to register for call ring indication.");
        return NULL;
    } else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerCallRingIndicationHandler request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/*-------------------< register for converged CallRingIndication  >-------------------------*/
static PyObject* convergedRegisterCallRingIndicationHdlr(PyObject* self, PyObject* args)
{
  PyObject* callback = nullptr;
  if (!PyArg_ParseTuple(args, "O", &callback)) {
    return NULL;
  }

  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
    return NULL;
  }

  Py_XINCREF(callback);

  Status s = rilSession.registerCallRingIndicationHdlr(
      [callback](const RIL_CDMA_SignalInfoRecord* signalInfoRecord) -> void {
        std::cout << "Received converged call ring indication" << std::endl;

        PyObject* dSignalInfoRecord = translate(signalInfoRecord);
        if (dSignalInfoRecord == nullptr) {
          std::cout << "dSignalInfoRecord is NULL" << std::endl;
          return;
        }

        PyObject* cbArgs = Py_BuildValue("(sN)", "RIL_CALL_UNSOL_CALL_RING", dSignalInfoRecord);
        if (cbArgs == nullptr) {
          return;
        }

        PyObject* result = PyObject_CallObject(callback, cbArgs);

        Py_XDECREF(cbArgs);
        Py_XDECREF(result);
        Py_XDECREF(callback);
      });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "Failed to register for converged call ring indication.");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "convergedRegisterCallRingIndicationHdlr request sent successfully" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }

  return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Signal Strength Indication Handler>------------------------------------*/
static PyObject* registerSignalStrengthIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerSignalStrengthIndicationHandler(
        [] (const RIL_SignalStrength& signalStrength)-> void {
            (void)signalStrength;
             std::cout << "Received signal strength indication" << std::endl;
             ofs << "Received signal strength indication: " << std::endl;
             ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerSignalStrengthIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerSignalStrengthIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}



/* ---------------------<register Unsol Call State Change Indication Handler>------------------------------------*/
static PyObject* registerUnsolCallStateChangeIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    PyObject* callback = nullptr;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.registerUnsolCallStateChangeIndicationHandler(
        [callback] ()-> void {
            std::cout<<"Received Call State Change: "<< std::endl;
            ofs << "Received Call State Change: "<< std::endl;
            ofs.close();

            PyObject* cbArgs = Py_BuildValue("(sO)", "RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED", Py_None);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerUnsolCallStateChangeIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerUnsolCallStateChangeIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Ims Network State Change Indication Handler>------------------------------------*/
static PyObject* registerImsNetworkStateChangeIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerImsNetworkStateChangeIndicationHandler(
        [] ()-> void {
            std::cout<<"Received ImsNetworkStateChangeIndication: "<< std::endl;
            ofs << "Received ImsNetworkStateChangeIndication: "<< std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerImsNetworkStateChangeIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerImsNetworkStateChangeIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translate(const RIL_UUS_Info* uusInfo) {
    PyObject* dUusInfo = PyDict_New();
    if (dUusInfo == nullptr) {
        return nullptr;
    }

    if (uusInfo == nullptr) {
        return dUusInfo;
    }

    PyDict_SetItemString(dUusInfo, "uusType", PyLong_FromLong(uusInfo->uusType));
    PyDict_SetItemString(dUusInfo, "uusDcs", PyLong_FromLong(uusInfo->uusDcs));
    if (uusInfo->uusData != nullptr && uusInfo->uusLength > 0) {
        PyDict_SetItemString(dUusInfo, "uusData", PyUnicode_FromStringAndSize(uusInfo->uusData, uusInfo->uusLength));
    }

    return dUusInfo;
}

static PyObject* translate(const RIL_IMS_SipErrorInfo* sipError) {
    PyObject* dSipError = PyDict_New();
    if (dSipError == nullptr) {
        return nullptr;
    }

    if (sipError == nullptr) {
        return dSipError;
    }

    PyDict_SetItemString(dSipError, "errorCode", PyLong_FromLong(sipError->errorCode));

    if (sipError->errorString != nullptr) {
        PyDict_SetItemString(dSipError, "errorString", PyUnicode_FromString(sipError->errorString));
    }

    return dSipError;
}

static PyObject* translate(const RIL_IMS_CallFailCauseResponse* callFailCause) {
    PyObject* dCallFailCause = PyDict_New();
    if (dCallFailCause == nullptr) {
        return nullptr;
    }

    if (callFailCause == nullptr) {
        return dCallFailCause;
    }

    PyDict_SetItemString(dCallFailCause, "failCause", PyLong_FromLong(callFailCause->failCause));
    PyDict_SetItemString(dCallFailCause, "extendedFailCause", PyLong_FromLong(callFailCause->extendedFailCause));

    if (callFailCause->errorInfo && callFailCause->errorInfoLen > 0) {
        PyDict_SetItemString(dCallFailCause, "errorInfo",
            PyUnicode_FromStringAndSize(reinterpret_cast<char*>(callFailCause->errorInfo), callFailCause->errorInfoLen));
    }

    if (callFailCause->networkErrorString != nullptr) {
        PyDict_SetItemString(dCallFailCause, "networkErrorString",
            PyUnicode_FromString(callFailCause->networkErrorString));
    }

    if (callFailCause->errorDetails) {
        PyObject* errorDetails = translate(callFailCause->errorDetails);
        if (errorDetails == nullptr) {
            return nullptr;
        }

        PyDict_SetItemString(dCallFailCause, "errorDetails", errorDetails);
    }

    return dCallFailCause;
}

static PyObject* translate(const RIL_IMS_ServiceStatusInfo* srvStatusInfo, size_t len) {
    if (srvStatusInfo == nullptr) {
        return PyList_New(0);
    }

    PyObject* lSrvStatusInfo = PyList_New(0);
    if (lSrvStatusInfo == nullptr) {
        return nullptr;
    }

    for (size_t j = 0; j < len; j++) {
        PyObject* dSrvStatusInfo = PyDict_New();
        if (dSrvStatusInfo == nullptr) {
            return nullptr;
        }

        PyDict_SetItemString(dSrvStatusInfo, "callType", PyLong_FromLong(srvStatusInfo[j].callType));
        PyDict_SetItemString(dSrvStatusInfo, "rttMode", PyLong_FromLong(srvStatusInfo[j].rttMode));

        PyObject* dAccTechStatus = PyDict_New();
        if (dAccTechStatus == nullptr) {
            return nullptr;
        }

        PyDict_SetItemString(dAccTechStatus, "networkMode", PyLong_FromLong(srvStatusInfo[j].accTechStatus.networkMode));
        PyDict_SetItemString(dAccTechStatus, "status", PyLong_FromLong(srvStatusInfo[j].accTechStatus.status));
        PyDict_SetItemString(dAccTechStatus, "restrictCause", PyLong_FromLong(srvStatusInfo[j].accTechStatus.restrictCause));

        PyObject* dRegistration = PyDict_New();
        if (dRegistration == nullptr) {
            return nullptr;
        }

        PyDict_SetItemString(dRegistration, "state", PyLong_FromLong(srvStatusInfo[j].accTechStatus.registration.state));
        PyDict_SetItemString(dRegistration, "errorCode", PyLong_FromLong(srvStatusInfo[j].accTechStatus.registration.errorCode));

        if (srvStatusInfo[j].accTechStatus.registration.errorMessage != nullptr) {
            PyDict_SetItemString(dRegistration, "errorMessage",
                PyUnicode_FromString(srvStatusInfo[j].accTechStatus.registration.errorMessage));
        }

        PyDict_SetItemString(dRegistration, "radioTech", PyLong_FromLong(srvStatusInfo[j].accTechStatus.registration.radioTech));

        if (srvStatusInfo[j].accTechStatus.registration.pAssociatedUris != nullptr) {
            PyDict_SetItemString(dRegistration, "pAssociatedUris",
                PyUnicode_FromString(srvStatusInfo[j].accTechStatus.registration.pAssociatedUris));
        }

        PyDict_SetItemString(dAccTechStatus, "registration", dRegistration);
        PyDict_SetItemString(dSrvStatusInfo, "accTechStatus", dAccTechStatus);

        PyList_Append(lSrvStatusInfo, dSrvStatusInfo);
    }

    return lSrvStatusInfo;
}

static PyObject* translate(const RIL_IMS_CallInfo* calls, size_t len) {
    // return an empty list if calls is NULL
    if (calls == nullptr) {
        return PyList_New(0);
    }

    std::cout << "len: " << len << std::endl;

    PyObject* lCalls = PyList_New(0);

    if (lCalls == nullptr) {
        std::cout << "lCalls is NULL" << std::endl;
        return nullptr;
    }

    for (size_t i = 0; i < len; i++) {
        PyObject* dCall = PyDict_New();
        if (dCall == nullptr) {
            std::cout << "dCall is NULL" << std::endl;
            return nullptr;
        }

        PyDict_SetItemString(dCall, "callState", PyLong_FromLong(calls[i].callState));
        PyDict_SetItemString(dCall, "index", PyLong_FromLong(calls[i].index));
        PyDict_SetItemString(dCall, "toa", PyLong_FromLong(calls[i].toa));
        PyDict_SetItemString(dCall, "isMpty", PyLong_FromLong(calls[i].isMpty));

        PyDict_SetItemString(dCall, "isMt", PyLong_FromLong(calls[i].isMt));
        PyDict_SetItemString(dCall, "als", PyLong_FromLong(calls[i].als));
        PyDict_SetItemString(dCall, "isVoice", PyLong_FromLong(calls[i].isVoice));
        PyDict_SetItemString(dCall, "isVoicePrivacy", PyLong_FromLong(calls[i].isVoicePrivacy));

        if (calls[i].number != nullptr) {
            PyDict_SetItemString(dCall, "number", PyUnicode_FromString(calls[i].number));
        }

        PyDict_SetItemString(dCall, "numberPresentation", PyLong_FromLong(calls[i].numberPresentation));

        if (calls[i].name != nullptr) {
            PyDict_SetItemString(dCall, "name", PyUnicode_FromString(calls[i].name));
        }

        PyDict_SetItemString(dCall, "namePresentation", PyLong_FromLong(calls[i].namePresentation));

        if (calls[i].redirNum != nullptr) {
            PyDict_SetItemString(dCall, "redirNum", PyUnicode_FromString(calls[i].redirNum));
        }

        PyDict_SetItemString(dCall, "redirNumPresentation", PyLong_FromLong(calls[i].redirNumPresentation));
        PyDict_SetItemString(dCall, "callType", PyLong_FromLong(calls[i].callType));
        PyDict_SetItemString(dCall, "callDomain", PyLong_FromLong(calls[i].callDomain));
        PyDict_SetItemString(dCall, "callSubState", PyLong_FromLong(calls[i].callSubState));
        PyDict_SetItemString(dCall, "isEncrypted", PyLong_FromLong(calls[i].isEncrypted));
        PyDict_SetItemString(dCall, "isCalledPartyRinging", PyLong_FromLong(calls[i].isCalledPartyRinging));
        PyDict_SetItemString(dCall, "isVideoConfSupported", PyLong_FromLong(calls[i].isVideoConfSupported));

        if (calls[i].historyInfo != nullptr) {
            PyDict_SetItemString(dCall, "historyInfo", PyUnicode_FromString(calls[i].historyInfo));
        }

        PyDict_SetItemString(dCall, "mediaId", PyLong_FromLong(calls[i].mediaId));
        PyDict_SetItemString(dCall, "causeCode", PyLong_FromLong(calls[i].causeCode));
        PyDict_SetItemString(dCall, "rttMode", PyLong_FromLong(calls[i].rttMode));

        if (calls[i].sipAlternateUri != nullptr) {
            PyDict_SetItemString(dCall, "sipAlternateUri", PyUnicode_FromString(calls[i].sipAlternateUri));
        }

        if (calls[i].localAbility != nullptr) {
            PyObject* lLocalAbility = translate(calls[i].localAbility, calls[i].localAbilityLen);
            if (lLocalAbility == nullptr) {
                std::cout << "lLocalAbility is NULL" << std::endl;
                return nullptr;
            }

            PyDict_SetItemString(dCall, "localAbility", lLocalAbility);
        }

        if (calls[i].peerAbility != nullptr) {
            PyObject* lPeerAbility = translate(calls[i].peerAbility, calls[i].peerAbilityLen);
            if (lPeerAbility == nullptr) {
                std::cout << "lPeerAbility is NULL" << std::endl;
                return nullptr;
            }

            PyDict_SetItemString(dCall, "peerAbility", lPeerAbility);
        }

        if (calls[i].callFailCauseResponse != nullptr) {
            PyObject* dCallFailCause = translate(calls[i].callFailCauseResponse);
            if (dCallFailCause == nullptr) {
                std::cout << "dCallFailCause is NULL" << std::endl;
                return nullptr;
            }

            PyDict_SetItemString(dCall, "callFailCauseResponse", dCallFailCause);
        }

        if (calls[i].terminatingNumber != nullptr) {
            PyDict_SetItemString(dCall, "terminatingNumber", PyUnicode_FromString(calls[i].terminatingNumber));
        }

        PyDict_SetItemString(dCall, "isSecondary", PyLong_FromLong(calls[i].isSecondary));

        if (calls[i].verstatInfo != nullptr) {
            PyObject* dVerstatInfo = PyDict_New();
            if (dVerstatInfo == nullptr) {
                std::cout << "dVerstatInfo is NULL" << std::endl;
                return nullptr;
            }

            PyDict_SetItemString(dVerstatInfo, "canMarkUnwantedCall", PyLong_FromLong(calls[i].verstatInfo->canMarkUnwantedCall));
            PyDict_SetItemString(dVerstatInfo, "verificationStatus", PyLong_FromLong(calls[i].verstatInfo->verificationStatus));

            PyDict_SetItemString(dCall, "verstatInfo", dVerstatInfo);
        }

        if (calls[i].uusInfo != nullptr) {
            PyObject* dUusInfo = translate(calls[i].uusInfo);
            if (dUusInfo == nullptr) {
                std::cout << "dUusInfo is NULL" << std::endl;
                return nullptr;
            }

            PyDict_SetItemString(dCall, "uusInfo", dUusInfo);
        }

        if (calls[i].audioQuality != nullptr) {
            PyDict_SetItemString(dCall, "audioQuality", PyLong_FromLong(*calls[i].audioQuality));
        }

        PyDict_SetItemString(dCall, "Codec", PyLong_FromLong(calls[i].Codec));

        if (calls[i].displayText != nullptr) {
            PyDict_SetItemString(dCall, "displayText", PyUnicode_FromString(calls[i].displayText));
        }

        if (calls[i].additionalCallInfo != nullptr) {
            PyDict_SetItemString(dCall, "additionalCallInfo", PyUnicode_FromString(calls[i].additionalCallInfo));
        }

        if (calls[i].childNumber != nullptr) {
            PyDict_SetItemString(dCall, "childNumber", PyUnicode_FromString(calls[i].childNumber));
        }

        PyDict_SetItemString(dCall, "emergencyServiceCategory", PyLong_FromLong(calls[i].emergencyServiceCategory));
        PyDict_SetItemString(dCall, "tirMode", PyLong_FromLong(calls[i].tirMode));

        PyList_Append(lCalls, dCall);
    }

    return lCalls;
}

/* ---------------------<register Ims Call State Change Indication Handler>------------------------------------*/
static PyObject* registerImsCallStateChangedIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.registerImsUnsolCallStateChangedIndicationHandler(
        [callback] (RIL_IMS_CallInfo* calls, size_t numCalls) -> void {
            std::cout << "Received ImsCallStateChangeIndication: " << std::endl;
            std::cout << "numCalls: " << numCalls << std::endl;

            if (calls == nullptr) {
                std::cout << "Payload is NULL." << std::endl;
                return;
            }

            PyObject* lCalls = translate(calls, numCalls);
            if (lCalls == nullptr) {
                std::cout << "lCalls is NULL" << std::endl;
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(sN)", "RIL_IMS_UNSOL_CALL_STATE_CHANGED", lCalls);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);

            ofs << "Received ImsCallStateChangeIndication: " << std::endl;
            for (size_t i = 0; calls != nullptr && i < numCalls; i++) {
                ofs << "calls[" << i << "].callState: " << calls[i].callState << std::endl;
                ofs << "calls[" << i << "].index: " << calls[i].index << std::endl;
                ofs << "calls[" << i << "].toa: " << calls[i].toa << std::endl;
                ofs << "calls[" << i << "].isMpty: " << calls[i].isMpty << std::endl;
                ofs << "calls[" << i << "].isMt: " << calls[i].isMt << std::endl;
                ofs << "calls[" << i << "].als: " << calls[i].als << std::endl;
                ofs << "calls[" << i << "].isVoice: " << calls[i].isVoice << std::endl;
                ofs << "calls[" << i << "].isVoicePrivacy: " << calls[i].isVoicePrivacy << std::endl;
                ofs << "calls[" << i << "].number: " << (calls[i].number ? calls[i].number : "NULL") << std::endl;
                ofs << "calls[" << i << "].numberPresentation: " << calls[i].numberPresentation << std::endl;
                ofs << "calls[" << i << "].name: " << (calls[i].name ? calls[i].name : "NULL") << std::endl;
                ofs << "calls[" << i << "].namePresentation: " << calls[i].namePresentation << std::endl;
                ofs << "calls[" << i << "].redirNum: " << (calls[i].redirNum ? calls[i].redirNum : "NULL") << std::endl;
                ofs << "calls[" << i << "].redirNumPresentation: " << calls[i].redirNumPresentation << std::endl;
                ofs << "calls[" << i << "].callType: " << calls[i].callType << std::endl;
                ofs << "calls[" << i << "].callDomain: " << calls[i].callDomain << std::endl;
                ofs << "calls[" << i << "].callSubState: " << calls[i].callSubState << std::endl;
                ofs << "calls[" << i << "].isEncrypted: " << calls[i].isEncrypted << std::endl;
                ofs << "calls[" << i << "].isCalledPartyRinging: " << calls[i].isCalledPartyRinging << std::endl;
                ofs << "calls[" << i << "].isVideoConfSupported: " << calls[i].isVideoConfSupported << std::endl;
                ofs << "calls[" << i << "].historyInfo: " << (calls[i].historyInfo ? calls[i].historyInfo : "NULL") << std::endl;
                ofs << "calls[" << i << "].mediaId: " << calls[i].mediaId << std::endl;
                ofs << "calls[" << i << "].causeCode: " << calls[i].causeCode << std::endl;
                ofs << "calls[" << i << "].rttMode: " << calls[i].rttMode << std::endl;
                ofs << "calls[" << i << "].sipAlternateUri: " << (calls[i].sipAlternateUri ? calls[i].sipAlternateUri : "NULL") << std::endl;

                ofs << "calls[" << i << "].localAbilityLen: " << calls[i].localAbilityLen << std::endl;
                for (size_t j = 0; calls[i].localAbility != nullptr && j < calls[i].localAbilityLen; j++) {
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".callType: " << calls[i].localAbility[j].callType << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".rttMode: " << calls[i].localAbility[j].rttMode << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".accTechStatus.networkMode: " << calls[i].localAbility[j].accTechStatus.networkMode << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".accTechStatus.status: " << calls[i].localAbility[j].accTechStatus.status << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".accTechStatus.restrictCause: " << calls[i].localAbility[j].accTechStatus.restrictCause << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".accTechStatus.registration.state: " << calls[i].localAbility[j].accTechStatus.registration.state << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".accTechStatus.registration.errorCode: " << calls[i].localAbility[j].accTechStatus.registration.errorCode << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".accTechStatus.registration.errorMessage: "
                        << (calls[i].localAbility[j].accTechStatus.registration.errorMessage ? calls[i].localAbility[j].accTechStatus.registration.errorMessage : "NULL")
                        << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".accTechStatus.registration.radioTech: " << calls[i].localAbility[j].accTechStatus.registration.radioTech << std::endl;
                    ofs << "calls[" << i << "].localAbility" << "[" << j << "]" << ".accTechStatus.registration.pAssociatedUris: "
                        << (calls[i].localAbility[j].accTechStatus.registration.pAssociatedUris ? calls[i].localAbility[j].accTechStatus.registration.pAssociatedUris : "NULL")
                        << std::endl;
                }

                ofs << "calls[" << i << "].peerAbilityLen: " << calls[i].peerAbilityLen << std::endl;
                for (size_t j = 0; calls[i].peerAbility != nullptr && j < calls[i].peerAbilityLen; j++) {
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".callType: " << calls[i].peerAbility[j].callType << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".rttMode: " << calls[i].peerAbility[j].rttMode << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".accTechStatus.networkMode: " << calls[i].peerAbility[j].accTechStatus.networkMode << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".accTechStatus.status: " << calls[i].peerAbility[j].accTechStatus.status << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".accTechStatus.restrictCause: " << calls[i].peerAbility[j].accTechStatus.restrictCause << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".accTechStatus.registration.state: " << calls[i].peerAbility[j].accTechStatus.registration.state << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".accTechStatus.registration.errorCode: " << calls[i].peerAbility[j].accTechStatus.registration.errorCode << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".accTechStatus.registration.errorMessage: "
                        << (calls[i].peerAbility[j].accTechStatus.registration.errorMessage ? calls[i].peerAbility[j].accTechStatus.registration.errorMessage : "NULL")
                        << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".accTechStatus.registration.radioTech: " << calls[i].peerAbility[j].accTechStatus.registration.radioTech << std::endl;
                    ofs << "calls[" << i << "].peerAbility" << "[" << j << "]" << ".accTechStatus.registration.pAssociatedUris: "
                        << (calls[i].peerAbility[j].accTechStatus.registration.pAssociatedUris ? calls[i].peerAbility[j].accTechStatus.registration.pAssociatedUris : "NULL")
                        << std::endl;
                }

                if (calls[i].callFailCauseResponse) {
                    ofs << "calls[" << i << "].callFailCauseResponse->failCause: " << calls[i].callFailCauseResponse->failCause << std::endl;
                    ofs << "calls[" << i << "].callFailCauseResponse->extendedFailCause: " << calls[i].callFailCauseResponse->extendedFailCause << std::endl;

                    ofs << "calls[" << i << "].callFailCauseResponse->errorInfoLen: " << calls[i].callFailCauseResponse->errorInfoLen << std::endl;
                    if (calls[i].callFailCauseResponse->errorInfo && calls[i].callFailCauseResponse->errorInfoLen > 0) {
                        ofs << "calls[" << i << "].callFailCauseResponse->errorInfo: ";
                        for (size_t k = 0; k < calls[i].callFailCauseResponse->errorInfoLen; k++) {
                            ofs << calls[i].callFailCauseResponse->errorInfo[k];
                        }
                        ofs << std::endl;
                    }

                    ofs << "calls[" << i << "].callFailCauseResponse->networkErrorString: "
                        << (calls[i].callFailCauseResponse->networkErrorString ? calls[i].callFailCauseResponse->networkErrorString : "NULL")
                        << std::endl;

                    if (calls[i].callFailCauseResponse->errorDetails) {
                        ofs << "calls[" << i << "].callFailCauseResponse->errorDetails->errorCode: " << calls[i].callFailCauseResponse->errorDetails->errorCode << std::endl;
                        ofs << "calls[" << i << "].callFailCauseResponse->errorDetails->errorString: "
                            << (calls[i].callFailCauseResponse->errorDetails->errorString ? calls[i].callFailCauseResponse->errorDetails->errorString : "NULL")
                            << std::endl;
                    }
                }

                ofs << "calls[" << i << "].terminatingNumber: " << (calls[i].terminatingNumber ? calls[i].terminatingNumber : "NULL") << std::endl;
                ofs << "calls[" << i << "].isSecondary: " << calls[i].isSecondary << std::endl;

                if (calls[i].verstatInfo) {
                    ofs << "calls[" << i << "].verstatInfo->canMarkUnwantedCall: " << calls[i].verstatInfo->canMarkUnwantedCall << std::endl;
                    ofs << "calls[" << i << "].verstatInfo->verificationStatus: " << calls[i].verstatInfo->verificationStatus << std::endl;
                }

                if (calls[i].uusInfo) {
                    ofs << "calls[" << i << "].uusInfo->uusType: " <<  calls[i].uusInfo->uusType << std::endl;
                    ofs << "calls[" << i << "].uusInfo->uusDcs: " << calls[i].uusInfo->uusDcs << std::endl;
                    ofs << "calls[" << i << "].uusInfo->uusLength: " << calls[i].uusInfo->uusLength << std::endl;
                    if (calls[i].uusInfo->uusData && calls[i].uusInfo->uusLength > 0) {
                        ofs << "calls[" << i << "].uusInfo->uusData: " << std::string(calls[i].uusInfo->uusData, calls[i].uusInfo->uusLength) << std::endl;
                    }
                }

                ofs << "calls[" << i << "].audioQuality: " << (calls[i].audioQuality ? std::to_string(*(calls[i].audioQuality)) : "NULL") << std::endl;
                ofs << "calls[" << i << "].Codec: " << calls[i].Codec << std::endl;
                ofs << "calls[" << i << "].displayText: " << (calls[i].displayText ? calls[i].displayText : "NULL") << std::endl;
                ofs << "calls[" << i << "].additionalCallInfo: " << (calls[i].additionalCallInfo ? calls[i].additionalCallInfo : "NULL") << std::endl;
                ofs << "calls[" << i << "].childNumber: " << (calls[i].childNumber ? calls[i].childNumber : "NULL") << std::endl;
                ofs << "calls[" << i << "].emergencyServiceCategory: " << calls[i].emergencyServiceCategory << std::endl;
                ofs << "calls[" << i << "].tirMode: " << calls[i].tirMode << std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerImsCallStateChangedIndicationHandler failed.");
        return NULL;
    } else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerImsCallStateChangedIndicationHandler request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}


/* ---------------------<register Converged Unsol Call State Change Indication
 * Handler>------------------------------------*/
static PyObject* convergedRegisterUnsolCallStateChangedIndicationHandler(PyObject* self,
                                                                         PyObject* args)
{
  PyObject* callback;

  if (!PyArg_ParseTuple(args, "O", &callback)) {
    return NULL;
  }

  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
    return NULL;
  }

  Py_XINCREF(callback);

  Status s = rilSession.registerUnsolCallStateChangedIndicationHandler(
      [callback](const std::vector<RIL_CallInfo> callList) -> void {
        std::cout << "Received convergedUnsolCallStateChangeIndication: " << std::endl;
        size_t numCalls = static_cast<size_t>(callList.size());
        std::cout << "numCalls: " << numCalls << std::endl;

        if (callList.size() <= 0) {
          std::cout << "Payload is NULL." << std::endl;
          return;
        }

        PyObject* lCalls = translate(callList, numCalls);
        if (lCalls == nullptr) {
          std::cout << "lCalls is NULL" << std::endl;
          return;
        }

        PyObject* cbArgs = Py_BuildValue("(sN)", "RIL_CALL_UNSOL_CALL_STATE_CHANGED", lCalls);
        if (cbArgs == nullptr) {
          return;
        }

        PyObject* result = PyObject_CallObject(callback, cbArgs);

        Py_XDECREF(cbArgs);
        Py_XDECREF(result);
      });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "convergedRegisterUnsolCallStateChangedIndicationHandler failed.");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "convergedRegisterUnsolCallStateChangedIndicationHandler request sent successfully"
              << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }

  return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Ims Ring Indication Handler>------------------------------------*/
static PyObject* registerImsUnsolCallRingIndicationHandler(PyObject* self, PyObject* args) {
    PyObject* callback = nullptr;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.registerImsUnsolCallRingIndicationHandler(
        [callback] () -> void {
            std::cout << "Received IMS call ring indication" << std::endl;

            PyObject* cbArgs = Py_BuildValue("(sO)", "RIL_IMS_UNSOL_CALL_RING", Py_None);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Failed to register for IMS call ring indication.");
        return NULL;
    } else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerImsUnsolCallRingIndicationHandler request sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* translate(const RIL_IMS_ServiceStatusInfo** info, const size_t count)
{
  PyObject* lInfo = PyList_New(0);
  if (lInfo == nullptr) {
    return nullptr;
  }

  if (count == 0) {
    return lInfo;
  }

  for (size_t i = 0; i < count; i++) {
    if (info[i] == nullptr) {
      continue;
    }

    PyObject* dInfo = PyDict_New();
    if (dInfo == nullptr) {
      return nullptr;
    }

    PyDict_SetItemString(dInfo, "callType", PyLong_FromLong(info[i]->callType));
    PyDict_SetItemString(dInfo, "accTechStatus", PyLong_FromLong(info[i]->accTechStatus.status));
    PyDict_SetItemString(dInfo, "rttMode", PyLong_FromLong(info[i]->rttMode));

    PyList_Append(lInfo, dInfo);
  }

  return lInfo;
}

/* ---------------------<register Ims Service Status Change Indication Handler>-----------------------*/
static PyObject* registerImsUnsolServiceStatusIndicationHandler(PyObject* self, PyObject* args)
{
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  PyObject* callback = nullptr;
  if (!PyArg_ParseTuple(args, "O", &callback)) {
    return NULL;
  }

  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
    return NULL;
  }

  Py_XINCREF(callback);

  Status s = rilSession.registerImsUnsolServiceStatusIndicationHandler(
      [callback](const size_t count, const RIL_IMS_ServiceStatusInfo** imsSrvStatusInfo) -> void {
        std::cout << "Received IMS service status indication" << std::endl;
        std::cout << "Received count :" << count << std::endl;
        ofs << "Received ImsUnsolServiceStatus: " << std::endl;

        for (size_t i = 0; i < count; i++) {
          std::cout << imsSrvStatusInfo[i]->callType << std::endl;
          std::cout << imsSrvStatusInfo[i]->accTechStatus.status << std::endl;
          std::cout << imsSrvStatusInfo[i]->rttMode << std::endl;
        }
        ofs.close();

        if (imsSrvStatusInfo == nullptr) {
          std::cout << "Payload is NULL." << std::endl;
          return;
        }

        PyObject* lInfo = translate(imsSrvStatusInfo, count);
        if (lInfo == nullptr) {
          std::cout << "lInfo is NULL" << std::endl;
          return;
        }

        PyObject* cbArgs = Py_BuildValue("(sN)", "RIL_IMS_UNSOL_SERVICE_STATUS_CHANGED", lInfo);
        if (cbArgs == nullptr) {
          return;
        }

        PyObject* result = PyObject_CallObject(callback, cbArgs);

        Py_XDECREF(cbArgs);
        Py_XDECREF(result);
        Py_XDECREF(callback);
      });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "Failed to register for IMS service status indication.");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "registerImsUnsolServiceStatusIndicationHandler request sent successfully"
              << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }

  return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------< register Ims Incoming SMS Indication Handler>------------------------------------*/
static PyObject* registerImsIncomingSmsHandler(PyObject* self, PyObject* args)
{
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

  PyObject* callback = nullptr;
  if (!PyArg_ParseTuple(args, "O", &callback)) {
    return NULL;
  }

  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
    return NULL;
  }

  Py_XINCREF(callback);

  Status s = rilSession.registerImsIncomingSmsHandler([callback](const RIL_IMS_IncomingSms& imsSms) {
    std::cout << std::endl << "== Incoming SMS ==" << std::endl;
    ofs << "Incoming SMS: " << std::endl;
    std::cout << std::endl << "format " << imsSms.format << std::endl;
    ofs << "format" << imsSms.format << std::endl;
    std::cout << std::endl << "verificationStatus " << imsSms.verificationStatus << std::endl;
    ofs << "verificationStatus " << imsSms.verificationStatus << std::endl;
    std::cout << std::hex;
    ofs << "hex: " << std::hex << std::endl;
    for (size_t i = 0; i < imsSms.pduLength; i++) {
      std::cout << static_cast<unsigned int>(imsSms.pdu[i]) << " ";
      ofs << "pdu: " << static_cast<unsigned int>(imsSms.pdu[i]) << std::endl;
    }
    std::cout << std::dec;
    ofs << "dec: " << std::dec << std::endl;
    std::cout << std::endl;
    ofs.close();

    PyObject* cbArgs =
        Py_BuildValue("(sy#)", "RIL_IMS_UNSOL_INCOMING_IMS_SMS", imsSms.pdu, imsSms.pduLength);
    if (cbArgs == nullptr) {
      return;
    }

    PyObject* result = PyObject_CallObject(callback, cbArgs);

    Py_XDECREF(cbArgs);
    Py_XDECREF(result);
    Py_XDECREF(callback);
  });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "registerImsIncomingSmsHandler");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "registerImsIncomingSmsHandler indication handler sent successfully" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }

  return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Ims Registration State Change Indication Handler>-----------------------*/
static PyObject* registerImsUnsolRegistrationStateIndicationHandler(PyObject* self, PyObject* args)
{
  ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
  PyObject* callback = nullptr;
  if (!PyArg_ParseTuple(args, "O", &callback)) {
    return NULL;
  }

  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
    return NULL;
  }

  Py_XINCREF(callback);

  Status s = rilSession.registerImsUnsolRegistrationStateIndicationHandler(
      [callback](const RIL_IMS_Registration& imsRegInfo) -> void {
        std::cout << "Received IMS registration state indication" << std::endl;
        ofs << "Received ImsUnsolRegistrationState: " << std::endl;

        std::cout << "IMS registration state " << imsRegInfo.state << std::endl;
        ofs << "IMS registration state " << imsRegInfo.state << std::endl;
        ofs.close();

        PyObject* cbArgs =
            Py_BuildValue("(si)", "RIL_IMS_UNSOL_REGISTRATION_STATE_CHANGED", imsRegInfo.state);
        if (cbArgs == nullptr) {
          return;
        }

        PyObject* result = PyObject_CallObject(callback, cbArgs);

        Py_XDECREF(cbArgs);
        Py_XDECREF(result);
        Py_XDECREF(callback);
      });

  if (s != Status::SUCCESS) {
    PyErr_SetString(RilApiError, "Failed to register for IMS registration state indication.");
    return NULL;
  } else {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "registerImsUnsolRegistrationStateIndicationHandler request sent successfully"
              << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
  }

  return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Voice Network State Indication Handler>------------------------------------*/
static PyObject* registerVoiceNetworkStateIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerVoiceNetworkStateIndicationHandler(
        [] ()-> void {
            std::cout<<"Received registerVoiceNetworkStateIndicationHandler: "<< std::endl;
            ofs << "Received registerVoiceNetworkStateIndicationHandler: "<< std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerVoiceNetworkStateIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerVoiceNetworkStateIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Ring Back Tone Indication Handler>------------------------------------*/
static PyObject* registerRingBackToneIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::cout << "Here" << std::endl;
    Status s = rilSession.registerRingBackToneIndicationHandler(
        [] (const int& tone)-> void {
            std::cout << "Received RingBackToneIndication." << std::endl;
            std::cout << "play tone: " << tone << std::endl;
            ofs << "Received RingBackToneIndication." << std::endl;
            ofs << "play tone: " << tone << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerRingBackToneIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerRingBackToneIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}


/* ---------------------<register Cell Info Indication Handler>------------------------------------*/
static PyObject* registerCellInfoIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    if (!PyCallable_Check(callback)){
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.registerCellInfoIndicationHandler(
        [callback] (const RIL_CellInfo_v12 cellInfoList[], size_t cellInfoListLen)->void {
        std::cout << "Received cell info indication" << std::endl;
        ofs << "Received cell info indication" << std::endl;

        if (cellInfoList == nullptr || cellInfoListLen == 0) {
          std::cout << "No cell info received." << std::endl;
          ofs << "No cell info received." << std::endl;
        } else {
          std::cout << cellInfoListLen << "elements in the cell info array." << std::endl;
          ofs << "elements in the cell info array." << std::endl;
        }

        PyObject* lCellInfoList = translate(cellInfoList, cellInfoListLen);
        if (lCellInfoList == nullptr) {
          return;
        }

        PyObject* cbArgs = Py_BuildValue("(sN)", "RIL_UNSOL_CELL_INFO_LIST", lCellInfoList);
        if (cbArgs == NULL) {
          return;
        }

        PyObject* result = PyObject_CallObject(callback, cbArgs);

        Py_XDECREF(cbArgs);
        Py_XDECREF(result);
        Py_XDECREF(callback);

        std::for_each(
            cellInfoList, cellInfoList + cellInfoListLen, [](const RIL_CellInfo_v12& cellInfo) {
              const char* mcc = nullptr;
              const char* mnc = nullptr;

              switch (cellInfo.cellInfoType) {
                case RIL_CELL_INFO_TYPE_GSM:
                  mcc = cellInfo.CellInfo.gsm.cellIdentityGsm.mcc;
                  mnc = cellInfo.CellInfo.gsm.cellIdentityGsm.mnc;
                  break;
                case RIL_CELL_INFO_TYPE_LTE:
                  mcc = cellInfo.CellInfo.lte.cellIdentityLte.mcc;
                  mnc = cellInfo.CellInfo.lte.cellIdentityLte.mnc;
                  break;
                case RIL_CELL_INFO_TYPE_WCDMA:
                  mcc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc;
                  mnc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc;
                  break;
                case RIL_CELL_INFO_TYPE_TD_SCDMA:
                  mcc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc;
                  mnc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc;
                  break;
                case RIL_CELL_INFO_TYPE_NR:
                  mcc = cellInfo.CellInfo.nr.cellIdentityNr.mcc;
                  mnc = cellInfo.CellInfo.nr.cellIdentityNr.mnc;
                  break;
                default:
                  break;
              }

              std::cout << "cellInfoType: " << cellInfo.cellInfoType
                        << " registered: " << cellInfo.registered << " mcc: " << (mcc ? mcc : "")
                        << " mnc: " << (mnc ? mnc : "") << std::endl;
              ofs << "cellInfoType: " << cellInfo.cellInfoType << std::endl
                        << " registered: " << cellInfo.registered << " mcc: " << (mcc ? mcc : "") << std::endl
                        << " mnc: " << (mnc ? mnc : "") << std::endl;
              ofs.close();
            });
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerCellInfoIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerCellInfoIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}


/* ---------------------< register Voice Radio Tech Indication Handler >------------------------------------*/
static PyObject* registerVoiceRadioTechIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerVoiceRadioTechIndicationHandler(
        [] (int rat) {
            std::cout << "Received VoiceRadioTechIndication, rat:" << rat << std::endl;
            ofs << "Received VoiceRadioTechIndication, rat:" << rat << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerVoiceRadioTechIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerVoiceRadioTechIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* ussdIndicationDataToDict(const char& mode, const std::string& message) {
    PyObject* dUssdData = PyDict_New();
    if (dUssdData == nullptr) {
        return nullptr;
    }

    PyDict_SetItemString(dUssdData, "mode", PyLong_FromLong(mode));
    PyDict_SetItemString(dUssdData, "message", PyUnicode_FromString(message.c_str()));

    return dUssdData;
}

/* ---------------------< register On Ussd Indication Handler >------------------------------------*/
static PyObject* registerOnUssdIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    PyObject* callback = nullptr;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.registerOnUssdIndicationHandler(
        [callback] (const char& mode, const std::string& message) {
            std::cout << "Received OnUssdIndication." << std::endl;
            std::cout << "mode: " << mode << std::endl;
            std::cout << "message: " << message << std::endl;
            ofs << "Received OnUssdIndication: " << std::endl;
            ofs << "mode: " << mode << std::endl;
            ofs << "message: " << message << std::endl;
            ofs.close();

            PyObject* dUssdData = ussdIndicationDataToDict(mode, message);
            if (dUssdData == nullptr) {
                return;
            }

            PyObject* cbArgs = Py_BuildValue("(sN)", "RIL_UNSOL_ON_USSD", dUssdData);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerOnUssdIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerOnUssdIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator <<(std::ostream& os, const RIL_SuppSvcNotification &arg)
{
  os << "RIL_SuppSvcNotification : " << std::endl;
  os << "notificationType: " << arg.notificationType << std::endl;
  os << "code: " << arg.code << std::endl;
  os << "index: " << arg.index << std::endl;
  os << "type: " << arg.type << std::endl;
  if (arg.number) {
    os << "number: " << arg.number << std::endl;
  }
  return os;
}

/* ---------------------< register Supp Svc Notification Indication Handler >------------------------------------*/
static PyObject* registerSuppSvcNotificationIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    std::cout << "Hi" << std::endl;
    Status s = rilSession.registerSuppSvcNotificationIndicationHandler(
        [] (const RIL_SuppSvcNotification& suppSvc) {
            std::cout << "hellp" << std::endl;
            std::cout << "Received SuppSvcIndication." << std::endl;
            std::cout <<"suppSvc:" << suppSvc << std::endl;
            ofs << "Received SuppSvcIndication." << std::endl;
            ofs <<"suppSvc:" << suppSvc << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerSuppSvcNotificationIndicationHandler failed.");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerSuppSvcNotificationIndicationHandler request sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------< register Incoming 3Gpp Sms Indication Handler>------------------------------------*/
static PyObject* registerIncoming3GppSmsIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);

    PyObject* callback = nullptr;
    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.registerIncoming3GppSmsIndicationHandler(
        [callback] (const uint8_t pdu[], size_t pduLength) {
            std::cout << std::endl << "== Incoming SMS ==" << std::endl;
            ofs << "Incoming SMS: " << std::endl;
            std::cout << std::hex;
            ofs << "hex: " << std::hex << std::endl;
            for (size_t i = 0; i < pduLength; i++) {
                std::cout << static_cast<unsigned int>(pdu[i]) << " ";
                ofs << "pdu: " << static_cast<unsigned int>(pdu[i]) << std::endl;
            }
            std::cout << std::dec;
            ofs << "dec: " << std::dec << std::endl;
            std::cout << std::endl;
            ofs.close();

            PyObject* cbArgs = Py_BuildValue("(sy#)", "RIL_UNSOL_RESPONSE_NEW_SMS", pdu, pduLength);
            if (cbArgs == nullptr) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
            Py_XDECREF(callback);
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerIncoming3GppSmsIndicationHandler");
        return NULL;
    } else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerIncoming3GppSmsIndicationHandler indication handler sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------< register Broadcast Sms Indication Handler>------------------------------------*/
static PyObject* registerBroadcastSmsIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerBroadcastSmsIndicationHandler(
        [] (const std::vector<uint8_t>& pdu) {
            std::cout << std::endl << "== Incoming Broadcast SMS ==" << std::endl;
            ofs << "Incoming Broadcast SMS: " << std::endl;
            std::cout << std::hex;
            ofs << "hex: " << std::hex<< std::endl;
            for (const uint8_t& i : pdu) {
                std::cout << i << " ";
            }

            std::cout << std::dec;
            ofs << "dec: " << std::dec << std::endl;
            std::cout << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerBroadcastSmsIndicationHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerBroadcastSmsIndicationHandler indication handler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/* ---------------------< register Unthrottle Apn Indication Handler>------------------------------------*/
static PyObject* registerUnthrottleApnIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerUnthrottleApnIndicationHandler(
        [] (const std::string& apn) {
            std::cout << "Received UnthrottleApn :" << apn << std::endl;
            ofs << "Received UnthrottleApn :" << apn << std::endl;
            ofs.close();
        }
    );
    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "UnthrottleApnIndication Handler Registration failed");
        return NULL;
    } else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "UnthrottleApn indication handler sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/* ---------------------< register Slicing Config Changed Indication Handler>------------------------------------*/
static PyObject* registerSlicingConfigChangedIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerSlicingConfigChangedIndicationHandler(
        [] (const rilSlicingConfig & config)  {
            std::cout << "Received SlicingConfigChanged :"  << std::endl;
            ofs << "Received SlicingConfigChanged :" << std::endl;
            std::cout << "SlicingConfigChanged UrspRuleSize: " << config.UsrpRuleSize << std::endl;
            ofs << "SlicingConfigChanged UrspRuleSize: " << config.UsrpRuleSize << std::endl;
            for (int i = 0; i < config.UsrpRuleSize; i++) {
                std::cout << "SlicingConfigChanged UrspRuleList: " << i << std::endl;
                ofs << "SlicingConfigChanged UrspRuleList: " << i << std::endl;
                std::cout << "SlicingConfigChanged tdSize: " << config.ursprules[i].tdSize << std::endl;
                ofs << "SlicingConfigChanged tdSize: " << config.ursprules[i].tdSize << std::endl;
                for (int j = 0; j < config.ursprules[i].tdSize; j++) {
                    std::cout << "SlicingConfigChanged TdList: " << j << std::endl;
                    ofs << "SlicingConfigChanged TdList: " << j << std::endl;
                    RIL_TrafficDescriptor trafficDescriptor = config.ursprules[i].tdList[j];
                    std::cout << "{";
                    ofs << "{";
                    if (trafficDescriptor.dnnValid) {
                        std::string str(trafficDescriptor.dnn);
                        std::cout << str;
                        ofs << str;
                    }
                    std::cout << ",";
                    ofs << ",";
                    if (trafficDescriptor.osAppIdValid) {
                        if (trafficDescriptor.osAppIdLength > 17) {
                            for (size_t k = 17; k < trafficDescriptor.osAppIdLength; k++) {
                                std::cout << (char)trafficDescriptor.osAppId[k];
                                ofs << (char)trafficDescriptor.osAppId[k];
                            }
                        }
                    }
                    std::cout << "},";
                    ofs << "},";
                }
            }
            ofs.close();
        }
    );
    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "Slicing Config Changed Indication Handler Registration failed");
        return NULL;
    } else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Slicing Config Changed Indication handler sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/* ---------------------< register Unthrottle Apn TD Indication Handler>------------------------------------*/
static PyObject* registerUnthrottleApnTdIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerUnthrottleApnTdIndicationHandler(
        [] (const RIL_DataProfileInfo& profile) {
            std::cout << "Received UnthrottleApn Td:" << std::endl;
            ofs << "Received UnthrottleApn Td:" << std::endl;
            std::cout << "Apn:" <<  profile.apn << std::endl;
            ofs << "Apn:" <<  profile.apn << std::endl;
            std::cout << "profileId:" <<  profile.profileId << std::endl;
            ofs << "profileId:" <<  profile.profileId << std::endl;
            std::cout << "protocol:" <<  profile.protocol << std::endl;
            ofs << "protocol:" <<  profile.protocol << std::endl;
            std::cout << "roamingprotocol:" <<  profile.roamingProtocol << std::endl;
            ofs << "roamingprotocol:" <<  profile.roamingProtocol << std::endl;
            std::cout << "authType:" <<  profile.authType << std::endl;
            ofs << "authType:" <<  profile.authType << std::endl;
            std::cout << "user:" <<  profile.user << std::endl;
            ofs << "user:" <<  profile.user << std::endl;
            std::cout << "password:" <<  profile.password << std::endl;
            ofs << "password:" <<  profile.password << std::endl;
            std::cout << "type:" <<  profile.type << std::endl;
            ofs << "type:" <<  profile.type << std::endl;
            std::cout << "maxConnsTime:" <<  profile.maxConnsTime << std::endl;
            ofs << "maxConnsTime:" <<  profile.maxConnsTime << std::endl;
            std::cout << "maxConns:" <<  profile.maxConns << std::endl;
            ofs << "maxConns:" <<  profile.maxConns << std::endl;
            std::cout << "waitTime:" <<  profile.waitTime << std::endl;
            ofs << "waitTime:" <<  profile.waitTime << std::endl;
            std::cout << "enabled:" <<  profile.enabled << std::endl;
            ofs << "enabled:" <<  profile.enabled << std::endl;
            std::cout << "supportedTypesBitmask:" <<  profile.supportedTypesBitmask << std::endl;
            ofs << "supportedTypesBitmask:" <<  profile.supportedTypesBitmask << std::endl;
            std::cout << "bearerBitmask:" <<  profile.bearerBitmask << std::endl;
            ofs << "bearerBitmask:" <<  profile.bearerBitmask << std::endl;
            std::cout << "mtu:" <<  profile.mtu << std::endl;
            ofs << "mtu:" <<  profile.mtu << std::endl;
            std::cout << "mtuV4:" <<  profile.mtuV4 << std::endl;
            ofs << "mtuV4:" <<  profile.mtuV4 << std::endl;
            std::cout << "mtuV6:" <<  profile.mtuV6 << std::endl;
            ofs << "mtuV6:" <<  profile.mtuV6 << std::endl;
            std::cout << "preferred:" <<  profile.preferred << std::endl;
            ofs << "preferred:" <<  profile.preferred << std::endl;
            if (profile.trafficDescriptorValid) {
                std::cout << "trafficDescriptor: " << std::endl;
                ofs << "trafficDescriptor: " << std::endl;
                std::cout << "{";
                if (profile.trafficDescriptor.dnnValid) {
                    std::string str(profile.trafficDescriptor.dnn);
                    std::cout << str;
                    ofs << str;
                }
                std::cout << ",";
                ofs << ",";
                if (profile.trafficDescriptor.osAppIdValid) {
                    if (profile.trafficDescriptor.osAppIdLength > 17) {
                        for (size_t j = 17; j < profile.trafficDescriptor.osAppIdLength; j++) {
                            std::cout << (char)profile.trafficDescriptor.osAppId[j];
                            ofs << (char)profile.trafficDescriptor.osAppId[j];
                        }
                    }
                }
                std::cout << "},";
                ofs << "},";
            }
            ofs.close();
        }
    );
    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "UnthrottleApn Td Indication Handler Registration failed");
        return NULL;
    } else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "UnthrottleApn TD indication handler sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}
/* ---------------------< register Data Call List Changed Indication Handler>------------------------------------*/
static PyObject* registerDataCallListChangedIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerDataCallListChangedIndicationHandler(
        [] (const RIL_Data_Call_Response_v11 response[], size_t sz) {
            std::cout<<"RadioDataCallListChangeInd size: "+std::to_string(sz)<<std::endl;
            ofs <<"RadioDataCallListChangeInd size: "+std::to_string(sz)<<std::endl;
            for (int i = 0; (i < sz); i++) {
                std::cout<<"RadioDataCallListChangeInd reading entry "<<std::endl;
                std::cout<<"status: "+std::to_string(response[i].status)<<std::endl;
                std::cout<<"suggestedRetryTime: "+std::to_string(response[i].suggestedRetryTime)<<std::endl;
                std::cout<<"cid: "+std::to_string(response[i].cid)<<std::endl;
                std::cout<<"active: "+std::to_string(response[i].active)<<std::endl;

                ofs << "RadioDataCallListChangeInd reading entry: "<<std::endl;
                ofs<<"status: "+std::to_string(response[i].status)<<std::endl;
                ofs<<"suggestedRetryTime: "+std::to_string(response[i].suggestedRetryTime)<<std::endl;
                ofs<<"cid: "+std::to_string(response[i].cid)<<std::endl;
                ofs<<"active: "+std::to_string(response[i].active)<<std::endl;
                if(response[i].type) {
                    std::string str(response[i].type);
                    std::cout<<"type: "+str<<std::endl;
                    ofs << "type: " << str << std::endl;
                }
                if(response[i].ifname) {
                    std::string str(response[i].ifname);
                    std::cout<<"ifname: "+str<<std::endl;
                    ofs << "ifname: " << str << std::endl;
                }
                if (response[i].linkAddressesLength > 0) {
                    for (int j = 0; j < std::min(static_cast<size_t>(MAX_LINK_ADDRESSES), response[i].linkAddressesLength); j++) {
                        if (response[i].linkAddresses[j].address) {
                            std::stringstream ss;
                            ss  <<std::string(response[i].linkAddresses[j].address)
                                <<" properties="<<std::to_string(response[i].linkAddresses[j].properties)
                                <<" deprecation="<<std::to_string(response[i].linkAddresses[j].deprecationTime)
                                <<" expiration="<<std::to_string(response[i].linkAddresses[j].expirationTime)<<std::endl;
                            std::cout << ss.str();
                            ofs << ss.str();
                        }
                    }
                }
                else if(response[i].addresses) {
                    std::string str(response[i].addresses);
                    std::cout<<"addresses: "+str<<std::endl;
                    ofs << "addresses: " << str << std::endl;
                }
                if(response[i].dnses) {
                    std::string str(response[i].dnses);
                    std::cout<<"dnses: "+str<<std::endl;
                     ofs << "dnses: " << str << std::endl;
                }
                if(response[i].gateways) {
                    std::string str(response[i].gateways);
                    std::cout<<"gateways: "+str<<std::endl;
                     ofs << "gateways: " << str << std::endl;
                }
                if(response[i].pcscf) {
                    std::string str(response[i].pcscf);
                    std::cout<<"pcscf: "+str<<std::endl;
                     ofs << "pcscf: " << str << std::endl;
                }
                std::cout<<"mtu: "+std::to_string(response[i].mtu)<<std::endl;
                ofs << "mtu: "+std::to_string(response[i].mtu)<<std::endl;
                std::cout<<" mtuV4 :"+std::to_string(response[i].mtuV4)<<std::endl;
                std::cout<<" mtuV6 :"+std::to_string(response[i].mtuV6)<<std::endl;

                ofs<<"mtuV4 :"+std::to_string(response[i].mtuV4)<<std::endl;
                ofs<<"mtuV6 :"+std::to_string(response[i].mtuV6)<<std::endl;
                std::cout<<" defaultQos :"<<std::endl;
                ofs<<"defaultQos :"<<std::endl;
                std::cout << dump(response[i].defaultQos) << std:: endl;
                ofs << dump(response[i].defaultQos) << std::endl;

                std::cout<<" qosSessions :"<<std::endl;
                ofs<<"qosSessions :"<<std::endl;
                for (int j = 0; j < response[i].qosSessionsLength; j++) {
                    std::cout << dump(response[i].qosSessions[j], "  ") << std::endl;
                    ofs<< dump(response[i].qosSessions[j], "  ") << std::endl;
                }

                std::cout<<" trafficDescriptors :";
                for (int k = 0; k < response[i].trafficDescriptorsLength; k++) {
                    std::cout << "{";
                    ofs << "{";
                    if (response[i].trafficDescriptors[k].dnnValid) {
                        std::string str(response[i].trafficDescriptors[k].dnn);
                        std::cout << str;
                        ofs << str;
                    }
                    std::cout << ",";
                    ofs << ",";
                    if (response[i].trafficDescriptors[k].osAppIdValid) {
                        if (response[i].trafficDescriptors[k].osAppIdLength > 17) {
                            for (size_t j = 17; j < response[i].trafficDescriptors[k].osAppIdLength; j++) {
                                std::cout << (char)response[i].trafficDescriptors[k].osAppId[j];
                                ofs << (char)response[i].trafficDescriptors[k].osAppId[j];
                            }
                        }
                    }
                    std::cout << "},";
                    ofs << "},";
                }
                std::cout<< std::endl;
                ofs << std::endl;
                ofs.close();
            }
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerDataCallListChangedIndicationHandler");
        return NULL;
    }
    else{
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerDataCallListChangedIndicationHandler indication handler sent successfully" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}


/* ---------------------<register Nitz Time Indication Handler >------------------------------------*/
static PyObject* registerNitzTimeIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerNitzTimeIndicationHandler(
        [] (const char *nitzTime) {
            std::cout<<"[PhoneMenu]: Received Nitz Time";
            std::cout<<nitzTime<<std::endl;
            ofs << "Received Nitz Time: " << std::endl;
            ofs << "nitzTime: " << nitzTime << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerNitzTimeIndicationHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerNitzTimeIndicationHandler indication handler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Radio State Indication Handler>------------------------------------*/
static PyObject* registerRadioStateIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerRadioStateIndicationHandler(
        [] (RIL_RadioState state) {
            std::cout<<"[PhoneMenu]: Received Radio State Change: ";
            std::cout<<state<<std::endl;
            ofs <<"Received Radio State Change: " << std::endl;
            ofs << "State: " << state << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerRadioStateIndicationHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerRadioStateIndicationHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Physical Config Indication Handler>------------------------------------*/
static PyObject* registerPhysicalConfigStructHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerPhysicalConfigStructHandler(
        [] (const Status status, const  std::vector<RIL_PhysicalChannelConfig> arg) {
            if (status == Status::FAILURE) {
                std::cout << "PhysicalConfigStructUpdate : Failure in socket data read. Exiting testcase" << std::endl;
                ofs << "PhysicalConfigStructUpdate : Failure in socket data read. Exiting testcase" << std::endl;
                ofs.close();
                return;
            }

            for (int j = 0; j < arg.size(); j++) {
                std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: List content = "
                    << std::to_string(j) << std::endl;
                ofs <<  "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: List content = "
                    << std::to_string(j) << std::endl;

                std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: status = "
                    << std::to_string(static_cast<int>(arg[j].status)) << std::endl;
                ofs <<  "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: status = "
                    << std::to_string(static_cast<int>(arg[j].status)) << std::endl;
                std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: BandwidthDownlink = "
                    << std::to_string(arg[j].cellBandwidthDownlink) << std::endl;
                ofs << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: BandwidthDownlink = "
                    << std::to_string(arg[j].cellBandwidthDownlink) << std::endl;
                std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: Network RAT "
                    << std::to_string(static_cast<int>(arg[j].rat)) << std::endl;
                ofs <<  "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: Network RAT "
                    << std::to_string(static_cast<int>(arg[j].rat)) << std::endl;
                std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: Range = "
                    << std::to_string(static_cast<int>(arg[j].rfInfo.range)) << std::endl;
                ofs << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: Range = "
                    << std::to_string(static_cast<int>(arg[j].rfInfo.range)) << std::endl;
                std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: ChannelNumber = "
                    << std::to_string(arg[j].rfInfo.channelNumber) << std::endl;
                ofs << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: ChannelNumber = "
                    << std::to_string(arg[j].rfInfo.channelNumber) << std::endl;
                std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: Number of Context Ids = "
                    << std::to_string(arg[j].num_context_ids) << std::endl;
                ofs << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: Number of Context Ids = "
                    << std::to_string(arg[j].num_context_ids) << std::endl;
                for (int i = 0; i < arg[j].num_context_ids; i++) {
                    std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: Cid "
                        << std::to_string(i) << " = " << std::to_string(arg[j].contextIds[i]) << std::endl;
                    ofs << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: Cid "
                        << std::to_string(i) << " = " << std::to_string(arg[j].contextIds[i]) << std::endl;
                }

                std::cout << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: physicalCellId = "
                    << std::to_string(arg[j].physicalCellId) << std::endl;
                ofs << "[registerPhysicalConfigStructHandler]: PhysicalConfigStructUpdate: physicalCellId = "
                    << std::to_string(arg[j].physicalCellId) << std::endl;
            }
            std::cout << "Received PhysicalConfigStructUpdate" << std::endl;
            ofs << "Received PhysicalConfigStructUpdate" << std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerPhysicalConfigStructHandler Failed");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerPhysicalConfigStructHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Data Nr Icon Changed Indication Handler>------------------------------------*/
static PyObject* registerDataNrIconChangeHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerDataNrIconChangeHandler(
        [] (const Status status, const  five_g_icon_type icon) {
            if (status == Status::FAILURE) {
                std::cout<<"UnsolDataNrIconChange : Failure in socket data read"<<std::endl;
                ofs.close();
                return;
            }
            std::cout<<"[registerDataNrIconChangeHandler]: UnsolDataNrIconChange: NR Icon Type = "
            + std::to_string(static_cast<int>(icon))<<std::endl;
            ofs<<"[registerDataNrIconChangeHandler]: UnsolDataNrIconChange: NR Icon Type = "
            + std::to_string(static_cast<int>(icon))<<std::endl;
            std::cout<<"Received UnsolDataNrIconChange"<<std::endl;
            ofs<<"Received UnsolDataNrIconChange"<<std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerDataNrIconChangeHandler Failed");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerDataNrIconChangeHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Radio Keep Alive Indication Handler>------------------------------------*/
static PyObject* registerRadioKeepAliveHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerRadioKeepAliveHandler(
        [] (const Status status, const uint32_t handle, const RIL_KeepaliveStatusCode statuscode) {
            if (status == Status::FAILURE) {
                std::cout<<"UnsolRadioKeepAlive : Failure in socket data read"<<std::endl;
                ofs.close();
                return;
            }
            std::cout<<"[registerRadioKeepAliveHandler]: UnsolRadioKeepAlive:Handle ="
            + std::to_string(handle)<<std::endl;
            ofs<<"[registerRadioKeepAliveHandler]: UnsolRadioKeepAlive:Handle ="
            + std::to_string(handle)<<std::endl;
            std::cout<<"[registerRadioKeepAliveHandler]: UnsolRadioKeepAlive:Status = "
            + std::to_string(statuscode)<<std::endl;
            ofs<<"[registerRadioKeepAliveHandler]: UnsolRadioKeepAlive:Status = "
            + std::to_string(statuscode)<<std::endl;
            std::cout<<"Received RadioKeepAliveIndication"<<std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerRadioKeepAliveHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerRadioKeepAliveHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Restricted State Changed Indication Handler>------------------------------------*/
static PyObject* registerRestrictedStateChangedIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerRestrictedStateChangedIndicationHandler(
        [] (const int state) {
           std::cout << "Received RestrictedStateChangedIndication state: " << state << std::endl;
           ofs << "Received RestrictedStateChangedIndication state: " << state << std::endl;
           ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerRestrictedStateChangedIndicationHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerRestrictedStateChangedIndicationHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator << (std::ostream &os, const RIL_CDMA_SMS_Subaddress &addr) {
  os << "Sub Address Type: " << addr.subaddressType << std::endl;
  os << "odd: " << addr.odd << std::endl;
  os << "Number of Digits: " << addr.number_of_digits;
  if (addr.number_of_digits > 0) {
    os << std::endl << "Digits: ";
    uint32_t size = (addr.number_of_digits > RIL_CDMA_SMS_SUBADDRESS_MAX) ?
            RIL_CDMA_SMS_SUBADDRESS_MAX : addr.number_of_digits;
    for (uint32_t i = 0; i < size; i++) {
        os << addr.digits[i] << " ";
    }
  }
  return os;
}

std::ostream& operator << (std::ostream &os, const RIL_CDMA_SMS_Message &sms) {
  os << "Teleservice ID: " << sms.uTeleserviceID << std::endl;
  os << "Is Service Present: " << sms.bIsServicePresent << std::endl;
  os << "Service Category: " << sms.uServicecategory << std::endl;
  os << "Bear Data Length: " << sms.uBearerDataLen << std::endl;
  os << "Address Info: " << std::endl << sms.sAddress << std::endl;
  os << "Subaddress Info: " << std::endl << sms.sSubAddress << std::endl;
  if (sms.uBearerDataLen > 0) {
    os << "Bear Data: ";
    uint32_t size = (sms.uBearerDataLen > RIL_CDMA_SMS_BEARER_DATA_MAX) ?
            RIL_CDMA_SMS_BEARER_DATA_MAX : sms.uBearerDataLen;
    for (uint32_t i = 0; i < size; i++) {
        os << sms.aBearerData[i] << " ";
    }
    os << std::endl;
  }
  os << "Expect More: " << sms.expectMore;
  return os;
}

/* ---------------------<register Cdma New Sms Indication Handler>------------------------------------*/
static PyObject* registerCdmaNewSmsIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerCdmaNewSmsIndicationHandler(
        [] (const RIL_CDMA_SMS_Message& sms) {
           std::cout << "Received CDMA MT SM: " << std::endl;
           std::cout << sms << std::endl;
           ofs << "Received CDMA MT SM: " << std::endl;
           ofs <<"SMS: " << sms << std::endl;
           ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerCdmaNewSmsIndicationHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerCdmaNewSmsIndicationHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_CallWaiting_v6 &arg)
{
  os << "RIL_CDMA_CallWaiting_v6 : " << std::endl;
  if (arg.number) {
    os << "number: " << arg.number << std::endl;
  }
  os << "numberPresentation: " << arg.numberPresentation << std::endl;
  if (arg.name) {
    os << "name: " << arg.name << std::endl;
  }
  os << arg.signalInfoRecord;
  os << "number_type: " << arg.number_type << std::endl;
  os << "number_plan: " << arg.number_plan << std::endl;
  return os;
}

/* ---------------------<register Cdma Call Waiting Indication Handler>------------------------------------*/
static PyObject* registerCdmaCallWaitingIndicationHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerCdmaCallWaitingIndicationHandler(
        [] (const RIL_CDMA_CallWaiting_v6& callWaitingRecord) {
           std::cout << "Received CdmaCallWaitingIndication: " << std::endl;
           std::cout << callWaitingRecord;
           ofs <<  "Received CdmaCallWaitingIndication: " << std::endl;
           ofs << "Call Waiting Record" << callWaitingRecord << std::endl;
           ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerCdmaCallWaitingIndicationHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerCdmaCallWaitingIndicationHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register LCE Data Change Handler>------------------------------------*/
static PyObject* registerLCEDataChangeHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerLCEDataChangeHandler(
        [] (const  RIL_LinkCapacityEstimate LCEData) {
           std::cout<<"UnsolLceDataChanged -> Downlink capacity in kbps: "
            + std::to_string(LCEData.downlinkCapacityKbps)<<std::endl;
            std::cout<<" UnsolLceDataChanged -> Uplink capacity in kbps: "
            + std::to_string(LCEData.uplinkCapacityKbps)<<std::endl;
            std::cout<<"Received LCEDataChangeIndicationq:"<<std::endl;

            ofs << "Received LCEDataChangeIndicationq: "<<std::endl;
            ofs << "UnsolLceDataChanged -> Downlink capacity in kbps: "<< std::to_string(LCEData.downlinkCapacityKbps)<<std::endl;
            ofs << " UnsolLceDataChanged -> Uplink capacity in kbps: " << std::to_string(LCEData.uplinkCapacityKbps)<<std::endl;
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerLCEDataChangeHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerLCEDataChangeHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

/* ---------------------<register Emergency Cb Mode IndHandler>------------------------------------*/
static PyObject* registerEmergencyCbModeIndHandler(PyObject* self, PyObject* args) {
    ofs.open(saveValuesToTempLocation(), std::ofstream::out | std::ofstream::trunc);
    Status s = rilSession.registerEmergencyCbModeIndHandler(
        [] (RIL_EmergencyCbMode mode) {
           std::cout << "Emergency Callback Mode: " << std::endl;
           ofs << "Emergency Callback Mode: " << std::endl;
            if(mode == RIL_EMERGENCY_CALLBACK_MODE_EXIT) {
                std::cout << "EXIT" << std::endl;
                ofs << "EXIT: "<< std::endl;
            } else if(mode == RIL_EMERGENCY_CALLBACK_MODE_ENTER) {
                std::cout << "ENTER" << std::endl;
                ofs << "ENTER: "<< std::endl;
            }
            ofs.close();
        }
    );

    if (s != Status::SUCCESS) {
        PyErr_SetString(RilApiError, "registerEmergencyCbModeIndHandler");
        return NULL;
    }
    else{
         std::cout << "------------------------------------------------" << std::endl;
        std::cout << "registerEmergencyCbModeIndHandler sent successfully" << std::endl;
         std::cout << "------------------------------------------------" << std::endl;
    }
    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* initialize(PyObject* self, PyObject* args) {
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.initialize(
        [callback] (Status s) {
            std::cout << "Error in RIL API session: " << static_cast<unsigned int>(s) << std::endl;

            PyObject* cbArgs = Py_BuildValue("(i)", s);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << "Failed to initialize RIL API Session" << std::endl;
        Py_XDECREF(callback);
        PyErr_SetString(RilApiError, "Failed to initialize RIL API session.");
        return NULL;
    } else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Successfully initialized RIL API Session" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

static PyObject* reinitialize(PyObject* self, PyObject* args) {
    PyObject* callback;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, " Parameter must be callable.");
        return NULL;
    }

    Py_XINCREF(callback);

    Status s = rilSession.reinitialize(
        [callback] (Status s) {
            std::cout << "Error in RIL API session: " << static_cast<unsigned int>(s) << std::endl;

            PyObject* cbArgs = Py_BuildValue("(i)", s);
            if (cbArgs == NULL) {
                return;
            }

            PyObject* result = PyObject_CallObject(callback, cbArgs);

            Py_XDECREF(cbArgs);
            Py_XDECREF(result);
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << "Failed to re-initialize RIL API Session" << std::endl;
        Py_XDECREF(callback);
        PyErr_SetString(RilApiError, "Failed to re-initialize RIL API session.");
        return NULL;
    } else {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Successfully re-initialized RIL API Session" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    return PyLong_FromLong(static_cast<long>(s));
}

static PyMethodDef RilApiMethods[] = {
  { "initialize", initialize, METH_VARARGS, "Initialize RIL API session" },
  { "reinitialize", reinitialize, METH_VARARGS, "Re-initialize RIL API session" },
#ifdef QCRIL_BUILD_AFL_TESTCASE_GEN
  { "setTestcaseOutputDir",
    setTestcaseOutputDir,
    METH_VARARGS,
    "Set the output directory for AFL testcase generation." },
  { "getTestcaseOutputDir",
    getTestcaseOutputDir,
    METH_VARARGS,
    "Get current output directory for AFL testcase generation." },
  { "setSaveTestcases",
    setSaveTestcases,
    METH_VARARGS,
    "Enable/Disable testcase generation for RIL API Session." },
  { "getSaveTestcases",
    getSaveTestcases,
    METH_VARARGS,
    "Get current value of saveTestcases for RIL API Session." },
#endif  // QCRIL_BUILD_AFL_TESTCASE_GEN
  { "registerSuppSvcNotificationIndicationHandler",
    registerSuppSvcNotificationIndicationHandler,
    METH_VARARGS,
    "Register supp svc notification indication handler" },
  { "registerOnUssdIndicationHandler",
    registerOnUssdIndicationHandler,
    METH_VARARGS,
    "Register on ussd indication handler" },
  { "registerEmergencyCbModeIndHandler",
    registerEmergencyCbModeIndHandler,
    METH_VARARGS,
    "Register Emergency Cb mode Ind handler" },
  { "registerLCEDataChangeHandler",
    registerLCEDataChangeHandler,
    METH_VARARGS,
    "Register LCEData Change Handler" },
  { "registerCdmaCallWaitingIndicationHandler",
    registerCdmaCallWaitingIndicationHandler,
    METH_VARARGS,
    "Register CDMA Call Waiting Indication handler" },
  { "registerCdmaNewSmsIndicationHandler",
    registerCdmaNewSmsIndicationHandler,
    METH_VARARGS,
    "Register CDMA New SMS Indication handler" },
  { "registerRestrictedStateChangedIndicationHandler",
    registerRestrictedStateChangedIndicationHandler,
    METH_VARARGS,
    "Register Restricted State Changed Indication handler" },
  { "registerRadioStateIndicationHandler",
    registerRadioStateIndicationHandler,
    METH_VARARGS,
    "Register Radio State Indication handler" },
  { "registerNitzTimeIndicationHandler",
    registerNitzTimeIndicationHandler,
    METH_VARARGS,
    "Register Nitz Time Indication Handler" },
  { "registerVoiceNetworkStateIndicationHandler",
    registerVoiceNetworkStateIndicationHandler,
    METH_VARARGS,
    "Register for voice network state indication handler" },
  { "registerImsNetworkStateChangeIndicationHandler",
    registerImsNetworkStateChangeIndicationHandler,
    METH_VARARGS,
    "Register IMS Network Change indication handler" },
  { "registerImsCallStateChangedIndicationHandler",
    registerImsCallStateChangedIndicationHandler,
    METH_VARARGS,
    "Register IMS call state change indication handler" },
  { "registerImsUnsolCallRingIndicationHandler",
    registerImsUnsolCallRingIndicationHandler,
    METH_VARARGS,
    "Register IMS call ring indication handler" },
  { "registerImsUnsolServiceStatusIndicationHandler",
    registerImsUnsolServiceStatusIndicationHandler,
    METH_VARARGS,
    "Register IMS service status indication handler" },
  { "registerImsIncomingSmsHandler",
    registerImsIncomingSmsHandler,
    METH_VARARGS,
    "Register IMS incoming sms indication handler" },
  { "registerImsUnsolRegistrationStateIndicationHandler",
    registerImsUnsolRegistrationStateIndicationHandler,
    METH_VARARGS,
    "Register IMS regi state indication handler" },
  { "registerRadioCapabilityChangedIndHandler",
    registerRadioCapabilityChangedIndHandler,
    METH_VARARGS,
    "Register for radio Capability Changed Ind Handler" },
  { "setRadioCapability", setRadioCapability, METH_VARARGS, "Request for set radio capability" },
  { "getRadioCapability", getRadioCapability, METH_VARARGS, "Request for get radio capability" },
  { "registerUiccSubStatusIndHandler",
    registerUiccSubStatusIndHandler,
    METH_VARARGS,
    "Register for UICC Sub status ind handler" },
  { "registerSignalStrengthIndicationHandler",
    registerSignalStrengthIndicationHandler,
    METH_VARARGS,
    "Register for signal strength indication handler" },
  { "registerCellInfoIndicationHandler",
    registerCellInfoIndicationHandler,
    METH_VARARGS,
    "Register for Cell info indication handler" },
  { "registerRingBackToneIndicationHandler",
    registerRingBackToneIndicationHandler,
    METH_VARARGS,
    "Register for ring back tone indication handler" },
  { "registerUnsolCallStateChangeIndicationHandler",
    registerUnsolCallStateChangeIndicationHandler,
    METH_VARARGS,
    "Register for unsol call state change handler" },
  { "convergedRegisterUnsolCallStateChangedIndicationHandler",
    convergedRegisterUnsolCallStateChangedIndicationHandler,
    METH_VARARGS,
    "Register for converged unsol call state change handler" },
  { "registerCallRingIndicationHandler",
    registerCallRingIndicationHandler,
    METH_VARARGS,
    "Request for register call ring indication handler" },
  { "convergedRegisterCallRingIndicationHdlr",
    convergedRegisterCallRingIndicationHdlr,
    METH_VARARGS,
    "Request for register converged call ring indication handler" },
  { "registerSimStatusChanged",
    registerSimStatusChanged,
    METH_VARARGS,
    "Register for SIM status changed" },
  { "registerDataCallListChangedIndicationHandler",
    registerDataCallListChangedIndicationHandler,
    METH_VARARGS,
    "Register for Data Call List Changed Indication handler" },
  { "registerUnthrottleApnIndicationHandler",
    registerUnthrottleApnIndicationHandler,
    METH_VARARGS,
    "Register for Untrottle Apn Indication Handler" },
  { "registerBroadcastSmsIndicationHandler",
    registerBroadcastSmsIndicationHandler,
    METH_VARARGS,
    "Request for register broadcast SMS indication handler" },
  { "registerVoiceRadioTechIndicationHandler",
    registerVoiceRadioTechIndicationHandler,
    METH_VARARGS,
    "Request for register Voice Radio tech" },
  { "registerNewSmsStatusReportIndicationHandler",
    registerNewSmsStatusReportIndicationHandler,
    METH_VARARGS,
    "Request fot register new SMS status" },
  { "registerIncoming3GppSmsIndicationHandler",
    registerIncoming3GppSmsIndicationHandler,
    METH_VARARGS,
    "Request for register incoming 3GPP SMS indication" },
  { "registerPhysicalConfigStructHandler",
    registerPhysicalConfigStructHandler,
    METH_VARARGS,
    "Request for register Physical Config Indication" },
  { "registerDataNrIconChangeHandler",
    registerDataNrIconChangeHandler,
    METH_VARARGS,
    "Request for register Nr Icon Change Indication" },
  { "registerRadioKeepAliveHandler",
    registerRadioKeepAliveHandler,
    METH_VARARGS,
    "Request for register Radio Keep Alive Indication" },
  { "registerSlicingConfigChangedIndicationHandler",
    registerSlicingConfigChangedIndicationHandler,
    METH_VARARGS,
    "Request for register Slicing Config Changed" },
  { "registerUnthrottleApnTdIndicationHandler",
    registerUnthrottleApnTdIndicationHandler,
    METH_VARARGS,
    "Request for register UnthrottleApn Indication with Profile Info" },
  { "ackSms", ackSms, METH_VARARGS, "Request for acknowlwdge SMS" },
  { "sendSms", sendSms, METH_VARARGS, "Request to send SMS" },
  { "imsQueryVirtualLineInfo",
    imsQueryVirtualLineInfo,
    METH_VARARGS,
    "Request for ims Query Virtual Line info" },
  { "imsSendRttMessage", imsSendRttMessage, METH_VARARGS, "Request for ims Send Rtt Message" },
  { "imsSendGeolocationInfo",
    imsSendGeolocationInfo,
    METH_VARARGS,
    "Request for ims Send Geo Location Info" },
  { "imsSendUiTtyMode", imsSendUiTtyMode, METH_VARARGS, "Request ims Send UiTy mode" },
  { "imsSetColr", imsSetColr, METH_VARARGS, "Request for ims Set Colr" },
  { "imsGetColr", imsGetColr, METH_VARARGS, "Request for ims get colr" },
  { "imsDeflectCall", imsDeflectCall, METH_VARARGS, "Request for ims Deflect call" },
  { "imsSetSuppSvcNotification",
    imsSetSuppSvcNotification,
    METH_VARARGS,
    "Request for ims SuppSvc status" },
  { "imsSetCallWaiting", imsSetCallWaiting, METH_VARARGS, "Request for ims Set Call waiting" },
  { "imsQueryCallWaiting", imsQueryCallWaiting, METH_VARARGS, "Request for ims Query Call waiting" },
  { "imsSetClir", imsSetClir, METH_VARARGS, "Request for ims Set Clir" },
  { "imsGetClir", imsGetClir, METH_VARARGS, "Request ims Get Clir" },
  { "imsQueryClip", imsQueryClip, METH_VARARGS, "Request for ims Query Clip" },
  { "imsGetSmsFormat", imsGetSmsFormat, METH_VARARGS, "Request for ims get sms format" },
  { "imsAckSms", imsAckSms, METH_VARARGS, "Request for ims Ack Sms" },
  { "imsSendSms", imsSendSms, METH_VARARGS, "Request for ims send SMS" },
  { "imsCancelModifyCall", imsCancelModifyCall, METH_VARARGS, "Request for ims Cancel Modify call" },
  { "imsRequestResumeCall",
    imsRequestResumeCall,
    METH_VARARGS,
    "Request for ims Requesr resume call" },
  { "imsRequestHoldCall", imsRequestHoldCall, METH_VARARGS, "Request for ims hold call" },
  { "imsAddParticipant", imsAddParticipant, METH_VARARGS, "Request for ims add participant" },
  { "imsModifyCallConfirm",
    imsModifyCallConfirm,
    METH_VARARGS,
    "Request for ims Modify Call Confirm" },
  { "imsModifyCallInitiate",
    imsModifyCallInitiate,
    METH_VARARGS,
    "Request ims Modify Call Initiate" },
  { "imsDtmfStop", imsDtmfStop, METH_VARARGS, "Request for ims Dtmf stop" },
  { "imsDtmfStart", imsDtmfStart, METH_VARARGS, "Request for ims Dtmf start" },
  { "imsDtmf", imsDtmf, METH_VARARGS, "Request for imsDtmf" },
  { "imsExplicitCallTransfer",
    imsExplicitCallTransfer,
    METH_VARARGS,
    "Request for ims Explicit Call Transfer" },
  { "imsRequestExitEmergencyCallbackMode",
    imsRequestExitEmergencyCallbackMode,
    METH_VARARGS,
    "Request for ims Request Exit Emergency callback" },
  { "imsRequestConference",
    imsRequestConference,
    METH_VARARGS,
    "Request for ims Request Conference" },
  { "CarrierInfoForImsiEncryption",
    CarrierInfoForImsiEncryption,
    METH_VARARGS,
    "Request for carrier info imsi encryption" },
  { "exitEmergencyCbMode", exitEmergencyCbMode, METH_VARARGS, "Request for exit Emergency Cb mode" },
  { "setBandMode", setBandMode, METH_VARARGS, "Request for set band mode" },
  { "setIndicationFilter", setIndicationFilter, METH_VARARGS, "Request for set indication filter" },
  { "imsGetImsSubConfig", imsGetImsSubConfig, METH_VARARGS, "Request for ims get IMS sub config" },
  { "imsSetServiceStatus", imsSetServiceStatus, METH_VARARGS, "Request for ims set service status" },
  { "imsQueryServiceStatus",
    imsQueryServiceStatus,
    METH_VARARGS,
    "Request for ims Query Service status" },
  { "imsRegistrationChange",
    imsRegistrationChange,
    METH_VARARGS,
    "Request for ims Registration change" },
  { "imsHangup", imsHangup, METH_VARARGS, "Request for IMS hangup" },
  { "imsAnswer", imsAnswer, METH_VARARGS, "Request for ims answer" },
  { "imsDial", imsDial, METH_VARARGS, "Request for IMS dial" },
  { "imsGetRegistrationState",
    imsGetRegistrationState,
    METH_VARARGS,
    "Request for ims Get Registration State" },
  { "SetPreferredDataModem",
    SetPreferredDataModem,
    METH_VARARGS,
    "Request for set preferred data modem" },
  { "SetLinkCapRptCriteria",
    SetLinkCapRptCriteria,
    METH_VARARGS,
    "Request for set link cap rpt criteria" },
  { "SetLinkCapFilter", SetLinkCapFilter, METH_VARARGS, "Request for Set Link Cap Filter" },
  { "PullLceData", PullLceData, METH_VARARGS, "Request Pull LCE data" },
  { "StartLceData", StartLceData, METH_VARARGS, "Request Stop LCE data" },
  { "StartLceData", StartLceData, METH_VARARGS, "Request Start LCE data" },
  { "ReportStkServiceIsRunning",
    ReportStkServiceIsRunning,
    METH_VARARGS,
    "Request for Stk service is running" },
  { "StkHandleCallSetupRequestedFromSim",
    StkHandleCallSetupRequestedFromSim,
    METH_VARARGS,
    "Request for Stk Handle Call Set up request from sim" },
  { "StkSendTerminalResponse",
    StkSendTerminalResponse,
    METH_VARARGS,
    "Request for stk send terminal response" },
  { "StkSendEnvelopeCommand",
    StkSendEnvelopeCommand,
    METH_VARARGS,
    "Request for Stk Send Envolope command" },
  { "EnterNetworkDepersonalization",
    EnterNetworkDepersonalization,
    METH_VARARGS,
    "Request for network depersonalisation" },
  { "shutDown", shutDown, METH_VARARGS, "Shut down request" },
  { "sendCdmaSms", sendCdmaSms, METH_VARARGS, "Send Cdma SMS" },
  { "getCdmaRoamingPreference",
    getCdmaRoamingPreference,
    METH_VARARGS,
    "Request for get Cdma Roaming Preference" },
  { "getCdmaSubscriptionSource",
    getCdmaSubscriptionSource,
    METH_VARARGS,
    "Request for get Cdma Subscription Service" },
  { "queryAvailableNetworks",
    queryAvailableNetworks,
    METH_VARARGS,
    "Request for query available networks" },
  { "setNetworkSelectionManual",
    setNetworkSelectionManual,
    METH_VARARGS,
    "Request for set Network Selection Mannual" },
  { "getImsRegState", getImsRegState, METH_VARARGS, "Request for get IMS Reg State" },
  { "sendCdmaBurstDtmf", sendCdmaBurstDtmf, METH_VARARGS, "Request for send Cdma Burst Dtmf" },
  { "sendCdmaFlash", sendCdmaFlash, METH_VARARGS, "Request for send Cdma flash" },
  { "getCdmaVoicePrefMode",
    getCdmaVoicePrefMode,
    METH_VARARGS,
    "Request for get Cdms Voice Pref mode" },
  { "setCdmaVoicePrefMode",
    setCdmaVoicePrefMode,
    METH_VARARGS,
    "Request for set Cdma Voice Pref mode" },
  { "getTtyMode", getTtyMode, METH_VARARGS, "Request fot get Tty mode" },
  { "setTtyMode", setTtyMode, METH_VARARGS, "Request fot set Tty mode" },
  { "setSuppSvcNotification",
    setSuppSvcNotification,
    METH_VARARGS,
    "Request for set Supp Svc Notification" },
  { "queryClip", queryClip, METH_VARARGS, "Request for query Clip" },
  { "changeBarringPassword",
    changeBarringPassword,
    METH_VARARGS,
    "Request for change barring password" },
  { "setCallWaiting", setCallWaiting, METH_VARARGS, "Request for set Call Waiting" },
  { "setCallForward", setCallForward, METH_VARARGS, "Request for set Call Forward" },
  { "queryCallWaiting", queryCallWaiting, METH_VARARGS, "Request for query Call Waiting" },
  { "setCallForward", setCallForward, METH_VARARGS, "Request for set Call Forward" },
  { "queryCallForwardStatus",
    queryCallForwardStatus,
    METH_VARARGS,
    "Request for query Call Forward Status" },
  { "setClir", setClir, METH_VARARGS, "Request fot set Clir" },
  { "getClir", getClir, METH_VARARGS, "Request for get Clir" },
  { "sendUssd", sendUssd, METH_VARARGS, "Request for send USSD" },
  { "reportSmsMemoryStatus",
    reportSmsMemoryStatus,
    METH_VARARGS,
    "Request for report SMS Memory Status" },
  { "deleteCdmaSmsOnRuim",
    deleteCdmaSmsOnRuim,
    METH_VARARGS,
    "Request for delete CDMA SMS on RUIM" },
  { "getCdmaSubscription", getCdmaSubscription, METH_VARARGS, "Request for get CDMA subscription" },
  { "setCdmaSmsBroadcastConfig",
    setCdmaSmsBroadcastConfig,
    METH_VARARGS,
    "RIL request for set CDMA SMS broadcast config" },
  { "answer", answer, METH_VARARGS, "Request for answer" },
  { "convergedAnswer", convergedAnswer, METH_VARARGS, "Request for converged Answer" },
  { "deactivateDataCall", deactivateDataCall, METH_VARARGS, "Request for deactivate data call" },
  { "simChangePin2Req", simChangePin2Req, METH_VARARGS, "Request for change sim pin2" },
  { "simChangePinReq", simChangePinReq, METH_VARARGS, "Request for change sim pin" },
  { "simEnterPuk2Req", simEnterPuk2Req, METH_VARARGS, "Request for enter sim puk2" },
  { "simEnterPin2Req", simEnterPin2Req, METH_VARARGS, "Request for enter sim pin2" },
  { "simEnterPukReq", simEnterPukReq, METH_VARARGS, "Request for enter sim puk" },
  { "writeSmsToSim", writeSmsToSim, METH_VARARGS, "Request fot write sms to sim" },
  { "GsmSetBroadcastSMSConfig",
    GsmSetBroadcastSMSConfig,
    METH_VARARGS,
    "Request for GSM set broadcast sms config" },
  { "GsmGetBroadcastSmsConfig",
    GsmGetBroadcastSmsConfig,
    METH_VARARGS,
    "Request for GSM get broadcast sms config" },
  { "simTransmitApduChannelReq",
    simTransmitApduChannelReq,
    METH_VARARGS,
    "Request for sim transmit apdu channel" },
  { "setDataProfile", setDataProfile, METH_VARARGS, "Request for set Data profile" },
  { "deleteSmsOnSim", deleteSmsOnSim, METH_VARARGS, "Request for delete sms on sim" },
  { "setCdmaSmsBroadcastActivation",
    setCdmaSmsBroadcastActivation,
    METH_VARARGS,
    "Request for CDMA SMS broadcast activation" },
  { "simCloseChannelReq", simCloseChannelReq, METH_VARARGS, "Request for sim close channel" },
  { "simIsimAuthenticationReq",
    simIsimAuthenticationReq,
    METH_VARARGS,
    "Request for sim isim authentication" },
  { "simGetImsiReq", simGetImsiReq, METH_VARARGS, "Request for get sim IMSI" },
  { "ackCdmaSms", ackCdmaSms, METH_VARARGS, "Request for ackCdma SMS" },
  { "simOpenChannelReq", simOpenChannelReq, METH_VARARGS, "Request for sim Open Channel" },
  { "simTransmitApduBasicReq",
    simTransmitApduBasicReq,
    METH_VARARGS,
    "Request to sim transmit apdu basic req" },
  { "setInitialAttachApn", setInitialAttachApn, METH_VARARGS, "Request to set initial apn" },
  { "setUnsolCellInfoListRate",
    setUnsolCellInfoListRate,
    METH_VARARGS,
    "Request for set unsol cell info" },
  { "VoiceRadioTech", VoiceRadioTech, METH_VARARGS, "Request for voice radio tech" },
  { "simStkSendEnvelopeWithStatusReq",
    simStkSendEnvelopeWithStatusReq,
    METH_VARARGS,
    "Request for sim stk send envolope with status" },
  { "simAuthenticationReq", simAuthenticationReq, METH_VARARGS, "Request for Sim Authentication" },
  { "setSmscAddress", setSmscAddress, METH_VARARGS, "Request for set SMSC" },
  { "getSmscAddress", getSmscAddress, METH_VARARGS, "Request for SMSC" },
  { "deviceIdentity", deviceIdentity, METH_VARARGS, "Request for device identity" },
  { "GsmSmsBroadcastActivation",
    GsmSmsBroadcastActivation,
    METH_VARARGS,
    "Request for GSM SMS broadcast activation" },
  { "explicitCallTransfer",
    explicitCallTransfer,
    METH_VARARGS,
    "Request for explicit call transfer" },
  { "queryAvailableBandMode",
    queryAvailableBandMode,
    METH_VARARGS,
    "Request for availble band mode" },
  { "oemhookRaw", oemhookRaw, METH_VARARGS, "Request for OEM HOOK RAW" },
  { "getDataCallList", getDataCallList, METH_VARARGS, "Request for get data Call list" },
  { "getMute", getMute, METH_VARARGS, "Request for get mute" },
  { "setMute", setMute, METH_VARARGS, "Request for setting mute" },
  { "separateConnection", separateConnection, METH_VARARGS, "Request for Seperate connection" },
  { "basebandVersion", basebandVersion, METH_VARARGS, "Request for base band version" },
  { "dtmfStop", dtmfStop, METH_VARARGS, "Request for dtmf stop" },
  { "dtmfStart", dtmfStart, METH_VARARGS, "Request for dtmf start" },
  { "QueryNetworkSelectionMode",
    QueryNetworkSelectionMode,
    METH_VARARGS,
    "Request for query Network Selection Mode" },
  { "setNetworkSelectionAutomatic",
    setNetworkSelectionAutomatic,
    METH_VARARGS,
    "Request for Set Network Selection Automatic" },
  { "getPreferredNetworkType",
    getPreferredNetworkType,
    METH_VARARGS,
    "Get Set Preferred network type" },
  { "dial", dial, METH_VARARGS, "Place a voice call." },
  { "convergedDial", convergedDial, METH_VARARGS, "Place a converged voice call." },
  { "getSignalStrength", getSignalStrength, METH_VARARGS, "Get Signal Strength" },
  { "getCellInfo", getCellInfo, METH_VARARGS, "Get Cell Info" },
  { "radioPower", radioPower, METH_VARARGS, "Get Radio Power new" },
  { "operatorName", operatorName, METH_VARARGS, "Get Operator Name" },
  { "getDataRegStatus", getDataRegStatus, METH_VARARGS, "Get Data Registration Status" },
  { "getVoiceRegStatus", getVoiceRegStatus, METH_VARARGS, "Get voice Registration Status" },
  { "simGetSimStatusReq", simGetSimStatusReq, METH_VARARGS, "Get Sim Status Request" },
  { "getCurrentCalls", getCurrentCalls, METH_VARARGS, "Get Current calls" },
  { "convergedGetCurrentCalls",
    convergedGetCurrentCalls,
    METH_VARARGS,
    "Get Converged Current calls" },
  { "simEnterPinReq", simEnterPinReq, METH_VARARGS, "Sim Enter Pin Request" },
  { "hangupWaitingOrBackground",
    hangupWaitingOrBackground,
    METH_VARARGS,
    "hang up waiting or background" },
  { "hangupForegroundResumeBackground",
    hangupForegroundResumeBackground,
    METH_VARARGS,
    "hang up foreground and resume background" },
  { "switchWaitingOrHoldingAndActive",
    switchWaitingOrHoldingAndActive,
    METH_VARARGS,
    "switch waiting or holding active" },
  { "convergedSwitchWaitingOrHoldingAndActive",
    convergedSwitchWaitingOrHoldingAndActive,
    METH_VARARGS,
    "convergedSwitch waiting or holding active" },
  { "conference", conference, METH_VARARGS, "Ril request for conference calls" },
  { "udub", udub, METH_VARARGS, "Ril request for udub" },
  { "lastCallFailCause", lastCallFailCause, METH_VARARGS, "Ril request for last call fail cause" },
  { "dtmf", dtmf, METH_VARARGS, "Ril request for dtmf" },
  { "simIOReq", simIOReq, METH_VARARGS, "RIl Sim IO request" },
  { "hangup", hangup, METH_VARARGS, "Hang Up Call" },
  { "convergedHangup", convergedHangup, METH_VARARGS, "convergedHang Up Call" },
  { "setupDataCall", setupDataCall, METH_VARARGS, "RIL request for set up data call" },
  { "setupDataCall_1_6", setupDataCall_1_6, METH_VARARGS, "RIL request for set up data call 1_6" },
  { "cancelUssd", cancelUssd, METH_VARARGS, "RIL request for cancel USSD " },
  { "simQueryFacilityLockReq",
    simQueryFacilityLockReq,
    METH_VARARGS,
    "RIL request for sim Query Facility Lock request" },
  { "simSetFacilityLockReq",
    simSetFacilityLockReq,
    METH_VARARGS,
    "RIL request for sim set facility lock req" },
  { "setPreferredNetworkType",
    setPreferredNetworkType,
    METH_VARARGS,
    "RIL request for set Preferred network type" },
  { "getCdmaSmsBroadcastConfig",
    getCdmaSmsBroadcastConfig,
    METH_VARARGS,
    "RIL request for get Cdma SMS broadcast config" },
  { "SetDataThrottling", SetDataThrottling, METH_VARARGS, "RIL request for SetDataThrottling" },
  { "GetDataNrIconType", GetDataNrIconType, METH_VARARGS, "RIL request for GetDataNrIconType" },
  { "StartKeepAlive", StartKeepAlive, METH_VARARGS, "RIL request for StartKeepAlive" },
  { "StopKeepAlive", StopKeepAlive, METH_VARARGS, "RIL request for StopKeepAlive" },
  { "GetSlicingConfig", GetSlicingConfig, METH_VARARGS, "RIL request for GetSlicingConfig" },
  { NULL, NULL, 0, NULL } /* Sentinel */
};

static struct PyModuleDef ril_api_module = {
    PyModuleDef_HEAD_INIT,
    "ril_api",    /* name of module */
    NULL,         /* module documentation, may be NULL */
    -1,           /* size of per-interpreter state of the module,
                    or -1 if the module keeps state in global variables. */
    RilApiMethods
};

PyMODINIT_FUNC PyInit_ril_api(void) {
    PyObject* m;
    m = PyModule_Create(&ril_api_module);
    if (m == NULL)
        return NULL;

    RilApiError = PyErr_NewException("ril_api.error", NULL, NULL);
    Py_XINCREF(RilApiError);
    if (PyModule_AddObject(m, "error", RilApiError) < 0) {
        Py_XDECREF(RilApiError);
        Py_CLEAR(RilApiError);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

