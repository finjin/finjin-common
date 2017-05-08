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
#include "finjin/common/CpuFeatures.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
#if FINJIN_ENABLE_SSE
    #if defined(__INTEL_COMPILER)
        //https://software.intel.com/en-us/articles/using-cpuid-to-detect-the-presence-of-sse-41-and-sse-42-instruction-sets
        /*    Copyright 2009 Intel Corporation
        *    sse41andsse42detection.cpp
        *    This file uses code first published by Intel as part of the processor enumeration
        *    article available on the internet at:
        *    http://software.intel.com/en-us/articles/intel-64-architecture-processor-topology-enumeration/
        *    Some of the original code from cpu_topo.c
        *    has been removed, while other code has been added to illustrate the CPUID usage
        *     to determine if the processor supports the SSE 4.1 and SSE 4.2 instruction sets.
        *    The reference code provided in this file is for demonstration purpose only. It assumes
        *    the hardware topology configuration within a coherent domain does not change during
        *    the life of an OS session. If an OS support advanced features that can change
        *    hardware topology configurations, more sophisticated adaptation may be necessary
        *    to account for the hardware configuration change that might have added and reduced
        *      the number of logical processors being managed by the OS.
        *
        *    Users of this code should be aware that the provided code
        *    relies on CPUID instruction providing raw data reflecting the native hardware
        *    configuration. When an application runs inside a virtual machine hosted by a
        *    Virtual Machine Monitor (VMM), any CPUID instructions issued by an app (or a guest OS)
        *    are trapped by the VMM and it is the VMM's responsibility and decision to emulate
        *    CPUID return data to the virtual machines. When deploying topology enumeration code based
        *    on CPUID inside a VM environment, the user must consult with the VMM vendor on how an VMM
        *    will emulate CPUID instruction relating to topology enumeration.
        *
        *    Original code written by Patrick Fay, Ronen Zohar and Shihjong Kuo .
        *     Modified by Garrett Drysdale for current application note.
        */

        typedef struct
        {
            unsigned __int32 EAX, EBX, ECX, EDX;
        } CPUIDinfo;

        #if (defined(__x86_64__) || defined(_M_X64))
            // This code is assembly for 64 bit target OS.
            // Assembly code must be compiled with the use-msasm switch for Linux targets with the
            // Intel compiler.
            int isCPUIDsupported(void)
            {
                // returns 1 if CPUID instruction supported on this processor, zero otherwise
                // This isn't necessary on 64 bit processors because all 64 bit processor support CPUID
                return((int)1);
            }

            static void get_cpuid_info(CPUIDinfo *Info, const unsigned int leaf, const unsigned int subleaf)
            {
                // Stores CPUID return Info in the CPUIDinfo structure.
                // leaf and subleaf used as parameters to the CPUID instruction
                // parameters and register usage designed to be safe for both Windows and Linux
                // Use the Intel compiler option -use-msasm when the target is Linux
                __asm
                {
                    mov r10d, subleaf; arg2, subleaf(in R8 on WIN, in RDX on Linux)
                    mov r8, Info; arg0, array addr(in RCX on WIN, in RDI on Linux)
                    mov r9d, leaf; arg1, leaf(in RDX on WIN, in RSI on Linux)
                    push rax
                    push rbx
                    push rcx
                    push rdx
                    mov eax, r9d
                    mov ecx, r10d
                    cpuid
                    mov DWORD PTR[r8], eax
                    mov DWORD PTR[r8 + 4], ebx
                    mov DWORD PTR[r8 + 8], ecx
                    mov DWORD PTR[r8 + 12], edx
                    pop rdx
                    pop rcx
                    pop rbx
                    pop rax
                }
            }

        #else
            // 32 bit
            //Note need to make sure -use-msasm switch is used with Intel compiler for Linux to get the
            // ASM code to compile for both windows and linux with one version source

            static int isCPUIDsupported(void)
            {
                // returns 1 if CPUID instruction supported on this processor, zero otherwise
                // This isn't necessary on 64 bit processors because all 64 bit Intel processors support CPUID
                __asm
                {
                    push ecx; save ecx
                    pushfd; push original EFLAGS
                    pop eax; get original EFLAGS
                    mov ecx, eax; save original EFLAGS
                    xor eax, 200000h; flip bit 21 in EFLAGS
                    push eax; save new EFLAGS value on stack
                    popfd; replace current EFLAGS value
                    pushfd; get new EFLAGS
                    pop eax; store new EFLAGS in EAX
                    xor eax, ecx; Bit 21 of flags at 200000h will be 1 if CPUID exists
                    shr eax, 21; Shift bit 21 bit 0 and return it
                    push ecx
                    popfd; restore bit 21 in EFLAGS first
                    pop ecx; restore ecx
                }
            }

            //Note need to make sure -use-msasm switch is used with Intel compiler for Linux to get the
            // ASM code to compile for both windows and linux with one version source
            static void get_cpuid_info(CPUIDinfo *Info, const unsigned int leaf, const unsigned int subleaf)
            {
                // Stores CPUID return Info in the CPUIDinfo structure.
                // leaf and subleaf used as parameters to the CPUID instruction
                // parameters and registure usage designed to be safe for both Win and Linux
                // when using -use-msasm
                __asm
                {
                    mov edx, Info; addr of start of output array
                    mov eax, leaf; leaf
                    mov ecx, subleaf; subleaf
                    push edi
                    push ebx
                    mov  edi, edx; edi has output addr
                    cpuid
                    mov DWORD PTR[edi], eax
                    mov DWORD PTR[edi + 4], ebx
                    mov DWORD PTR[edi + 8], ecx
                    mov DWORD PTR[edi + 12], edx
                    pop ebx
                    pop edi
                    ret
                }
            }
        #endif

        static int isGenuineIntel(void)
        {
            // returns largest function # supported by CPUID if it is a Geniune Intel processor AND it supports
            // the CPUID instruction, 0 if not
            CPUIDinfo Info;
            int rVal = 0;
            char procString[] = "GenuineIntel";

            if (isCPUIDsupported())
            {
                // execute CPUID with eax = 0, subleaf doesn't matter so set it to zero
                get_cpuid_info(&Info, 0x0, 0x0);
                if ((Info.EBX == ((int *)procString)[0]) &&
                    (Info.EDX == ((int *)procString)[1]) && (Info.ECX == ((int *)procString)[2]))
                {
                    rVal = Info.EAX;
                }
            }
            return(rVal);
        }

        static bool DetectSSE42Supported(void)
        {
            // returns 1 if is a Nehalem or later processor, 0 if prior to Nehalem

            CPUIDinfo Info;
            int rVal = 0;
            // The code first determines if the processor is an Intel Processor.  If it is, then
            // feature flags bit 19 (SSE 4.1) and 20 (SSE 4.2) in ECX after CPUID call with EAX = 0x1
            // are checked.
            // If both bits are 1 (indicating both SSE 4.1 and SSE 4.2 exist) then
            // the function returns 1
            const int SSE4_1_FLAG = 0x080000;
            const int SSE4_2_FLAG = 0x100000;
            const int CHECKBITS = SSE4_1_FLAG | SSE4_2_FLAG;

            if (isGenuineIntel() >= 1)
            {
                // execute CPUID with eax (leaf) = 1 to get feature bits,
                // subleaf doesn't matter so set it to zero
                get_cpuid_info(&Info, 0x1, 0x0);
                if ((Info.ECX & CHECKBITS) == CHECKBITS)
                {
                    rVal = 1;
                }
            }
            return rVal == 1;
        }
    #elif defined(_MSC_VER)
        static bool DetectSSE42Supported()
        {
            //https://msdn.microsoft.com/en-us/library/hskdteyh(v=vs.90).aspx
            auto bSSE42Extensions = false;

            int cpuInfo[4] = { -1 };

            //Get count
            __cpuid(cpuInfo, 0);
            auto idCount = cpuInfo[0];

            //Get info
            if (idCount >= 1)
            {
                __cpuid(cpuInfo, 1);
                bSSE42Extensions = (cpuInfo[2] & 0x100000) ? true : false;
            }

            return bSSE42Extensions;
        }
    #endif
#endif


//Implementation----------------------------------------------------------------
bool CpuFeatures::SupportsSSE42()
{
#if FINJIN_ENABLE_SSE
    static bool supported = DetectSSE42Supported();
    return supported;
#else
    return false;
#endif
}
