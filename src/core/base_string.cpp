
internal b8 IsAlpha(u8 c)
{
	b8 result = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	return result;
}

internal b8 IsUpper(u8 c)
{
	b8 result = (c >= 'A' && c <= 'Z');
	return result;
}

internal b8 IsLower(u8 c)
{
	b8 result = (c >= 'a' && c <= 'z');
	return result;
}

internal b8 IsDigit(u8 c)
{
	b8 result = (c >= '0' && c <= '9');
	return result;
}

internal b8 IsSpace(u8 c)
{
	b8 result = 0;
	
	u8 spaceChars[] = {10,  11, 13, 32, 9};
	for(u32 id = 0; id < ArrayCount(spaceChars); id++)
	{
		if(spaceChars[id] == c)
		{
			result = 1;
			break;
		}
	}
	return result;
}

internal b8 IsSymbol(u8 c)
{
	b8 result = ((c >= '!' && c <= '/') || (c >= ':' && c <= '@') ||
				 (c >= '[' && c <= '`') || (c >= '{' && c <= '~'));
	return result;
}

internal u8 ToUpper(u8 c)
{
	if(IsLower(c))
	{
		c = c - 'a' + 'A';
	}

	return c;
}

internal u8 ToLower(u8 c)
{
	if(IsUpper(c))
	{
		c += 32;
	}

	return c;
}

internal u64 CStringLength(char* cstr)
{	
	u64 result = 0;
	if(cstr)
	{
		for(;*cstr != 0; cstr++)
			result += 1;		
	}
	return result;
}

// NOTE(afb) :: Constructors
internal String8 Str8(u8* str, u64 size)
{
	String8 result;
	result.Str = str;
	result.Length = size;
	return result;
}

// NOTE(afb) :: Manipulations
internal String8 Substr8(String8 str, u64 start, u64 count)
{
	String8 result;
	u64 top = Min(start, str.Length);
	u64 bottom = Min(start+count, str.Length);
	
	result.Str = str.Str + top;
	result.Length = bottom - top;
	return result;
}

internal String8 Trim8(String8 str, String8 bad)
{
	String8 result = Trim8Front(Trim8Back(str, bad), bad);
	return result;
}

internal String8 Trim8Front(String8 str, String8 bad)
{
	if(bad.Length == 0) return str;
	
	String8 curr = Substr8(str, 0, bad.Length);
	while(Str8Match(curr, bad, MF_None))
	{
		str = Substr8(str, bad.Length, str.Length - bad.Length);
		curr = Substr8(str, 0, bad.Length);
	}
	return str;
}

internal String8 Trim8Back(String8 str, String8 bad)
{
	if(bad.Length == 0) return str;
	
	String8 curr = Suffix8(str, bad.Length);
	while(Str8Match(curr, bad, MF_None))
	{
		str = Substr8(str, 0, str.Length - bad.Length);
		curr = Suffix8(str, bad.Length);
	}
	return str;
}

internal String8 Trim8Space(String8 str)
{
	String8 result = {0};

	u64 start = 0;
	for(u64 i = 0; i < str.Length; i++)
	{
		if(IsSpace(str.Str[i]))
		{
			start++;
		}
		else
		{
			break;
		}
	}
	result.Str = str.Str + start;
	result.Length = str.Length - start;

	u64 end = 0;
	if(result.Length)
	{
		for(u64 i = 0; i < result.Length; i++)
		{
			if(IsSpace(str.Str[str.Length - (i+1)]))
			{
				end++;
			}
			else
			{
				break;
			}
		}
	}
	result.Length -= end;
	return result;
}

// NOTE(afb) :: Comparison
internal b8 Str8Match(String8 a, String8 b, MatchFlags flags)
{
	b8 result = 0;
	
	if(a.Length == b.Length)
	{
		result = 1;
		
		if(flags & MF_IgnoreCase)
		{
			for(u64 index = 0; index < a.Length; index++)
			{
				if(ToUpper(a.Str[index]) != ToUpper(b.Str[index]))
				{
					result = 0;
					break;
				}
			}
		}
		else
		{
			for(u64 index = 0; index < a.Length; index++)
			{
				if(a.Str[index] != b.Str[index])
				{
					result = 0;
					break;
				}
			}
		}
	}

	return result;
}

