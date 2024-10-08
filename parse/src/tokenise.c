typedef enum
{
	emphasis_state_none,
	emphasis_state_strong,
	emphasis_state_emphasis
} emphasis_state;

static void handle_loc_error(uint32_t line, uint32_t column, const char* format, ...)
{
	print_error("Parsing error (line %u, column %u): ", line, column);

	va_list args;
	va_start(args, format);
	print_error_vargs(format, args);
	va_end(args);

	print_error_char('\n');

	exit_failure();
}

static void handle_peek_error(const peek_state* peek, const char* format, ...)
{
	print_error("Parsing error (line %u, column %u): ", peek->line, peek->column);

	va_list args;
	va_start(args, format);
	print_error_vargs(format, args);
	va_end(args);

	print_error_char('\n');

	exit_failure();
}

static void handle_tokenise_error(const tokenise_context* ctx, const char* format, ...)
{
	print_error("Parsing error (line %u, column %u): ", ctx->peek.line, ctx->peek.column);

	va_list args;
	va_start(args, format);
	print_error_vargs(format, args);
	va_end(args);

	print_error_char('\n');

	exit_failure();
}

static line_token* add_line_token(tokenise_context* ctx, line_token_type type)
{
	line_token* line = mem_alloc(sizeof(line_token));//&ctx->lines[ctx->line_count++];
	line->type = type;
	line->line = ctx->peek.line;
	line->text = ctx->write_ptr;

#ifndef NDEBUG
	// Make it easier to read tokens in the watch window
	if (type == line_token_type_newline || type == line_token_type_eof)
		line->text = nullptr;
#endif

	++ctx->line_count;
	ctx->current_line = line;

	return line;
}

static char peek_char_internal(tokenise_context* ctx, peek_state* peek)
{
	char c = *peek->read_ptr++;

	peek->prev_line = peek->line;
	peek->prev_column = peek->column;
	peek->line = peek->next_line;
	peek->column = peek->next_column;

	if (c & 0b10000000) // UTF-8 code point with multiple bytes
	{
		if (!(c & 0b01000000))
		{
			// Ignore bytes in the middle of a UTF-8 code point
		}
		else
		{
			// First byte, so increment position
			++peek->next_column;

			// Check for invalid characters
			if ((uint8_t)c == 0xE2 && (uint8_t)peek->read_ptr[0] == 0x80)
			{
				switch ((uint8_t)peek->read_ptr[1])
				{
				case 0x93:
					handle_tokenise_error(ctx, "En dash characters are not permitted; replace with two hyphens \"--\".");
				case 0x94:
					handle_tokenise_error(ctx, "Em dash characters are not permitted; replace with three hyphens \"---\".");
				case 0x99:
					handle_tokenise_error(ctx, "Typographic apostrophes are not permitted; replace with standard keyboard apostrophe \"'\".");
				case 0x9C:
				case 0x9D:
				case 0x98:
					handle_tokenise_error(ctx, "Typographic quotation marks are not permitted; replace with standard keyboard quotations \"\"...\"\" for first level quotations, or backticks \"`...`\" for second level quotations.");
				}
			}
		}
	}
	else // ASCII
	{
		if (c < 32)
		{
			if (c == 0)
			{
				return 0;
			}
			else if (c == '\t')
			{
				const uint32_t tab_size = 4 - ((peek->next_column - 1) % 4);
				peek->next_column += tab_size;
			}
			else if (c == '\r')
			{
				if (*peek->read_ptr == '\n')
				{
					c = *peek->read_ptr++;

					++peek->next_line;
					peek->next_column = 1;
				}
				else
				{
					handle_tokenise_error(ctx, "Unsupported control character; this error may be caused by file corruption or attempting to load a binary file.");
				}
			}
			else if (c == '\n')
			{
				++peek->next_line;
				peek->next_column = 1;
			}
			else
			{
				handle_tokenise_error(ctx, "Unsupported control character; this error may be caused by file corruption or attempting to load a binary file.");
			}
		}
		else if (c == 127)
		{
			handle_tokenise_error(ctx, "Unsupported control character; this error may be caused by file corruption or attempting to load a binary file.");
		}
		else
		{
			++peek->next_column;
		}
	}

	return c;
}

