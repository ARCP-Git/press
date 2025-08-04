static void create_docx_content_types(void)
{
	file f = open_file(OUTPUT_DIR "/docx/[Content_Types].xml", file_mode_write);

	print_str(f,
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
		"<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">\n"
		"\t<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>\n"
		"\t<Default Extension=\"xml\" ContentType=\"application/xml\"/>\n"
		"\t<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>\n"
		"\t<Override PartName=\"/word/numbering.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.numbering+xml\"/>\n"
		"\t<Override PartName=\"/word/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml\"/>\n"
		"</Types>"
	);

	close_file(f);
}

static void create_docx_rels(void)
{
	file f = open_file(OUTPUT_DIR "/docx/_rels/.rels", file_mode_write);

	print_str(f,
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
		"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n"
		"\t<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>\n"
		"</Relationships>"
	);

	close_file(f);
}

static void create_docx_doc_rels(void)
{
	file f = open_file(OUTPUT_DIR "/docx/word/_rels/document.xml.rels", file_mode_write);

	print_str(f,
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
		"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n"
		"\t<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>\n"
		"\t<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/numbering\" Target=\"numbering.xml\"/>\n"
		"</Relationships>"
	);

	close_file(f);
}

static void create_docx_styles(void)
{
	file f = open_file(OUTPUT_DIR "/docx/word/styles.xml", file_mode_write);

	print_str(f,
		"<w:styles xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">\n"
		"\t<w:style w:type=\"paragraph\" w:default=\"1\" w:styleId=\"Normal\">\n"
		"\t\t<w:name w:val=\"Normal\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:spacing w:after=\"240\"/>\n"
		"\t\t\t<w:jc w:val=\"both\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:lang w:val=\"en-CA\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:styleId=\"Title\">\n"
		"\t\t<w:name w:val=\"Title\"/>\n"
		"\t\t<w:next w:val=\"Normal\"/>\n"
		"\t\t<w:link w:val=\"TitleChar\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:spacing w:after=\"480\" w:line=\"240\" w:lineRule=\"auto\"/>\n"
		"\t\t\t<w:jc w:val=\"center\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:spacing w:val=\"-10\"/>\n"
		"\t\t\t<w:sz w:val=\"56\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:styleId=\"Subtitle\">\n"
		"\t\t<w:name w:val=\"Subtitle\"/>\n"
		"\t\t<w:basedOn w:val=\"Title\"/>\n"
		"\t\t<w:next w:val=\"Normal\"/>\n"
		"\t\t<w:link w:val=\"SubtitleChar\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:numPr>\n"
		"\t\t\t\t<w:ilvl w:val=\"1\"/>\n"
		"\t\t\t</w:numPr>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:spacing w:val=\"15\"/>\n"
		"\t\t\t<w:sz w:val=\"44\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:styleId=\"Heading1\">\n"
		"\t\t<w:name w:val=\"heading 1\"/>\n"
		"\t\t<w:next w:val=\"Normal\"/>\n"
		"\t\t<w:link w:val=\"Heading1Char\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:keepNext/>\n"
		"\t\t\t<w:keepLines/>\n"
		"\t\t\t<w:spacing w:before=\"240\" w:after=\"240\"/>\n"
		"\t\t\t<w:outlineLvl w:val=\"0\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:sz w:val=\"40\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:styleId=\"Heading2\">\n"
		"\t\t<w:name w:val=\"heading 2\"/>\n"
		"\t\t<w:next w:val=\"Normal\"/>\n"
		"\t\t<w:link w:val=\"Heading2Char\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:keepNext/>\n"
		"\t\t\t<w:keepLines/>\n"
		"\t\t\t<w:spacing w:before=\"240\" w:after=\"240\"/>\n"
		"\t\t\t<w:outlineLvl w:val=\"1\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:sz w:val=\"32\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:styleId=\"Heading3\">\n"
		"\t\t<w:name w:val=\"heading 3\"/>\n"
		"\t\t<w:next w:val=\"Normal\"/>\n"
		"\t\t<w:link w:val=\"Heading3Char\"/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:keepNext/>\n"
		"\t\t\t<w:keepLines/>\n"
		"\t\t\t<w:outlineLvl w:val=\"2\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:sz w:val=\"28\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:styleId=\"Heading4\">\n"
		"\t\t<w:name w:val=\"heading 4\"/>\n"
		"\t\t<w:next w:val=\"Normal\"/>\n"
		"\t\t<w:link w:val=\"Heading4Char\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:keepNext/>\n"
		"\t\t\t<w:keepLines/>\n"
		"\t\t\t<w:spacing w:before=\"80\" w:after=\"40\"/>\n"
		"\t\t\t<w:outlineLvl w:val=\"3\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:i/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:styleId=\"Heading5\">\n"
		"\t\t<w:name w:val=\"heading 5\"/>\n"
		"\t\t<w:basedOn w:val=\"Normal\"/>\n"
		"\t\t<w:next w:val=\"Normal\"/>\n"
		"\t\t<w:link w:val=\"Heading5Char\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:keepNext/>\n"
		"\t\t\t<w:keepLines/>\n"
		"\t\t\t<w:spacing w:before=\"80\" w:after=\"40\"/>\n"
		"\t\t\t<w:outlineLvl w:val=\"4\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:styleId=\"ListParagraph\">\n"
		"\t\t<w:name w:val=\"List Paragraph\"/>\n"
		"\t\t<w:basedOn w:val=\"Normal\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:numPr>\n"
		"\t\t\t\t<w:numId w:val=\"1\"/>\n"
		"\t\t\t</w:numPr>\n"
		"\t\t\t<w:ind w:left=\"357\" w:hanging=\"357\"/>\n"
		"\t\t\t<w:contextualSpacing/>\n"
		"\t\t</w:pPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:customStyle=\"1\" w:styleId=\"BlockQuote\">\n"
		"\t\t<w:name w:val=\"Block Quote\"/>\n"
		"\t\t<w:basedOn w:val=\"Normal\"/>\n"
		"\t\t<w:next w:val=\"QuoteBlockCitation\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:spacing w:before=\"240\" w:after=\"240\"/>\n"
		"\t\t\t<w:ind w:left=\"567\" w:right=\"567\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"paragraph\" w:customStyle=\"1\" w:styleId=\"QuoteBlockCitation\">\n"
		"\t\t<w:name w:val=\"Quote Block Citation\"/>\n"
		"\t\t<w:qFormat/>\n"
		"\t\t<w:pPr>\n"
		"\t\t\t<w:spacing w:after=\"240\"/>\n"
		"\t\t\t<w:ind w:left=\"567\" w:right=\"567\"/>\n"
		"\t\t\t<w:jc w:val=\"right\"/>\n"
		"\t\t</w:pPr>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"character\" w:default=\"1\" w:styleId=\"DefaultParagraphFont\">\n"
		"\t\t<w:name w:val=\"Default Paragraph Font\"/>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"character\" w:customStyle=\"1\" w:styleId=\"Heading1Char\">\n"
		"\t\t<w:name w:val=\"Heading 1 Char\"/>\n"
		"\t\t<w:basedOn w:val=\"DefaultParagraphFont\"/>\n"
		"\t\t<w:link w:val=\"Heading1\"/>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:sz w:val=\"40\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"character\" w:customStyle=\"1\" w:styleId=\"Heading2Char\">\n"
		"\t\t<w:name w:val=\"Heading 2 Char\"/>\n"
		"\t\t<w:basedOn w:val=\"DefaultParagraphFont\"/>\n"
		"\t\t<w:link w:val=\"Heading2\"/>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:sz w:val=\"32\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"character\" w:customStyle=\"1\" w:styleId=\"Heading3Char\">\n"
		"\t\t<w:name w:val=\"Heading 3 Char\"/>\n"
		"\t\t<w:basedOn w:val=\"DefaultParagraphFont\"/>\n"
		"\t\t<w:link w:val=\"Heading3\"/>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:sz w:val=\"28\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"character\" w:customStyle=\"1\" w:styleId=\"Heading4Char\">\n"
		"\t\t<w:name w:val=\"Heading 4 Char\"/>\n"
		"\t\t<w:basedOn w:val=\"DefaultParagraphFont\"/>\n"
		"\t\t<w:link w:val=\"Heading4\"/>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:i/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"character\" w:customStyle=\"1\" w:styleId=\"Heading5Char\">\n"
		"\t\t<w:name w:val=\"Heading 5 Char\"/>\n"
		"\t\t<w:basedOn w:val=\"DefaultParagraphFont\"/>\n"
		"\t\t<w:link w:val=\"Heading5\"/>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:i/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"character\" w:customStyle=\"1\" w:styleId=\"TitleChar\">\n"
		"\t\t<w:name w:val=\"Title Char\"/>\n"
		"\t\t<w:basedOn w:val=\"DefaultParagraphFont\"/>\n"
		"\t\t<w:link w:val=\"Title\"/>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:spacing w:val=\"-10\"/>\n"
		"\t\t\t<w:sz w:val=\"56\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"\t<w:style w:type=\"character\" w:customStyle=\"1\" w:styleId=\"SubtitleChar\">\n"
		"\t\t<w:name w:val=\"Subtitle Char\"/>\n"
		"\t\t<w:basedOn w:val=\"DefaultParagraphFont\"/>\n"
		"\t\t<w:link w:val=\"Subtitle\"/>\n"
		"\t\t<w:rPr>\n"
		"\t\t\t<w:rFonts w:ascii=\"Georgia\" w:hAnsi=\"Georgia\"/>\n"
		"\t\t\t<w:spacing w:val=\"15\"/>\n"
		"\t\t\t<w:sz w:val=\"44\"/>\n"
		"\t\t</w:rPr>\n"
		"\t</w:style>\n"
		"</w:styles>"
	);

	close_file(f);
}

