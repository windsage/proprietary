/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef MARSHAL_MODULE_CONFIG_HPP_INCLUDED
#define MARSHAL_MODULE_CONFIG_HPP_INCLUDED

#include <Marshal.h>
#include <telephony/RIL_ModuleList_i.hpp>

template <>
Marshal::Result Marshal::write<RIL_ModuleList>(const RIL_ModuleList& arg);

template <>
Marshal::Result Marshal::read<RIL_ModuleList>(RIL_ModuleList& arg) const;

template <>
Marshal::Result Marshal::write<ModuleConfig::ModuleList>(const ModuleConfig::ModuleList& arg);

template <>
Marshal::Result Marshal::read<ModuleConfig::ModuleList>(ModuleConfig::ModuleList& arg) const;
#endif
