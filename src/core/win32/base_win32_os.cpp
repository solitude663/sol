#include <windows.h>

internal void* OS_Reserve(u64 size)
{
	void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
	return result;
}

internal void OS_Commit(void* base, u64 size)
{
	VirtualAlloc(base, size, MEM_COMMIT, PAGE_READWRITE);
}

internal void OS_Release(void* basem u64 size = 0)
{
	UnusedVariable(0);
	VirtualFree(base, 0, MEM_RELEASE);
}

internal OS_Handle OS_FileOpen(String8 path, u64 flags)
{
	OS_Handle result = {0};
	TempArena scratch = GetScratch(0);

	DWORD accessFlags = 0;
	accessFlags |= flags & OS_AccessFlag_Read ? GENERIC_READ : 0;
	accessFlags |= flags & OS_AccessFlag_Write ? GENERIC_WRITE : 0;
	DWORD shareFlags = 0;
	shareFlags |= flags & OS_AccessFlag_Shared ? FILE_SHARE_READ : 0;
	DWORD creationFlags = flags & OS_AccessFlag_CreateNew ? CREATE_ALWAYS : OPEN_EXISTING;
	
	char* cPath = ToCString(scratch.Arena, path);   
	HANDLE fileHandle = CreateFileA(cPath, accessFlags, shareFlags, 0, creationFlags, 0, 0);
	result.Handle = fileHandle != INVALID_HANDLE_VALUE ? (ptr_value)fileHandle : 0;
	
	ReleaseScratch(scratch);
	return result;
}

internal void OS_FileClose(OS_Handle file)
{
	CloseHandle((HANDLE)file.Handle);
}

internal String8 OS_FileRead(Arena* arena, OS_Handle handle, u64 start, u64 count)
{
	String8 result = {0};
	
	HANDLE file = (HANDLE)handle.Handle;
	LARGE_INTEGER fileSize;
	if(GetFileSizeEx(file, &fileSize) && file != 0)
	{
		LARGE_INTEGER bytesAdvanced;
		LARGE_INTEGER bytesToAdvance;
		bytesToAdvance.QuadPart = start;
		SetFilePointerEx(file, bytesToAdvance, &bytesAdvanced, FILE_BEGIN);
		if(bytesToAdvance.QuadPart == bytesAdvanced.QuadPart)
		{
			u64 end = Min((start+count), (u64)fileSize.QuadPart);
			DWORD bytesToRead = (DWORD)end - (DWORD)bytesAdvanced.QuadPart;
			result.Str = PushArray(arena, u8, bytesToRead);
			
			DWORD bytesRead;
			if(ReadFile(file, result.Str, bytesToRead, &bytesRead, 0))
			{
				result.Length = bytesRead;
			}
			// TODO(afb) :: Reset allocation on failure

			bytesToAdvance.QuadPart = 0;
			SetFilePointerEx(file, bytesToAdvance, 0, FILE_BEGIN);
		}
	}

	return result;
}

internal String8 OS_FileReadAll(Arena* arena, String8 path)
{
	String8 result = {0};
	TempArena scratch = GetScratch(arena);

	char* cPath = ToCString(scratch.Arena, path);

	HANDLE fileHandle = CreateFileA(cPath, GENERIC_READ,
									0, 0, OPEN_EXISTING, 0, 0);
	if(fileHandle)
	{
		LARGE_INTEGER fileSize;
		GetFileSizeEx(fileHandle, &fileSize);

		result.Str = PushArray(arena, u8, fileSize.LowPart);
		
		DWORD bytesRead = 0;
		BOOL readSuccess = ReadFile(fileHandle, result.Str, fileSize.LowPart, &bytesRead, 0);
		result.Length = bytesRead;

		// TODO(afb) :: Release memory if read fails
		result.Length = readSuccess ? bytesRead : 0;
		CloseHandle(fileHandle);
	}
	
	ReleaseScratch(scratch);
	return result;
}

