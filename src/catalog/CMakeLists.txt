# **************************************************************************** 
# * Project:  wxGIS
# * Purpose:  cmake script
# * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
# ****************************************************************************
# *   Copyright (C) 2013 Bishop
# *
# *    This program is free software: you can redistribute it and/or modify
# *    it under the terms of the GNU General Public License as published by
# *    the Free Software Foundation, either version 3 of the License, or
# *    (at your option) any later version.
# *
# *    This program is distributed in the hope that it will be useful,
# *    but WITHOUT ANY WARRANTY; without even the implied warranty of
# *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# *    GNU General Public License for more details.
# *
# *    You should have received a copy of the GNU General Public License
# *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ****************************************************************************
cmake_minimum_required (VERSION 2.8)
set(PROJECT_NAME catalog)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/catalog)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/catalog)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED base)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
endif(GDAL_FOUND)

option(wxGIS_USE_POSTGRES "Set OFF to build without PostGIS support" OFF)
if(wxGIS_USE_POSTGRES)
    find_package(POSTGRES REQUIRED)
    if(POSTGRES_FOUND)
        include_directories(${POSTGRES_INCLUDE_DIR})
        add_definitions(-DHAVE_PG)
    endif(POSTGRES_FOUND)
endif(wxGIS_USE_POSTGRES)

option(wxGIS_USE_OPENSSL "Set OFF to build without OpenSSL support" OFF)
if(wxGIS_USE_OPENSSL)
    find_package( OpenSSL )
    if(OPENSSL_FOUND)
        include_directories(${OPENSSL_INCLUDE_DIR})
        add_definitions(-DHAVE_OPENSSL)
    endif(OPENSSL_FOUND) 
endif(wxGIS_USE_OPENSSL)

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${LIB_HEADERS}/catalog.h
    ${LIB_HEADERS}/catop.h
    ${LIB_HEADERS}/gxarchfolder.h
    ${LIB_HEADERS}/gxarchivefactory.h
    ${LIB_HEADERS}/gxcatalog.h
    ${LIB_HEADERS}/gxcsvfactory.h
    ${LIB_HEADERS}/gxdataset.h
    ${LIB_HEADERS}/gxdiscconnection.h
    ${LIB_HEADERS}/gxdiscconnections.h
    ${LIB_HEADERS}/gxevent.h
    ${LIB_HEADERS}/gxfile.h
    ${LIB_HEADERS}/gxfilefactory.h
    ${LIB_HEADERS}/gxfilters.h
    ${LIB_HEADERS}/gxfolder.h
    ${LIB_HEADERS}/gxfolderfactory.h
    ${LIB_HEADERS}/gxmldataset.h
    ${LIB_HEADERS}/gxmapinfofactory.h
    ${LIB_HEADERS}/gxmlfactory.h
    ${LIB_HEADERS}/gxprjfactory.h
    ${LIB_HEADERS}/gxobject.h
    ${LIB_HEADERS}/gxrasterfactory.h
    ${LIB_HEADERS}/gxshapefactory.h
    ${LIB_HEADERS}/gxspatreffolder.h
    ${LIB_HEADERS}/gxxmlconnstor.h
    ${LIB_HEADERS}/gxpostgisdataset.h
    ${LIB_HEADERS}/gxdbconnections.h
    ${LIB_HEADERS}/gxdbconnfactory.h    
    ${LIB_HEADERS}/gxwebconnections.h
    ${LIB_HEADERS}/gxwebconnfactory.h
    ${LIB_HEADERS}/gxremoteconn.h
    ${LIB_HEADERS}/gxobjectfactory.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/catop.cpp
    ${LIB_SOURCES}/gxarchfolder.cpp
    ${LIB_SOURCES}/gxarchivefactory.cpp
    ${LIB_SOURCES}/gxcatalog.cpp
    ${LIB_SOURCES}/gxcsvfactory.cpp
    ${LIB_SOURCES}/gxdataset.cpp
    ${LIB_SOURCES}/gxdiscconnection.cpp
    ${LIB_SOURCES}/gxdiscconnections.cpp
    ${LIB_SOURCES}/gxevent.cpp
    ${LIB_SOURCES}/gxfile.cpp
    ${LIB_SOURCES}/gxfilefactory.cpp
    ${LIB_SOURCES}/gxfilters.cpp
    ${LIB_SOURCES}/gxfolder.cpp
    ${LIB_SOURCES}/gxfolderfactory.cpp
    ${LIB_SOURCES}/gxmldataset.cpp
    ${LIB_SOURCES}/gxmapinfofactory.cpp
    ${LIB_SOURCES}/gxmlfactory.cpp
    ${LIB_SOURCES}/gxprjfactory.cpp
    ${LIB_SOURCES}/gxobject.cpp
    ${LIB_SOURCES}/gxrasterfactory.cpp
    ${LIB_SOURCES}/gxshapefactory.cpp
    ${LIB_SOURCES}/gxspatreffolder.cpp    
    ${LIB_SOURCES}/gxxmlconnstor.cpp    
    ${LIB_SOURCES}/gxpostgisdataset.cpp
    ${LIB_SOURCES}/gxdbconnections.cpp
    ${LIB_SOURCES}/gxdbconnfactory.cpp    
    ${LIB_SOURCES}/gxwebconnections.cpp
    ${LIB_SOURCES}/gxwebconnfactory.cpp
    ${LIB_SOURCES}/gxremoteconn.cpp
    ${LIB_SOURCES}/gxobjectfactory.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_CLT)
set( WXGISCATALOG_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISDATASOURCE_LIB_NAME})

include(installlib)
