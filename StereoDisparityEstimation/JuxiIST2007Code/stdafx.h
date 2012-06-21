/* stdafx.h
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-11
 * include file for standard system include files, or project specific include files
 * that are used frequently, but are changed infrequently
 */

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>

//opencv includes
#include "cv.h"
#include "highgui.h"

// Depth Perceptioin classes
#include "LinFilter.h"
#include "DisparityMapper.h"