internal b32 OS_FileWrite(OS_Handle file, String8 data, u64 offset)
{
	b32 result = 0;
	if(file.Handle != 0)
	{
		HANDLE fileHandle = (HANDLE)file.Handle;

		LARGE_INTEGER off;
		off.QuadPart = offset;
		if(SetFilePointerEx(fileHandle, off, 0, FILE_BEGIN))
		{
			if(WriteFile(fileHandle, data.Str, (DWORD)data.Length, 0, 0))
			{
				 result = 1;
			}
		}
	}

	return result;
}

internal b32 OS_FileWrite(OS_Handle file, String8List data, u64 offset)
{
	b32 result = 0;
	if(file.Handle != 0)
	{
		HANDLE fileHandle = (HANDLE)file.Handle;
		
		LARGE_INTEGER off;
		off.QuadPart = offset;
		if(SetFilePointerEx(fileHandle, off, 0, FILE_BEGIN))
		{
			result = 1;
			for(String8Node* node = data.First; node != 0; node = node->Next)
			{
				if(!WriteFile(fileHandle, node->Str.Str, (DWORD)node->Str.Length, 0, 0))
				{
					result = 0;
				}
			}
		}
	}

	return result;
}


internal b32 OS_FileCopy(String8 destPath, String8 srcPath, b32 replace)
{
	TempArena scratch = GetScratch(0);
	char* src = ToCString(scratch.Arena, srcPath);
	char* dest = ToCString(scratch.Arena, destPath);

	b32 result = CopyFile(src, dest, replace);
	ReleaseScratch(scratch);
	return result;
}

internal b32 OS_FileMove(String8 destPath, String8 srcPath, b32 replace = true)
{
	TempArena scratch = GetScratch(0);
	char* src = ToCString(scratch.Arena, srcPath);
	char* dest = ToCString(scratch.Arena, destPath);

	u32 flags = MOVEFILE_COPY_ALLOWED;
	flags |= replace ? MOVEFILE_REPLACE_EXISTING : 0;
	b32 result = MoveFileEx(src, dest, flags);
	ReleaseScratch(scratch);
	return result;
}

internal b32 OS_FileDelete(String8 path)
{
	b32 result = 1;
	TempArena scratch = GetScratch(0);
	char* cPath = ToCString(scratch.Arena, path);
	BOOL success = DeleteFile(cPath);

	if(!success)
	{
		result = GetLastError() != ERROR_ACCESS_DENIED;
	}

	ReleaseScratch(scratch);
	return result;
}

internal u64 OS_GetFileSize(String8 path)
{
	u64 result = 0;
	TempArena scratch = GetScratch(0);
	char* cStr = ToCString(scratch.Arena, path);

	WIN32_FILE_ATTRIBUTE_DATA data;
	if(GetFileAttributesExA(cStr, GetFileExInfoStandard, &data))
	{
		result = ((u64)data.nFileSizeHigh << 32) | data.nFileSizeLow;
	}

	ReleaseScratch(scratch);
	return result;
}

