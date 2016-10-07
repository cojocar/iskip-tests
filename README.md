`IskipTests` are a set of tests aimed to test the correctness and the
performance of the compiled code.

# Requirements

A working build environment is needed as well a arm-none-eabi\* toolchain.

Run the script `./setup.sh`. The script will pull in the required dependencies

# Crosscompiling IskipTests (as a library)

By default, libraries are produced in the `build` directory.
```sh
CC="clang -target arm-none-eabi -mcpu=cortex-m3 -mfloat-abi=soft -mthumb" make
```

# Compiling IskipTests as native elfs

Different compilers are supported.
```sh
CC="clang" make all-elfs
```
# Testing the tests

The directory [tests](./tests) provides testing from `IskipTests`.

```sh
$ make all-elf
$ mkfifo pipe.{in,out}
$ ./build/aes-test.elf < pipe.in > pipe.out
```

Launch the test from a different shell.

```sh
$ cd ./tests/
$ ./test_aes.py --pipe-emu ../pipe --multi-step 100
Piped communcation: ../pipe
open_out: ../pipe.in
open_in: ../pipe.out
Doing 10000 rounds of aes
avg=53.85995 avg_=0.00539 ms, stdev=0.000000000, f=10000, s=1, l=1
avg=48.39599 avg_=0.00484 ms, stdev=5.463957787, f=10000, s=1, l=2
avg=46.55639 avg_=0.00466 ms, stdev=5.164442359, f=10000, s=1, l=3
avg=45.66729 avg_=0.00457 ms, stdev=4.730233510, f=10000, s=1, l=4
avg=45.11480 avg_=0.00451 ms, stdev=4.372763021, f=10000, s=1, l=5
^C^C pressed
Do you want me to dump the payload [y/n] (default: n):
```

Optionally, the communication is dumped as a hex string.

Alternatively, the test python scripts can communication with a serial port by
using the `--serial-dev` switch.

# Disclaimer

Please note that although some of the authors are (or were) employed by Google,
this is not an official Google product.
