#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

internal i32 OS_LinuxGetHandle(OS_Handle handle)
{
	i32 result = 0;
	result = (i32)(sptr_value)handle.Handle;
	return result;
}

internal void* OS_Reserve(u64 size)
{
	void* result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS,
						-1, 0);
   	return result;
}

internal void OS_Commit(void* base, u64 size)
{
	mprotect(base, size, PROT_READ | PROT_WRITE);
}

internal void OS_Release(void* base, u64 size)
{
	munmap(base, size);
}

internal OS_Handle OS_FileOpen(String8 path, u64 flags)
{
	OS_Handle result = {0};
	TempArena scratch = GetScratch(0);

	u64 access_flags = 0;
	if((flags & OS_AccessFlag_Read) && (flags & OS_AccessFlag_Write))
		access_flags = O_RDWR;
	else if(flags & OS_AccessFlag_Read)
		access_flags = O_RDONLY;
	else if(flags & OS_AccessFlag_Write)
		access_flags = O_WRONLY;
	
	u64 creation_flags = flags & OS_AccessFlag_CreateNew ? (O_TRUNC | O_CREAT): O_APPEND;
	u64 mode = 0;
	if(flags & OS_AccessFlag_CreateNew)
	{
		if(flags & OS_AccessFlag_Read && flags & OS_AccessFlag_Write)
			mode = S_IRWXU;	
		else if(flags & OS_AccessFlag_Write)
			mode = S_IWUSR;
		else if(flags & OS_AccessFlag_Read)
			mode = S_IRUSR;
	}
	
	char* c_path = ToCString(scratch.Arena, path);   
	i32 file_handle = open(c_path, access_flags | creation_flags, mode);
	result.Handle = file_handle < 0 ? (ptr_value)(sptr_value)file_handle : file_handle;
	
	ReleaseScratch(scratch);
	return result;
}

internal void OS_FileClose(OS_Handle file)
{
	i32 handle_value = (i32)(sptr_value)(ptr_value)file.Handle;
	close(handle_value);
}

internal String8 OS_FileRead(Arena* arena, OS_Handle handle, u64 start, u64 count)
{
	String8 result = EmptyString;
	
	i32 file = OS_LinuxGetHandle(handle);

	struct stat stats = {0};	
	if((file != -1) && (fstat(file, &stats) == 0))
	{
		off_t file_size = stats.st_size;	

		off_t bytes_advanced = lseek(file, start, SEEK_SET);
		if(bytes_advanced > -1)
		{
			u64 end = Min((start+count), (u64)file_size);
			u64 bytes_to_read = end - bytes_advanced;
			result.Str = PushArray(arena, u8, bytes_to_read);

			ssize_t bytes_read = read(file, result.Str, bytes_to_read);
			if(bytes_read != -1)
			{
				result.Length = bytes_read;
			}
			// TODO(afb) :: Reset allocation on failure

			lseek(file, 0, SEEK_SET);
		}
	}

	return result;
}


internal String8 OS_FileReadAll(Arena* arena, String8 path)
{
	String8 result = {0};
	TempArena scratch = GetScratch(arena);

	char* c_path = ToCString(scratch.Arena, path);

	i32 file = open(c_path, O_RDONLY, 0);
	if(file >= 0)
	{
		struct stat stats = {0};	
		fstat(file, &stats);
		off_t file_size = stats.st_size;	

		result.Str = PushArray(arena, u8, file_size);
		ssize_t bytes_read = read(file, result.Str, file_size);
		result.Length = bytes_read;

		// TODO(afb) :: Release memory if read fails
		close(file);
	}
	
	ReleaseScratch(scratch);
	return result;
}

internal b32 OS_FileWrite(OS_Handle file, String8 data, u64 offset)
{
	b32 result = 0;
	if(OS_FileIsValid(file))
	{
		i32 file_handle = OS_LinuxGetHandle(file);

		lseek(file_handle, offset, SEEK_SET);
		result = write(file_handle, data.Str, data.Length) != -1;
	}

	return result;
}

internal b32 OS_FileWrite(OS_Handle file, String8List data, u64 offset)
{
	b32 result = 0;
	if(OS_FileIsValid(file))
	{
		i32 file_handle = OS_LinuxGetHandle(file);

		lseek(file_handle, offset, SEEK_SET);

		result = true;
		for(String8Node* node = data.First; node != 0; node = node->Next)
		{
			String8 str = node->Str;
			result = result && (write(file_handle, str.Str, str.Length) != -1);
		}
	}

	return result;
}