static void peek_init(tokenise_context* ctx, peek_state* peek)
{
	*peek = ctx->peek;
}

static char peek_char(tokenise_context* ctx, peek_state* peek)
{
	peek->pc = peek->c;

	char c = peek_char_internal(ctx, peek);
	if (c == '/' && *peek->read_ptr == '*')
	{
		// Consume '*'
		c = peek_char_internal(ctx, peek);

		for (;;)
		{
			if (c == 0)
			{
				handle_tokenise_error(ctx, "Comments must be closed \"/*...*/\".");
			}
			else if (c == '*' && *peek->read_ptr == '/')
			{
				// Consume '/'
				peek_char_internal(ctx, peek);
				c = peek_char_internal(ctx, peek);
				break;
			}

			c = peek_char_internal(ctx, peek);
		}
	}

	peek->c = c;

	return c;
}

static void peek_apply(tokenise_context* ctx, peek_state* peek)
{
	ctx->peek = *peek;
}

static char get_char(tokenise_context* ctx)
{
	return peek_char(ctx, &ctx->peek);
}

static char advance_read_ptr(tokenise_context* ctx, size_t count)
{
	assert(count);

	char c;
	do
	{
		c = get_char(ctx);
	} while (--count);

	return c;
}

static void put_char(tokenise_context* ctx, char c)
{
	*ctx->write_ptr++ = c;
}

static void put_text_token(tokenise_context* ctx, text_token_type token)
{
	*ctx->write_ptr++ = token;
}

static uint32_t arabic_to_int(const char* str, char terminator)
{
	uint32_t digits[10];
	int current_digit = 0;

	while (*str != terminator)
		digits[current_digit++] = *str++ - '0';

	uint32_t result = 0;
	uint32_t multiplier = 1;

	for (int i = current_digit - 1; i >= 0; --i)
	{
		result += digits[i] * multiplier;
		multiplier *= 10;
	}

	return result;
}

static uint32_t arabic_to_int_new(tokenise_context* ctx, peek_state* peek, char c, char terminator)
{
	enum { max_digits = 3 };
	enum { max_value = 999 };

	uint32_t digits[max_digits];
	int current_digit = 0;

	if (c == '0')
		handle_tokenise_error(ctx, "Numbers must begin from 1.");
	else if (c == terminator)
		handle_tokenise_error(ctx, "Number expected.");

	for (;;)
	{
		if (c == terminator)
			break;
		else if (current_digit == max_digits)
			handle_tokenise_error(ctx, "Number is too large; maximum value is 1000.");
		else if (c < '0' || c > '9')
			handle_tokenise_error(ctx, "Invalid character; number expected.");
		else
			digits[current_digit++] = c - '0';

		c = peek_char(ctx, peek);
	}

	uint64_t result = 0;
	uint64_t multiplier = 1;

	for (int i = current_digit - 1; i >= 0; --i)
	{
		result += digits[i] * multiplier;
		multiplier *= 10;
	}

	if (result >= max_value)
		handle_tokenise_error(ctx, "Number is too large; maximum value is %d.", max_value);

	return (uint32_t)result;
}

static uint32_t roman_to_int(tokenise_context* ctx, peek_state* peek, char c, int len)
{
	char peaked_char;
	const char* roman_string;

	for (int i = 0; i < roman_numeral_max; ++i)
	{
		roman_string = roman_upper_strings[i];
		if (c == roman_string[0])
		{
			peek_init(ctx, peek);
			for (int i = 1; i < len; ++i)
			{
				peaked_char = peek_char(ctx, peek);
				if (peaked_char != roman_string[i])
					break;
			}

			return i + 1;
		}
	}

	handle_tokenise_error(ctx, "Ordered lists using Roman numerals currently have a maximum value of %d.", roman_numeral_max);
	return 0;
}

