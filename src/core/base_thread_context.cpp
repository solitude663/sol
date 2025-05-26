
per_thread ThreadContext* ThreadCTX;

internal ThreadContext TCTXAlloc()
{
	ThreadContext context = {0};
	for(u32 threadId = 0; threadId < ArrayCount(context.ScratchArenas); threadId++)
	{
		context.ScratchArenas[threadId] = ArenaAlloc(GB(8));
	}
	return context;
}

internal void TCTXRelease(ThreadContext* context)
{
	for(u32 threadId = 0; threadId < ArrayCount(context->ScratchArenas); threadId++)
	{
		ArenaFree(context->ScratchArenas[threadId]);
	}
}

internal ThreadContext* GetThreadContext()
{
	return ThreadCTX;
}

internal void SetThreadContext(ThreadContext* threadContext)
{
	ThreadCTX = threadContext;
}

internal TempArena GetScratch(Arena* conflict)
{
	TempArena result = {0};
	
	ThreadContext* tc = GetThreadContext();
	for(u64 scratchId = 0; scratchId < ArrayCount(tc->ScratchArenas); scratchId++)
	{
		if(tc->ScratchArenas[scratchId] != conflict)
		{
			result = TempArenaBegin(tc->ScratchArenas[scratchId]);
			break;
		}
	}
	
	return result;
}
