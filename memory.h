// $Id: memory.h,v 1.7 2004/01/25 21:04:19 iain Exp $
// Functions and macros for accessing game memory.

#ifndef GIT_MEMORY_H
#define GIT_MEMORY_H

#include "config.h"
#include <string.h>

// --------------------------------------------------------------
// Macros for reading and writing big-endian data.

GIT_INLINE git_uint16 byteswap16 (git_uint16 v) {
#if defined(USE_BIG_ENDIAN)
	return v;
#elif defined(__GNUC__)
	return __builtin_bswap16(v);
#else
	return
		((v & 0xFF00) >> 8) |
		((v & 0x00FF) << 8);
#endif
}

GIT_INLINE git_uint32 byteswap32 (git_uint32 v) {
#if defined(USE_BIG_ENDIAN)
	return v;
#elif defined(__GNUC__)
	return __builtin_bswap32(v);
#else
	return
		((v & 0xFF000000) >> 24) |
		((v & 0x00FF0000) >> 8) |
		((v & 0x0000FF00) << 8) |
		((v & 0x000000FF) << 24);
#endif
}

GIT_INLINE git_uint32 read32 (const git_uint8 *ptr) {
	git_uint32 v;
	memcpy(&v, ptr, 4);
	return byteswap32(v);
}

GIT_INLINE git_uint16 read16 (const git_uint8 *ptr) {
	git_uint16 v;
	memcpy(&v, ptr, 2);
	return byteswap16(v);
}

GIT_INLINE void write32 (git_uint8 *ptr, git_uint32 v) {
	git_uint32 t = byteswap32(v);
	memcpy(ptr, &t, 4);
}

GIT_INLINE void write16 (git_uint8 *ptr, git_uint16 v) {
	git_uint16 t = byteswap16(v);
	memcpy(ptr, &t, 2);
}

GIT_INLINE git_uint32 readtag (const char *ptr) {
	return read32((const git_uint8 *)ptr);
}

// Accessing single bytes is easy on any platform.

#define read8(ptr)     (*((git_uint8*)(ptr)))
#define write8(ptr, v) (read8(ptr)=(git_uint8)(v))

// --------------------------------------------------------------
// Globals

extern git_uint32 gRamStart; // The start of RAM.
extern git_uint32 gExtStart; // The start of extended memory (initialised to zero).
extern git_uint32 gEndMem;   // The current end of memory.
extern git_uint32 gOriginalEndMem; // The value of EndMem when the game was first loaded.

// This is the entire gamefile, as read-only memory. It contains
// both the ROM, which is constant for the entire run of the program,
// and the original RAM, which is useful for checking what's changed
// when saving to disk or remembering a position for UNDO.
extern const git_uint8 * gRom;

// This is the current contents of RAM. This pointer actually points
// to the start of ROM, so that you don't have to keep adding and
// subtracting gRamStart, but don't try to access ROM via this pointer.
extern git_uint8 * gRam;

// --------------------------------------------------------------
// Functions

// Initialise game memory. This sets up all the global variables
// declared above. Note that it does *not* copy the given memory
// image: it must be valid for the lifetime of the program.

extern void initMemory (const git_uint8 * game, git_uint32 gameSize);

// Verifies the gamefile based on its checksum. 0 on success, 1 on failure.

extern int verifyMemory ();

// Resizes the game's memory. Returns 0 on success, 1 on failure.

extern int resizeMemory (git_uint32 newSize, int isInternal);

// Resets memory to its initial state. Call this when the game restarts.

extern void resetMemory (git_uint32 protectPos, git_uint32 protectSize);

// Disposes of all the data structures allocated in initMemory().

extern void shutdownMemory ();

// Utility functions -- these just pass an appropriate
// string to fatalError().

extern noreturn void memReadError (git_uint32 address);
extern noreturn void memWriteError (git_uint32 address);

// Functions for reading and writing game memory.

GIT_INLINE git_uint32 memRead32 (git_uint32 address)
{
	if (address <= gRamStart - 4)
		return read32 (gRom + address);
	else if (address <= gEndMem - 4)
		return read32 (gRam + address);
    else
        return memReadError (address), 0;
}

GIT_INLINE git_uint32 memRead16 (git_uint32 address)
{
	if (address <= gRamStart - 4)
		return read16 (gRom + address);
	else if (address <= gEndMem - 2)
		return read16 (gRam + address);
    else
        return memReadError (address), 0;
}

GIT_INLINE git_uint32 memRead8 (git_uint32 address)
{
    if (address <= gRamStart - 4)
        return read8 (gRom + address);
    else if (address < gEndMem)
        return read8 (gRam + address);
    else
        return memReadError (address), 0;
}

GIT_INLINE void memWrite32 (git_uint32 address, git_uint32 val)
{
	if (address >= gRamStart && address <= (gEndMem - 4))
		write32 (gRam + address, val);
	else
        memWriteError (address);
}

GIT_INLINE void memWrite16 (git_uint32 address, git_uint32 val)
{
	if (address >= gRamStart && address <= (gEndMem - 2))
		write16 (gRam + address, val);
	else
        memWriteError (address);
}

GIT_INLINE void memWrite8 (git_uint32 address, git_uint32 val)
{
	if (address >= gRamStart && address < gEndMem)
		write8 (gRam + address, val);
	else
        memWriteError (address);
}

#endif // GIT_MEMORY_H
