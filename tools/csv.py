#! /usr/bin/env python3

# reads lines from stdin, prints corresponding csv notation

try:
    while True:
        print(",".join(input().split()), flush=True)
except EOFError:
    pass
