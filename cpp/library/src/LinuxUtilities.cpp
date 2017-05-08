//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Common (finjin-common).
//
//Finjin Common is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "finjin/common/LinuxUtilities.hpp"
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------

//Implementation----------------------------------------------------------------
Path LinuxUtilities::GetProcessFilePath()
{
    Path result;

    const int charCount = 4096;
    char buff[charCount];

    ssize_t charsCopied = readlink("/proc/self/exe", buff, charCount - 1);
    if (charsCopied != -1)
    {
        buff[charsCopied] = 0;
        result = buff;
    }

    return result;
}

Path LinuxUtilities::GetWorkingDirectory()
{
    const int charCount = 4096;
    char buff[charCount];
    return getcwd(buff, charCount - 1);
}
