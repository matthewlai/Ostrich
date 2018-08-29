#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <bin file>"
    exit 1
fi

dfu-util -a 0 -s 0x08000000:leave -R -D $1
