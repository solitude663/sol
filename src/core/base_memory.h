#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

#define ARENA_ALLOCATION_GRANULARITY MB(64)
#define ARENA_COMMIT_GRANULARITY KB(4)
#define ARENA_DEFAULT_ALIGN 8

struct Arena
{
	u8* Base;
	u64 Size;
	u64 Used;
	u64 SizeCommited;
	u64 Align;

	// Realloc
	u64 LastOffset;
	u64 LastAllocSize;	
};

struct TempArena
{
	::Arena* Arena;
	u64 Position;
};

internal Arena* ArenaAlloc(u64 size, u64 align);
internal Arena* ArenaAllocDefault();
internal void   ArenaFree(Arena* arena);
internal void*  ArenaPushNoZero(Arena* arena, u64 size);
internal void*  ArenaPush(Arena* arena, u64 size);
// TODO internal void*  ArenaRealloc(Arena* arena, u8* ptr, u64 size);
internal void   ArenaClear(Arena* arena);

internal TempArena TempArenaBegin(Arena* arena);
internal void      TempArenaEnd(TempArena temp);
	
#endif // Header guard
