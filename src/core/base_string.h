#ifndef BASE_STRING_H
#define BASE_STRING_H

#define Str8Print(str) (i32)(str).Length, (str).Str

internal u64 CStringLength(char* cstr);

struct String8
{
	u8* Str;
	u64 Length;

	String8() = default;

	inline String8(char* cstr)
	{
		Str = (u8*)cstr;
		Length = CStringLength(cstr);
	}

	inline String8(u8* str, u64 len)
	{
		Str = str;
		Length = len;
	}
};

struct String8Node
{
	String8Node* Next;
	String8 Str;
};

struct String8List
{
	String8Node* First;
	String8Node* Last;
	u64 NodeCount;
	u64 Length;
};

struct String8Array
{
	String8* Values;
	u64 Count;
};

enum MatchFlags
{
	MF_None = (0),
	MF_IgnoreCase = (1 << 0),
};

internal b8 IsAlpha(u8 c);
internal b8 IsUpper(u8 c);
internal b8 IsLower(u8 c);
internal b8 IsDigit(u8 c);
internal b8 IsSpace(u8 c);
internal b8 IsSymbol(u8 c);

internal u8 ToUpper(u8 c);
internal u8 ToLower(u8 c);

// internal u64 CStringLength(char* cstr);

// NOTE(afb) :: Constructors
internal String8 Str8(u8* str, u64 size);
#define Str8C(cstr) {(u8*)(cstr), CStringLength(cstr)}
// #define Str8Lit(str) Str8((u8*)str, sizeof(str) - 1)
// #define Str8Lit(str) {(u8*)str, sizeof(str) - 1}

// NOTE(afb) :: Manipulations
#define Prefix8(str, count) Substr8(str, 0, count)
#define Suffix8(str, count) Substr8(str, (str).Length - (count), (count))
internal String8 Substr8(String8 str, u64 start, u64 count);
internal String8 Trim8(String8 str, String8 bad);
internal String8 Trim8Front(String8 str, String8 bad);
internal String8 Trim8Back(String8 str, String8 bad);
internal String8 Trim8Space(String8 str);

// NOTE(afb) :: Comparison
internal b8 Str8Match(String8 a, String8 b, MatchFlags flags);
internal b8 operator==(String8& a, String8& b);
internal u64 Str8Find(String8 str, String8 search); // TODO(afb) :: Match flags
internal u64 Str8FindLast(String8 str, String8 search); // TODO(afb) :: Match flags

// NOTE(afb) :: Allocation
internal String8 Str8Copy(Arena* arena, String8 a);
internal String8 Str8Fill(Arena* arena, u8 byte, u64 size);
internal String8 Str8Concat(Arena* arena, String8 a, String8 b);
internal String8 Str8Format(Arena* arena, String8 format, ...);
internal String8 Str8FormatExplicit(Arena* arena, String8 format, va_list args);

// TODOa(afb) :: Rename to Str8Replace
internal String8 Str8Replace(Arena* arena, String8 str, String8 id, String8 target);
internal char*   ToCString(Arena* arena, String8 str);

// NOTE(afb) :: String8List
#define Str8ListPushF(arena, sb, format, ...) Str8ListPush((arena), (sb), Str8Format((arena), (format), __VA_ARGS__))
internal void Str8ListPushNode(String8List* list, String8Node* node);
internal void Str8ListPushNodeFront(String8List* list, String8Node* node);
internal void Str8ListPush(Arena* arena, String8List* list, String8 str);
internal void Str8ListPushFront(Arena* arena, String8List* list, String8 str);

internal String8 Str8Join(Arena* arena, String8List list, String8 join);
internal String8List Str8Split(Arena* arena, String8 str, String8 split);
internal String8List Str8SplitRemoveEmpty(Arena* arena, String8 str, String8 split);

// NOTE(afb) :: String8Array
internal String8Array Str8ListToArray(Arena* arena, String8List list);

// NOTE(afb) :: Conversions
// TODO(afb) :: Rename Str8ToU32
internal u64 U64FromStr8(String8 str);
internal f64 F64FromStr8(String8 str);

const String8 EmptyString = {(u8*)"", 0};
#endif // Header guard
