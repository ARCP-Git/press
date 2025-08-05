/*
	Rules:
	* First word is always capitalised.
	* Last word is always capitalised.
	* Words of four letters or more.
	* Words after a colon.
	* Words after an em dash.
*/

const char* heading_caps_blacklist[] = {
	// Articles
	"A",
	"An",
	"The",
	// Short conjunctions
	"And",
	"As",
	"But",
	"For",
	"If",
	"Nor",
	"Or",
	"So",
	"Yet",
	// Short prepositions
	"As",
	"At",
	"By",
	"For",
	"In",
	"Of",
	"Off",
	"On",
	"Per",
	"To",
	"Up",
	"Via"
};
const int64_t heading_caps_blacklist_count = sizeof(heading_caps_blacklist) / sizeof(const char*);

// TODO: Pass these by pointer
static char heading_word[4];
static bool heading_restart;

static void check_heading_word(const char* heading)
{
	for (int64_t i = 0; i < heading_caps_blacklist_count; ++i)
	{
		if (strcmp(heading_word, heading_caps_blacklist[i]) == 0)
			handle_error("Heading contains capitalised word that should be lowercase: \"%s\", in heading: \"%s\".", heading_word, heading);
	}
}

static const char* skip_initial_chars_in_header(const char* heading)
{
	for (;;)
	{
		const char c = *heading++;

		if (
			(c >= '0' && c <= '9') ||
			(c <= ' ') ||
			c == '.'
		)
		{
		}
		else
		{
			return heading;
		}
	}
}

static int get_next_heading_word(const char* str)
{
	for (int i = 0; i < 4; ++i)
	{
		if (
			str[i] == text_token_type_em_dash ||
			str[i] == ':'
		)
		{
			heading_restart = true;
			heading_word[i] = 0;
			return i;
		}
		else if (
			str[i] == text_token_type_null ||
			str[i] == ',' ||
			str[i] == ' ' ||
			str[i] == '(' ||
			str[i] == ')' ||
			str[i] == '-'
		)
		{
			heading_word[i] = 0;
			return i;
		}
		else
		{
			heading_word[i] = str[i];
		}
	}

	return 4;
}

void check_heading_caps(const char* heading)
{
	const char* str = heading;

	str = skip_initial_chars_in_header(str);
	heading_restart = false;

	for (;;)
	{
		if (*str == text_token_type_null)
			return;

		const int len = get_next_heading_word(str);
		if (len && heading_restart)
		{
			heading_restart = false;
			str += len;
		}
		else if (len && len < 4)
		{
			check_heading_word(heading);
			str += len;
		}
		else
		{
			++str;
		}
	}
}