static void create_docx_numbering(void)
{
	file f = open_file(OUTPUT_DIR "/docx/word/numbering.xml", file_mode_write);

	print_str(f,
		"<w:numbering xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">\n"
		"\t<w:abstractNum w:abstractNumId=\"0\">\n"
		"\t\t<w:nsid w:val=\"5DF9545D\"/>\n"
		"\t\t<w:multiLevelType w:val=\"singleLevel\"/>\n"
		"\t\t<w:tmpl w:val=\"76B45018\"/>\n"
		"\t\t<w:lvl w:ilvl=\"0\">\n"
		"\t\t\t<w:start w:val=\"1\"/>\n"
		"\t\t\t<w:numFmt w:val=\"decimal\"/>\n"
		"\t\t\t<w:lvlText w:val=\"%1.\"/>\n"
		"\t\t</w:lvl>\n"
		"\t</w:abstractNum>\n"
		"\t<w:num w:numId=\"1\">\n"
		"\t\t<w:abstractNumId w:val=\"0\"/>\n"
		"\t</w:num>\n"
		"</w:numbering>"
	);

	close_file(f);
}

typedef enum
{
	docx_run_type_none,
	docx_run_type_normal,
	docx_run_type_strong,
	docx_run_type_emphasis
} docx_run_type;

