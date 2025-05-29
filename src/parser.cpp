inline b8 IsAtEnd(Lexer* lexer)
{	
	return !(lexer->CurrentOffset < lexer->Data.Length);
}

inline u8 CurrentChar(Lexer* lexer)
{	
	return lexer->Data.Str[lexer->CurrentOffset];
}

internal enum TokenType GetTokenTypeOrIdentifier(String8 lexeme)
{
	enum TokenType result = Token_Identifier;
	if(Str8Match(lexeme, "print", MF_None))
		result = Token_Print;
	if(Str8Match(lexeme, "int", MF_None))
		result = Token_Int;
	if(Str8Match(lexeme, "void", MF_None))
		result = Token_Void;
	if(Str8Match(lexeme, "if", MF_None))
		result = Token_If;
	if(Str8Match(lexeme, "else", MF_None))
		result = Token_Else;
	if(Str8Match(lexeme, "while", MF_None))
		result = Token_While;

	return result;
}

internal u8 AdvanceChar(Lexer* lexer)
{
	u8 result = 0;
	if(!IsAtEnd(lexer))
	{
		u8 curr = lexer->Data.Str[lexer->CurrentOffset++];
		if(curr == '\n')
		{
			lexer->RowNumber++;
			lexer->ColNumber = 0;
		}
		else
		{
			lexer->ColNumber++;
		}

		result = curr;
	}	
	return result;
}

internal u8 PeekChar(Lexer* lexer, u32 depth)
{
	u8 result = 0;
	if((lexer->CurrentOffset + depth) < lexer->Data.Length)
		result = lexer->Data.Str[lexer->CurrentOffset + depth];
	return result;
}

