#include "tokens.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void free_tokens(tokens_t *t) {
    if (t == NULL) return;

    if (t->tokens != NULL) {
        for (int i = 0; i < t->num; i++) {
            if (t->tokens[i] == NULL) continue;
            free(t->tokens[i]);
        }
        free(t->tokens);
    }
    free(t);
}

tokens_t *tokenize_line(char **lineptr, size_t line_size) {
    tokens_t *tkn;
    char *token, *saveptr;

    const char* DELIMITER = "&";

    token = strtok_r(*lineptr, DELIMITER, &saveptr);

    if (token == NULL) {
        /* line is empty */
        return NULL;
    }

    tkn = malloc(sizeof(tokens_t));

    if (tkn == NULL) {
        /* malloc failed */
        return NULL;
    }

    tkn->num = 1;
    tkn->tokens = malloc(sizeof(tkn->tokens) * tkn->num);

    if (tkn->tokens == NULL) {
        /* malloc failed */
        free(tkn);
        return NULL;
    }

    tkn->tokens[0] = malloc(sizeof(char) * (strlen(token) + 1));
    strncpy(tkn->tokens[0], token, strlen(token) + 1);

    /* look for the rest of the tokens */
    while ((token = strtok_r(NULL, DELIMITER, &saveptr)) != NULL) {
        /* for every new token found */
        /* allocate space for the token pointer */
        char **tmp = realloc(tkn->tokens, sizeof(tkn->tokens) * (tkn->num + 1));

        if (tmp == NULL) {
            /* realloc failed */
            free_tokens(tkn);
            return NULL;
        }

        tkn->tokens = tmp;

        char *extracted_tk = malloc(sizeof(char) * (strlen(token) + 1));

        if (extracted_tk == NULL) {
            /* malloc failed */
            free_tokens(tkn);
            return NULL;
        }

        strncpy(extracted_tk, token, strlen(token) + 1);

        tkn->tokens[tkn->num] = extracted_tk;
        tkn->num++;
    }

    return tkn;
}


int parse_redirect(char *lineptr, char **redir_ptr) {
    /* redirection syntax is as follows:
     * command [args ...] > file */

    char *token, *saveptr;

    bool contains = (strchr(lineptr, '>') != NULL);

    token = strtok_r(lineptr, ">", &saveptr);

    if (token == NULL) {
        /* string is empty */
        return -1;
    }

    /* line is non-empty */

    if ((token = strtok_r(NULL, ">", &saveptr)) == NULL) {
        if(contains) {
            /* operator present, but file string is empty */
            return -1;
        }
        /* no redirection operator present */
        return 0;
    }

    *redir_ptr = token;         /* store file string pointer */

    if (strtok_r(NULL, ">", &saveptr) != NULL) {
        /* more than one redirect character present */
        return -1;
    }

    /* check if there are multiple files in the obtained token string */
    token = strtok_r(*redir_ptr, WHITESPACE, &saveptr);

    if (token == NULL) {
        /* file string token is all whitespace */
        return -1;
    }

    /* file string no longer has any leading whitespace */

    *redir_ptr = token;         /* update file string pointer position */

    /* check if file string has more than one file */
    if ((token = strtok_r(NULL, WHITESPACE, &saveptr)) != NULL) {
        /* extra files found */

        return -1;
    }

    /* if no extra files were found, the call to strtok_r() would have cleaned up any
     * trailing whitespace as well by replacing them with the null byte ('\0') */

    return 1;
}


/* purely for debugging */
void print_tokens(tokens_t *t) {
    if (t == NULL) return;

    printf("Number of tokens: %ld\n", t->num);

    if (t->tokens == NULL) return;

    for (int i = 0; i < t->num; i++) {
        printf("Token %d: %s\n", i, t->tokens[i]);
    }
}
