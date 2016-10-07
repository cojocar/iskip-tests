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
import hmac
import hashlib
import sys
import os
import time
import numpy
import struct
import argparse

from util import get_io_from_args, get_arg_parse_default
from util import get_rand_key, get_rand_buf, to_hex, to_hex_perl

HMAC_KEY_BYTES = 32
HMAC_MESSAGE_BYTES = 32
HMAC_DIGEST_BYTES = 32

class HMACRemote:
    def __init__(self, cmd_ser=None):
        self._ser = cmd_ser

        self._old_write = self._ser.write
        self._ser.write = self.ser_write
        self._write_log = []

    def ser_write(self, c):
        self._write_log.append(c)
        return self._old_write(c)

    def getWriteLog(self):
        return self._write_log

    def cmd_set_hmac_key(self, key):
        assert(len(key) == HMAC_KEY_BYTES)

        ### new protocol
        self._ser.write(chr(0x3a))
        self._ser.flush()

        for k in key:
            self._ser.write(chr(k))
            self._ser.flush()

        #while True:
        #    k = self._ser.read(1)
        #    print(repr(k))

        ##### new protocol
        #####key_out = []
        ######cnt = 0
        #####for _ in key:
        #####    k = self._ser.read(1)
        #####    #print('cnt=%d [%s]' % (cnt, repr(k)))
        #####    key_out.append(ord(k))
        #####    #cnt += 1
        #####    #if cnt == 32:
        #####    #    break

        #print(repr(key))
        #print("key_out=" + to_hex(key_out))

    def cmd_hmac_sw(self, buf):
        assert(len(buf) == HMAC_MESSAGE_BYTES)
        self._ser.write(chr(0x3b))
        self._ser.flush()

        for b in buf:
            self._ser.write(chr(b))
            self._ser.flush()

        self.cmd_hmac_start()

        ret = []
        for _ in range(HMAC_DIGEST_BYTES):
            b = self._ser.read(1)
            ret.append(ord(b))
            #print(repr(b))

        return ret

    def cmd_hmac_start(self):
        self._ser.write(chr(0x3c))
        self._ser.flush()

    def cmd_hmac_sw_set_multi(self, cnt = 10):
        # set FACTOR * cnt (FACTOR is defined in hmac/main.c) as the number of
        # hmac rounds
        assert(cnt < 256)

        # write command
        self._ser.write('^')
        self._ser.flush()

        # write count
        self._ser.write(chr(cnt))
        self._ser.flush()

        bbs = ''
        # read count (4 bytes)
        for _ in range(4):
            b = self._ser.read(1)
            #print(repr(b))
            bbs += b
            #print(repr(bbs))
        #print(len(bbs))
        d = struct.unpack('<I', bbs)[0]
        #print("ACK: %d" % d)
        #print("%s" % hex(d))

    def cmd_hmac_sw_start_multi(self, buf):
        assert(len(buf) == HMAC_MESSAGE_BYTES)
        self._ser.write('>')
        self._ser.flush()

        for b in buf:
            self._ser.write(chr(b))
            self._ser.flush()

        ret = []
        for _ in range(HMAC_DIGEST_BYTES):
            b = self._ser.read(1)
            #print(repr(b))
            ret.append(ord(b))

        return ret

def main_loop(ser):
    h = HMACRemote(cmd_ser=ser)

    print("Doing one round of hmac")
    timmings = []
    for cnt in range(100000):
        key = get_rand_key(key_len=HMAC_KEY_BYTES)
        #key = [ord('A')] * HMAC_KEY_BYTES
        #print key
        print("key    =" + to_hex(key))
        h.cmd_set_hmac_key(key)

        buf = get_rand_buf(buf_len=HMAC_MESSAGE_BYTES)
        #buf = [ord('A')] * HMAC_MESSAGE_BYTES
        print("buf    =" + to_hex(buf))

        start = time.time()
        hmac_sig = h.cmd_hmac_sw(buf)
        end = time.time()


        timmings.append(float(end-start))
        CNT_T = 1
        if cnt % CNT_T == 0:
            npa = numpy.array(timmings)
            print("avg=%2.5f ms, stdev=%2.9f"% (numpy.average(npa) *
                float(1000.0), numpy.std(npa)))
            timmings = []
        print("hmac   =" + to_hex(hmac_sig))

        d = hmac.new(\
                ''.join([chr(k) for k in key]), \
                ''.join([chr(b) for b in buf]), \
                hashlib.sha256)
        pyhmac = d.hexdigest()
        #print("pyhmac =" + to_hex(d.digest()))
        #print("pyhmac =" + d.hexdigest())
        #print("pyhmac =" + to_hex(py_hmac))
        if pyhmac != to_hex(hmac_sig):
            print("diff: %s != %s" % (pyhmac, hmac_sig))
            print(to_hex_perl(h.getWriteLog()))
            break

def main_loop_timing(ser, rounds=200):
    h = HMACRemote(cmd_ser=ser)

    FACTOR = 100
    print("Doing %d rounds of hmacs" % (FACTOR*rounds))

    timmings = []
    for cnt in range(100000):
        key = get_rand_key(key_len=HMAC_KEY_BYTES)
        #print("key    =" + to_hex(key))
        h.cmd_set_hmac_key(key)

        buf = get_rand_buf(buf_len=HMAC_MESSAGE_BYTES)
        #print("buf    =" + to_hex(buf))

        #RND = 10
        #RND = 200
        #RND = 50
        #RND = 10 # use a smaller value for the emulator
        #RND = 2
        #RND = 200
        RND = rounds

        h.cmd_hmac_sw_set_multi(RND)

        start = time.time()
        hmac_sig = h.cmd_hmac_sw_start_multi(buf)
        end = time.time()

        timmings.append(1000.0 * float(end-start)) #ms
        CNT_T = 1
        if CNT_T != 0 and cnt % CNT_T == 0:
            npa = numpy.array(timmings)
            print("avg=%2.5f avg_=%2.5f ms, stdev=%2.9f, f=%d, s=%d, l=%d"% (
                numpy.average(npa),
                numpy.average(npa)/(float(FACTOR) * float(RND)),
                numpy.std(npa), FACTOR*RND, CNT_T,
                len(npa)))
            #timmings = []
        #print("hmac   =" + to_hex(hmac_sig))

        d = hmac.new(\
                ''.join([chr(k) for k in key]), \
                ''.join([chr(b) for b in buf]), \
                hashlib.sha256)
        pyhmac = d.hexdigest()
        #print("pyhmac =" + to_hex(d.digest()))
        #print("pyhmac =" + d.hexdigest())
        #print("pyhmac =" + to_hex(py_hmac))
        if pyhmac != to_hex(hmac_sig):
            print("diff: %s != %s" % (pyhmac, to_hex(hmac_sig)))
            s = ''
            for c in hmac_sig:
                s += chr(c)
            print("str=%s" % s)
            print(repr(h.getWriteLog()))
            print(to_hex_perl(h.getWriteLog()))
            break

def main():
    random.seed()

    parser = argparse.ArgumentParser("HMAC test")
    get_arg_parse_default(parser)

    parser.add_argument('--multi-step', metavar='N', type=int,
            help='How many hmac rounds (*100). Default=200.',
            default=200)

    args = parser.parse_args()
    ser = get_io_from_args(args)

    if ser.isOpen():
        ser.flushInput()
        ser.flushOutput()
        ser.flushAPP()
        if args.multi_step <= 1:
            main_loop(ser)
        else:
            main_loop_timing(ser, args.multi_step)

if __name__ == '__main__':
    main()