static bool check_space(tokenise_context* ctx, char c)
{
	if (c == ' ')
	{
		if (ctx->peek.pc == ' ')
			handle_tokenise_error(ctx, "Extraneous space.");

		put_char(ctx, c);

		return true;
	}
	else if (c == '\t')
	{
		handle_tokenise_error(ctx, "Tabs are only permitted on a new line to represent a block quote.");
	}

	return false;
}

static bool check_emphasis(tokenise_context* ctx, char c, emphasis_state* state)
{
	if (c == '*')
	{
		peek_state peek;
		peek_init(ctx, &peek);

		c = peek_char(ctx, &peek);
		if (c == '*')
		{
			peek_apply(ctx, &peek);

			if (peek_char(ctx, &peek) == '*')
				handle_peek_error(&peek, "Only two levels of '*' allowed.");

			if (*state == emphasis_state_none)
			{
				put_text_token(ctx, text_token_type_strong_begin);
				*state = emphasis_state_strong;
			}
			else if (*state == emphasis_state_strong)
			{
				put_text_token(ctx, text_token_type_strong_end);
				*state = emphasis_state_none;
			}
			else
			{
				handle_loc_error(peek.prev_line, peek.prev_column, "Emphasis tags '*' cannot be mixed with strong tags \"**\".");
			}
		}
		else
		{
			if (*state == emphasis_state_none)
			{
				put_text_token(ctx, text_token_type_emphasis_begin);
				*state = emphasis_state_emphasis;
			}
			else if (*state == emphasis_state_emphasis)
			{
				put_text_token(ctx, text_token_type_emphasis_end);
				*state = emphasis_state_none;
			}
			else
			{
				handle_peek_error(&peek, "Emphasis tags '*' cannot be mixed with strong tags \"**\".");
			}
		}

		return true;
	}

	return false;
}

static bool check_dash(tokenise_context* ctx, char c)
{
	if (c == '-')
	{
		peek_state peek;
		peek_init(ctx, &peek);

		if (peek_char(ctx, &peek) == '-')
		{
			if (peek_char(ctx, &peek) == '-')
			{
				peek_apply(ctx, &peek);
				put_text_token(ctx, text_token_type_em_dash);

				if (peek_char(ctx, &peek) == '-')
					handle_peek_error(&peek, "Too many hyphens.");
			}
			else
			{
				get_char(ctx);
				put_text_token(ctx, text_token_type_en_dash);
			}

			return true;
		}
	}

	return false;
}

static bool check_fraction_slash(tokenise_context* ctx, char c)
{
	if (c == '/')
	{
		peek_state peek;
		peek_init(ctx, &peek);

		if (peek_char(ctx, &peek) == '/')
		{
			peek_apply(ctx, &peek);
			put_text_token(ctx, text_token_type_fraction_slash);

			if (peek_char(ctx, &peek) == '/')
				handle_peek_error(&peek, "Too many slashes.");

			return true;
		}
	}

	return false;
}

static bool check_newline(tokenise_context* ctx, char c, int char_count)
{
	if (c == '\n')
	{
		if (!char_count)
			handle_tokenise_error(ctx, "Empty text blocks are not permitted.");

		if (ctx->peek.pc == ' ')
			handle_loc_error(ctx->peek.prev_line, ctx->peek.prev_column, "Trailing spaces are not permitted.");

		// Null terminate
		put_char(ctx, 0);

		return true;
	}

	return false;
}

