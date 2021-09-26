#! /usr/bin/env python
#
# The packaging tool of Testworld Project
#
# Author: HenryAWE
# License: The 3-clause BSD License

import argparse
import zipfile


parser = argparse.ArgumentParser("Packaging Tool Options")
parser.add_argument("--output", "-o")
parser.add_argument("--input", "-i", nargs='+', required=True)
parser.add_argument("--mode", "-m", default="w")
args = parser.parse_args()

z = zipfile.ZipFile(args.output, args.mode)
for file in args.input:
    z.write(file)
