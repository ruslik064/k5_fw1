#!/usr/bin/env python3

# Copyright (c) 2025 muzkr
#
#   https://github.com/muzkr
#
# Licensed under the MIT License (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://mit-license.org/
#
#     Unless required by applicable law or agreed to in writing, software
#     distributed under the License is distributed on an "AS IS" BASIS,
#     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#     See the License for the specific language governing permissions and
#     limitations under the License.
#


import argparse
import serial
import signal
from time import sleep

import _prog as pp


def load_image(file: str) -> bytes:

    a = bytearray()
    with open(file, "rb") as fd:
        buf = bytearray(512)
        while True:
            len1 = fd.readinto(buf)
            if len1 > 0:
                a.extend(memoryview(buf)[:len1])
            if len1 < len(buf):
                break

    return a


def main():

    # Usage: serialtool.py --port /dev/ttyUSB0 --bl-ver 1.01 file
    ap = argparse.ArgumentParser(description="UV-K5 V2 serial tool")
    ap.add_argument(
        "--port", "-p", help="serial port, eg., '/dev/ttyUSB0'", required=True
    )
    ap.add_argument(
        "--bl-ver",
        help="bootloader version, eg. '1.01'. Max 4 characters. Default '?'",
        required=False,
        default="?",
    )
    ap.add_argument("file", help="firmware image file")
    args = ap.parse_args()

    port: str = args.port
    bl_ver: str = args.bl_ver
    fw_file: str = args.file

    try:
        fw_image = load_image(fw_file)
        if 0 == len(fw_image):
            print("Invalid firmware image: {}: empty file".format(fw_file))
            return
    except Exception as e:
        print("Cannot load firmware image '{}': {}".format(fw_file, e))
        return

    if len(bl_ver) > 4:
        print("Invalid bootloader version '{}': more than 4 characters".format(bl_ver))
        return

    try:
        ser = serial.Serial(port, baudrate=38400, timeout=0.0001, write_timeout=None)
    except Exception as e:
        print("Cannot open port '{}': {}".format(port, e))
        return

    print("UV-K5 V2 serial tool")
    print("Press Ctrl-C to quit")
    print("Firmware image loaded: {}, size = {}".format(fw_file, len(fw_image)))

    quit_flag = False

    def quit_handler(sig, frame):
        nonlocal quit_flag
        quit_flag = True

    signal.signal(signal.SIGINT, quit_handler)

    prog = pp.Programmer(ser, fw_image, bl_ver)

    while (not quit_flag) and prog.loop():
        sleep(0)

    print("Quit")
    ser.close()


if __name__ == "__main__":
    main()
