typedef struct
{
	document*			doc;
	line_token*			tokens;
	document_element*	elements;
	document_note*		notes;
	document_element*	note_elements;
	document_chapter*	current_chapter;
	uint32_t			current_element;
	uint32_t			current_note;
	uint32_t			current_note_element;
	uint32_t			current_token;
	uint32_t			token_count;
	uint32_t			chapter_count;
	uint32_t			element_count;
	uint32_t			note_count;
	uint32_t			note_element_count;
	bool				within_note;
} finalise_context;

static line_token* finalise_peek_next_token(finalise_context* ctx, uint32_t* token_index)
{
	assert(*token_index < ctx->token_count);

	line_token* token = &ctx->tokens[*token_index];
	++(*token_index);

	return token;
}

static line_token* finalise_get_next_token(finalise_context* ctx)
{
	assert(ctx->current_token < ctx->token_count);

	return &ctx->tokens[ctx->current_token++];
}

static document_element* finalise_add_element(finalise_context* ctx, document_element_type type, const char* text)
{
	document_element* element;

	if (ctx->within_note)
	{
		assert(ctx->current_note_element < ctx->note_element_count);

		document_note* note = &ctx->notes[ctx->current_note];
		note->element_count++;

		element = &ctx->notes->elements[ctx->current_note_element++];
	}
	else
	{
		assert(ctx->current_element < ctx->element_count);

		const uint32_t index = ctx->current_chapter->element_count++;
		++ctx->current_element;

		element = &ctx->current_chapter->elements[index];
	}

	element->type = type;
	element->text = text;

	return element;
}

static line_token* finalise_paragraph(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_paragraph_begin, nullptr);
	finalise_add_element(ctx, document_element_type_text_block, token->text);

	token = finalise_get_next_token(ctx);
	while (token->type == line_token_type_paragraph)
	{
		finalise_add_element(ctx, document_element_type_line_break, nullptr);
		finalise_add_element(ctx, document_element_type_text_block, token->text);
		token = finalise_get_next_token(ctx);
	}

	finalise_add_element(ctx, document_element_type_paragraph_end, nullptr);

	return token;
}

static line_token* finalise_heading_1(finalise_context* ctx, line_token* token)
{
	const uint32_t chapter_index = ctx->doc->chapter_count++;

	document_chapter* chapter = &ctx->doc->chapters[chapter_index];
	chapter->elements = &ctx->elements[ctx->current_element];
	chapter->notes = &ctx->notes[ctx->current_note];
	chapter->element_count = 0;
	chapter->note_count = 0;

	ctx->current_chapter = chapter;

	finalise_add_element(ctx, document_element_type_heading_1, token->text);

	return finalise_get_next_token(ctx);
}

static line_token* finalise_heading_2(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_heading_2, token->text);

	return finalise_get_next_token(ctx);
}

static line_token* finalise_heading_3(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_heading_3, token->text);

	return finalise_get_next_token(ctx);
}

static line_token* finalise_heading_4(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_heading_4, token->text);

	return finalise_get_next_token(ctx);
}

static line_token* finalise_heading_5(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_heading_5, token->text);

	return finalise_get_next_token(ctx);
}

static line_token* finalise_table(finalise_context* ctx, line_token* token)
{
	document_element* element = finalise_add_element(ctx, document_element_type_table, nullptr);

	uint32_t peek = ctx->current_token;

	uint32_t row_count = 0;
	uint32_t column_count = 0;
	uint32_t header_row_count = 0;

	while (token->type != line_token_type_newline)
	{
		if (token->type == line_token_type_table_row)
		{
			++row_count;
			column_count = 0;

			token = finalise_peek_next_token(ctx, &peek);
			while (token->type == line_token_type_table_cell || token->type == line_token_type_table_merge)
			{
				++column_count;
				token = finalise_peek_next_token(ctx, &peek);
			}
		}
		else if (token->type == line_token_type_table_header_delimiter_row)
		{
			header_row_count = row_count;

			token = finalise_peek_next_token(ctx, &peek);
			while (token->type != line_token_type_table_row)
				token = finalise_peek_next_token(ctx, &peek);
		}
	}

	const int64_t size = sizeof(document_table) + (sizeof(document_cell) * row_count * column_count);
	element->table = (document_table*)mem_alloc(size);
	memset(element->table, 0x00, size);
	element->table->width = column_count;
	element->table->height = row_count;
	element->table->header_row_count = header_row_count;

	int32_t i = -1;
	uint32_t span_count = 1;
	for (uint32_t y = 0; y < row_count; ++y)
	{
		for (uint32_t x = 0; x < column_count; ++x)
		{
			token = finalise_get_next_token(ctx);

			if (token->type == line_token_type_table_cell)
			{
				i += span_count;
				span_count = 1;
				element->table->cells[i].column_span = 1;

				if (token->length)
					element->table->cells[i].text_element = token->text;
			}
			else if (token->type == line_token_type_table_merge)
			{
				++span_count;
				++element->table->cells[i].column_span;
			}
		}

		token = finalise_get_next_token(ctx);
		if (token->type == line_token_type_table_header_delimiter_row)
		{
			// Set the alignment of each column in the first row
			for (uint32_t i = 0; i < column_count; ++i)
			{
				token = finalise_get_next_token(ctx);
				if (token->type == line_token_type_table_header_align_right)
					element->table->cells[i].alignment = document_align_right;
				else if (token->type == line_token_type_table_header_align_centre)
					element->table->cells[i].alignment = document_align_centre;
				else
					element->table->cells[i].alignment = document_align_left;
			}

			token = finalise_get_next_token(ctx);
		}
	}

	// Copy first row alignments into every row
	i = 0;
	for (uint32_t y = 0; y < row_count; ++y)
	{
		for (uint32_t x = 0; x < column_count; ++x)
			element->table->cells[i++].alignment = element->table->cells[x].alignment;
	}

	return token;
}