static void begin_docx_text_run(file f, docx_run_type type, docx_run_type wanted, const char* props)
{
	if (type != docx_run_type_none && type != wanted)
	{
		print_str(f,
			"</w:t>\n"
			"\t\t\t</w:r>\n"
		);
	}

	if (type == docx_run_type_none || wanted != docx_run_type_normal)
	{
		print_str(f, "\t\t\t<w:r>\n");
		print_str(f, props);
		print_str(f, "\t\t\t\t<w:t xml:space=\"preserve\">");
	}
}

static void end_docx_text_run(file f, docx_run_type type)
{
	if (type != docx_run_type_none)
	{
		print_str(f,
			"</w:t>\n"
			"\t\t\t</w:r>\n"
		);
	}
}

static void print_docx_text_block(file f, const char* text)
{
	docx_run_type type = docx_run_type_none;

	while (*text)
	{
		if (*text == text_token_type_strong_begin)
		{
			begin_docx_text_run(f, type, docx_run_type_strong, "\t\t\t\t<w:rPr><w:b w:val=\"true\"/></w:rPr>\n");
			type = docx_run_type_strong;
		}
		else if (*text == text_token_type_strong_end)
		{
			end_docx_text_run(f, type);
			type = docx_run_type_none;
		}
		else if (*text == text_token_type_emphasis_begin)
		{
			begin_docx_text_run(f, type, docx_run_type_emphasis, "\t\t\t\t<w:rPr><w:i w:val=\"true\"/></w:rPr>\n");
			type = docx_run_type_emphasis;
		}
		else if (*text == text_token_type_emphasis_end)
		{
			end_docx_text_run(f, type);
			type = docx_run_type_none;
		}
		else if (*text == text_token_type_note)
		{
			// TODO
		}
		else
		{
			if (type == docx_run_type_none)
			{
				begin_docx_text_run(f, type, docx_run_type_normal, "");
				type = docx_run_type_normal;
			}

			print_char_token(f, *text);
		}

		++text;
	}

	end_docx_text_run(f, type);
}