internal b8 operator==(String8& a, String8& b)
{
	return Str8Match(a, b, MF_None);
}

// NOTE(afb) :: Allocation
internal String8 Str8Copy(Arena* arena, String8 a)
{
	String8 result;
	result.Str = PushArray(arena, u8, a.Length);
	result.Length = a.Length;
	MemoryCopy(result.Str, a.Str, a.Length);
	return result;
}

internal String8 Str8Concat(Arena* arena, String8 a, String8 b)
{
	String8 result;		
	u64 len = a.Length + b.Length;
	u8* str = PushArray(arena, u8, len);

	MemoryCopy(str, a.Str, a.Length);
	MemoryCopy(str + a.Length, b.Str, b.Length);
	result.Str = str;
	result.Length = len;	
	return result;
}

internal String8 Str8Format(Arena* arena, char* format, ...)
{
	va_list args;
	va_start(args, format);
	String8 f = Str8C(format);
	String8 result = Str8FormatExplicit(arena, f, args);
	va_end(args);
	return result;
}

internal String8 Str8Format(Arena* arena, String8 format, ...)
{
	va_list args;
	va_start(args, format);
	String8 result = Str8FormatExplicit(arena, format, args);
	va_end(args);
	return result;
}

internal String8 Str8FormatExplicit(Arena* arena, String8 format, va_list args)
{	
	va_list countPtr, printPtr;	
	va_copy(countPtr, args);
	va_copy(printPtr, countPtr);

	u64 len = 0;
	for(u64 i = 0; i < format.Length; i++)
	{
		u8 curr = format.Str[i];
		if(curr != '%')
		{
			len++;
		}
		else
		{
			// TODO(afb) :: Do better error handling other than just crashing.
			Assert((i + 1) < format.Length);
			curr = format.Str[++i];
			switch(curr)
			{
				case('b'):
				{
					b32 value = va_arg(countPtr, b32);
					if(value == 0) len += 4;
					else len += 5;
				}break;

				case('c'):
				{
					u8 value = (u8)va_arg(countPtr, i32);
					UnusedVariable(value);
					len += 1;
				}break;

				case('d'):
				{
					i32 value = va_arg(countPtr, i32);
					len += GetI64Length((i64)value);
				}break;

				case('l'):
				{
					i64 value = va_arg(countPtr, i64);
					len += GetI64Length(value);
				}break;

				case('u'):
				{
					Assert((i + 1) < format.Length);
					curr = format.Str[++i];
					if(curr == 'l')
					{
						u64 value = va_arg(countPtr, u64);
						len += GetU64Length(value);
					}
					else if(curr == 'd')
					{
						u32 value = va_arg(countPtr, u32);
						len += GetU64Length((u64)value);
					}
					else
					{
						Assert(0);
					}
				}break;

				case('S'):
				{
					String8 value = va_arg(countPtr, String8);
					len += value.Length;
				}break;

				case('s'):
				{
					char* value = va_arg(countPtr, char*);					
					len += CStringLength(value);
				}break;

				default:
				{
					Assert(0);
				}
			}
		}
	}
	va_end(countPtr);

	// NOTE(afb) :: snprintf wants the '\0' so but I'm using length based strings
	// so increment the length by 1 so it can think its adding the null terminatior.
	// Or smth like that
	len++; 
	u8* bytes = PushArray(arena, u8, len);
	u64 inc = 0;
	for(u64 i = 0; i < format.Length; i++)
	{
		u8 curr = format.Str[i];
		if(curr != '%')
		{
			bytes[inc++] = curr;
		}
		else
		{
			curr = format.Str[++i];
			char* off = (char*)bytes+inc;
			switch(curr)
			{
				case('b'):
				{
					b32 value = va_arg(printPtr, b32);
					inc += snprintf(off, len-inc, "%s", value ? "true" : "false");
				}break;

				case('c'):
				{
					u8 value = (u8)va_arg(printPtr, i32);
					inc += snprintf(off, len-inc, "%c", (char)value);
				}break;

				case('d'):
				{
					i32 value = va_arg(printPtr, i32);
					inc += snprintf(off, len-inc, "%d", value);
				}break;

				case('l'):
				{
					i64 value = va_arg(printPtr, i64);
					inc += snprintf(off, len-inc, "%ld", value);
				}break;

				case('u'):
				{
					curr = format.Str[++i];
					if(curr == 'l')
					{
						u64 value = va_arg(printPtr, u64);
						inc += snprintf(off, len-inc, "%lu", value);
					}
					else if(curr == 'd')
					{
						u32 value = va_arg(printPtr, u32);
						inc += snprintf(off, len-inc, "%u", value);
					}
					else
					{
						Assert(0);
					}
				}break;

				case('S'):
				{
					String8 value = va_arg(printPtr, String8);
					inc += snprintf(off, len-inc, "%.*s", Str8Print(value));
				}break;

				case('s'):
				{
					char* value = va_arg(printPtr, char*);
					inc += snprintf(off, len-inc, "%s", value);
				}break;

				default:
				{
					Assert(0);
				}
			}
		}
	}	
	va_end(printPtr);

	String8 result;
	result.Str = bytes;
	result.Length = len-1;
	
	return result;
}