static line_token* finalise_paragraph_break(finalise_context* ctx, line_token* token)
{
	// Skip empty lines
	token = finalise_get_next_token(ctx);
	while (token->type == line_token_type_newline)
		token = finalise_get_next_token(ctx);

	finalise_add_element(ctx, document_element_type_paragraph_break_begin, nullptr);
	finalise_add_element(ctx, document_element_type_text_block, token->text);

	token = finalise_get_next_token(ctx);
	while (token->type == line_token_type_paragraph)
	{
		finalise_add_element(ctx, document_element_type_line_break, nullptr);
		finalise_add_element(ctx, document_element_type_text_block, token->text);
		token = finalise_get_next_token(ctx);
	}

	finalise_add_element(ctx, document_element_type_paragraph_end, nullptr);

	return token;
}

static line_token* finalise_note(finalise_context* ctx, line_token* token)
{
	assert(ctx->current_note < ctx->note_count);

	document_note* note = &ctx->notes[ctx->current_note];
	note->elements = &ctx->note_elements[ctx->current_note_element];
	note->element_count = 0;

	ctx->within_note = true;

	finalise_add_element(ctx, document_element_type_paragraph_begin, nullptr);
	finalise_add_element(ctx, document_element_type_text_block, token->text);

	token = finalise_get_next_token(ctx);
	while (token->type == line_token_type_paragraph)
	{
		finalise_add_element(ctx, document_element_type_line_break, nullptr);
		finalise_add_element(ctx, document_element_type_text_block, token->text);
		token = finalise_get_next_token(ctx);
	}

	finalise_add_element(ctx, document_element_type_paragraph_end, nullptr);

	for (;;)
	{
		if (token->type == line_token_type_paragraph)
			token = finalise_paragraph(ctx, token);
		else if (token->type == line_token_type_newline)
			token = finalise_get_next_token(ctx);
		else if (token->type == line_token_type_paragraph_break)
			token = finalise_paragraph_break(ctx, token);
		else if (token->type == line_token_type_table_row)
			token = finalise_table(ctx, token);
		else
			break;
	}

	ctx->within_note = false;
	++ctx->current_note;
	++ctx->current_chapter->note_count;

	return token;
}

static line_token* finalise_right_aligned(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_right_aligned_begin, nullptr);
	finalise_add_element(ctx, document_element_type_text_block, token->text);

	token = finalise_get_next_token(ctx);
	while (token->type == line_token_type_right_aligned)
	{
		finalise_add_element(ctx, document_element_type_line_break, nullptr);
		finalise_add_element(ctx, document_element_type_text_block, token->text);
		token = finalise_get_next_token(ctx);
	}

	finalise_add_element(ctx, document_element_type_paragraph_end, nullptr);

	return token;
}

static line_token* finalise_centre_aligned(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_centre_aligned_begin, nullptr);
	finalise_add_element(ctx, document_element_type_text_block, token->text);

	token = finalise_get_next_token(ctx);
	while (token->type == line_token_type_centre_aligned)
	{
		finalise_add_element(ctx, document_element_type_line_break, nullptr);
		finalise_add_element(ctx, document_element_type_text_block, token->text);
		token = finalise_get_next_token(ctx);
	}

	finalise_add_element(ctx, document_element_type_paragraph_end, nullptr);

	return token;
}

static line_token* finalise_blockquote(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_blockquote_begin, nullptr);

	for (;;)
	{
		if (token->type == line_token_type_block_newline)
		{
			token = finalise_get_next_token(ctx);
		}
		else if (token->type == line_token_type_block_citation)
		{
			finalise_add_element(ctx, document_element_type_blockquote_citation, token->text);
			token = finalise_get_next_token(ctx);
			break;
		}
		else if (token->type == line_token_type_block_paragraph)
		{
			finalise_add_element(ctx, document_element_type_paragraph_begin, nullptr);
			finalise_add_element(ctx, document_element_type_text_block, token->text);

			token = finalise_get_next_token(ctx);
			while (token->type == line_token_type_block_paragraph)
			{
				finalise_add_element(ctx, document_element_type_line_break, nullptr);
				finalise_add_element(ctx, document_element_type_text_block, token->text);
				token = finalise_get_next_token(ctx);
			}

			finalise_add_element(ctx, document_element_type_paragraph_end, nullptr);
		}
		else
		{
			break;
		}
	}

	finalise_add_element(ctx, document_element_type_blockquote_end, nullptr);

	return token;
}

