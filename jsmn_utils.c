#include "jsmn_utils.h"

#ifdef UNIT_TESTING
#include <setjmp.h>
#include <cmocka.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

JSMN_EXPORT
const char *jsmn_strerror(jsmnerr errno)
{
    switch (errno) {
        case JSMN_SUCCESS:
            return "*** Success, should not be printing an error. ***";
        case JSMN_ERROR_NOMEM:
            return "Not enough tokens were provided.";
        case JSMN_ERROR_LENGTH:
            return "Input data too long.";
        case JSMN_ERROR_INVAL:
            return "Invalid character inside JSON string.";
        case JSMN_ERROR_PART:
            return "The string is not a full JSON packet, more bytes expected.";
        case JSMN_ERROR_BRACKETS:
            return "The JSON string has unmatched brackets.";
    }

    return NULL;
}

JSMN_EXPORT
jsmntok_t *jsmn_tokenize(const char *json, const size_t json_len, jsmnint_t *rv)
{
    jsmn_parser p;

    jsmn_init(&p);
    *rv = jsmn_parse(&p, json, json_len, NULL, 0);

    /* enum jsmnerr has four errors, thus */
    if (*rv >= (jsmnint_t)JSMN_ERROR_MAX) {
        fprintf(stderr, "jsmn_parse error: %s\n", jsmn_strerror(*rv));
        return NULL;
    }

/*  fprintf(stderr, "jsmn_parse: %d tokens found.\n", *rv); */

    jsmntok_t *tokens = calloc(*rv, sizeof(jsmntok_t));

    jsmn_init(&p);
    *rv = jsmn_parse(&p, json, json_len, tokens, *rv);

    return tokens;
}

JSMN_EXPORT
jsmnint_t jsmn_tokenize_noalloc(jsmntok_t *tokens, const uint32_t num_tokens, const char *json, const size_t json_len)
{
    jsmn_parser p;
    jsmn_init(&p);

    jsmnint_t rv;

    rv = jsmn_parse(&p, json, json_len, tokens, num_tokens);

    /* enum jsmnerr has four errors, thus */
    if (rv >= (jsmnint_t)JSMN_ERROR_MAX) {
        fprintf(stderr, "jsmn_parse error: %s\n", jsmn_strerror(rv));
        return rv;
    }

/*  fprintf(stderr, "jsmn_parse: %d tokens found.\n", rv); */

    return rv;
}

JSMN_EXPORT
int jsmn_streq(const char *json, const jsmntok_t *tok, const char *s)
{
    if ((tok->type & JSMN_STRING) && strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return JSMN_SUCCESS;
    }
    return -1;
}

JSMN_EXPORT
jsmnint_t jsmn_get_prev_sibling(const jsmntok_t *tokens, const jsmnint_t t)
{
#if defined(JSMN_NEXT_SIBLING) && defined(JSMN_PARENT_LINKS)
    jsmnint_t sibling;

    /* Start with parent's first child */
    if (tokens[t].parent == JSMN_NEG) {
        return JSMN_NEG;
    }

    sibling = tokens[t].parent + 1;

    /* If the first child is the current token */
    if (sibling == t) {
        return JSMN_NEG;
    }

    /* Loop until we find previous sibling */
    while (tokens[sibling].next_sibling != t) {
        sibling = tokens[sibling].next_sibling;
    }

    return sibling;
#else
    jsmnint_t remaining, sibling = t;
    for (remaining = JSMN_NEG; remaining != 1 && sibling != JSMN_NEG; remaining++, sibling--) {
        remaining -= tokens[sibling]->size;
    }
    return sibling;
#endif
}

JSMN_EXPORT
jsmnint_t jsmn_get_next_sibling(const jsmntok_t *tokens, const jsmnint_t t)
{
#if defined(JSMN_NEXT_SIBLING)
    return tokens[t].next_sibling;
#else
    jsmnint_t remaining, sibling = t;
    for (remaining = 1; remaining != JSMN_NEG; remaining--, sibling++) {
        remaining += tokens[sibling]->size;
    }
    return sibling;
#endif
}

static
jsmnint_t jsmn_lookup_object(const char *json, const jsmntok_t *tokens, const jsmnint_t parent, const char *key)
{
    /* first child is the first token after the parent */
    jsmnint_t child = parent + 1;

    /* loop through children */
    while (child != JSMN_NEG) {
        /* if child's string is equal to key */
        if (jsmn_streq(json, &tokens[child], key) == JSMN_SUCCESS) {
            /* return current child */
            return child;
        }

        /* move to the next child */
        child = jsmn_get_next_sibling(tokens, child);
    }

    /* key didn't match any of the json keys */
    return JSMN_NEG;
}

static
jsmnint_t jsmn_lookup_array(const jsmntok_t *tokens, const jsmnint_t parent, const jsmnint_t key)
{
    /* if parent's children is less than or equal to key, key is bad */
    if (tokens[parent].size <= key)
        return JSMN_NEG;

    /* first child is the first token after the parent */
    jsmnint_t i, child = parent + 1;
    /* loop through children until you reach the nth child */
    for (i = 0; i < key; i++) {
        child = jsmn_get_next_sibling(tokens, child);
    }

    /* return nth child */
    return child;
}

