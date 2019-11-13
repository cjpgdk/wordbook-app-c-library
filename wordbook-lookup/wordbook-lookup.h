#ifndef WORDBOOK_LOOKUP_H
#define WORDBOOK_LOOKUP_H

void print_help_text(const char *app);
void print_all_dictionaries(const char *search);
// convert all chars to lower and reutrn strstr (Return cannot be used! Only checked!)
char *strstr_i(const char *s1, const char *s2);

#endif // WORDBOOK_LOOKUP_H
