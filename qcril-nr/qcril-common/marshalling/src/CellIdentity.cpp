/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <type_traits>
#include <telephony/ril.h>
#include <marshal/CellIdentity.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_CellIdentityGsm_v12>(const RIL_CellIdentityGsm_v12 &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.lac);
    write(arg.cid);
    write(arg.arfcn);
    write(arg.bsic);
    write(arg.operatorNames);
    write(arg.reg_plmn_mcc);
    write(arg.reg_plmn_mnc);
    write(arg.primary_plmn_mcc);
    write(arg.primary_plmn_mnc);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityGsm_v12>(RIL_CellIdentityGsm_v12 &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.lac);
    read(arg.cid);
    read(arg.arfcn);
    read(arg.bsic);
    read(arg.operatorNames);
    read(arg.reg_plmn_mcc);
    read(arg.reg_plmn_mnc);
    read(arg.primary_plmn_mcc);
    read(arg.primary_plmn_mnc);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_CellIdentityGsm_v12>(RIL_CellIdentityGsm_v12 &arg) {
    release(arg.mcc);
    release(arg.mnc);
    release(arg.lac);
    release(arg.cid);
    release(arg.arfcn);
    release(arg.bsic);
    release(arg.operatorNames);
    release(arg.reg_plmn_mcc);
    release(arg.reg_plmn_mnc);
    release(arg.primary_plmn_mcc);
    release(arg.primary_plmn_mnc);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityCdma>(const RIL_CellIdentityCdma &arg) {
    write(arg.networkId);
    write(arg.systemId);
    write(arg.basestationId);
    write(arg.longitude);
    write(arg.latitude);
    write(arg.operatorNames);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityCdma>(RIL_CellIdentityCdma &arg) const {
    read(arg.networkId);
    read(arg.systemId);
    read(arg.basestationId);
    read(arg.longitude);
    read(arg.latitude);
    read(arg.operatorNames);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_CellIdentityCdma>(RIL_CellIdentityCdma &arg) {
    release(arg.networkId);
    release(arg.systemId);
    release(arg.basestationId);
    release(arg.longitude);
    release(arg.latitude);
    release(arg.operatorNames);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityLte_v12>(const RIL_CellIdentityLte_v12 &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.ci);
    write(arg.pci);
    write(arg.tac);
    write(arg.earfcn);
    write(arg.operatorNames);
    write(arg.bandwidth);
    write(arg.reg_plmn_mcc);
    write(arg.reg_plmn_mnc);
    write(arg.primary_plmn_mcc);
    write(arg.primary_plmn_mnc);
    write(arg.band);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityLte_v12>(RIL_CellIdentityLte_v12 &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.ci);
    read(arg.pci);
    read(arg.tac);
    read(arg.earfcn);
    read(arg.operatorNames);
    read(arg.bandwidth);
    read(arg.reg_plmn_mcc);
    read(arg.reg_plmn_mnc);
    read(arg.primary_plmn_mcc);
    read(arg.primary_plmn_mnc);
    read(arg.band);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_CellIdentityLte_v12>(RIL_CellIdentityLte_v12 &arg) {
    release(arg.mcc);
    release(arg.mnc);
    release(arg.ci);
    release(arg.pci);
    release(arg.tac);
    release(arg.earfcn);
    release(arg.operatorNames);
    release(arg.bandwidth);
    release(arg.reg_plmn_mcc);
    release(arg.reg_plmn_mnc);
    release(arg.primary_plmn_mcc);
    release(arg.primary_plmn_mnc);
    release(arg.band);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityWcdma_v12>(const RIL_CellIdentityWcdma_v12 &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.lac);
    write(arg.cid);
    write(arg.psc);
    write(arg.uarfcn);
    write(arg.operatorNames);
    write(arg.reg_plmn_mcc);
    write(arg.reg_plmn_mnc);
    write(arg.primary_plmn_mcc);
    write(arg.primary_plmn_mnc);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityWcdma_v12>(RIL_CellIdentityWcdma_v12 &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.lac);
    read(arg.cid);
    read(arg.psc);
    read(arg.uarfcn);
    read(arg.operatorNames);
    read(arg.reg_plmn_mcc);
    read(arg.reg_plmn_mnc);
    read(arg.primary_plmn_mcc);
    read(arg.primary_plmn_mnc);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_CellIdentityWcdma_v12>(RIL_CellIdentityWcdma_v12 &arg) {
    release(arg.mcc);
    release(arg.mnc);
    release(arg.lac);
    release(arg.cid);
    release(arg.psc);
    release(arg.uarfcn);
    release(arg.operatorNames);
    release(arg.reg_plmn_mcc);
    release(arg.reg_plmn_mnc);
    release(arg.primary_plmn_mcc);
    release(arg.primary_plmn_mnc);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityTdscdma>(const RIL_CellIdentityTdscdma &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.lac);
    write(arg.cid);
    write(arg.cpid);
    write(arg.operatorNames);
    write(arg.reg_plmn_mcc);
    write(arg.reg_plmn_mnc);
    write(arg.primary_plmn_mcc);
    write(arg.primary_plmn_mnc);
    write(arg.uarfcn);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityTdscdma>(RIL_CellIdentityTdscdma &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.lac);
    read(arg.cid);
    read(arg.cpid);
    read(arg.operatorNames);
    read(arg.reg_plmn_mcc);
    read(arg.reg_plmn_mnc);
    read(arg.primary_plmn_mcc);
    read(arg.primary_plmn_mnc);
    read(arg.uarfcn);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_CellIdentityTdscdma>(RIL_CellIdentityTdscdma &arg) {
    release(arg.mcc);
    release(arg.mnc);
    release(arg.lac);
    release(arg.cid);
    release(arg.cpid);
    release(arg.operatorNames);
    release(arg.reg_plmn_mcc);
    release(arg.reg_plmn_mnc);
    release(arg.primary_plmn_mcc);
    release(arg.primary_plmn_mnc);
    release(arg.uarfcn);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityNr>(const RIL_CellIdentityNr &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.reg_plmn_mcc);
    write(arg.reg_plmn_mnc);
    write(arg.primary_plmn_mcc);
    write(arg.primary_plmn_mnc);
    write(arg.nci);
    write(arg.pci);
    write(arg.tac);
    write(arg.nrarfcn);
    write(arg.operatorNames);
    write(arg.bandwidth);
    write(arg.band);
    write(arg.snpn_info_valid);
    write(arg.snpn_info);
    write(arg.cag_info_valid);
    write(arg.cag_info);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityNr>(RIL_CellIdentityNr &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.reg_plmn_mcc);
    read(arg.reg_plmn_mnc);
    read(arg.primary_plmn_mcc);
    read(arg.primary_plmn_mnc);
    read(arg.nci);
    read(arg.pci);
    read(arg.tac);
    read(arg.nrarfcn);
    read(arg.operatorNames);
    read(arg.bandwidth);
    read(arg.band);
    read(arg.snpn_info_valid);
    read(arg.snpn_info);
    read(arg.cag_info_valid);
    read(arg.cag_info);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_CellIdentityNr>(RIL_CellIdentityNr &arg) {
    release(arg.mcc);
    release(arg.mnc);
    release(arg.reg_plmn_mcc);
    release(arg.reg_plmn_mnc);
    release(arg.primary_plmn_mcc);
    release(arg.primary_plmn_mnc);
    release(arg.nci);
    release(arg.pci);
    release(arg.tac);
    release(arg.nrarfcn);
    release(arg.operatorNames);
    release(arg.bandwidth);
    release(arg.band);
    release(arg.snpn_info_valid);
    release(arg.snpn_info);
    release(arg.cag_info_valid);
    release(arg.cag_info);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityOperatorNames>(const RIL_CellIdentityOperatorNames &arg) {
    write(arg.alphaShort);
    write(arg.alphaLong);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityOperatorNames>(RIL_CellIdentityOperatorNames &arg) const {
    read(arg.alphaShort);
    read(arg.alphaLong);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_CellIdentityOperatorNames>(RIL_CellIdentityOperatorNames &arg) {
    release(arg.alphaShort);
    release(arg.alphaLong);
    return Result::SUCCESS;
}
