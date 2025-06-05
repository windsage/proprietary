/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#ifdef __cplusplus
#include <memory>
#include <string>
#include <vector>
#include <functional>

class Module;
class Dispatcher;

#endif

struct message_id_info;
typedef struct message_id_info &message_id_ref;
typedef struct message_id_info *message_id_p;

#ifdef __cplusplus
struct message_id_info
{
    friend class Dispatcher;
    private:
        std::string m_name;

    public:
        size_t idx = 0;

        message_id_info() = delete;
        message_id_info(const message_id_info& other) = delete;
        message_id_info(message_id_info&& other) = delete;
        message_id_info& operator=(const message_id_info& other) = delete;
        message_id_info& operator=(message_id_info&& other) = delete;

        message_id_info(const std::string &name): m_name(name) {}

        bool operator==(const message_id_info &other) {
            return idx == other.idx;
        }

         bool operator!=(const message_id_info &other) {
            return idx != other.idx;
        }

        const std::string get_name() const { return m_name; };
        void set_name(std::string name) {m_name = name;}
        int get_idx() const { return idx; }

        ~message_id_info() {
        }
};

template<>
struct std::hash<std::reference_wrapper<message_id_info>>
{
    size_t operator() (std::reference_wrapper<message_id_info> id) const {
        size_t ret = 0;
        std::hash<size_t> h;
        ret = h(id.get().idx);
        return ret;
    }
};

template<>
struct std::equal_to<std::reference_wrapper<message_id_info>>
{
    bool operator() (std::reference_wrapper<message_id_info> lhs,
          std::reference_wrapper<message_id_info> rhs) const {
        bool ret = false;
        std::equal_to<size_t> h;
        ret = h(lhs.get().idx, rhs.get().idx);
        return ret;
    }
};
#endif