internal void EatWhitespace(Lexer* lexer)
{
	while(lexer->CurrentOffset < lexer->Data.Length)
	{
		u8 c = CurrentChar(lexer);
		if((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
		{
			AdvanceChar(lexer);
		}
		else
		{
			break;
		}		
	}	
}

internal enum TokenType GetToken2(Lexer* lexer, enum TokenType default_token,
									u8 expected, enum TokenType t1)
{
	enum TokenType result = default_token;
	u8 next = PeekChar(lexer, 1);
	if(next == expected)
	{
		result = t1;
		AdvanceChar(lexer);
	}
	return result;
}

internal enum TokenType GetToken3(Lexer* lexer, enum TokenType default_token,
									u8 expected1, enum TokenType t1,
									u8 expected2, enum TokenType t2)
{
	enum TokenType result = default_token;
	u8 next = PeekChar(lexer, 1);
	if(next == expected1)
	{
		result = t1;
		AdvanceChar(lexer);
	}
	else if(next == expected2)
	{
		result = t2;
		AdvanceChar(lexer);
	}
	return result;
}


internal Token ParseToken(Lexer* lexer)
{
	Token result = {(enum TokenType)0};
	result.Type = Token_Invalid;

	EatWhitespace(lexer);
	
	if(lexer->CurrentOffset >= lexer->Data.Length)
	{
		result.Type = Token_EOF;
		return result;
	}

	u32 row = lexer->RowNumber + 1;
	u32 col = lexer->ColNumber + 1;
	u8 current = CurrentChar(lexer);
	
	switch(current)
	{
		case('('):
		case(')'):
		case('{'):
		case('}'):
		case('+'):
		case('*'):
		case('-'):
		case('/'):			
		case('%'):
		case('|'): // TODO(afb) :: Make the && and || tokens
		case('&'): // TODO(afb) :: Make the && and || tokens  
		case('^'):
		case('~'):
		case(';'):
		case('\0'):		
		{			
			result.Type = (enum TokenType)current;
			result.Row = row;
			result.Col = col;
			result.Lexeme = Substr8(lexer->Data, lexer->CurrentOffset, 1);

			AdvanceChar(lexer);
		}break;
		
		case('='):
		{
			u64 start = lexer->CurrentOffset;
			result.Type = GetToken2(lexer, Token_Equal, '=', Token_EQ);
			AdvanceChar(lexer);
			result.Lexeme = Substr8(lexer->Data, start, lexer->CurrentOffset - start);
		}break;

		case('!'):
		{
			u64 start = lexer->CurrentOffset;
			result.Type = GetToken2(lexer, Token_Not, '=', Token_NE);
			AdvanceChar(lexer);
			result.Lexeme = Substr8(lexer->Data, start, lexer->CurrentOffset - start);
		}break;
		
		case('>'):
		{
			u64 start = lexer->CurrentOffset;
			result.Type = GetToken2(lexer, Token_GT, '=', Token_GE);
			AdvanceChar(lexer);
			result.Lexeme = Substr8(lexer->Data, start, lexer->CurrentOffset - start);
		}break;

		case('<'):
		{
			u64 start = lexer->CurrentOffset;
			result.Type = GetToken2(lexer, Token_LT, '=', Token_LE);
			AdvanceChar(lexer);
			result.Lexeme = Substr8(lexer->Data, start, lexer->CurrentOffset - start);
		}break;

		case(':'):
		{
			u64 start = lexer->CurrentOffset;
			result.Type = GetToken3(lexer, Token_Colon, '=', Token_ColonEqual,
									':', Token_ColonColon);
			AdvanceChar(lexer);
			result.Lexeme = Substr8(lexer->Data, start, lexer->CurrentOffset - start);
		}break;

		
		default:
		{
			if(IsDigit(current))
			{
				u8* start = lexer->Data.Str + lexer->CurrentOffset;
				i64 value_int = 0;
				
				while(!IsAtEnd(lexer) && IsDigit(CurrentChar(lexer)))
				{
					u8 curr = AdvanceChar(lexer);
					value_int = (value_int * 10) + (curr - '0');
				}
								
				result.Type = Token_Integer;
				result.Lexeme = Str8(start, (&lexer->Data.Str[lexer->CurrentOffset] - start));
				result.Row = row;
				result.Col = col;
				
				// TODO(afb) :: Unsigned values
				result.Value = value_int;
			}
			else if(IsAlpha(current) || current == '_')
			{
				u8* start = lexer->Data.Str + lexer->CurrentOffset;
				
				while(!IsAtEnd(lexer) && (IsDigit(current) || IsAlpha(current) || current == '_'))
				{
					AdvanceChar(lexer);
					current = CurrentChar(lexer);
				}
				
				u8* end = lexer->Data.Str + lexer->CurrentOffset;

				String8 lexeme = Str8(start, end - start);

				result.Type = GetTokenTypeOrIdentifier(lexeme);
				result.Lexeme = lexeme;
				result.Row = row;
				result.Col = col;
			}
		}
	}


	if(result.Type == Token_Invalid)
	{
		// TODO(afb) :: ReportLexerError(lexer, "Unknown character %c", current, row, col);
		fprintf(stderr, "Bad Token. Unidentified character %c\n", (char)current);
		AdvanceChar(lexer);
	}
	
	return result;
}


internal Token PeekToken(Lexer* lexer, i32 depth = 0)
{
	/* NOTE(afb) ::
	 * Should not request deeper than the buffer can hold. It will eat tokens that 
	 * were not used yet used. */
	Assert(depth < TOKEN_RING_BUFFER_SIZE);

	i32 tokens_processed = 0;
	i32 tokens_to_process = (depth+1) - lexer->LiveTokens;
	for(i32 i = 0; i < tokens_to_process; i++)
	{
		Token token = ParseToken(lexer);
		u32 offset = lexer->CurrentTokenIndex + lexer->LiveTokens + i;
		u32 index = offset & (TOKEN_RING_BUFFER_SIZE - 1);
		// u32 index = (lexer->CurrentTokenIndex + lexer->LiveTokens + i) % TOKEN_RING_BUFFER_SIZE;
		lexer->TokenRingBuffer[index] = token;
		tokens_processed++;
	}
	// NOTE(afb) ::If you ask for a token futhers than what is already parsed then	
	// adjust the amount of LiveTokens accordingly, if not then don't.
	// lexer->LiveTokens += ((depth+1) > lexer->LiveTokens) ? ((depth+1) - lexer->LiveTokens) : 0;
	lexer->LiveTokens += tokens_processed;
	
	u32 index = (lexer->CurrentTokenIndex + depth) & (TOKEN_RING_BUFFER_SIZE - 1);
	Token result = lexer->TokenRingBuffer[index];
	return result;
}


internal void Advance(Lexer* lexer, u32 distance = 1)
{
	for(u32 i = 0; i < distance; i++)
	{
		lexer->CurrentTokenIndex = (lexer->CurrentTokenIndex+1) % TOKEN_RING_BUFFER_SIZE;
		lexer->LiveTokens--;
	}
}

internal Token NextToken(Lexer* lexer)
{
	Token result = PeekToken(lexer, 0);
	Advance(lexer);
	return result;
}


// PARSER ==================================
internal b8 IsBinaryOp(i32 type)
{
	return ((type == '+') || (type == '-') || (type == '*') || (type == '/') || (type == '%') ||
			(type == Token_EQ) || (type == Token_NE) || (type == Token_GT) || (type == Token_GE) ||
			(type == Token_LT) || (type == Token_LE));
}

internal i32 OperatorPrecedence(enum TokenType type)
{
	switch(type)
	{
		case Token_EQ:
		case Token_NE:
			return 0;
		case Token_GT:
		case Token_GE:
		case Token_LT:
		case Token_LE:
			return 10;
		case '+':
		case '-':
			return 20;
		case '*':
		case '/':
		case '%':
			return 30;
		default:
			return 0;
	}
}

internal b8 IsLeftAssociative(enum TokenType type)
{
	return ((type == '+') || (type == '-') || (type == '*') || (type == '/') ||
			(type == Token_GE) || (type == Token_GT) || (type == Token_LE) || (type == Token_LT) ||
			(type == Token_EQ) || (type == Token_NE));
}


internal ASTNode* CreateASTNode(Arena* arena, ASTType type)
{
	ASTNode* result = PushStruct(arena, ASTNode);
	result->Type = type;
	return result;
}

internal Type* ParseType(Parser* parser)
{
	Type* result = PushStruct(parser->Arena, Type);
	
	Token token = PeekToken(parser->Lexer);
	if(token.Type == Token_Int)
	{
		result->Kind = Type_Int;
		Advance(parser->Lexer);
	}
	else if(token.Type == Token_Void)
	{
		result->Kind = Type_Void;
		Advance(parser->Lexer);
	}
	else
	{
		Assert(0 && "Unknown type");
	}
	return result;
}

internal ASTNode* ParsePrimary(Arena* arena, Parser* parser)
{
	ASTNode* result = 0;	
	Token token = PeekToken(parser->Lexer);

	if(token.Type == Token_Integer || token.Type == Token_Identifier)
	{
		Advance(parser->Lexer);
		result = CreateASTNode(arena, AST_Primary);
		result->Primary.Value = token;
	}
	else
	{
		Assert(0 && "ParsePrimary token not valid");
	}

	return result;
}

internal ASTNode* ParseBinaryExpression(Arena* arena, Parser* parser, i32 min_precedence = 0)
{
	ASTNode* left = ParsePrimary(arena, parser);

	for(;;)
	{
		Token op = PeekToken(parser->Lexer);
		if(!IsBinaryOp(op.Type)) break;

		i32 precedence = OperatorPrecedence(op.Type);
		if(precedence < min_precedence)
			break;
		
		b8 left_associative = IsLeftAssociative(op.Type);
		i32 new_min_precedence = left_associative ? precedence + 1 : precedence;

		Advance(parser->Lexer);
		
		ASTNode* right = ParseBinaryExpression(arena, parser, new_min_precedence);
		ASTNode* temp = CreateASTNode(arena, AST_Binary);
		temp->Binary.Operator = op;
		temp->Binary.Left = left;
		temp->Binary.Right = right;
		left = temp;
	}

	return left;
}

internal Token MatchToken(Parser* parser, enum TokenType match)
{
	Token result = NextToken(parser->Lexer);
	if(result.Type != match)
	{
		result.Type = match;
		Assert(0);
	}
	// ReportParserError(parser, &token, match);
	return result;
}

internal ASTNode* ParseStatement(Arena* arena, Parser* parser)
{
	ASTNode* result = 0;

	Token current = PeekToken(parser->Lexer);
	switch(current.Type)
	{
		case(Token_If):
		{
			MatchToken(parser, Token_If);
			MatchToken(parser, Token_LParen);
			ASTNode* cond = ParseBinaryExpression(arena, parser);
			MatchToken(parser, Token_RParen);
			ASTNode* thenClause = ParseStatement(arena, parser);

			ASTNode* elseClause = 0;
			if(PeekToken(parser->Lexer).Type == Token_Else)
			{
				MatchToken(parser, Token_Else);
				elseClause = ParseStatement(arena, parser);
			}

			result = CreateASTNode(arena, AST_If);
			result->If.Cond = cond;
			result->If.Then = thenClause;
			result->If.Else = elseClause;
		}break;

		case(Token_While):
		{
			MatchToken(parser, Token_While);
			MatchToken(parser, Token_LParen);
			ASTNode* cond = ParseBinaryExpression(arena, parser);
			MatchToken(parser, Token_RParen);
			ASTNode* body = ParseStatement(arena, parser);

			result = CreateASTNode(arena, AST_While);
			result->While.Cond = cond;
			result->While.Body = body;
		}break;

		
		case(Token_Print):
		{
			MatchToken(parser, Token_Print);
			ASTNode* expr = ParseBinaryExpression(arena, parser);
			result = CreateASTNode(arena, AST_Print);
			result->Print.Expr = expr;
			
			MatchToken(parser, Token_SemiColon);			
		}break;

		case(Token_Identifier):
		{
			Token id = PeekToken(parser->Lexer);
			Token op = PeekToken(parser->Lexer, 1);
		 
			if(op.Type == Token_Equal) // NOTE(afb) :: Assignment
			{
				Advance(parser->Lexer, 2);
				ASTNode* value = ParseBinaryExpression(arena, parser);
				MatchToken(parser, Token_SemiColon);
				result = CreateASTNode(arena, AST_Assignment);				
				result->Assign.Id = id.Lexeme;
				result->Assign.Expr = value;
			}
			else if(op.Type == Token_Colon) // NOTE(afb) :: Typed declaration
			{
				Advance(parser->Lexer, 2);
				Type* type = ParseType(parser);
				MatchToken(parser, Token_SemiColon);
				result = CreateASTNode(arena, AST_Declaration); // TODO				
			}
			else if(op.Type == Token_ColonColon) // NOTE(afb) :: Implicit declaration
			{
				// TODO(afb) :: Constant variables.
				Advance(parser->Lexer, 2);

				MatchToken(parser, Token_LParen);
				// TODO(afb) :: Argument list
				MatchToken(parser, Token_RParen);
				// TODO(afb) :: Return type

				ASTNode* body = ParseStatement(arena, parser);
				Assert(body->Type == AST_Block);

				result = CreateASTNode(arena, AST_Function);
				result->FDecl.Name = id;
				result->FDecl.Return = ParseType(parser);
				result->FDecl.Body = body;

				// AddSymbol(id.Lexeme);
			}
			else if(op.Type == Token_LParen)
			{
				// TODO(afb) :: Function call
				Assert(0 && "Function call not implemented");
			}
			else
			{
				result = ParseBinaryExpression(arena, parser);
				MatchToken(parser, Token_SemiColon);
			}
		}break;

		case(Token_LBrace):
		{
			ASTNode** stmts = 0;
			current = MatchToken(parser, Token_LBrace);
			while(current.Type != Token_RBrace)
			{
				ASTNode* stmt = ParseStatement(arena, parser);
				arrput(stmts, stmt);
				current = PeekToken(parser->Lexer);
			}			
			MatchToken(parser, Token_RBrace);

			result = CreateASTNode(arena, AST_Block);
			result->Block.Stmts = stmts;
			result->Block.Count = arrlenu(stmts);
		}break;
		
		default:
		{
			result = ParseBinaryExpression(arena, parser);
			MatchToken(parser, Token_SemiColon);
		}break;
	}
	
	return result;
}
