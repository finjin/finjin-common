#!/usr/bin/env python3

#Copyright (c) 2017 Finjin
#
#This file is part of Finjin Common (finjin-common).
#
#Finjin Common is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#This Source Code Form is subject to the terms of the Mozilla Public
#License, v. 2.0. If a copy of the MPL was not distributed with this
#file, You can obtain one at http://mozilla.org/MPL/2.0/.


"""
Usage: wrap_file.py [wrap | wrap-big | wrap-little | unwrap] in_file out_file

This tool:
1)Wraps a file, adding a header, and writes the header + original content to a new file:
  wrap_file.py wrap input_file.txt wrapped_file.wrappedtext
2)Unwraps a file, reading header, and writes the original content a new file:
  wrap_file.py unwrap wrapped_file.wrappedtext original_file
"""


#Imports-----------------------------------------------------------------------
import os
import sys
import struct


#Values------------------------------------------------------------------------
FINJIN_SIGNATURE_BYTES = bytes('finj', 'utf-8')
FINJIN_SIGNATURE = struct.unpack('I', process_bytes(FINJIN_SIGNATURE_BYTES, False))[0]
FINJIN_SIGNATURE_REVERSED = struct.unpack('I', process_bytes(FINJIN_SIGNATURE_BYTES, True))[0]

EMBEDDED_FILE_FORMAT = 0

GENERIC_FILE_FORMAT_CLASS = 0
IMAGE_FILE_FORMAT_CLASS = 1
SOUND_FILE_FORMAT_CLASS = 2


#Functions---------------------------------------------------------------------
def process_bytes(the_bytes, swap_bytes):
    if swap_bytes:
        the_bytes = bytearray(reversed(the_bytes))
    return the_bytes

def is_valid_format_class(format_class):
    return format_class in [GENERIC_FILE_FORMAT_CLASS, IMAGE_FILE_FORMAT_CLASS, SOUND_FILE_FORMAT_CLASS]

def read_binary_file(filename):
    f = open(filename, 'rb')
    file_bytes = f.read()
    f.close()
    return file_bytes

def get_file_format_class(ext):
    if ext in ['astc', 'bmp', 'dds', 'jpg', 'jpeg', 'png', 'pvr', 'pkm', 'ktx', 'tga']:
        return IMAGE_FILE_FORMAT_CLASS
    elif ext in ['ogg', 'wav']:
        return SOUND_FILE_FORMAT_CLASS
    else:
        return GENERIC_FILE_FORMAT_CLASS

def wrap_embedded_file(in_file_path, swap_bytes, out_file_path):
    in_file_name, in_file_extension = os.path.splitext(in_file_path)

    #Remove leading dot from extension
    if not in_file_extension:
        raise Exception('Input file must have an extension')
    in_file_extension = in_file_extension[1:]

    #Read input file----------------------
    file_bytes = read_binary_file(in_file_path)

    #Write output file----------------------
    f = open(out_file_path, 'wb')

    #Uint32: Finjin signature
    f.write(process_bytes(struct.pack('I', FINJIN_SIGNATURE), swap_bytes))

    #Uint32: Format
    f.write(process_bytes(struct.pack('I', EMBEDDED_FILE_FORMAT), swap_bytes))

    #Uint32: Format version
    f.write(process_bytes(struct.pack('I', 1), swap_bytes))

    #Uint32: File format class
    file_format_class = get_file_format_class(in_file_extension)
    f.write(process_bytes(struct.pack('I', file_format_class), swap_bytes))

    #Uint32: File format class version
    f.write(process_bytes(struct.pack('I', 1), swap_bytes))

    #Uint32: File extension length
    in_file_extension_utf8 = bytes(in_file_extension, 'utf-8')
    f.write(process_bytes(struct.pack('I', len(in_file_extension_utf8)), swap_bytes))

    #UTF-8[File extension length]: File extension (without leading dot)
    f.write(in_file_extension_utf8)

    #Uint64: Embedded file length
    f.write(process_bytes(struct.pack('Q', len(file_bytes)), swap_bytes))

    #Byte[Embedded file length]: Embedded file
    f.write(file_bytes)

    f.close()

def unwrap_embedded_file(in_file_path, out_file_path):
    #Read input file---------------------------
    f = open(in_file_path, 'rb')

    #Uint32: Finjin signature
    signature = struct.unpack('I', f.read(4))[0]
    swap_bytes = False
    if signature != FINJIN_SIGNATURE:
        if signature == FINJIN_SIGNATURE_REVERSED:
            swap_bytes = True
        else:
            raise Exception('Invalid signature', signature)

    #Uint32: Format
    file_format = struct.unpack('I', process_bytes(f.read(4), swap_bytes))[0]
    if file_format != EMBEDDED_FILE_FORMAT:
        raise Exception('Invalid file format', file_format)

    #Uint32: Format version
    file_format_version = struct.unpack('I', process_bytes(f.read(4), swap_bytes))[0]
    if file_format_version == 0:
        raise Exception('Invalid file format version', file_format_version)

    #Uint32: File format class
    file_format_class = struct.unpack('I', process_bytes(f.read(4), swap_bytes))[0]
    if not is_valid_format_class(file_format_class):
        raise Exception('Invalid file format class', file_format_class)

    #Uint32: File format class version
    file_format_class_version = struct.unpack('I', process_bytes(f.read(4), swap_bytes))[0]
    if file_format_class_version == 0:
        raise Exception('Invalid file format class version', file_format_class_version)

    #Uint32: File extension length
    extension_utf8_length = struct.unpack('I', process_bytes(f.read(4), swap_bytes))[0]
    if extension_utf8_length == 0:
        raise Exception('Invalid file extension length', extension_utf8_length)

    #UTF-8[File extension length]: File extension (without leading dot)
    extension_utf8 = f.read(extension_utf8_length)
    extension = extension_utf8.decode('utf-8', 'strict')

    #Uint64: Embedded file length
    embedded_file_byte_count = struct.unpack('Q', process_bytes(f.read(8), swap_bytes))[0]

    #Byte[Embedded file length]: Embedded file
    embedded_file_bytes = f.read(embedded_file_byte_count)

    f.close()

    #Write output file--------------------------
    f = open(out_file_path + '.' + extension, 'wb')
    f.write(embedded_file_bytes)
    f.close()


#Main--------------------------------------------------------------------------
if __name__== '__main__':
    if len(sys.argv) < 4:
        print('Usage: python wrap_file.py [wrap | wrap-big | wrap-little | unwrap] in_file out_file')
        print('Example: python wrap_file.py wrap in-image.png out-image.texture')
        print('Example: python wrap_file.py unwrap in-image.texture out-image (extension will be appended automatically)')
        sys.exit(1)

    mode = sys.argv[1]
    in_file_path = sys.argv[2]
    out_file_path = sys.argv[3]

    if mode == 'wrap':
        wrap_embedded_file(in_file_path, False, out_file_path)
    elif mode == 'wrap-big':
        wrap_embedded_file(in_file_path, sys.byteorder != 'big', out_file_path)
    elif mode == 'wrap-little':
        wrap_embedded_file(in_file_path, sys.byteorder != 'little', out_file_path)
    elif mode == 'unwrap':
        unwrap_embedded_file(in_file_path, out_file_path)
