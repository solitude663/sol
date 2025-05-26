#include "core/base_inc.h"
#include "core/base_inc.cpp"

internal void MainEntry(i32 argc, char** argv)
{
	if(argc != 2)
	{
		printf("usage %s <file>\n", argv[0]);
		return;
	}
	String8 filename = Str8C(argv[1]);

	Arena* arena = ArenaAllocDefault();
	String8 contents = OS_FileReadAll(arena, filename);
	printf("%.*s\n", Str8Print(contents));
}

int main(int argc, char** argv)
{
	BaseMainThreadEntry(MainEntry, argc, argv);
	return 0;
}
