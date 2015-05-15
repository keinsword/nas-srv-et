#!/bin/bash

gcc -g3 nas-srv-et-app/client-app/src/client.c \
nas-srv-et-app/client-app/src/client-func.c \
nas-srv-et-lib/src/common-func.c \
nas-srv-et-lib/src/crc.c \
nas-srv-et-lib/src/protocol.c \
nas-srv-et-lib/src/errors.c \
-o client

