#!/bin/bash
#
#******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

set -x

# at least 3 arguments
if [[ $# -le 2 ]]; then
    echo "syntax: gen_db.sh sqlite_tool db_file sql_file0 sql_file1..."
    exit 1
fi

# get info from arg list
SQLITE=$1
TARGET_DB=$2

ARGS=("$@")
NUM_ARGS=${#ARGS[@]}
MAX_INDEX=$(($NUM_ARGS-1))

# seperate the config sql list and ecc sql list
ECC_SQL_FILES=""
CONFIG_SQL_FILES=""
for i in $(seq 2 $MAX_INDEX); do
    if [[ ${ARGS[$i]} == *"_config.sql"* ]]; then
        CONFIG_SQL_FILES+="${ARGS[$i]} "
    else
        ECC_SQL_FILES+="${ARGS[$i]} "
    fi
done

# generate the db file
rm $TARGET_DB
if [ ! -z "$ECC_SQL_FILES" ]; then
    SORTED_ECC_SQL_LIST=`echo -n $ECC_SQL_FILES | tr ' ' '\n' | sort -V | tr '\n' ' '`
    (echo BEGIN TRANSACTION\;; cat $SORTED_ECC_SQL_LIST; echo COMMIT TRANSACTION\;) | $SQLITE $TARGET_DB
fi

if [ ! -z "$CONFIG_SQL_FILES" ]; then
    SORTED_CONFIG_SQL_LIST=`echo -n $CONFIG_SQL_FILES | tr ' ' '\n' | sort -V | tr '\n' ' '`
    (echo BEGIN TRANSACTION\;; cat $SORTED_CONFIG_SQL_LIST; echo COMMIT TRANSACTION\;) | $SQLITE $TARGET_DB
fi
