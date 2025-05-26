#if OS_WINDOWS
#include "win32/base_win32_os.cpp"
#elif OS_LINUX
#include "linux/base_linux_os.cpp"
#else
#error OS not supported
#endif

#ifndef FLAG_CAPACITY
#define FLAG_CAPACITY 256
#endif

global Flag Flags[FLAG_CAPACITY];
global u64 FlagCount;

internal u64* OS_FlagInt(String8 name, u64 defaultValue, String8 usage)
{
	u64 current = FlagCount++;
	Assert(current < ArrayCount(Flags));

	Flags[current].Type = FT_Int;
	Flags[current].Name = name;
	Flags[current].IntValue = defaultValue;
	Flags[current].Usage = usage;
	return &Flags[current].IntValue;
}

internal void OS_FlagIntVar(u64* ptr, String8 name, u64 defaultValue, String8 usage)
{
	u64 current = FlagCount++;
	Assert(current < ArrayCount(Flags));

	Flags[current].Type = FT_IntPtr;
	Flags[current].Name = name;
	Flags[current].PtrValue = (ptr_value)ptr;
	Flags[current].Usage = usage;
	*ptr = defaultValue;
}

internal f64* OS_FlagFloat(String8 name, f64 defaultValue, String8 usage)
{
	u64 current = FlagCount++;
	Assert(current < ArrayCount(Flags));

	Flags[current].Type = FT_Float;
	Flags[current].Name = name;
	Flags[current].FloatValue = defaultValue;
	Flags[current].Usage = usage;
	return &Flags[current].FloatValue;
}

internal void OS_FlagFloatVar(f64* ptr, String8 name, f64 defaultValue, String8 usage)
{
	u64 current = FlagCount++;
	Assert(current < ArrayCount(Flags));

	Flags[current].Type = FT_FloatPtr;
	Flags[current].Name = name;
	Flags[current].PtrValue = (ptr_value)ptr;
	Flags[current].Usage = usage;
	*ptr = defaultValue;
}

internal b32* OS_FlagBool(String8 name, b32 defaultValue, String8 usage)
{
	u64 current = FlagCount++;
	Assert(current < ArrayCount(Flags));

	Flags[current].Type = FT_Bool;
	Flags[current].Name = name;
	Flags[current].BoolValue = defaultValue;
	Flags[current].Usage = usage;
	return &Flags[current].BoolValue;
}

internal void OS_FlagBoolVar(b32* ptr, String8 name, b32 defaultValue, String8 usage)
{
	u64 current = FlagCount++;
	Assert(current < ArrayCount(Flags));

	Flags[current].Type = FT_BoolPtr;
	Flags[current].Name = name;
	Flags[current].PtrValue = (ptr_value)ptr;
	Flags[current].Usage = usage;
	*ptr = defaultValue;
}

internal String8* OS_FlagString(String8 name, String8 defaultValue, String8 usage)
{
	u64 current = FlagCount++;
	Assert(current < ArrayCount(Flags));

	Flags[current].Type = FT_String;
	Flags[current].Name = name;
	Flags[current].StringValue = defaultValue;
	Flags[current].Usage = usage;
	return &Flags[current].StringValue;
}

internal void OS_FlagStringVar(String8* ptr, String8 name, String8 defaultValue, String8 usage)
{
	u64 current = FlagCount++;
	Assert(current < ArrayCount(Flags));

	Flags[current].Type = FT_StringPtr;
	Flags[current].Name = name;
	Flags[current].PtrValue = (ptr_value)ptr;
	Flags[current].Usage = usage;
	*ptr = defaultValue;
}

internal b8 OS_FlagParse(u64 argc, char** argv)
{
	b8 result = 1;
	
	String8 program_name = Str8C(argv[0]);
	UnusedVariable(program_name);
	
	for(u64 flagId = 0; flagId < FlagCount; flagId++)
	{
		Flag* flag = &Flags[flagId];
		for(u64 argIndex = 1; argIndex < argc; argIndex++)
		{
			String8 arg = Str8C(argv[argIndex]);
			String8 argName = Suffix8(arg, arg.Length - 2);
			
			if(!Str8Match(flag->Name, argName, MF_None)) continue;

			b8 hasNextIndex = (argIndex + 1) < argc;
			switch(flag->Type)
			{
				case(FT_Int):
				case(FT_IntPtr):
				{
					if(!hasNextIndex) return 0;
					String8 strValue = Str8C(argv[argIndex + 1]);
					u64 value = U64FromStr8(strValue);

					if(flag->Type == FT_Int) flag->IntValue = value;
					else *(u64*)flag->PtrValue = value;
				}break;

				case(FT_Float):
				case(FT_FloatPtr):
				{
					if(!hasNextIndex) return 0;
					String8 strValue = Str8C(argv[argIndex + 1]);
					f64 value = F64FromStr8(strValue);

					if(flag->Type == FT_Float) flag->FloatValue = value;
					else *(f64*)flag->PtrValue = value;
				}break;

				
				case(FT_Bool):
				case(FT_BoolPtr):
				{
					b32 value = 0;
					if(hasNextIndex)
					{
						String8 strValue = Str8C(argv[argIndex + 1]);						
						if(strValue.Str[0] == '-')
						{
							value = 1;
						}
						else
						{
							if(Str8Match(strValue, "true", MF_IgnoreCase))
								value = 1;
							else if(Str8Match(strValue, "false", MF_IgnoreCase))
								value = 0;
							else
								return 0;						
						}
					}
					else
					{
						value = 1;
					}
					
					if(flag->Type == FT_Bool) flag->BoolValue = value;
					else *(b32*)flag->PtrValue = value;
					
				}break;

				case(FT_String):
				case(FT_StringPtr):
				{
					if(!hasNextIndex) return 0;
					String8 strValue = Str8C(argv[argIndex + 1]);

					if(flag->Type == FT_String) flag->StringValue = strValue;
					else *(String8*)flag->PtrValue = strValue;
				}break;

				
				default:
				{
					Assert(0);
				}
			}
		}
	}

	return result;
}

