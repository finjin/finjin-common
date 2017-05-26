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


//Implementation----------------------------------------------------------------
ValueOrError<bool> LinuxUtilities::GetProcessFilePath(Path& path)
{
    const int charCount = 4096;
    char buff[charCount];

    ssize_t charsCopied = readlink("/proc/self/exe", buff, charCount - 1);
    if (charsCopied != -1)
    {
        if (path.assign(buff, charsCopied).HasError())
            return ValueOrError<bool>::CreateError();
        return true;
    }

    return false;
}

Path LinuxUtilities::GetProcessFilePath()
{
    Path result;
    GetProcessFilePath(result);
    return result;
}

ValueOrError<bool> LinuxUtilities::GetWorkingDirectory(Path& path)
{
    const int charCount = 4096;
    char buff[charCount];
    if (path.assign(getcwd(buff, charCount - 1)).HasError())
        return ValueOrError<bool>::CreateError();
    return !path.empty();
}

Path LinuxUtilities::GetWorkingDirectory()
{
    Path result;
    GetWorkingDirectory(result);
    return result;
}
