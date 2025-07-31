#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import base64

def encode(infile, outfile):
    with open(infile, 'r', encoding='utf-8') as fd:
        content = fd.read()

    encoded_content = "BASE64:" + base64.b64encode(content.encode('utf-8')).decode('ascii')

    with open(outfile, 'w', encoding='utf-8') as fd:
        fd.write(encoded_content)

def decode(infile, outfile):
    with open(infile, 'r', encoding='utf-8') as fd:
        content = fd.read()

    if not content.startswith("BASE64:"):
        print("Error: Invalid encrypted file format")
        sys.exit(1)

    base64_content = content[7:]  # Skip "BASE64:"
    decoded_content = base64.b64decode(base64_content).decode('utf-8')

    with open(outfile, 'w', encoding='utf-8') as fd:
        fd.write(decoded_content)

def usage():
    print("Usage: " + sys.argv[0] + " -e/-d <input file> <output file>")

if __name__ == '__main__':
    argc = len(sys.argv)
    if argc != 4:
        usage()
        exit()

    if sys.argv[1] == "-e":
        encode(sys.argv[2], sys.argv[3])
    elif sys.argv[1] == "-d":
        decode(sys.argv[2], sys.argv[3])
    else:
        usage()
