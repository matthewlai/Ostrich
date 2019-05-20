#!/bin/bash

sudo openocd -f interface/stlink-v2-1.cfg -f $OSTRICH_PATH/scripts/stm32f7x.cfg