static char tokenise_text(tokenise_context* ctx, char c)
{
	if (c == ' ')
		handle_tokenise_error(ctx, "Leading spaces are not permitted.");

	int char_count = 0;
	int quote_level = 0;
	emphasis_state em_state = emphasis_state_none;

	for (;;)
	{
		if (c == '\\')
		{
			c = get_char(ctx);

			// TODO: Why not just put '[' and ']' text directly? They aren't special chars.
			if (c == '[')
				put_text_token(ctx, text_token_type_left_square_bracket);
			else if (c == ']')
				put_text_token(ctx, text_token_type_right_square_bracket);
			else
				handle_tokenise_error(ctx, "Invalid character escape sequence.");
		}
		else if (c == '"')
		{
			if (quote_level == 0)
			{
				put_text_token(ctx, text_token_type_quote_level_1_begin);
				quote_level = 1;
			}
			else if (quote_level == 1)
			{
				put_text_token(ctx, text_token_type_quote_level_1_end);
				quote_level = 0;
			}
			else
			{
				handle_tokenise_error(ctx, "Mixed quotation levels; first level quotations require pairs of double quotation marks \"\"...\"\", and second level quotations require pairs of backticks \"`...`\". Third level quotations are not currently supported.");
			}
		}
		else if (c == '`')
		{
			if (quote_level == 1)
			{
				put_text_token(ctx, text_token_type_quote_level_2_begin);
				quote_level = 2;
			}
			else if (quote_level == 2)
			{
				put_text_token(ctx, text_token_type_quote_level_2_end);
				quote_level = 1;
			}
			else
			{
				handle_tokenise_error(ctx, "Mixed quotation levels; first level quotations require pairs of double quotation marks \"\"...\"\", and second level quotations require pairs of backticks \"`...`\". Third level quotations are not currently supported.");
			}
		}
		else if (c == '[')
		{
			c = get_char(ctx);

			if (c >= '0' && c <= '9')
			{
				const uint32_t index = arabic_to_int_new(ctx, &ctx->peek, c, ']');
				++ctx->note_count;

				if (index != ctx->note_count)
					handle_tokenise_error(ctx, "Expected note number %d.", ctx->note_count);

				put_text_token(ctx, text_token_type_note);
			}
			else
			{
				put_char(ctx, '[');
				continue;
			}
		}
		else if (check_space(ctx, c))
		{
		}
		else if (check_emphasis(ctx, c, &em_state))
		{
		}
		else if (check_dash(ctx, c))
		{
		}
		else if (check_fraction_slash(ctx, c))
		{
		}
		else if (c == '_')
		{
			put_text_token(ctx, text_token_type_joiner);
		}
		else if (check_newline(ctx, c, char_count))
		{
			break;
		}
		else
		{
			++char_count;
			put_char(ctx, c);
		}

		c = get_char(ctx);
	}

	if (quote_level != 0)
		handle_tokenise_error(ctx, "Unterminated quote.");

	if (em_state == emphasis_state_strong)
		handle_tokenise_error(ctx, "Unterminated strong markup \"**\".");
	else if (em_state == emphasis_state_emphasis)
		handle_tokenise_error(ctx, "Unterminated emphasis markup '*'.");

	return get_char(ctx);
}

static char tokenise_newline(tokenise_context* ctx, char c, bool blockquote)
{
	const line_token_type type = blockquote ? line_token_type_block_newline : line_token_type_newline;
	add_line_token(ctx, type);

	c = get_char(ctx);

	return c;
}

static char tokenise_paragraph(tokenise_context* ctx, char c, bool blockquote)
{
	const line_token_type type = blockquote ? line_token_type_block_paragraph : line_token_type_paragraph;
	add_line_token(ctx, type);

	return tokenise_text(ctx, c);
}

static char tokenise_heading(tokenise_context* ctx, char c)
{
	int32_t depth = -1;
	do
	{
		++depth;
		c = get_char(ctx);
	} while (c == '#');

	if (depth == 0)
		ctx->note_count = 0;
	else if (depth > 2)
		handle_tokenise_error(ctx, "Exceeded maximum heading depth of 3.");

	if (c != ' ')
		handle_tokenise_error(ctx, "Heading tags '#' must be followed by a space.");

	add_line_token(ctx, line_token_type_heading_1 + depth);

	// Consume space
	c = get_char(ctx);

	return tokenise_text(ctx, c);
}

