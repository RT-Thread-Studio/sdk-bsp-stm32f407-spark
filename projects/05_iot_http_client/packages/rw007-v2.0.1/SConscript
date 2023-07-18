#-*- encoding: utf-8 -*-
#---------------------------------------------------------------------------------
# @File:   Sconscript for package 
# @Author: liu2guang
# @Date:   2018-09-19 18:07:00(v0.1.0) 
# 
# @LICENSE: GPLv3: https://github.com/rtpkgs/buildpkg/blob/master/LICENSE.
#
#---------------------------------------------------------------------------------
import os
from building import * 
Import('RTT_ROOT')
Import('rtconfig')

#---------------------------------------------------------------------------------
# Package configuration
#---------------------------------------------------------------------------------
PKGNAME = "rw007"
VERSION = "v0.0.1"
DEPENDS = ["PKG_USING_RW007"]

#---------------------------------------------------------------------------------
# Compile the configuration 
#
# SOURCES: Need to compile c and c++ source, auto search when SOURCES is empty
# 
# LOCAL_CPPPATH: Local file path (.h/.c/.cpp)
# LOCAL_CCFLAGS: Local compilation parameter 
# LOCAL_ASFLAGS: Local assembly parameters
# 
# CPPPATH: Global file path (.h/.c/.cpp), auto search when LOCAL_CPPPATH/CPPPATH 
#          is empty # no pass!!!
# CCFLAGS: Global compilation parameter 
# ASFLAGS: Global assembly parameters
#
# CPPDEFINES: Global macro definition
# LOCAL_CPPDEFINES: Local macro definition 
# 
# LIBS: Specify the static library that need to be linked
# LIBPATH: Specify the search directory for the library file (.lib/.a)
#
# LINKFLAGS: Link options
#---------------------------------------------------------------------------------
SOURCES          = ["src/spi_wifi_rw007.c"] 

if GetDepend(['RW007_USING_STM32_DRIVERS']):
    SOURCES     += ["example/rw007_stm32_port.c"]

LOCAL_CPPPATH    = [] 
LOCAL_CCFLAGS    = "" 
LOCAL_ASFLAGS    = ""

CPPPATH          = [GetCurrentDir(), os.path.join(GetCurrentDir(), 'inc')] 
CCFLAGS          = "" 
ASFLAGS          = ""

CPPDEFINES       = []
LOCAL_CPPDEFINES = []

LIBS             = [] 
LIBPATH          = [] 

LINKFLAGS        = "" 

SOURCES_IGNORE   = []
CPPPATH_IGNORE   = []

#---------------------------------------------------------------------------------
# Main target
#---------------------------------------------------------------------------------
objs = DefineGroup(name = PKGNAME, src = SOURCES, depend = DEPENDS, 
                   CPPPATH          = CPPPATH, 
                   CCFLAGS          = CCFLAGS, 
                   ASFLAGS          = ASFLAGS, 
                   LOCAL_CPPPATH    = LOCAL_CPPPATH, 
                   LOCAL_CCFLAGS    = LOCAL_CCFLAGS, 
                   LOCAL_ASFLAGS    = LOCAL_ASFLAGS, 
                   CPPDEFINES       = CPPDEFINES, 
                   LOCAL_CPPDEFINES = LOCAL_CPPDEFINES, 
                   LIBS             = LIBS, 
                   LIBPATH          = LIBPATH,
                   LINKFLAGS        = LINKFLAGS)  

Return("objs") 
#---------------------------------------------------------------------------------
# End
#---------------------------------------------------------------------------------
