
#Includes----------------------------------------------------------------------
#if __APPLE__
    #if defined(__i386__)
        #include "/Users/derek/finjin-3rd-party/cpp/boost/libs/context/src/asm/jump_i386_sysv_macho_gas.S"
    #elif defined(__x86_64__)
        #include "/Users/derek/finjin-3rd-party/cpp/boost/libs/context/src/asm/jump_x86_64_sysv_macho_gas.S"
    #elif defined(__arm64__)
        #include "/Users/derek/finjin-3rd-party/cpp/boost/libs/context/src/asm/jump_arm64_aapcs_macho_gas.S"
    #elif defined(__arm__)
        #include "/Users/derek/finjin-3rd-party/cpp/boost/libs/context/src/asm/jump_arm_aapcs_macho_gas.S"
    #endif
#endif
