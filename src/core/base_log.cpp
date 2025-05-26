
global u32 ANSI_BG_BLACK	= 40;
global u32 ANSI_BG_RED		= 41;
global u32 ANSI_BG_GREEN	= 42;
global u32 ANSI_BG_YELLOW	= 43;
global u32 ANSI_BG_BLUE		= 44;
global u32 ANSI_BG_CYAN		= 46;
global u32 ANSI_BG_WHITE	= 47;

global u32 ANSI_FG_BLACK	= 30;
global u32 ANSI_FG_RED		= 31;
global u32 ANSI_FG_GREEN	= 32;
global u32 ANSI_FG_YELLOW	= 33;
global u32 ANSI_FG_BLUE		= 34;
global u32 ANSI_FG_CYAN		= 36;
global u32 ANSI_FG_WHITE	= 37;

global u32 ANSI_FG_B_RED	= 91;

internal Logger* LoggerInit(String8 pre_format)
{	
	Arena* arena = ArenaAlloc(MB(32));
	Logger* logger = PushStruct(arena, Logger);
	logger->Arena = arena;
	logger->PreFormat = pre_format;
	return logger;
}

internal void LogMsg(Logger* logger, LogMsgKind kind, String8 msg)
{
	if(!logger)
	{
		ThreadContext* ctx = GetThreadContext();
		logger = ctx->Logger;
	}

	TempArena temp = GetScratch(0);

	String8 log = {0};
	if(kind == LogMsgKind_Info)
		log = Str8Format(temp.Arena, "\x1b[%dm[INFO]\x1b[0m %S %S", ANSI_FG_GREEN, logger->PreFormat, msg);
	else if(kind == LogMsgKind_Warning)
		log = Str8Format(temp.Arena, "\x1b[%dm[WARN]\x1b[0m %S %S", ANSI_FG_YELLOW, logger->PreFormat, msg);
	else if(kind == LogMsgKind_Error)
		log = Str8Format(temp.Arena, "\x1b[%dm[ERROR]\x1b[0m %S %S", ANSI_FG_B_RED, logger->PreFormat, msg);
	else if(kind == LogMsgKind_Panic)
		log = Str8Format(temp.Arena, "\x1b[%dm[PANIC]\x1b[0m %S %S", ANSI_FG_RED, logger->PreFormat, msg);		
	else Assert(0);
	
	Str8ListPush(logger->Arena, &(logger->Logs[kind]), log);
	printf("%.*s\n", Str8Print(log));

	// TODO(afb) :: Create OS_Exit();
	if(kind == LogMsgKind_Panic) exit(1);
	ReleaseScratch(temp);
	
}
// internal void LogMsgF(Logger* logger, LogMsgKind kind, String8 format, ...);