internal b32 OS_FileCopy(String8 dest_path, String8 src_path, b32 replace)
{
	b32 result = 0;
	TempArena temp = GetScratch(0);
	char* src = ToCString(temp.Arena, src_path);
	char* dest = ToCString(temp.Arena, dest_path);
	
	i32 src_file = open(src, O_RDONLY);
    i32 dest_file = open(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (src_file == -1 || dest_file == -1)
	{
		off_t offset_in = 0, offset_out = 0;
		ssize_t bytes_copied;
		while ((bytes_copied = copy_file_range(src_file, &offset_in,
											   dest_file, &offset_out, 4096, 0)) > 0);

		result = bytes_copied >= 0;
	
		close(src_file);
		close(dest_file);
	}	
	ReleaseScratch(temp);
	return result;
}

internal b32 OS_FileMove(String8 dest_path, String8 src_path, b32 replace = true)
{
	UnusedVariable(replace);
	TempArena temp = GetScratch(0);
	char* src = ToCString(temp.Arena, src_path);
	char* dest = ToCString(temp.Arena, dest_path);
	
	b32 result = rename(src, dest) == 0;
	ReleaseScratch(temp);
	return result;
}

internal b32 OS_FileDelete(String8 path)
{
	b32 result = 0;
	TempArena temp = GetScratch(0);
	char* c_path = ToCString(temp.Arena, path);
	result = unlink(c_path) == 0;

	ReleaseScratch(temp);
	return result;
}

internal u64 OS_GetFileSize(String8 path)
{
	u64 result = 0;
	TempArena temp = GetScratch(0);
	char* c_path = ToCString(temp.Arena, path);

	i32 file = open(c_path, O_RDONLY);
	if(file >= 0)
	{
		struct stat stats = {0};	
		fstat(file, &stats);
		result = stats.st_size;
		close(file);
	}	
	ReleaseScratch(temp);
	return result;
}

// TODO(afb) :: Is there a way to do this without creating a handle
internal b32 OS_FileExists(String8 path)
{
	b32 result;
	TempArena temp = GetScratch(0);
	char* c_path = ToCString(temp.Arena, path);
	result = access(c_path, F_OK) == 0;	
	ReleaseScratch(temp);
	return result;
}

internal b32 OS_FileIsValid(OS_Handle file)
{
	b32 result = OS_LinuxGetHandle(file) >= 0;
	return result;
}

internal b32 OS_DirCreate(String8 path)
{
	b32 result;
	TempArena temp = GetScratch(0);
	char* c_path = ToCString(temp.Arena, path);
	result = mkdir(c_path, 0755) == 0;		
	ReleaseScratch(temp);
	return result;
}

internal OS_FileInfo OS_GetFileInfo(String8 path)
{
	OS_FileInfo result = {0};	
	struct stat file_stats = {0};
	
	OS_Handle file_handle = OS_FileOpen(path, OS_AccessFlag_Read);
	i32 handle = OS_LinuxGetHandle(file_handle);
	if(fstat(handle, &file_stats) == 0)
	{
		result.Name = path;
		result.FileSize = file_stats.st_size;
		result.Flags = S_ISDIR(file_stats.st_mode) ? OS_FileFlag_Directory : OS_FileFlag_None;
	}
	OS_FileClose(file_handle);

	return result;
}

internal OS_FileIter OS_FileIterMake(Arena* arena, String8 path)
{
	TempArena temp = GetScratch(arena);

	OS_FileIter result = {0};
	
	char* c_path = ToCString(temp.Arena, path);
	DIR* dir_handle = opendir(c_path);	
	if(dir_handle)
	{
		result.Handle.Handle = (ptr_value)dir_handle;

		struct dirent* data = readdir(dir_handle);
		
		result.Info.Name = Str8Copy(arena, Str8C(data->d_name));
		result.Info.FileSize = OS_GetFileSize(result.Info.Name);
		result.Info.Flags = (data->d_type == DT_DIR) ? OS_FileFlag_Directory : OS_FileFlag_None;
	}
	
	ReleaseScratch(temp);
	return result;
}

internal OS_FileIter OS_FileIterNext(Arena* arena, OS_FileIter iter)
{
	DIR* dir_handle = (DIR*)iter.Handle.Handle;
	if(dir_handle)
	{
		struct dirent* data = readdir(dir_handle);
		if(data)
		{
			iter.Info.Name = Str8Copy(arena, Str8C(data->d_name));
			iter.Info.FileSize = OS_GetFileSize(iter.Info.Name);
			iter.Info.Flags = (data->d_type == DT_DIR) ? OS_FileFlag_Directory : OS_FileFlag_None;
		}
		else
		{
			closedir(dir_handle);
			iter.Handle.Handle = 0;
		}
	}
	return iter;
}

internal void OS_FileIterClose(OS_FileIter iter)
{
	DIR* dir_handle = (DIR*)iter.Handle.Handle;
	closedir(dir_handle);
}

internal b32 OS_FileIterValid(OS_FileIter iter)
{
	return iter.Handle.Handle != 0;
}
#if 0
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
#endif
