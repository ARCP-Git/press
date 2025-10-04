static void output_css(html_context* ctx)
{
	// Default universal settings
	print_str(ctx->f,
		// This adds support for light and dark modes based on browser settings
		":root {\n"
		"	color-scheme: light dark;\n"
		"}\n\n"

		"body {\n"
		// Set font and base size
		"	font-family: 'Georgia', serif;\n"
		"	font-size: 18px;\n"
		// Set theme colours
		"	color: light-dark(#000000, #E0E0E0);\n"
		"	background-color: light-dark(#FFFFFF, #1E1E1E);\n"
		// Centre text at a comfortable column width for reading
		"	max-width: 55ch;\n"
		"	margin-left: auto;\n"
		"	margin-right: auto;\n"
		// Add padding so we don't go up against the edges of small displays
		"	padding-left: 1em;\n"
		"	padding-right: 1em;\n"
		"	padding-bottom: 1em;\n"
		// Prevents very long links from extending the page size
		"	overflow-wrap: break-word;\n"
		"}\n\n"

		"aside {\n"
		"	outline: solid;\n"
		"	margin-top: 1em;\n"
		"	margin-bottom: 1em;\n"
		"	padding: 1em;\n"
		"}\n\n"

		"aside p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		"aside h1 {\n"
		"	font-size: 18px;\n"
		"	text-align: left;\n"
		"	margin-top: 0;\n"
		"}\n\n"

		"aside p + p {\n"
		"	text-indent: 1.5em;\n"
		"}\n\n"

		// Paragraphs following asides are not indented
		"aside + p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Chapter headings are centred
		"h1 {\n"
		"	text-align: center;\n"
		"}\n\n"

		// Title heading
		"h1.title {\n"
		"	font-size: 48px;\n"
		"	padding-top: 128px;\n"
		"	padding-bottom: 128px;\n"
		"}\n\n"

		// Superscript
		"sup {\n"
		"	line-height: 0;\n"		// Prevent notes from increasing line height
		"	font-size: 0.75em;\n"
		"}\n\n"

		// Remove underlines from hyperlinks
		"a {\n"
		"	text-decoration: none;\n"
		"}\n\n"

		// Add underline when hovering over link
		"a:hover {\n"
		"	text-decoration: underline;\n"
		"}\n\n"

		// Links should not stand out when printing
		"@media print {\n"
		"	a {\n"
		"		color: black;\n"
		"	}\n"
		"}\n\n"

		// Avoid new lines after a heading when printing
		"h1, h2, h3, h4, h5 {\n"
		"	page-break-after: avoid;\n"
		"}\n\n"

		// Paragraphs which don't follow a heading are not indented
		"p {\n"
		"	margin-top: 0;\n"
		"	text-indent: 1.5em;\n"
		"	text-align: justify;\n"
		"	hyphens: auto;\n"
		"	margin-bottom: 0;\n"
		"}\n\n"

		// Right-aligned paragraphs
		"p.right-aligned {\n"
		"	margin-top: 1em;\n"
		"	margin-bottom: 1em;\n"
		"	text-align: right;\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Centre-aligned paragraphs
		"p.centre-aligned {\n"
		"	margin-top: 1em;\n"
		"	margin-bottom: 1em;\n"
		"	text-align: center;\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Paragraphs following aligned are not indented
		"p.right-aligned + p,\n"
		"p.centre-aligned + p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Paragraph with previous gap
		"p.paragraph-break {\n"
		"	margin-top: 1em;\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Paragraph with previous gap
		"p.footnote-paragraph-break {\n"
		"	font-size: 0.75em;\n"
		"	margin-top: 1em;\n"
		"	text-indent: 0;\n"
		"}\n\n"

		"p.footnote-paragraph-break + p.footnote {\n"
		"	margin-top: 1em;\n"
		"}\n\n"

		// Authors
		"p.authors {\n"
		"	text-align: center;\n"
		"	padding-top: 0;\n"
		"	padding-bottom: 128px;\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// First footnote paragraph
		"p.footnote {\n"
		"	margin-top: 1.5em;\n"
		"	text-indent: 0;\n"
		"	font-size: 0.75em;\n"
		"}\n\n"

		// Subsequent footnote paragraph
		"p.footnote_paragraph {\n"
		"	text-indent: 1.5em;\n"
		"	font-size: 0.75em;\n"
		"}\n\n"

		// Paragraphs after tables
		"table + p.footnote_paragraph {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		"p.footnote + table, p.footnote_paragraph + table {\n"
		"	margin-top: 0.75em;\n"
		"	margin-bottom: 0.75em;\n"
		"}\n\n"

		"table + p.footnote {\n"
		"	margin-top: 0.75em;\n"
		"}\n\n"

		// Footnotes following another footnote
		"p.footnote_paragraph + p.footnote {\n"
		"	margin-top: 1em;\n"
		"}\n\n"

		"p.footnote + p.footnote {\n"
		"	margin-top: 1em;\n"
		"}\n\n"

		// Paragraphs after headings are not indented
		"h1 + p,\n"
		"h2 + p,\n"
		"h3 + p,\n"
		"h4 + p,\n"
		"h5 + p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Blockquote indentation
		"blockquote {\n"
		"	margin-left: 1.5em;\n"
		"}\n\n"

		// First paragraphs within a blockquote are not indented
		"blockquote p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Paragraphs following first blockquote paragraph are indented
		"blockquote p + p {\n"
		"	text-indent: 1.5em;\n"
		"}\n\n"

		// Paragraphs following blockquotes are not indented
		"blockquote + p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Paragraphs following dinkuses are not indented
		"hr + p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Lists
		"ol, ul {\n"
		"	text-align: justify;\n"
		"	hyphens: auto;\n"
		"	margin-left: 1.5em;\n"
		"	padding-left: 0;\n"
		"}\n\n"

		// Paragraphs following lists are not be indented
		"ol + p,\n"
		"ul + p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		// Chapter list should be left-aligned
		"ul.chapters {\n"
		"	text-align: left;\n"
		"}\n\n"

		// Turn HR tags into dinkuses ("* * *")
		"hr {\n"
		"	border: none;\n"
		"	word-spacing: 1em;\n"
		"	margin-top: 1.5em;\n"
		"	margin-bottom: 1.5em;\n"
		"}\n\n"

		"hr::before {\n"
		"	color: light-dark(#000000, #E0E0E0);\n"
		"	content: '* * *';\n"
		"	display: block;\n"
		"	text-align: center;\n"
		"}\n\n"

		// Tables
		"table {\n"
		"	border: 1px solid white;\n"
		"	border-collapse: collapse;\n"
		//"	padding: 0;\n"
		"	margin-top: 1em;\n"
		"	margin-bottom: 1em;\n"
		"	margin-left: auto;\n"
		"	margin-right: auto;\n"
		"}\n\n"

		"th, td {\n"
		"	border: 1px solid white;\n"
		"	font-size: 0.75em;\n"
		"	padding: 0.25em;\n"
		//"	padding-left: 1em;\n"
		"	vertical-align: top;\n"
		"}\n\n"

		"th:empty, td:empty {\n"
		"	border-top: none;\n"
		"	border-bottom: none;\n"
		"	border-left: none;\n"
		"	border-right: none;\n"
		"}\n\n"

		// Each cell is padded on the left except the first one on each row
		//"tr td:first-child {\n"
		//"	padding-left: 0;\n"
		//"}\n\n"

		// Paragraphs after tables
		"table + p {\n"
		"	text-indent: 0;\n"
		"}\n\n"

		"td:empty::after {\n"
		"	content: \"\\00A0\";\n"
		"}\n\n"
	);
}