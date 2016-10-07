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


import os

elem_type = 'uint8_t'
if 'BSEARCH_ELEM_TYPE' in os.environ:
    elem_type = os.environ['BSEARCH_ELEM_TYPE']

def generate_bsearch(t=0, level=1):
    if level == 0:
        return ''
    s = ''
    s += '\t'*t + '/* start lvl=%s */\n' % (level)
    s += '\t'*t + 'm = (l+r)/2;\n'
    s += '\t'*t     + 'if (a[m] == x) {\n'
    s += '\t'*(t+1) +  'MY_PRINTS_FOUND("O");\n'
    s += '\t'*(t)   + '} else if (a[m] < x) {\n'
    s += '\t'*(t+1) +  'l = m+1;\n'
    s += '\t'*(t+1) +  'MY_PRINTS("R");\n'
    s += '\t'*(t+1) +  'CHECK;\n'
    s += generate_bsearch(t+1, level-1)
    s += '\t'*(t) +  '} else {\n'
    s += '\t'*(t+1) +  'r = m-1;\n'
    s += '\t'*(t+1) +  'MY_PRINTS("L");\n'
    s += '\t'*(t+1) +  'CHECK;\n'
    s += generate_bsearch(t+1, level-1)
    s += '\t'*(t) +  '}\n'
    s += '\t'*t + '/* end lvl=%s */\n' % (level)
    return s

def generate_prolog(t=0):
    s = '/* Generated code */\n'
    s += '\t'*t + '#include "bsearch.h"\n'
    s += '\t'*t + '#include "io.h"\n'
    s += '\n'
    s += '\t'*t + '#include <stdint.h>\n'
    s += '\n'
    s += '\t'*t + '#define CHECK do { if (l > r || r >= n) goto __not_found; } while (0)\n'
    s += '\t'*t + '/* prolog start */\n'
    s += '\t'*t + 'uint8_t bsearch_unrolled(%s a[], uint32_t n, %s x)\n' % (elem_type, elem_type)
    s += '\t'*t + '{\n'
    s += '\t'*(t+1) + 'int32_t m, l, r;\n'
    s += '\t'*(t+1) + 'l = 0;\n'
    s += '\t'*(t+1) + 'r = n-1;\n'
    s += '\t'*(t+0) + '/* prolog end */\n'
    return s

def generate_epilog(t=0):
    s =  '\t'*t + '/* epilog start */\n'
    s += '\t'*(t+1) + 'return 0;\n'
    s += '\t'*(t+1) +  'PRINTS("E");\n'
    s += '\t'*(t) + '__not_found:\n'
    s += '\t'*(t+1) +  'MY_PRINTS_NOTFOUND("N");\n'
    s += '\t'*(t+1) + 'return 1;\n'
    s += '\t'*t + '}\n'
    s += '\t'*t + '/* epilog end */\n'
    return s

if __name__ == '__main__':
    print(generate_prolog())
    print(generate_bsearch(t=1, level=10))
    print(generate_epilog())
