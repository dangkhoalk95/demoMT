#!/usr/bin/env python

import sys
import os
import argparse
import re

BL_START_ADDR_PATTERN = 'define symbol __ICFEDIT_region_BL_start__ = (\S+);'
ROM_START_ADDR_PATTERN = 'define symbol __ICFEDIT_region_ROM_start__ = (\S+);'
PWR_START_ADDR_PATTERN = 'define symbol __ICFEDIT_region_PWR_ROM_start__ = (\S+);'
RTOS_bin_name = 'rtos.bin'
PWR_bin_name = 'mt768x_default_PerRate_TxPwr.bin'
BUF_BLOCK = (1024 * 8)

def parse_icf_file(flash_icf):
    b_BL_base_found = False
    b_ROM_base_found = False
    b_PWR_base_found = False
    PWR_base = '0x0'
    for line in file(flash_icf):
        if not b_BL_base_found:
            ret = re.search(BL_START_ADDR_PATTERN, line)
            if ret is not None:
                BL_base = ret.group(1)
                b_BL_base_found = True
                continue
        if not b_ROM_base_found:
            ret = re.search(ROM_START_ADDR_PATTERN, line)
            if ret is not None:
                ROM_base = ret.group(1)
                b_ROM_base_found = True
                continue
        if not b_PWR_base_found:
            ret = re.search(PWR_START_ADDR_PATTERN, line)
            if ret is not None:
                PWR_base = ret.group(1)
                b_PWR_base_found = True
                continue
        if b_BL_base_found and b_ROM_base_found and b_PWR_base_found:
            break
    if (not b_BL_base_found) or (not b_ROM_base_found):
        print 'Invalid icf file, cannot find BL_BASE, ROM_BASE.'
        sys.exit(-1)
    print '\r\nFlash layout setting:'
    print 'BL_base:\t\t' + BL_base + '\r\nROM_base:\t\t' + ROM_base + '\r\nPWR_BASE:\t\t' + PWR_base + '\r\n'
    return int(BL_base, 16), int(ROM_base, 16), int(PWR_base, 16)

def is_padding(raw_data):
    for i in raw_data:
        if ord(i) != 0x00:
            return False
    return True

def extract_bin(BL_base, ROM_base, PWR_base, iar_image):
    rtos_seek = ROM_base - BL_base
    f_in = open(iar_image, 'rb+')
    f_rtos = open(RTOS_bin_name, 'wb+')
    f_in.seek(rtos_seek)
    if PWR_base != 0:
        if (PWR_base - BL_base) > os.path.getsize(iar_image):
            print 'Wrong, iar image size is smaller than PWR binary offset'
            sys.exit(-1)
        f_pwr = open(PWR_bin_name, 'wb+')
        while True:
            raw_data = f_in.read(BUF_BLOCK)
            if is_padding(raw_data):
                break;
            else:
                f_rtos.write(raw_data)
        f_in.seek(PWR_base - BL_base)
        raw_data = f_in.read()
        f_pwr.write(raw_data)
        f_pwr.close()
        print 'Extract ' + RTOS_bin_name + ' and ' + PWR_bin_name
    else:
        if (ROM_base - BL_base) > os.path.getsize(iar_image):
            print 'Wrong, iar image size is smaller than ROM binary offset'
            sys.exit(-1)
        raw_data = f_in.read()
        f_rtos.write(raw_data)
        print 'Extract ' + RTOS_bin_name
    f_in.close()
    f_rtos.close()

if __name__=='__main__':
    parser = argparse.ArgumentParser(description='Extract RTOS binary from IAR image')
    parser.add_argument('iar_image', help='path to your iar image')
    parser.add_argument('flash_icf', help='path to flash layout setting file of your IAR project')
    setting = parser.parse_args()
    print 'iar image:\t\t%s' % setting.iar_image
    print 'iar layout setting:\t%s' % setting.flash_icf

    BL_base, ROM_base, PWR_base = parse_icf_file(setting.flash_icf)
    extract_bin(BL_base, ROM_base, PWR_base, setting.iar_image)

    sys.exit(0)