// TODO(afb) :: Is there a way to do this without creating a handle
internal b32 OS_FileExists(String8 path)
{
	TempArena scratch = GetScratch(0);
	char* cPath = ToCString(scratch.Arena, path);
	HANDLE fileHandle = CreateFileA(cPath, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	b32 result = fileHandle != INVALID_HANDLE_VALUE;
	CloseHandle(fileHandle);
	ReleaseScratch(scratch);
	return result;
}

internal b32 OS_FileIsValid(OS_Handle file)
{
	b32 result = file.Handle != 0;
	return result;
}

internal b32 OS_DirCreate(String8 path)
{
	b32 result = 1;
	TempArena scratch = GetScratch(0);
	char* cPath = ToCString(scratch.Arena, path);
	BOOL success = CreateDirectory(cPath, 0);
	if(!success)
	{
		result = GetLastError() != ERROR_PATH_NOT_FOUND;
	}
	
	ReleaseScratch(scratch);
	return result;
}

internal OS_FileIter OS_FileIterMake(Arena* arena, String8 path)
{
	TempArena scratch = GetScratch(arena);

	OS_FileIter result = {0};
	
	char* cPath = ToCString(scratch.Arena, path);
	WIN32_FIND_DATA data;
	HANDLE iterHandle = FindFirstFileExA(cPath, FindExInfoBasic, &data, FindExSearchNameMatch,
										 0, FIND_FIRST_EX_CASE_SENSITIVE);
	
	if(iterHandle != INVALID_HANDLE_VALUE)
	{
		OS_Handle handle = {(u64)iterHandle};
		result.Handle = {handle};
		result.Info.Name = Str8Copy(arena, Str8C(data.cFileName));
		result.Info.FileSize = ((u64)data.nFileSizeHigh << 32) | data.nFileSizeLow;
		result.Info.Flags = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? OS_FileFlag_Directory : OS_FileFlag_None;
	}
	
	ReleaseScratch(scratch);
	return result;
}

internal OS_FileIter OS_FileIterNext(Arena* arena, OS_FileIter iter)
{
	if(iter.Handle.Handle != 0)
	{
		WIN32_FIND_DATA data;
		HANDLE handle = (HANDLE)iter.Handle.Handle;
		if(FindNextFileA(handle, &data))
		{
			iter.Info.Name = Str8Copy(arena, Str8C(data.cFileName));
			iter.Info.FileSize = ((u64)data.nFileSizeHigh << 32) | data.nFileSizeLow;
			iter.Info.Flags = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? OS_FileFlag_Directory : OS_FileFlag_None;
		}
		else
		{
			OS_FileIterClose(iter);
			iter.Handle.Handle = 0;
		}
	}

	return iter;
}

internal void OS_FileIterClose(OS_FileIter iter)
{
	FindClose((HANDLE)iter.Handle.Handle);
}

internal b32 OS_FileIterValid(OS_FileIter iter)
{
	return iter.Handle.Handle != 0;
}

internal OS_FileInfo OS_GetFileInfo(String8 path)
{
	OS_FileInfo result = {0};
	TempArena scratch = GetScratch(0);
	char* cPath = ToCString(scratch.Arena, path);

	WIN32_FILE_ATTRIBUTE_DATA data;
	if(GetFileAttributesExA(cPath, GetFileExInfoStandard, &data))
	{
		result.Name = path;
		result.FileSize = ((u64)data.nFileSizeHigh << 32) | data.nFileSizeLow;
		result.Flags = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? OS_FileFlag_Directory : OS_FileFlag_None;
	}
	ReleaseScratch(scratch);
	return result;
}

internal String8 OS_RunCommand(Arena* arena, String8 commandToExecute)
{
	TempArena temp = GetScratch(arena);

	String8 result = {0};
		
	char inputBuffer[4096];
	DWORD bytesRead = 0;

	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	HANDLE readPipe, writePipe;	
	if(!CreatePipe(&readPipe, &writePipe, &sa, 0))
	{
		// fprintf(stderr, "Failed to create pipe\n");
		return String8{0}; // TODO(afb) :: Create a constant empty string
	}

	char* command = ToCString(temp.Arena, Str8Format(temp.Arena,
													 "cmd.exe /c %S",
													 commandToExecute));

	PROCESS_INFORMATION pi = {0};
	STARTUPINFOA si = {0};
	si.hStdOutput = writePipe;
	si.hStdError = writePipe;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.dwFlags = STARTF_USESTDHANDLES;	
	if(!CreateProcessA(0, command, 0, 0, TRUE, 0, 0, 0, &si, &pi))
	{
		// fprintf(stderr, "Failed to create process(%d)\n", GetLastError());
		return String8{0}; // TODO(afb) :: Create a constant empty string
	}
	CloseHandle(writePipe);

	String8List sb = {0};
	while(ReadFile(readPipe, inputBuffer, sizeof(inputBuffer) - 1, &bytesRead, 0))
	{
		String8 output = Str8Copy(temp.Arena, Str8((u8*)inputBuffer, bytesRead));
		Str8ListPush(temp.Arena, &sb, output);
	}
	result = Str8Join(arena, sb, "");

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(readPipe);

	ReleaseScratch(temp);
	return result;
}
