internal u64 FNV1A64(void* data, u64 size)
{
	u64 hash = FNV1A_OFFSET64;
	u8* it = (u8*)data;
	for(u64 i = 0; i < size; i++)
	{
		hash = hash ^ it[i];
		hash = hash * FNV1A_PRIME64;
	}
	return hash;
}

internal u32 FNV1A32(void* data, u64 size)
{
	u32 hash = FNV1A_OFFSET32;
	u8* it = (u8*)data;
	for(u64 i = 0; i < size; i++)
	{
		hash = hash ^ it[i];
		hash = hash * FNV1A_PRIME32;
	}
	return hash;
}

internal u64 GetU64Length(u64 value)
{	
	f64 nDigits = (f64)floorl(log10l(labs((long)value))) + 1;
	return (u64)nDigits;	
}

internal u64 GetI64Length(i64 value)
{
	if(value == 0) return 1;  // Special case for 0
    return (u64)log10l(value < 0 ? -(f64)value : (f64)value) + 1;

	// 
	// f64 nDigits = (f64)floorl(log10l(labs((long)value))) + 1;
	// return (u64)nDigits;
}