internal String8 Str8Fill(Arena* arena, u8 byte, u64 size)
{
	String8 result;
	result.Str = PushArray(arena, u8, size);
	result.Length = size;
	MemorySet(result.Str, size, byte);
	return result;
}

// NOTE(afb) :: String8List
internal void Str8ListPushNode(String8List* list, String8Node* node)
{
	if(!list->First)
	{
		list->First = node;
		list->Last = node;
		
	}
	else
	{
		list->Last->Next = node;
		list->Last = node;
	}

	list->NodeCount++;
	list->Length += node->Str.Length;
}

internal void Str8ListPushNodeFront(String8List* list, String8Node* node)
{
	node->Next = list->First;
	list->First = node;

	list->NodeCount++;
	list->Length += node->Str.Length;
}

internal void Str8ListPush(Arena* arena, String8List* list, String8 str)
{
	String8Node* node = PushStruct(arena, String8Node);
	node->Str = str;
	Str8ListPushNode(list, node);
}


internal void Str8ListPushFront(Arena* arena, String8List* list, String8 str)
{
	String8Node* node = PushStruct(arena, String8Node);
	node->Str = str;
	Str8ListPushNodeFront(list, node);
}

internal String8 Str8Join(Arena* arena, String8List list, String8 join)
{
	u64 len = list.NodeCount ? ((list.NodeCount - 1) * join.Length + list.Length) : 0;
	u8* buffer = PushArray(arena, u8, len);

	u64 passed = 0;
	String8Node* node = list.First;
	for(u64 i = 0; i < list.NodeCount; i++)
	{
		MemoryCopy(buffer + passed, node->Str.Str, node->Str.Length);
		passed += node->Str.Length;
		
		if(i < list.NodeCount-1)
		{
			MemoryCopy(buffer+passed, join.Str, join.Length);
			passed += join.Length;
		}
		
		node = node->Next;
	}

	return Str8(buffer, len);
}

internal String8List Str8Split(Arena* arena, String8 str, String8 split)
{
	String8List result = {0};
	if(split.Length == 0)
	{
		Str8ListPush(arena, &result, str);
		return result;
	}

	String8 curr = str;	
	for(;;)
	{
		if(Str8Match(Prefix8(curr, split.Length), split, MF_None))
		{
			curr.Str += split.Length;
			curr.Length -= split.Length;
		}

		u64 index = Str8Find(curr, split);
		if(index != curr.Length)
		{
			Str8ListPush(arena, &result, Prefix8(curr, index));
			curr = Substr8(curr, index, curr.Length - index);
		}
		else
		{
			if(curr.Length) Str8ListPush(arena, &result, curr);
			break;
		}
	}

	return result;
}