static char tokenise_ordered_list_arabic(tokenise_context* ctx, char c)
{
	peek_state peek;
	peek_init(ctx, &peek);

	char peeked_char;
	do
	{
		peeked_char = peek_char(ctx, &peek);
	} while (peeked_char >= '0' && peeked_char <= '9');

	if (peeked_char == '.')
	{
		peeked_char = peek_char(ctx, &peek);
		if (peeked_char == ' ')
		{
			line_token* line = add_line_token(ctx, line_token_type_ordered_list_arabic);
			line->index = arabic_to_int_new(ctx, &ctx->peek, c, '.');
			peek_apply(ctx, &peek);

			return tokenise_text(ctx, get_char(ctx));
		}
	}

	return tokenise_paragraph(ctx, c, false);
}

static char tokenise_ordered_list_roman(tokenise_context* ctx, char c)
{
	peek_state peek;
	peek_init(ctx, &peek);

	int len = 1;
	char peeked_char;

	for (;;)
	{
		peeked_char = peek_char(ctx, &peek);
		if (peeked_char != 'I' && peeked_char != 'V' && peeked_char != 'X')
			break;

		++len;
	}

	if (peeked_char == '.')
	{
		peeked_char = peek_char(ctx, &peek);
		if (peeked_char == ' ')
		{
			line_token* line = add_line_token(ctx, line_token_type_ordered_list_roman);
			line->index = roman_to_int(ctx, &ctx->peek, c, len);
			peek_apply(ctx, &peek);

			return tokenise_text(ctx, get_char(ctx));
		}
	}

	return tokenise_paragraph(ctx, c, false);
}

static char tokenise_ordered_list_letter(tokenise_context* ctx, char c, bool blockquote)
{
	peek_state peek;
	peek_init(ctx, &peek);

	if (peek_char(ctx, &peek) == '.' && peek_char(ctx, &peek) == ' ')
	{
		peek_apply(ctx, &peek);

		line_token* line = add_line_token(ctx, line_token_type_ordered_list_letter);
		line->index = c - 'a' + 1;

		return tokenise_text(ctx, get_char(ctx));
	}

	return tokenise_paragraph(ctx, c, blockquote);
}

static bool tokenise_check_dinkus(tokenise_context* ctx)
{
	peek_state peek;
	peek_init(ctx, &peek);

	if (peek_char(ctx, &peek) == ' ' && peek_char(ctx, &peek) == '*' && peek_char(ctx, &peek) == ' ' && peek_char(ctx, &peek) == '*')
	{
		if (peek_char(ctx, &peek) != '\n')
			handle_peek_error(&peek, "A dinkus \"* * *\", may not be followed by additional characters.");

		add_line_token(ctx, line_token_type_dinkus);
		peek_apply(ctx, &peek);

		return true;
	}

	return false;
}

static char tokenise_unordered_list(tokenise_context* ctx, char c)
{
	peek_state peek;
	peek_init(ctx, &peek);

	if (peek_char(ctx, &peek) == ' ')
	{
		if (tokenise_check_dinkus(ctx))
			return get_char(ctx);

		add_line_token(ctx, line_token_type_unordered_list);
		peek_apply(ctx, &peek);

		return tokenise_text(ctx, get_char(ctx));
	}

	return tokenise_paragraph(ctx, c, false);
}

static char tokenise_blockquote_citation(tokenise_context* ctx, char c)
{
	peek_state peek;
	peek_init(ctx, &peek);

	if (peek_char(ctx, &peek) == '-' && peek_char(ctx, &peek) == '-')
	{
		add_line_token(ctx, line_token_type_block_citation);
		peek_apply(ctx, &peek);

		return tokenise_text(ctx, get_char(ctx));
	}

	return tokenise_paragraph(ctx, c, true);
}

static char tokenise_blockquote(tokenise_context* ctx, char c)
{
	c = get_char(ctx);
	switch (c)
	{
	case '\n':
		c = tokenise_newline(ctx, c, true);
		break;
	case '-':
		c = tokenise_blockquote_citation(ctx, c);
		break;
	default:
		c = tokenise_paragraph(ctx, c, true);
	}

	return c;
}

static char tokenise_right_aligned(tokenise_context* ctx, char c)
{
	c = get_char(ctx);
	if (c != ' ')
		handle_tokenise_error(ctx, "Right-aligned text \">\" must be followed by a space.");

	// Consime space
	c = get_char(ctx);

	add_line_token(ctx, line_token_type_right_aligned);

	return tokenise_text(ctx, c);
}

