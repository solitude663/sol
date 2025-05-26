internal Arena* ArenaAlloc(u64 size, u64 align = ARENA_DEFAULT_ALIGN)
{
	u64 allocGranularity = ARENA_ALLOCATION_GRANULARITY;
	u64 allocationSize = size + allocGranularity - 1;
	allocationSize -= allocationSize % allocGranularity;
	void* base = OS_Reserve(allocationSize);

	u64 initialCommitSize = ARENA_COMMIT_GRANULARITY;
	Assert(initialCommitSize >= sizeof(Arena));

	OS_Commit(base, initialCommitSize);
	Arena* arena = (Arena*)base;
	arena->Base = (u8*)base + sizeof(Arena);
	arena->Size = allocationSize - sizeof(Arena);
	arena->SizeCommited = initialCommitSize - sizeof(Arena);
	arena->Used = 0;
	arena->Align = align;

	return arena;
}

internal Arena* ArenaAllocDefault()
{
	return ArenaAlloc(GB(8));
}

internal void ArenaFree(Arena* arena)
{
	OS_Release((void*)arena);
}

internal b8 IsPowerOfTwo(ptr_value x) {
	return (x & (x - 1)) == 0;
}

internal ptr_value AlignForward(ptr_value ptr, u64 align) {
	ptr_value p, a, modulo;

	Assert(IsPowerOfTwo(align));

	p = ptr;
	a = (ptr_value)align;
	// Same as (p % a) but faster as 'a' is a power of two
	modulo = p & (a-1);

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}

internal void* ArenaPushNoZero(Arena* arena, u64 size)
{
	void* result = 0;
	
	u8* target = arena->Base + arena->Used;
	ptr_value offset_from_base = AlignForward((ptr_value)target, arena->Align);
	offset_from_base -= (ptr_value)arena->Base; // Convert to relative offset;

	u64 new_offset = offset_from_base + size;
	if(new_offset < arena->Size)
	{
		if(new_offset > arena->SizeCommited)
		{
			u64 sizeToCommit = new_offset - arena->SizeCommited;
			sizeToCommit += ARENA_COMMIT_GRANULARITY - 1;
			sizeToCommit -= sizeToCommit % ARENA_COMMIT_GRANULARITY;
			OS_Commit(arena->Base + arena->SizeCommited, sizeToCommit);
			arena->SizeCommited += sizeToCommit;
		}

		result = arena->Base + offset_from_base;
		arena->Used = new_offset;
	}
	else
	{
		// TODO(afb) :: Error handling
		Assert(0);
	}

	return result;
}

internal void* ArenaPush(Arena* arena, u64 size)
{
	void* result = ArenaPushNoZero(arena, size);
	MemorySet(result, size, 0);
	return result;
}

#if 0
internal void* ArenaRealloc(Arena* arena, void* ptr, u64 new_size)
{
	void* result = 0;

	u64 old_size = arena->LastAllocSize;
	i64 diff = new_size - old_size;
	u64 new_offset = arena->Used + diff;

	u8* mem = (u8*)ptr;
	if((arena->Base + arena->LastOffset) == mem)
	{
		if(diff < 0)
		{
			arena->Used += diff;
		}
		else
		{
			u8* target = arena->Base + arena->Used;
			ptr_value offset_from_base = AlignForward((ptr_value)target, arena->Align);
			offset_from_base -= (ptr_value)arena->Base; // Convert to relative offset;

			u64 new_offset = diff + offset_from_base;
			if(new_offset < arena->Size)
			{
				if(new_offset > arena->SizeCommited)
				{
					u64 size_to_commit = new_offset - arena->SizeCommited;
					size_to_commit += ARENA_COMMIT_GRANULARITY - 1;
					size_to_commit -= size_to_commit % ARENA_COMMIT_GRANULARITY;
					OS_Commit(arena->Base + arena->SizeCommited, size_to_commit);
					arena->SizeCommited += size_to_commit;
				}
					
				result = arena->Base + offset_from_base;
				arena->Used = new_offset + diff;
			}
			else
			{
				// TODO(afb) :: Error handling
				Assert(0);
			}
		}
	}
	else
	{
		result = ArenaPushNoZero(arena, new_size);		
	}
}
#endif


internal void ArenaClear(Arena* arena)
{
	arena->Used = 0;
}

#define PushStruct(arena, type) (type*)ArenaPushNoZero(arena, sizeof(type))
#define PushStructZero(arena, type) (type*)ArenaPush(arena, sizeof(type))

#define PushArray(arena, type, count) (type*)ArenaPushNoZero(arena, sizeof(type) * count)
#define PushArrayZero(arena, type, count) (type*)ArenaPush(arena, sizeof(type) * count)

internal TempArena TempArenaBegin(Arena* arena)
{
	TempArena result;
	result.Arena = arena;
	result.Position = arena->Used;
	return result;
}

internal void TempArenaEnd(TempArena temp)
{
	temp.Arena->Used = temp.Position;
}
