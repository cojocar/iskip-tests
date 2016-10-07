#!/usr/bin/env python
#
# Copyright 2016 The IskipTests Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import random
import struct

import os
import sys
import argparse
from util import get_io_from_args, get_arg_parse_default
from util import to_hex_perl

from util import get_rand_key, get_rand_buf, to_hex

def get_sorted_rand_buf(elem_size=1, length=1024):
    end = 2 ** (8 * elem_size)
    start = int(0.4 * float(end))
    ret = [random.randrange(start, end) for _ in range(length)]
    return sorted(ret)

BUF_BYTES = 1024
ELEM_SIZE = 2
BUF_LEN = BUF_BYTES / ELEM_SIZE

def get_format_for_size(elem_size):
    if elem_size == 1:
        return 'B'
    elif elem_size == 2:
        return 'H'
    elif elem_size == 4:
        return 'I'
    else:
        raise Exception("unknown elem_size: %d" % elem_size)

class BSearch:
    def __init__(self, cmd_ser=None):
        self._ser = cmd_ser

        self._old_write = self._ser.write
        self._ser.write = self.ser_write
        self._write_log = []

    def ser_write(self, c):
        self._write_log.append(c)
        return self._old_write(c)

    def _write_elem(self, elem):
        for b in struct.pack(">%s" % get_format_for_size(ELEM_SIZE), elem):
            self._ser.write(b)
            self._ser.flush()


    def getWriteLog(self):
        return self._write_log

    def cmd_set_search_buf(self, buf=['\1']*1024):
        self._ser.write('B')
        for d in buf:
            self._write_elem(d)
            #print("XXX: " + str(c))
            ans = self._ser.readline()
            #print("val=%x ans=%s" % (d, repr(ans)))

        # this should print the checsum
        ans = self._ser.readline()

        self.cmd_verify()
        return self.cmd_checksum()

    def cmd_verify(self):
        self._ser.write('V')
        return self._ser.readline()

    def cmd_checksum(self):
        self._ser.write('A')
        return self._ser.readline()

    def cmd_search_unrolled(self, v=0x1):
        self._ser.write('S')
        self._write_elem(v)

        ret = ''
        ret += self._ser.readline()
        #print(ret)
        ret += self._ser.readline()
        #print(ret)
        return ret

    def cmd_search_normal(self, v=0x1):
        self._ser.write('s')
        self._write_elem(v)

        #while True:
        #    print(self._ser.read(1)),
        ret = ''
        ret += self._ser.readline()
        #print(ret)
        ret += self._ser.readline()
        #print(ret)
        return ret

class LocalBuffer:
    def __init__(self, size=BUF_LEN):
        self._buf = get_sorted_rand_buf(elem_size=ELEM_SIZE, \
                length=size)

    def checksum(self):
        s = 0
        for b in self._buf:
            s += b
        return s & 0xffffffff

    def get_buf(self):
        return list(self._buf)

def main_loop(ser):
    b = BSearch(cmd_ser=ser)

    for _ in range(100):
        buf = LocalBuffer()

        print(b.cmd_set_search_buf(buf.get_buf())),
        print("[+] done setting buffer")
        print(b.cmd_checksum()),
        print("[+] done checksum")
        print(buf.checksum())
        print(b.cmd_verify()),
        print("[+] done verify")

        in_cnt = 0
        out_cnt = 0
        for cnt in range(10000):
            if cnt % 100 == 0 and cnt != 0:
                #inratio = float('nan')
                inratio = 100.0 * (float(in_cnt)/float(cnt))
                print("searched for %d numbers [in: %02.1f%%]" % (cnt,
                    inratio))
            if random.randrange(0, 2) == 1:
                v = random.randrange(0, 2 ** (8*ELEM_SIZE))
            else:
                v = random.choice(buf.get_buf())
            #v=1
            ret_sn = b.cmd_search_normal(v)
            sn = ret_sn.split('\r\n')[1]
            ##print("[+] done search normal")
            ret_su = b.cmd_search_unrolled(v)
            su = ret_su.split('\r\n')[1]
            print("N:\n|||%s||||U:\n>>>%s<<<" % (ret_sn, ret_su))
            if v not in buf.get_buf():
                #print("OUT")
                out_cnt += 1
            else:
                in_cnt += 1
                #print("IN")

            if sn != su:
                print("diff: %s %s" % (ret_sn, ret_su))
                print("diff2: |%s| vs |%s|" % (sn, su))
                print("diff2: |%s| vs |%s|" % (repr(sn), repr(su)))
                print(to_hex_perl(b.getWriteLog()))
                return
            #print("[+] done search unrolled")
            #print(su)


def main():
    random.seed()

    parser = argparse.ArgumentParser("Binary search test")

    get_arg_parse_default(parser)

    args = parser.parse_args()
    ser = get_io_from_args(args)

    if ser.isOpen():
        ser.flushInput()
        ser.flushOutput()
        ser.flushAPP()
        main_loop(ser)

if __name__ == '__main__':
    main()
