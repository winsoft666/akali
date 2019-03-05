/*******************************************************************************
* Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/

#ifndef PPX_EXPORT_H_
#define PPX_EXPORT_H_
#pragma once


#ifdef PPX_STATIC
#define PPX_API 
#else
#if defined(PPX_EXPORTS)
#	if defined(_MSC_VER)
#		define PPX_API __declspec(dllexport)
#	else
#		define PPX_API 
#	endif
#else
#	if defined(_MSC_VER)
#		define PPX_API __declspec(dllimport)
#	else
#		define PPX_API 
#	endif
#endif
#endif

#endif // !PPX_EXPORT_H_