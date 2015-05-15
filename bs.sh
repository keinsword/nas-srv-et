#!/bin/bash

gcc -g3 nas-srv-et-app/server-app/src/server.c \
nas-srv-et-app/server-app/src/server-func.c \
nas-srv-et-app/server-app/src/netns-func.c \
nas-srv-et-lib/src/common-func.c \
nas-srv-et-lib/src/crc.c \
nas-srv-et-lib/src/protocol.c \
nas-srv-et-lib/src/errors.c \
-lconfig -o server

