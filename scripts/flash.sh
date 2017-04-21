#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <bin file>"
    exit 1
fi

st-flash --reset write $1 0x8000000
