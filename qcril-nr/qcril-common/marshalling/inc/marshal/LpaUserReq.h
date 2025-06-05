/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_MARSHALLING_LPA_REQ
#define _QTI_MARSHALLING_LPA_REQ

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<UimLpaUserReq>(const UimLpaUserReq& arg);

template <>
Marshal::Result Marshal::read<UimLpaUserReq>(UimLpaUserReq& arg) const;

template <>
Marshal::Result Marshal::read<UimLpaUserResponse>(UimLpaUserResponse& arg) const;

template <>
Marshal::Result Marshal::read<UimLpaProfileInfo>(UimLpaProfileInfo& arg) const;

template <>
Marshal::Result Marshal::write<UimLpaUserResponse>(const UimLpaUserResponse& arg) ;

template <>
Marshal::Result Marshal::write<UimLpaProfileInfo>(const UimLpaProfileInfo& arg) ;

template <>
Marshal::Result Marshal::write<UimLpaSrvAddrOpResp>(const UimLpaSrvAddrOpResp& arg) ;

template <>
Marshal::Result Marshal::read<UimLpaSrvAddrOpResp>(UimLpaSrvAddrOpResp& arg) const;

template <>
Marshal::Result Marshal::write<UimLpaSrvAddrOpReq>(const UimLpaSrvAddrOpReq& arg);

template <>
Marshal::Result Marshal::read<UimLpaSrvAddrOpReq>(UimLpaSrvAddrOpReq& arg) const;

template <>
Marshal::Result Marshal::write<UimLpaHttpTrnsReq>(const UimLpaHttpTrnsReq& arg);

template <>
Marshal::Result Marshal::read<UimLpaHttpTrnsReq>(UimLpaHttpTrnsReq& arg) const;

template <>
Marshal::Result Marshal::write<UimLpaHttpCustomHeader>(const UimLpaHttpCustomHeader& arg);

template <>
Marshal::Result Marshal::read<UimLpaHttpCustomHeader>(UimLpaHttpCustomHeader& arg) const;

template <>
Marshal::Result Marshal::read<UimLpaAddProfProgressInd>(UimLpaAddProfProgressInd& arg) const;

template <>
Marshal::Result Marshal::write<UimLpaAddProfProgressInd>(const UimLpaAddProfProgressInd& arg);

template <>
Marshal::Result Marshal::write<UimLpaHttpTxnIndype>(const UimLpaHttpTxnIndype& arg);

template <>
Marshal::Result Marshal::read<UimLpaHttpTxnIndype>(UimLpaHttpTxnIndype& arg) const;

template <>
Marshal::Result Marshal::release<UimLpaUserReq>(UimLpaUserReq& arg);

template <>
Marshal::Result Marshal::release<UimLpaUserResponse>(UimLpaUserResponse& arg);

template <>
Marshal::Result Marshal::release<UimLpaSrvAddrOpReq>(UimLpaSrvAddrOpReq& arg);

template <>
Marshal::Result Marshal::release<UimLpaSrvAddrOpResp>(UimLpaSrvAddrOpResp& arg);

template <>
Marshal::Result Marshal::release<UimLpaHttpTrnsReq>(UimLpaHttpTrnsReq& arg);

template <>
Marshal::Result Marshal::release<UimLpaHttpCustomHeader>(UimLpaHttpCustomHeader& arg);

template <>
Marshal::Result Marshal::release<UimLpaProfileInfo>(UimLpaProfileInfo& arg);

template <>
Marshal::Result Marshal::release<UimLpaHttpTxnIndype>(UimLpaHttpTxnIndype& arg);

template <>
Marshal::Result Marshal::release<UimLpaHttpTxnIndype>(UimLpaHttpTxnIndype& arg);

template <>
Marshal::Result Marshal::release<UimLpaAddProfProgressInd>(UimLpaAddProfProgressInd& arg);
#endif
