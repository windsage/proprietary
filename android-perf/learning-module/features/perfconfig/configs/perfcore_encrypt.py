#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import base64

def encode(infile, outfile):
    try:
        with open(infile, 'r', encoding='utf-8') as fd:
            content = fd.read()
    except Exception as e:
        print(f"Error reading file {infile}: {str(e)}")
        return False

    try:
        base64_content = base64.b64encode(content.encode('utf-8'))

        encrypted_bytes = bytearray(base64_content)
        for i in range(len(encrypted_bytes)):
            encrypted_bytes[i] += 33

        encrypted_content = "BASE64:" + encrypted_bytes.decode('latin-1')

        with open(outfile, 'w', encoding='latin-1') as fd:
            fd.write(encrypted_content)

        print(f"Successfully encrypted {infile} -> {outfile}")
        return True
    except Exception as e:
        print(f"Error encrypting file {infile}: {str(e)}")
        return False

def decode(infile, outfile):
    try:
        with open(infile, 'r', encoding='latin-1') as fd:
            content = fd.read()

        if not content.startswith("BASE64:"):
            print(f"Error: Invalid encrypted file format - missing BASE64 prefix")
            return False

        encrypted_data = content[7:]  # Skip "BASE64:"

        encrypted_bytes = bytearray(encrypted_data.encode('latin-1'))
        for i in range(len(encrypted_bytes)):
            encrypted_bytes[i] -= 33

        decoded_content = base64.b64decode(encrypted_bytes).decode('utf-8')

        with open(outfile, 'w', encoding='utf-8') as fd:
            fd.write(decoded_content)

        print(f"Successfully decrypted {infile} -> {outfile}")
        return True
    except Exception as e:
        print(f"Error decrypting file {infile}: {str(e)}")
        return False

def usage():
    print("Usage: " + sys.argv[0] + " -e/-d <input file> <output file>")

if __name__ == '__main__':
    argc = len(sys.argv)
    if argc != 4:
        usage()
        sys.exit(1)

    if sys.argv[1] == "-e":
        success = encode(sys.argv[2], sys.argv[3])
    elif sys.argv[1] == "-d":
        success = decode(sys.argv[2], sys.argv[3])
    else:
        usage()
        sys.exit(1)

    sys.exit(0 if success else 1)
