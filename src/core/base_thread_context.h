#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

struct ThreadContext
{
	::Logger* Logger;
	Arena *ScratchArenas[2];
	b32 IsMainThread;
};

internal ThreadContext  TCTXAlloc();
internal void           TCTXRelease(ThreadContext* threadContext);
internal ThreadContext* GetThreadContext();
internal void           SetThreadContext(ThreadContext thtreadContext);

internal TempArena GetScratch(Arena* conflict); // TODO(afb) :: Add variable conflict count
#define ReleaseScratch(temp) TempArenaEnd(temp)


#endif // Header guard
