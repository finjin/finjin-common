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
Usage: new_source_file.py cpp destination_directory [name1 [name2 ...]]

This tool creates a set of skeleton source files in the specified directory,
creating useful comments to separate the different parts of the files.
"""

#Imports-----------------------------------------------------------------------
import os
import sys


#Functions---------------------------------------------------------------------
def to_utf8(text):
    return text.encode('utf-8')

def write_cpp(output_directory, name):
    with open(os.path.join(output_directory, name) + '.hpp', 'wb') as output_file:
        output_file.write(to_utf8("""
#pragma once


//Includes---------------------------------------------------------------------


//Macros-----------------------------------------------------------------------


//Types------------------------------------------------------------------------
namespace MyOrganization {{ namespace MyProject {{

    class {0}
    {{
    public:
        {0}();

    private:
        int value;
    }};

}} }}
""".format(name)))

    with open(os.path.join(output_directory, name) + '.cpp', 'wb') as output_file:
        output_file.write(to_utf8("""
//Includes---------------------------------------------------------------------
#include "{0}.hpp"

using namespace MyOrganization::MyProject;


//Macros-----------------------------------------------------------------------


//Local functions--------------------------------------------------------------


//Implementation---------------------------------------------------------------
{0}::{0}()
{{
    this->value = 0;
}}
""".format(name)))

def write_python(output_directory, name):
    with open(os.path.join(output_directory, name) + '.py', 'wb') as output_file:
        output_file.write(to_utf8("""#!/usr/bin/env python3


#Imports-----------------------------------------------------------------------
import os
import sys


#Main--------------------------------------------------------------------------
if __name__== '__main__':
    pass
""".format(name)))

LANGUAGE_HANDLERS = {
    'cpp' : write_cpp,
    'python' : write_python
    }


#Main--------------------------------------------------------------------------
if __name__== '__main__':
    arg_count = len(sys.argv)
    if arg_count < 3:
        print('Usage: python new_source_file.py [cpp or python] destination_directory [name1 [name2 [name3 ...]]]')
        print('Example: python new_source_file.py cpp ../output-directory MyClass MyOtherClass')
        sys.exit(1)

    language = sys.argv[1]
    output_directory = sys.argv[2]
    if arg_count == 3:
        name = input('Enter a file name (for example: MyFile): ')
        names = [name.strip(),] if name else None
        manual_entry = True
    else:
        manual_entry = False
        names = sys.argv[3:]

    if names is not None:
        language_handler = LANGUAGE_HANDLERS.get(language, None)
        if language_handler is None:
            print("Unsupported language '{0}' specified.".format(language))
        else:
            for name in names:
                language_handler(output_directory, name)
            if manual_entry:
                print("File(s) created in '{0}'.".format(os.path.abspath(output_directory)))
                input("Press enter key to finish...")