internal String8 OS_FileExt(String8 path)
{
	String8 result = {0};
	u64 index = Str8FindLast(path, ".");
	if(index != path.Length)
	{
		result = Substr8(path, index, path.Length - index);
	}
	return result;
}

internal String8 OS_Dir(String8 path)
{
	String8 result = path;
	u64 index = Str8FindLast(path, OS_PATH_SEPARATOR);
	if(index != path.Length)
	{
		result = Substr8(path, 0, index);
	}
	return result;
}


internal String8Array OS_DirFiles(Arena* arena, String8 path)
{
	TempArena temp = GetScratch(arena);
	String8List list = {0};

	String8 search_path = OS_PathConcat(temp.Arena, path, "*");
	for(OS_FileIter iter = OS_FileIterMake(temp.Arena, search_path);
		OS_FileIterValid(iter);
		iter = OS_FileIterNext(temp.Arena,  iter))
	{
		Str8ListPush(temp.Arena, &list, iter.Info.Name);
	}

	String8Array result = Str8ListToArray(arena, list);	
	ReleaseScratch(temp);
	
	return result;
}

// NOTE(afb) :: Path
internal String8 OS_PathNormalize(Arena* arena, String8 file_path)
{	
	String8 result = {0};
	TempArena temp = GetScratch(arena);

	String8 path_sep = OS_PATH_SEPARATOR;
	String8 bad_path_sep = OS_WRONG_PATH_SEPARATOR;

	String8 path = Str8Replace(temp.Arena, file_path, bad_path_sep, path_sep);
	String8Array path_parts = Str8ListToArray(temp.Arena,
											  Str8Split(temp.Arena, path, path_sep));

	u32* parts_to_save = PushArray(temp.Arena, u32, path_parts.Count);
	u64 current = 0;
	b32 last_is_back = false;
	
	for(u32 i = 0; i < path_parts.Count; i++)
	{
		if(Str8Match("..", path_parts.Values[i], MF_None))
		{
			if(current && !last_is_back) current--;
			else
			{
				parts_to_save[current++] = i;
			}
			last_is_back = true;
		}
		else if(Str8Match(".", path_parts.Values[i], MF_None))
		{
			last_is_back = false;
			if((i+1) >= path_parts.Count) parts_to_save[current++] = i;
		}
		else
		{
			parts_to_save[current++] = i;
			last_is_back = false;
		}
	}

	String8List sb = {0};
	for(u32 i = 0; i < current; i++)
	{
		u32 index = parts_to_save[i];
		Str8ListPush(temp.Arena, &sb, path_parts.Values[index]);
	}

		// TODO(afb) :: Optimize for linux
#if OS_WINDOWS
	result = Str8Join(arena, sb, path_sep);
#else
	String8 temp_result = Str8Join(temp.Arena, sb, path_sep);
	result = Str8Concat(arena, "/", temp_result);
#endif
	
	ReleaseScratch(temp);
	return result;
}

internal String8 OS_PathConcat(Arena* arena, String8 a, String8 b) // TODO(afb) :: Varadic 
{
	TempArena temp = GetScratch(arena);
	if((a.Length && a.Str[a.Length-1] != OS_PATH_SEPARATOR[0]) &&
	   (b.Length && b.Str[0] != OS_PATH_SEPARATOR[0]))
		a = Str8Concat(temp.Arena, a, OS_PATH_SEPARATOR);

	String8 result = Str8Concat(temp.Arena, a, b);
	result = OS_PathNormalize(arena, result);
	ReleaseScratch(temp);
	return result;
}


internal b32 OS_PathExists(String8 path)
{
	OS_FileInfo info = OS_GetFileInfo(path);
	b32 result = IsDirectory(info.Flags);
	return result;
}

internal void OS_PathWalkHelper(String8 path, WalkFunc proc, void* obj)
{
	TempArena temp = GetScratch(0);

#if OS_WINDOWS
	String8 raw_path = Substr8(path, 0, path.Length-1);
#else
	String8 raw_path = path;
#endif
	
	for(OS_FileIter iter = OS_FileIterMake(temp.Arena, path);
		OS_FileIterValid(iter);
		iter = OS_FileIterNext(temp.Arena, iter))
	{
		OS_FileInfo info = iter.Info;
		String8 filename = OS_PathConcat(temp.Arena, raw_path, info.Name);

		if(Str8Match(info.Name, ".", MF_None) || Str8Match(info.Name, "..", MF_None))
			continue;

		proc(obj, filename, info);

		if(IsDirectory(info.Flags))
		{
			// String8 search_path = OS_PathConcat(temp.Arena, filename, "*");
			// OS_PathWalkHelper(search_path, proc, obj);
			OS_PathWalk(filename, proc, obj);
		}

	}

	ReleaseScratch(temp);
}

internal b32 OS_PathWalk(String8 path, WalkFunc proc, void* obj)
{
	if(!OS_PathExists(path))
	{
		return false;
	}
	TempArena temp = GetScratch(0);

#if OS_WINDOWS	
	String8 search_path = OS_PathConcat(temp.Arena, path, "*");
#else
	String8 search_path = path;
#endif
	OS_PathWalkHelper(search_path, proc, obj);

	ReleaseScratch(temp);
	return true;
}
