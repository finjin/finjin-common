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
#include "finjin/common/AppleUtilities.hpp"
#include <unistd.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSProcessInfo.h>

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
Path AppleUtilities::GetProcessFilePath()
{
    //For more platforms, see http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe

    auto arguments = [NSProcessInfo processInfo].arguments;
    NSString* exe = [arguments objectAtIndex:0];
    return exe.UTF8String;
}

Path AppleUtilities::GetWorkingDirectory()
{
    const int charCount = 4096;
    char buff[charCount];
    return getcwd(buff, charCount - 1);
}

size_t AppleUtilities::GetVolumeIDHash(const Path& path)
{
    auto nsstrPath = [NSString stringWithUTF8String:path.c_str()];
    auto pathUrl = [NSURL fileURLWithPath:nsstrPath];
    
    id<NSCopying, NSCoding, NSObject> volId = nil;
    [pathUrl getResourceValue:&volId forKey:NSURLVolumeIdentifierKey error:nil];
    if (volId != nil)
        return volId.hash;
    else
        return 0;
}

void AppleUtilities::GetCommandLineArgs(CommandLineArgsProcessor& argsProcessor)
{
    NSArray* args = [NSProcessInfo processInfo].arguments;
    
    argsProcessor.Create(static_cast<size_t>(args.count - 1));
    
    for (NSUInteger i = 1; i < args.count; i++)
    {
        NSString* arg = (NSString*)args[i];
        argsProcessor[static_cast<size_t>(i - 1)] = arg.UTF8String;
    }
}