static char tokenise_bracket(tokenise_context* ctx, char c)
{
	c = get_char(ctx);
	if (c == ']')
	{
		handle_tokenise_error(ctx, "Empty brackets \"[]\" are not permitted.");
	}
	else if (c == '0')
	{
		handle_tokenise_error(ctx, "Notes must begin from 1.");
	}
	else if (c >= '1' && c <= '9')
	{
		const uint32_t index = arabic_to_int_new(ctx, &ctx->peek, c, ']');

		line_token* line = add_line_token(ctx, line_token_type_note);
		line->index = index;

		c = get_char(ctx);
		if (c != ' ')
			handle_tokenise_error(ctx, "Notes must be followed by a space.");

		c = tokenise_text(ctx, get_char(ctx));

	}
	else
	{
		handle_tokenise_error(ctx, "Notes must contain arabic numerals.");
	}

	return c;
}

/*
	"//" at the start of a line represents a single-line comment. Lines that start with a single
	slash are considered paragraphs.
*/
static char tokenise_comment(tokenise_context* ctx, char c)
{
	peek_state peek;
	peek_init(ctx, &peek);

	// Check for "//"
	if (peek_char(ctx, &peek) == '/')
	{
		peek_apply(ctx, &peek);

		for (;;)
		{
			// Skip past line
			c = get_char(ctx);
			if (c == '\n')
				return get_char(ctx);
		}
	}

	// Not a comment
	return tokenise_paragraph(ctx, c, false);
}

static void tokenise(char* data, line_tokens* out_tokens, document_metadata* metadata)
{
	tokenise_context ctx = {
		.buffer				= data,
		.write_ptr			= data,
		.peek				= {
			.read_ptr		= data,
			.next_line		= 1,
			.next_column	= 1
		},
		.metadata			= metadata
	};

	/*
		We are using linear allocation so we can just assign the line array to the next address. No
		other allocations are allowed while the list is being filled.
	*/
	ctx.lines = mem_push();

	/*
		This loop checks the first character of each line and delegates parsing to specialised
		tokenisation functions. Sometimes the first character may be a false positive, in which case
		tokenisation is delegated to the more appropriate function. For example, a line starting
		with "1. " represents a list item, whereas "1 apple" would represent a paragraph.
	*/
	char c = get_char(&ctx);
	for (;;)
	{
		if (c == 0)
			break;
		else if (c == '/')
			c = tokenise_comment(&ctx, c);
		else if (c == '{')
			c = tokenise_metadata(&ctx, c);
		else if (c == '[')
			c = tokenise_bracket(&ctx, c);
		else if (c == '\t')
			c = tokenise_blockquote(&ctx, c);
		else if (c == '>')
			c = tokenise_right_aligned(&ctx, c);
		else if (c == '\n')
			c = tokenise_newline(&ctx, c, false);
		else if (c == '#')
			c = tokenise_heading(&ctx, c);
		else if (c == '*')
			c = tokenise_unordered_list(&ctx, c);
		else if (c >= '1' && c <= '9')
			c = tokenise_ordered_list_arabic(&ctx, c);
		else if (c >= 'a' && c <= 'z')
			c = tokenise_ordered_list_letter(&ctx, c, false);
		else if (c == 'I' || c == 'V' || c == 'X')
			c = tokenise_ordered_list_roman(&ctx, c);
		else
			c = tokenise_paragraph(&ctx, c, false);
	}

	/*
		Make later parsing simpler by allowing validadation functions to check for only new lines,
		without having to also check for end of file.
	*/
	add_line_token(&ctx, line_token_type_newline);

	add_line_token(&ctx, line_token_type_eof);
	out_tokens->lines = ctx.lines;
	out_tokens->count = ctx.line_count;

	/*
		Create and populate metadata lists after tokenisation is complete, in order to maintain
		contiguous token list and allow for linear allocations.
	*/
	finalise_metadata(&ctx);
}