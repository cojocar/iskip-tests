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

from util import get_rand_key, get_rand_buf, to_hex, to_hex_perl
import argparse
from util import get_io_from_args, get_arg_parse_default

from Crypto import Random
from Crypto.Cipher import AES

import random
import hmac
import hashlib
import binascii
import os
import sys
import struct
import time
import numpy

AES_KEY_BYTES = 16
AES_MESSAGE_BYTES = 16
AES_DIGEST_BYTES = 16

class AES128Remote:
    def __init__(self, cmd_ser=None, debug_proto=False):
        self._ser = cmd_ser
        self._debug_proto = debug_proto

        self._old_write = self._ser.write
        self._ser.write = self.ser_write
        self._write_log = []

    def ser_write(self, c):
        self._write_log.append(c)
        return self._old_write(c)


    def getWriteLog(self):
        return self._write_log

    def cmd_set_aes_key(self, key):
        assert(len(key) == AES_KEY_BYTES)

        self._write_cmd_buf(':', key)

        if self._debug_proto:
            key_out = self._read_cmd_buf(expected_len=len(key))
            print("key_out=" + to_hex(key_out))

    def cmd_set_aes_enc_buf(self, buf):
        assert(len(buf) == AES_MESSAGE_BYTES)

        self._write_cmd_buf('*', buf)

        if self._debug_proto:
            buf_out = self._read_cmd_buf(expected_len=len(buf))
            print("buf_out=" + to_hex(key_out))

    def cmd_set_aes_dec_buf(self, buf):
        self._write_cmd_buf('@', buf)

        if self._debug_proto:
            buf_out = self._read_cmd_buf(expected_len=len(buf))
            print("buf_out=" + to_hex(key_out))

    def cmd_inc_aes_enc_buf(self):
        self._ser.write('+')

    def cmd_copy_aes_enc_to_dec_buf(self):
        self._ser.write('%')

    def cmd_encode_aes(self):
        self._ser.write('>')

        # the output buffer is printed
        ret = self._read_cmd_buf(expected_len=AES_MESSAGE_BYTES)
        #print("out_buf=" + to_hex(ret))
        return ret

    def cmd_decode_aes(self):
        self._ser.write('<')

        # the output buffer is printed
        ret = self._read_cmd_buf(expected_len=AES_MESSAGE_BYTES)
        #print("out_buf=" + to_hex(ret))
        return ret

    def cmd_set_aes_multi(self, cnt=10):

        assert(cnt < 256)
        # write command
        self._ser.write('F')
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
        #print("ACK: %d %s" % (d, hex(d)))

    def cmd_encode_aes_multi(self):
        self._ser.write('M')
        self._ser.flush()
        ret = self._read_cmd_buf(expected_len=AES_MESSAGE_BYTES)
        return ret

    def cmd_check_aes(self):
        self._write_cmd_buf('=', buf)

        ret = self._read_cmd_buf(expected_len=AES_MESSAGE_BYTES)
        print("out_buf(c)=" + to_hex(ret))

        l = self._ser.readline()
        print("l=%s" % l)

    def _write_cmd_buf(self, cmd, buf):
        self._ser.write(cmd)
        for b in buf:
            self._ser.write(chr(b))

    def _read_cmd_buf(self, expected_len=None):
        ret = []
        for _ in range(expected_len):
            b = self._ser.read(1)
            ret.append(ord(b))
            #print(b),

        return ret

a = None
def main_loop(ser):
    global a
    a = AES128Remote(cmd_ser=ser)

    for _ in range(100000):
        key = get_rand_key(key_len=AES_KEY_BYTES)
        print("key    =" + to_hex(key))
        a.cmd_set_aes_key(key)

        buf = get_rand_buf(buf_len=AES_MESSAGE_BYTES)
        print("buf    =" + to_hex(buf))
        a.cmd_set_aes_enc_buf(buf)

        aes_encode = a.cmd_encode_aes()
        print("encodeR =" + to_hex(aes_encode))

        pyaes = AES.AESCipher(''.join([chr(k) for k in key]), AES.MODE_ECB)
        r = pyaes.encrypt(''.join([chr(b) for b in buf]))
        pyaes_s = binascii.hexlify(bytearray(r)).decode('utf-8')
        print("encodeP =" + pyaes_s)

        if pyaes_s != to_hex(aes_encode):
            print("diff: %s != %s" % (pyaes_s, to_hex(aes_encode)))
            print(repr(a.getWriteLog()))
            print(to_hex_perl(a.getWriteLog()))
            break

def main_loop_timing(ser, rounds=200):
    global a
    a = AES128Remote(cmd_ser=ser)
    FACTOR = 100

    print("Doing %d rounds of aes" % (FACTOR*rounds))

    timmings = []
    for cnt in range(100000):
        key = get_rand_key(key_len=AES_KEY_BYTES)
        #key = [0x41] * AES_KEY_BYTES
        #print("key    =" + to_hex(key))
        a.cmd_set_aes_key(key)

        buf = get_rand_buf(buf_len=AES_MESSAGE_BYTES)
        #buf = [0x41] * AES_MESSAGE_BYTES
        #print("buf    =" + to_hex(buf))
        a.cmd_set_aes_enc_buf(buf)

        a.cmd_set_aes_multi(rounds)

        start = time.time()
        aes_encode = a.cmd_encode_aes_multi()
        end = time.time()
        #print("encode =" + to_hex(aes_encode))

        timmings.append(1000.0 * float(end-start)) #ms
        RND = rounds

        CNT_T = 1
        if CNT_T != 0 and cnt % CNT_T == 0:
            npa = numpy.array(timmings)
            print("avg=%2.5f avg_=%2.5f ms, stdev=%2.9f, f=%d, s=%d, l=%d"% (
                numpy.average(npa),
                numpy.average(npa)/(float(FACTOR) * float(RND)),
                numpy.std(npa), FACTOR*RND, CNT_T,
                len(npa)))

        pyaes = AES.AESCipher(''.join([chr(k) for k in key]), AES.MODE_ECB)
        r = pyaes.encrypt(''.join([chr(b) for b in buf]))
        pyaes_s = binascii.hexlify(bytearray(r)).decode('utf-8')
        #print("encodeB =" + to_hex(aes_encode))
        #print("encodeP =" + pyaes_s)

        if pyaes_s != to_hex(aes_encode):
            print("diff: %s != %s" % (pyaes_s, to_hex(aes_encode)))
            print(repr(a.getWriteLog()))
            print(to_hex_perl(a.getWriteLog()))
            break

def main():
    random.seed()

    parser = argparse.ArgumentParser("AES test")

    get_arg_parse_default(parser)

    parser.add_argument('--multi-step', metavar='N', type=int,
            help='How many aes rounds (*100). Default=100.',
            default=100)

    args = parser.parse_args()
    ser = get_io_from_args(args)

    global a
    try:
        if ser.isOpen():
            ser.flushInput()
            ser.flushOutput()
            ser.flushAPP()
            if args.multi_step <= 1:
                main_loop(ser)
            else:
                main_loop_timing(ser, args.multi_step)
    except KeyboardInterrupt:
        print("^C pressed")
        should_dump = raw_input("Do you want me to dump the payload"
        " [y/n] (default: n): ")
        if should_dump == 'y':
            print(repr(a.getWriteLog()))
            print(to_hex_perl(a.getWriteLog()))


if __name__ == '__main__':
    main()
