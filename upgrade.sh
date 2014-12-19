#!/usr/bin/env sh

if [ $# == 0 ]; then
    echo "wrong # of args"
    exit 1
fi

fname="$1/build/$1.bin"
if [ -f $fname ]; then
    crc=`crc32 $fname`
    tftp 192.168.0.1 -m binary -c put $fname $crc.bin
fi
