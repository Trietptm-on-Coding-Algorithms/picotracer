## picotracer

`Pintool` that traces API calls and Syscalls.

### Building

Just type `make` assuming your `Pin` instalation directory is `/opt/pin`. If it is not, adjust the `Makefile`.

Note: This `Pintool` was only tested with `Pin` versions 3.13-98189 and 3.6-97554 on **Linux x64**.

### Running

```
$ ./pin -t /path/to/picotracer/obj-intel64/picotracer.so -s -- /bin/hostname
whatever

Address             IP                  API
----------------------------------------------------------------------
0x00007f0a1a4c2ab0  0x0000564512c666a4  __libc_start_main
0x0000564512c660e8  0x0000564512c670ec  _init
0x0000564512c66780  0x0000564512c67109
0x0000564512c66300  0x0000564512c663c5  rindex@plt
0x0000564512c66230  0x0000564512c66407  strcmp@plt
0x0000564512c66230  0x0000564512c6653b  strcmp@plt
0x0000564512c66230  0x0000564512c66552  strcmp@plt
0x0000564512c661b0  0x0000564512c6643f  getopt_long@plt
0x0000564512c66bb0  0x0000564512c66634
0x0000564512c668b0  0x0000564512c66cd2
0x0000564512c66130  0x0000564512c668be  __errno_location@plt
0x0000564512c66290  0x0000564512c66923  malloc@plt
0x0000564512c66320  0x0000564512c668ec  gethostname@plt
0x0000564512c66200  0x0000564512c66900  memchr@plt
0x0000564512c66140  0x0000564512c66cda  puts@plt
0x0000564512c66390  0x0000564512c6675e  .plt.got
0x0000564512c666b0  0x0000564512c66763

Syscall      Stats
----------------------------------------------------------------------
 0           1 (3.57%)
 1           1 (3.57%)
 3           2 (7.14%)
 5           3 (10.71%)
 9           5 (17.86%)
 10          4 (14.29%)
 11          1 (3.57%)
 12          3 (10.71%)
 21          3 (10.71%)
 63          1 (3.57%)
 158         1 (3.57%)
 231         1 (3.57%)
 257         2 (7.14%)
```

**Warning**: `Pin` will try to find symbols in a multitude of ways, in case it fails the API name returned is a 'blank' string.

