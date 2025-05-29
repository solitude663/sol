#include "core/base_inc.h"
#include "core/base_inc.cpp"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "parser.h"
#include "parser.cpp"

internal void PrintTokens(Parser* parser)
{
	for(;;)
	{
		Token t = NextToken(parser->Lexer);
		if(t.Type == Token_EOF && t.Type != Token_Invalid) break;
		
		printf("%.*s\n", Str8Print(t.Lexeme));
	}	
}

internal void PrintAST(ASTNode* node, i32 indent_level = 0)
{
	for(i32 i = 0; i < indent_level; i++) printf("  ");
	
	switch(node->Type)
	{
		case(AST_Primary):
		{
			printf("PRIMARY %.*s\n", Str8Print(node->Primary.Value.Lexeme));
		}break;
		
		case(AST_Binary):
		{
			printf("BINARY %.*s\n", Str8Print(node->Binary.Operator.Lexeme));
			PrintAST(node->Binary.Left, indent_level+1);
			PrintAST(node->Binary.Right, indent_level+1);
		}break;

		case(AST_Print):
		{
			printf("PRINT\n");
			PrintAST(node->Print.Expr, indent_level+1);
		}break;
		
		default:
		{			
			Assert(0 && "PrintAST unknown node tyep");
		}
	}
}


internal void ParseFile(Arena* arena, String8 filename)
{   
	String8 file_contents = OS_FileReadAll(arena, filename);

	Lexer lexer = {0};
	lexer.Filename = filename;
	lexer.Data = file_contents;

	Parser parser = {0};
	parser.Lexer = &lexer;

	printf("Parsing %.*s...\n", Str8Print(filename));

#if 0
	PrintTokens(&parser);
#else
	ASTNode** program = 0;
	for(;;)
	{
		Token t = PeekToken(&lexer);
		if(t.Type == Token_EOF) break;
		if(t.Type == Token_Invalid) Assert(0);

		
		ASTNode* stmt = ParseStatement(arena, &parser);
		arrput(program, stmt);
	}
#endif
}

internal void MainEntry(i32 argc, char** argv)
{
	if(argc != 2)
	{
		printf("usage %s <file>\n", argv[0]);
		return;
	}

	Arena* arena = ArenaAllocDefault();
	String8 filename = Str8C(argv[1]);
	ParseFile(arena, filename);
}

int main(int argc, char** argv)
{
	BaseMainThreadEntry(MainEntry, argc, argv);
	return 0;
}
