internal void BaseMainThreadEntry(void (*entry)(int, char**), int argc, char **argv)
{
	ThreadContext ctx = TCTXAlloc();
	ctx.IsMainThread = 1;
	ctx.Logger = LoggerInit("");
	SetThreadContext(&ctx);
	
	// TODO(afb) :: OS_Init
	
	entry(argc, argv);
	TCTXRelease(&ctx);  
}

internal void BaseThreadEntry(void (*entry)(void *p), void *params)
{
	ThreadContext ctx = TCTXAlloc();
	SetThreadContext(&ctx);
	entry(params);
	TCTXRelease(&ctx);
}
