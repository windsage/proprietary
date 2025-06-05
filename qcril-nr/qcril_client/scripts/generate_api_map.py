#******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************

#!/usr/bin/env python3

import os.path as path
import subprocess
import argparse

parser = argparse.ArgumentParser(
    description="Generates a header file that maps RIL Request IDs to string names."
    )
parser.add_argument('-i', "--input_file")
parser.add_argument('-o', "--output_file")

def find_request_defs(lines):
    requests = dict()
    prefix = "#define RIL_REQUEST_"
    for line in lines:
        if line.startswith(prefix):
            line = line[len(prefix):]
            line_split = line.split()
            if (len(line_split) > 1):
                req_name, req_num = line_split[0], line_split[1]
                requests[req_num] = req_name
    return requests

def write_header(requests, output_path):
    out_str = "#pragma once\n"
    out_str += "#define INIT_REQUEST_API_MAP(map_name) \\\n"
    out_str += "do { \\\n"
    indent = " " * 4
    for req_num in requests.keys():
        try:
            req_name = requests[req_num]
            out_str += "{0}map_name[{1}] = \"{2}\";\\\n".format(indent, req_num, req_name)
        except KeyError:
            continue
    out_str += "} while(0)\n"
    with open(output_path, 'w') as f:
        f.write(out_str)


def main(input_file, output_file):
    requests = {}
    with open(input_file, 'r') as header:
        lines = header.readlines()
        requests = find_request_defs(lines)
    write_header(requests, output_file)

if __name__ == "__main__":
    args = parser.parse_args()
    main(args.input_file, args.output_file)
