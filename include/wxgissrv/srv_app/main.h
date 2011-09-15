/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  Main application class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#pragma once

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wxgis/base.h"

#include "wx/defs.h"
#include "wx/cmdline.h"
#include "wx/string.h"

//#include "wxgis/core/config.h"
//#include "wx/file.h"
//#include <wx/ffile.h>
//#include "wx/app.h"
////#include "wx/log.h"
//#include "wx/apptrait.h"
//#include "wx/platinfo.h"
//#include <wx/dir.h>
//#include <wx/log.h>
//#include <wx/xml/xml.h>
//#include <wx/stdpaths.h>
//#include <wx/filename.h>
//#include <wx/dynload.h>
//#include <wx/dynlib.h>
//#include <wx/intl.h>

//#include <iostream>
//#include <stdio.h>
//#include <ctime>
//#include <vector>

// without this pragma, the stupid compiler precompiles #defines below so that
// changing them doesn't "take place" later!
#ifdef __VISUALC__
    #pragma hdrstop
#endif

#if !wxUSE_THREADS
    #error "This program requires thread support!"
#endif // wxUSE_THREADS


bool parse_commandline_parameters( int argc, char** argv );
int main(int argc, char** argv);
