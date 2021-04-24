#pragma once
/**
 * @file Tunning.h
 *
 * @brief MemEx Tunning values, redefine these for your needs.
 *
 * @author Balan Narcis
 * Contact: balannarcis96@gmail.com
 *
 */

namespace MemEx {
	
#ifndef SmallMemBlockSize
#define SmallMemBlockSize		  512
#endif 

#ifndef MediumMemBlockSize
#define MediumMemBlockSize		  1024
#endif 
#ifndef LargeMemBlockSize
#define LargeMemBlockSize		  4096
#endif 
#ifndef ExtraLargeMemBlockSize
#define ExtraLargeMemBlockSize	  (24 * 1024)
#endif 

#ifndef SmallMemBlockCount
#define SmallMemBlockCount		  4096
#endif 
#ifndef MediumMemBlockCount
#define MediumMemBlockCount		  4096
#endif 
#ifndef LargeMemBlockCount
#define LargeMemBlockCount		  4096
#endif 
#ifndef ExtraLargeMemBlockCount
#define ExtraLargeMemBlockCount   4096
#endif 

}