internal String8List Str8SplitRemoveEmpty(Arena* arena, String8 str, String8 split)
{
	String8List result = {0};
	if(split.Length == 0)
	{
		Str8ListPush(arena, &result, str);
		return result;
	}

	String8 curr = str;	
	for(;;)
	{
		while(Str8Match(Prefix8(curr, split.Length), split, MF_None))
		{
			curr.Str += split.Length;
			curr.Length -= split.Length;
		}

		u64 index = Str8Find(curr, split);
		if(index != curr.Length)
		{
			Str8ListPush(arena, &result, Prefix8(curr, index));
			curr = Substr8(curr, index, curr.Length - index);
		}
		else
		{
			if(curr.Length) Str8ListPush(arena, &result, curr);
			break;
		}
	}

	return result;
}


// NOTE(afb) :: If 'search' is not found it returns the function returns the
// length of the string
internal u64 Str8Find(String8 str, String8 search)
{
	u64 result = str.Length;
	for(u64 i = 0; i < str.Length; i++)
	{
		String8 sub = Substr8(str, i, search.Length);
		if(Str8Match(sub, search, MF_None))
		{
			result = i;
			break;
		}
	}

	return result;
}

internal u64 Str8FindLast(String8 str, String8 search)
{
	u64 result = str.Length;
	for(u64 i = 0; i < str.Length; i++)
	{
		u64 index = str.Length - (i+1);
		String8 sub = Substr8(str, index, search.Length);
		if(Str8Match(sub, search, MF_None))
		{
			result = index;
			break;
		}
	}

	return result;
}

internal String8 Str8Replace(Arena* arena, String8 str, String8 id, String8 target)
{
	TempArena scratch = GetScratch(arena);

	String8List list = {0};
	String8 curr = str;
	for(;;)
	{
		while(Str8Match(Prefix8(curr, id.Length), id, MF_None))
		{
			Str8ListPush(scratch.Arena, &list, target);
			curr = Substr8(curr, id.Length, curr.Length - id.Length);
		}		
		
		u64 index = Str8Find(curr, id);
		if(index != curr.Length)
		{
			Str8ListPush(scratch.Arena, &list, Prefix8(curr, index));
			curr = Substr8(curr, index, curr.Length - index);
		}
		else
		{
			if(curr.Length) Str8ListPush(scratch.Arena, &list, curr);
			break;
		}
	}

	String8 result = Str8Join(arena, list, "");

	ReleaseScratch(scratch);
	return result;
}

internal char* ToCString(Arena* arena, String8 str)
{
	char* result = PushArray(arena, char, str.Length + 1);
	MemoryCopy(result, str.Str, str.Length);
	result[str.Length] = 0;
	return result;
}

internal u64 U64FromStr8(String8 str)
{
	str = Trim8Space(str);
	u64 result = 0;
	
	for(u64 i = 0; i < str.Length; i++)
	{
		u8 digit = str.Str[i];
		if(IsDigit(digit))
		{
			result *= 10;
			result += (u64)(digit - '0');
		}
		else
		{
			result = 0;
			break;
		}
	}

	return result;
}

internal f64 F64FromStr8(String8 str)
{
	f64 result = 0;

	u64 dotIndex = Str8Find(str, ".");
	u64 intValue = U64FromStr8(Prefix8(str, dotIndex));
	String8 floatPart = Substr8(str, dotIndex+1, str.Length - dotIndex - 1);

	f64 floatValue = 0;
	i32 div = 1;
	for(u64 i = 0; i < floatPart.Length; i++)
	{
		u8 value = floatPart.Str[i];
		if(IsDigit(value))
		{
			floatValue *= 10;
			floatValue += (value - '0');
			div *= 10;
		}
		else
		{
			return 0;
		}
	}
	
	floatValue /= div;
	result = (f64)intValue + floatValue;

	return result;
}

internal String8Array Str8ListToArray(Arena* arena, String8List list)
{
	String8Array result = {0};
	result.Values = PushArray(arena, String8, list.NodeCount);
	result.Count = list.NodeCount;
	
	String8Node* node = list.First;
	for(u64 i = 0; i < list.NodeCount; i++)
	{
		result.Values[i] = node->Str;
		node = node->Next;
	}
	return result;
}
