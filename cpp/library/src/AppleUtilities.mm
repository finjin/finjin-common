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
#define FINJIN_APPLE_OBJCPP_UTILITIES 1
#include "finjin/common/AppleUtilities.hpp"
#include <unistd.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSProcessInfo.h>
#import <Foundation/NSString.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSThread.h>

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
ValueOrError<void> AppleUtilities::GetProcessFilePath(Path& path)
{
    //For more platforms, see http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe

    auto arguments = [NSProcessInfo processInfo].arguments;
    NSString* exe = [arguments objectAtIndex:0];
    return path.assign(exe.UTF8String);
}

ValueOrError<void> AppleUtilities::GetWorkingDirectory(Path& path)
{
    const int charCount = 4096;
    char buff[charCount];
    return path.assign(getcwd(buff, charCount - 1));
}

ValueOrError<void> AppleUtilities::GetSystemDirectory(Path& path, bool& isSystemCreated, NSSearchPathDirectory which, bool tryCreateIfNotPresent)
{
    path.clear();
    isSystemCreated = false;

    auto nsuserPath = [NSSearchPathForDirectoriesInDomains(which, NSUserDomainMask, YES) lastObject];
    if (nsuserPath != nullptr)
    {
        auto fileManager = [NSFileManager defaultManager];
        auto existed = [fileManager fileExistsAtPath:nsuserPath];
        auto created = false;
        if (!existed && tryCreateIfNotPresent)
        {
            created = [fileManager createDirectoryAtPath:nsuserPath withIntermediateDirectories:YES attributes:nullptr error:nullptr];
            if (!created)
            {
                //Creation failed
                return ValueOrError<void>();
            }
        }

        if (existed || created)
        {
            if (path.assign(nsuserPath.UTF8String).HasError())
                return ValueOrError<void>::CreateError();
            path.PlatformNormalize();
            isSystemCreated = !tryCreateIfNotPresent && !created;
        }
    }

    return ValueOrError<void>();
}

size_t AppleUtilities::GetVolumeIDHash(const Path& path)
{
    auto nsstrPath = [NSString stringWithUTF8String:path.c_str()];
    auto pathUrl = [NSURL fileURLWithPath:nsstrPath];

    id<NSCopying, NSCoding, NSObject> volId = nullptr;
    [pathUrl getResourceValue:&volId forKey:NSURLVolumeIdentifierKey error:nullptr];
    if (volId != nullptr)
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

void AppleUtilities::SetApplicationMultithreaded()
{
    //From https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmAutoreleasePools.html
    //  If you create secondary threads using the POSIX thread APIs instead of NSThread,
    //  you cannot use Cocoa unless Cocoa is in multithreading mode. Cocoa enters multithreading
    //  mode only after detaching its first NSThread object. To use Cocoa on secondary POSIX threads,
    //  your application must first detach at least one NSThread object, which can immediately exit.
    //  You can test whether Cocoa is in multithreading mode with the NSThread class method isMultiThreaded.
    [[NSThread new] start];
}
