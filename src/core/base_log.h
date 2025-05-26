#ifndef BASE_LOG_H
#define BASE_LOG_H

typedef enum LogMsgKind
{
	LogMsgKind_Info,
	LogMsgKind_Warning,
	LogMsgKind_Error,
	LogMsgKind_Panic,
	LogMsgKind_Count,
} LogMsgKind;

struct Logger
{
	::Arena* Arena;

	String8 PreFormat;
	String8List Logs[LogMsgKind_Count];
};

internal Logger* LoggerInit(String8 pre_format);

internal void LogMsg(Logger* logger, LogMsgKind kind, String8 msg);
internal void LogMsgF(Logger* logger, LogMsgKind kind, String8 format, ...);

#define LogInfo(l, msg)		LogMsg(l, LogMsgKind_Info, msg)
#define LogInfoF(l, msg, ...)	LogMsgF(l, LogMsgKind_Info, format, __VA_ARGS__)

#define LogWarning(l, msg)	LogMsg(l, LogMsgKind_Warning, msg)
#define LogWarningF(l, msg, ...) LogMsgF(l, LogMsgKind_Warning, format, __VA_ARGS__)

#define LogError(l, msg)	LogMsg(l, LogMsgKind_Error, msg)
#define LogErrorF(l, msg, ...)	LogMsgF(l, LogMsgKind_Error, format, __VA_ARGS__)

#define LogPanic(l, msg)	LogMsg(l, LogMsgKind_Panic, msg)
#define LogPanicF(l, msg, ...)	LogMsgF(l, LogMsgKind_Panic, format, __VA_ARGS__)

#endif // Header guard
