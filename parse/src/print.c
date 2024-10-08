void print_str(file f, const char* s)
{
	fputs(s, f.f);
}

void print_fmt(file f, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(f.f, format, args);
	va_end(args);
}

void print_char(file f, char c)
{
	fputc(c, f.f);
}

void print_char_token(file f, char c)
{
	switch (c)
	{
	case text_token_type_en_dash:
		print_en_dash(f);
		break;
	case text_token_type_em_dash:
		print_em_dash(f);
		break;
	case '\'':
		print_apostrophe(f);
		break;
	case text_token_type_quote_level_1_begin:
		print_quote_level_1_begin(f);
		break;
	case text_token_type_quote_level_1_end:
		print_quote_level_1_end(f);
		break;
	case text_token_type_quote_level_2_begin:
		print_quote_level_2_begin(f);
		break;
	case text_token_type_quote_level_2_end:
		print_quote_level_2_end(f);
		break;
	case text_token_type_left_square_bracket:
		print_char(f, '[');
		break;
	case text_token_type_right_square_bracket:
		print_char(f, ']');
		break;
	case text_token_type_joiner:
		print_joiner(f);
		break;
	case text_token_type_fraction_slash:
		print_fraction_slash(f);
		break;
	case text_token_type_note:
	case text_token_type_strong_end:
	case text_token_type_emphasis_end:
	case text_token_type_strong_begin:
	case text_token_type_emphasis_begin:
		break;
	default:
		print_char(f, c);
	}
}

void print_tabs(file f, int depth)
{
	print_char(f, '\n');
	for (int i = 0; i < depth; ++i)
		print_char(f, '\t');
}

void print_en_dash(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x80);
	print_char(f, 0x93);
}

void print_em_dash(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x80);
	print_char(f, 0x94);
}

void print_apostrophe(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x80);
	print_char(f, 0x99);
}

void print_joiner(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x81);
	print_char(f, 0xA0);
}

void print_fraction_slash(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x81);
	print_char(f, 0x84);
}

void print_quote_level_1_begin(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x80);
	print_char(f, 0x9C);
}

void print_quote_level_1_end(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x80);
	print_char(f, 0x9D);
}

void print_quote_level_2_begin(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x80);
	print_char(f, 0x98);
}

void print_quote_level_2_end(file f)
{
	print_char(f, 0xE2);
	print_char(f, 0x80);
	print_char(f, 0x99);
}

void print_simple_text(file f, const char* text)
{
	while (*text)
		print_char_token(f, *text++);
}