JSMN_EXPORT
jsmnint_t jsmn_lookup(const char *json, const jsmntok_t *tokens, const size_t num_keys, ...)
{
    jsmnint_t i, pos;

    /* keys may be either const char * or jsmnint_t, at this point we don't care */
    va_list keys;
    va_start(keys, num_keys);

    /* start at position zero */
    pos = 0;
    for (i = 0; i < num_keys; i++) {
        if (tokens[pos].type & JSMN_OBJECT) {
            /* if `pos`.type is an object, treat key as a const char * */
            pos = jsmn_lookup_object(json, tokens, pos, va_arg(keys, void *));
            if (pos == JSMN_NEG) { break; }
            /* move position to current key's value (with check) */
            if (tokens[pos].type & JSMN_KEY) {
                pos++;
            }
        } else if (tokens[pos].type & JSMN_ARRAY) {
            /* if `pos`.type is an array, treat key as a jsmnint_t (by way of uintptr_t) */
            pos = jsmn_lookup_array(tokens, pos, (uintptr_t)va_arg(keys, void *));
        } else {
            /* `pos` must be either an object or array */
            pos = JSMN_NEG;
            break;
        }

        /* if jsmn_parse_{object,array} returns JSMN_NEG, break */
        if (pos == JSMN_NEG) {
            break;
        }
    }

    va_end(keys);
    return pos;
}

JSMN_EXPORT
void jsmn_explodeJSON(const char *json, const size_t len)
{
    jsmnint_t rv, i, depth;
    char c;

    jsmntok_t *tokens = jsmn_tokenize(json, len, &rv);
    jsmntok_t *token;

    if (rv >= (jsmnint_t)-4) {
        printf("jsmn_parse error: %s\n", jsmn_strerror(rv));
        return;
    }

    const char *jsmntype[] = { "", "Object", "Array", "", "String", "", "", "", "Primitive", };
    const char *jsmnextr[] = { "", "Key  ", "Value", "", "", "", "", "", "", };

    printf("\n");
    printf("    Token |      Type |    Start |      End |   Length | Children |   Parent |  Sibling | K/V   | \n");
    printf("----------+-----------+----------+----------+----------+----------+----------+----------+-------+-\n");
    for (i = 0, depth = 0; i < rv; i++) {
        token = &tokens[i];
        printf(   "%9d", i);
        printf(" | %9s", jsmntype[token->type & JSMN_VAL_TYPE]);
        printf(" | %8d", token->start);
        printf(" | %8d", token->end);
        printf(" | %8d", token->end - token->start);
        printf(" | %8d", token->size);
        printf(" | %8d", token->parent != JSMN_NEG ? token->parent : -1);
        printf(" | %8d", token->next_sibling != JSMN_NEG ? token->next_sibling : -1);
        printf(" | %5s", jsmnextr[(token->type & (JSMN_KEY | JSMN_VALUE)) >> 4]);
        printf(" |");

        if (token->type & JSMN_CONTAINER) {
            printf("%*s  %s\n", depth << 2, "", token->type & JSMN_OBJECT ? "{" : "[");
            depth += 1;
            continue;
        }

        if (token->type & JSMN_KEY) {
            c = (token->type & JSMN_STRING) ? '\"' : ' ';
            printf("%*s%c%.*s%c :\n", depth << 2, "", c, token->end - token->start, &json[token->start], c);
            continue;
        }

        printf("%*s", depth << 2, "");
        if (token->type & JSMN_INSD_OBJ)
            printf("  ");
        c = (token->type & JSMN_STRING) ? '\"' : ' ';
        printf("%c%.*s%c", c, token->end - token->start, &json[token->start], c);
        if ((token->type & JSMN_INSD_OBJ && tokens[token->parent].next_sibling != JSMN_NEG) ||
               token->next_sibling != JSMN_NEG) {
            printf(",\n");
            continue;
        }

        printf("\n          |           |          |          |          |          |          |          | Close |");

        depth -= 1;
        if (depth == JSMN_NEG) {
          printf("  %c\n", tokens[0].type == JSMN_OBJECT ? '}' : ']');
          continue;
        }

        if (tokens[token->parent].type & JSMN_ARRAY) {
            printf("%*s  ]", depth << 2, "");
            if (tokens[token->parent].parent != 0 &&
                    tokens[tokens[token->parent].parent].next_sibling != JSMN_NEG) {
                printf(",");
            }
            printf("\n");
        }
        else if (tokens[tokens[token->parent].parent].type & JSMN_OBJECT) {
            printf("%*s  }", depth << 2, "");
            if (tokens[tokens[token->parent].parent].parent != JSMN_NEG &&
                    tokens[tokens[tokens[token->parent].parent].parent].next_sibling != JSMN_NEG) {
                printf(",");
            }
            printf("\n");
        }
    }

    free(tokens);
}
