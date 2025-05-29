#ifndef PARSER_H
#define PARSER_H

enum ASTType
{
	AST_Invalid, // NOTE(afb) :: For testing
	
	AST_Primary,
	AST_Binary,
	AST_Unary,

	AST_Print,
	AST_Declaration, // TODO
	AST_Assignment,
	AST_Block,
	AST_If,
	AST_While,

	AST_Function,
	AST_Function_Body,
	AST_Function_Decl,
	AST_Basic_Block,
	AST_Return,
	AST_Function_Call,
};

enum TokenType
{
	Token_Invalid = 0,
	Token_EOF = 1,

	Token_LBrace = '{',
	Token_RBrace = '}',

	Token_LParen = '(',
	Token_RParen = ')',

	Token_Plus = '+',
	Token_Minus = '-',
	Token_Star = '*',
	Token_Slash = '/',	

	Token_Equal = '=',	
	
	Token_Percent = '%',
	Token_Pipe = '|',
	Token_And = '&',
	Token_UpArrow = '^',
	Token_Tilda = '~',

	Token_Colon = ':',
	Token_SemiColon = ';',

	Token_GT = '>',
	Token_LT = '<',
	Token_Not = '!',
	
	Token_Barrier = 300,
#define TOKEN_DEF(type, str) Token_##type,
#include "token.def"
#undef TokenDef

	Token_Integer,
	Token_Identifier,
	Token_Print,	
};

struct Token
{
	enum TokenType Type;
	String8 Lexeme;
	u32 Row;
	u32 Col;

	i64 Value;
};

struct Lexer
{
	String8 Filename;
	String8 Data;
	
	u64 CurrentOffset;

	u32 RowNumber;
	u32 ColNumber;

#define TOKEN_RING_BUFFER_SIZE 8
	Token TokenRingBuffer[TOKEN_RING_BUFFER_SIZE];
	i32 LiveTokens;
	u32 CurrentTokenIndex;
};

struct Parser
{
	Arena* Arena;
	Lexer* Lexer;
};

enum TypeKind
{
	Type_Void,
	Type_Int,
};

struct Type;
struct Type
{
	TypeKind Kind;
	Type* Base;
};

struct ASTNode;

struct ASTPrimary
{
	Token Value;
};

struct ASTBinary
{
	ASTNode* Left;
	ASTNode* Right;
	Token Operator;
};

struct ASTPrint
{
	ASTNode* Expr;
};

struct ASTAssignment
{
	String8 Id;
	ASTNode* Expr;
};

struct ASTIf
{
	ASTNode* Cond;
	ASTNode* Then;
	ASTNode* Else;
};

struct ASTWhile
{
	ASTNode* Cond;
	ASTNode* Body;
};

struct ASTBlock
{
	ASTNode** Stmts;
	u64 Count;
};

struct ASTFunction
{
	Token Name;
	Type* Return;
	ASTNode* Body;
};


struct ASTNode
{
	ASTType Type;

	union
	{
		ASTPrimary Primary;
		ASTBinary Binary;
		ASTPrint Print;
		ASTAssignment Assign;
		ASTIf If;
		ASTBlock Block;
		ASTWhile While;
		ASTFunction FDecl;
		
		// ASTUnary Unary;
		// ASTBlock Block;
		// ASTPrint Print;
		// ASTBinding Binding;
		// ASTReturn Return;
		// ASTCall Call;
	};
};

#endif
