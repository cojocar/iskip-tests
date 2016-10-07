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

import serial
import random

def flushAPP(file_in):
    # wait for banner, APP=
    a = ''
    while True:
        x = file_in.readline()
        #print(repr(x))
        #x = file_in.read(1)
        #a += x
        #print(repr(a))
        if 'APP=' in x:
            break
    return

class MyFakeSerial:
    def __init__(self, pipe_in, pipe_out):
        print("open_out: %s" % pipe_out)
        self._f_out = open(pipe_out, 'w')

        print("open_in: %s" % pipe_in)
        self._f_in = open(pipe_in, 'r')

        self._write_buffer_log = []


    def read(self, l):
        self.flushOutput()
        return self._f_in.read(l)

    def write(self, x):
        self.flushOutput()
        x = self._f_out.write(x)
        self.flushOutput()
        self._write_buffer_log.append(x)
        return x

    def readline(self):
        self.flushOutput()
        return self._f_in.readline()

    def flush(self):
        self._f_out.flush()

    def flushInput(self):
        self._f_in.flush()

    def flushOutput(self):
        self._f_out.flush()

    def flushAPP(self):
        return flushAPP(self._f_in)

    def isOpen(self):
        return True

    def getWriteBufferLog(self):
        return self._write_buffer_log

def get_serial(device):
    ser = serial.Serial()

    ser.port = device
    ser.baudrate = 115200
    ser.bytesize = serial.EIGHTBITS #number of bits per bytes
    ser.parity = serial.PARITY_NONE #set parity check: no parity
    ser.stopbits = serial.STOPBITS_ONE #number of stop bits
    ser.timeout = None          #block read
    #ser.timeout = 1            #non-block read
    #ser.timeout = 2              #timeout block read
    ser.xonxoff = False     #disable software flow control
    ser.rtscts = False     #disable hardware (RTS/CTS) flow control
    ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
    ser.writeTimeout = 2     #timeout for write

    try:
        ser.open()
    except Exception, e:
        print "error opening serial: " + str(e)
        exit()

    #ser.flushAPP = lambda : flushAPP(ser)
    ser.flushAPP = lambda : None
    return ser

def get_pipe(pipe_base):
    return MyFakeSerial(pipe_base + '.out', \
            pipe_base + '.in')

def get_io(device='/dev/ttyUSB0', pipe_base=None):

    if pipe_base is not None:
        print("Piped communcation: %s" % pipe_base)
        return get_pipe(pipe_base)
    else:
        print("Serial communcation: %s" % device)
        return get_serial(device)

def get_io_from_args(args):
    if args.serial_dev is not None:
        return get_io(device=args.serial_dev)
    elif args.pipe_emu is not None:
        return get_io(device=None, pipe_base=args.pipe_emu)
    else:
        # default is serial
        return get_io()

def get_arg_parse_default(parser):
    g = parser.add_mutually_exclusive_group()
    g.add_argument('--serial-dev', help='Use this path for serial communication')
    g.add_argument('--pipe-emu', help='Use this path for the pipe communcation with an emulator')

def get_rand_key(key_len):
    return [random.randrange(0, 256) for _ in range(key_len)]

def get_rand_buf(buf_len):
    return [random.randrange(0, 256) for _ in range(buf_len)]

def to_hex(buf):
    s = ''
    for c in buf:
        s += '%02x' % (c)
    return s

def to_hex_perl(buf):
    s = ''
    for c in buf:
        s += '\\x%02x' % ord(c)
    return s