static line_token* finalise_ordered_list(finalise_context* ctx, line_token* token, line_token_type line_type, document_element_type doc_type)
{
	finalise_add_element(ctx, doc_type, nullptr);
	finalise_add_element(ctx, document_element_type_list_item, token->text);

	token = finalise_get_next_token(ctx);
	while (token->type == line_type)
	{
		finalise_add_element(ctx, document_element_type_list_item, token->text);
		token = finalise_get_next_token(ctx);
	}

	finalise_add_element(ctx, document_element_type_ordered_list_end, nullptr);

	return token;
}

static line_token* finalise_unordered_list(finalise_context* ctx, line_token* token)
{
	finalise_add_element(ctx, document_element_type_unordered_list_begin, nullptr);
	finalise_add_element(ctx, document_element_type_list_item, token->text);

	token = finalise_get_next_token(ctx);
	while (token->type == line_token_type_unordered_list)
	{
		finalise_add_element(ctx, document_element_type_list_item, token->text);
		token = finalise_get_next_token(ctx);
	}

	finalise_add_element(ctx, document_element_type_unordered_list_end, nullptr);

	return token;
}

static line_token* finalise_dinkus(finalise_context* ctx)
{
	finalise_add_element(ctx, document_element_type_dinkus, nullptr);

	return finalise_get_next_token(ctx);
}

static void finalise(line_tokens* tokens, const doc_mem_req* mem_req, document* out_doc)
{
	const size_t chapter_size = sizeof(document_chapter) * mem_req->chapter_count;
	const size_t element_size = sizeof(document_element) * mem_req->element_count;
	const size_t note_size = sizeof(document_note) * mem_req->note_count;
	const size_t note_element_size = sizeof(document_element) * mem_req->note_element_count;

	const size_t total_size =
		chapter_size +
		element_size +
		note_size +
		note_element_size
	;
	uint8_t* mem = mem_alloc(total_size);

	out_doc->chapters = (document_chapter*)mem;
	mem += chapter_size;

	document_element* elements = (document_element*)mem;
	mem += element_size;

	document_note* notes = (document_note*)mem;
	mem += note_size;

	document_element* note_elements = (document_element*)mem;

	finalise_context ctx = {
		.doc				= out_doc,
		.tokens				= tokens->lines,
		.elements			= elements,
		.notes				= notes,
		.note_elements		= note_elements,
		.token_count		= tokens->count,
		.chapter_count		= mem_req->chapter_count,
		.element_count		= mem_req->element_count,
		.note_count			= mem_req->note_count,
		.note_element_count	= mem_req->note_element_count
	};

	line_token* token = finalise_get_next_token(&ctx);
	for (;;)
	{
		switch (token->type)
		{
		case line_token_type_eof:
			assert(ctx.current_element == ctx.element_count);
			return;
		case line_token_type_note:
			token = finalise_note(&ctx, token);
			break;
		case line_token_type_dinkus:
			token = finalise_dinkus(&ctx);
			break;
		case line_token_type_paragraph:
			token = finalise_paragraph(&ctx, token);
			break;
		case line_token_type_heading_1:
			token = finalise_heading_1(&ctx, token);
			break;
		case line_token_type_heading_2:
			token = finalise_heading_2(&ctx, token);
			break;
		case line_token_type_heading_3:
			token = finalise_heading_3(&ctx, token);
			break;
		case line_token_type_heading_4:
			token = finalise_heading_4(&ctx, token);
			break;
		case line_token_type_heading_5:
			token = finalise_heading_5(&ctx, token);
			break;
		case line_token_type_preformatted:
			//token = finalise_preformatted(&ctx, token);
			token = finalise_get_next_token(&ctx);
			break;
		case line_token_type_right_aligned:
			token = finalise_right_aligned(&ctx, token);
			break;
		case line_token_type_centre_aligned:
			token = finalise_centre_aligned(&ctx, token);
			break;
		case line_token_type_block_paragraph:
			token = finalise_blockquote(&ctx, token);
			break;
		case line_token_type_paragraph_break:
			token = finalise_paragraph_break(&ctx, token);
			break;
		case line_token_type_ordered_list_roman:
			token = finalise_ordered_list(&ctx, token, line_token_type_ordered_list_roman, document_element_type_ordered_list_begin_roman);
			break;
		case line_token_type_ordered_list_arabic:
			token = finalise_ordered_list(&ctx, token, line_token_type_ordered_list_arabic, document_element_type_ordered_list_begin_arabic);
			break;
		case line_token_type_ordered_list_letter:
			token = finalise_ordered_list(&ctx, token, line_token_type_ordered_list_letter, document_element_type_ordered_list_begin_letter);
			break;
		case line_token_type_unordered_list:
			token = finalise_unordered_list(&ctx, token);
			break;
		case line_token_type_table_row:
			token = finalise_table(&ctx, token);
			break;
		default:
			token = finalise_get_next_token(&ctx);
		}
	}
}