static void create_docx_document(const document* doc)
{
	file f = open_file(OUTPUT_DIR "/docx/word/document.xml", file_mode_write);

	print_str(f,
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
		"<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">\n"
		"\t<w:body>\n"
	);

	int depth = 2;
	bool inside_block_quote = false;

	for (uint32_t chapter_index = 0; chapter_index < doc->chapter_count; ++chapter_index)
	{
		document_chapter* chapter = &doc->chapters[chapter_index];

		for (uint32_t element_index = 0; element_index < chapter->element_count; ++element_index)
		{
			document_element* element = &chapter->elements[element_index];

			switch (element->type)
			{
			case document_element_type_heading_1:
				print_str(f,
					"\t\t<w:p>\n"
					"\t\t\t<w:pPr><w:pStyle w:val=\"Heading1\"/></w:pPr>\n"
				);
				print_docx_text_block(f, element->text);
				print_str(f, "\t\t</w:p>\n");
				break;
			case document_element_type_heading_2:
				print_str(f,
					"\t\t<w:p>\n"
					"\t\t\t<w:pPr><w:pStyle w:val=\"Heading2\"/></w:pPr>\n"
				);
				print_docx_text_block(f, element->text);
				print_str(f, "\t\t</w:p>\n");
				break;
			case document_element_type_heading_3:
				print_str(f,
					"\t\t<w:p>\n"
					"\t\t\t<w:pPr><w:pStyle w:val=\"Heading3\"/></w:pPr>\n"
				);
				print_docx_text_block(f, element->text);
				print_str(f, "\t\t</w:p>\n");
				break;
			case document_element_type_text_block:
				print_docx_text_block(f, element->text);
				break;
			case document_element_type_paragraph_begin:
				print_str(f, "\t\t<w:p>\n");
				if (inside_block_quote)
					print_str(f, "\t\t\t<w:pPr><w:pStyle w:val=\"BlockQuote\"/></w:pPr>\n");
				break;
			case document_element_type_paragraph_end:
				print_str(f, "\t\t</w:p>\n");
				break;
			case document_element_type_blockquote_begin:
				inside_block_quote = true;
				break;
			case document_element_type_blockquote_end:
				inside_block_quote = false;
				break;
			case document_element_type_blockquote_citation:
				print_str(f,
					"\t\t<w:p>\n"
					"\t\t\t<w:pPr><w:pStyle w:val=\"QuoteBlockCitation\"/></w:pPr>\n"
				);
				print_docx_text_block(f, element->text);
				print_str(f, "\t\t</w:p>\n");
				break;
			case document_element_type_ordered_list_begin_arabic:
				break;
			case document_element_type_ordered_list_end:
				break;
			case document_element_type_list_item:
				print_str(f,
					"\t\t<w:p>\n"
					"\t\t\t<w:pPr><w:pStyle w:val=\"ListParagraph\"/></w:pPr>\n"
				);
				print_docx_text_block(f, element->text);
				print_str(f, "\t\t</w:p>\n");
				break;
			}
		}
	}

	print_str(f,
		"\t\t<w:sectPr w:rsidR=\"005F75AD\" w:rsidRPr=\"005F75AD\">\n"
		"\t\t\t<w:pgSz w:w=\"11906\" w:h=\"16838\"/>\n"
		"\t\t\t<w:pgMar w:top=\"1440\" w:right=\"1440\" w:bottom=\"1440\" w:left=\"1440\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>\n"
		"\t\t\t<w:cols w:space=\"708\"/>\n"
		"\t\t\t<w:docGrid w:linePitch=\"360\"/>\n"
		"\t\t</w:sectPr>\n"
		"\t</w:body>\n"
		"</w:document>"
	);

	close_file(f);
}

static void generate_docx(const document* doc)
{
	create_dir(OUTPUT_DIR "\\docx");
	create_dir(OUTPUT_DIR "\\docx\\_rels");
	create_dir(OUTPUT_DIR "\\docx\\word");
	create_dir(OUTPUT_DIR "\\docx\\word\\_rels");

	create_docx_content_types();
	create_docx_rels();
	create_docx_doc_rels();
	create_docx_styles();
	create_docx_numbering();
	create_docx_document(doc);

	// The following two arrays MUST be in sync
	const char* inputs[] = {
		OUTPUT_DIR "/docx/[Content_Types].xml",
		OUTPUT_DIR "/docx/_rels/.rels",
		OUTPUT_DIR "/docx/word/_rels/document.xml.rels",
		OUTPUT_DIR "/docx/word/styles.xml",
		OUTPUT_DIR "/docx/word/numbering.xml",
		OUTPUT_DIR "/docx/word/document.xml"
	};
	const uint32_t input_count = sizeof(inputs) / sizeof(const char*);

	const char* outputs[] = {
		"[Content_Types].xml",
		"_rels/.rels",
		"word/_rels/document.xml.rels",
		"word/styles.xml",
		"word/numbering.xml",
		"word/document.xml"
	};
	static_assert(sizeof(outputs) == sizeof(inputs));
	const uint32_t output_count = sizeof(outputs) / sizeof(const char*);

	const char* odt_path = generate_path(OUTPUT_DIR "/%s.docx", doc->metadata.filename);
	generate_zip(odt_path, inputs, outputs, output_count);

	delete_dir(OUTPUT_DIR "\\docx");
}