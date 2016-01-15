#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../jsmn.h"
#include "../jsmn_iterator.h"

#include <malloc.h>

#define countof(_x_) (sizeof(_x_)/sizeof((_x_)[0]))
#define min(a, b) ((a) > (b) ? (b) : (a))

const unsigned int max_indent_level = 6;
const unsigned int max_render_level = 6;

void print_string(const char *psz, int start, int end) {
  printf("%.*s", (end - start), (psz + start));
}

const char *type_to_text(jsmntype_t type) {
  switch(type) {
    case JSMN_UNDEFINED:  return "UNDEFINED";
    case JSMN_OBJECT:     return "OBJECT";
    case JSMN_ARRAY:      return "ARRAY";
    case JSMN_STRING:     return "STRING";
    case JSMN_PRIMITIVE:  return "PRIMITIVE";
  }
  return "*UNKNOWN*";
}

/** Generate the indentation for an JSON depth */
const char *indent_text(unsigned int indent_level) {
  /* Allow indentation up to level 35 */
  static const char indent_buf[] = "                                                                      ";

  if (indent_level * 2 > sizeof(indent_buf))
    return indent_buf;

  return indent_buf + sizeof(indent_buf) - (indent_level * 2) - 1;
}


/**
 * @brief Recursive printing of json tree
 * 
 * @param psz           Original JSON string
 * @param jsmn_tokens   JSMN tokens
 * @param jsmn_len      JSMN token count
 * @param parser_pos    JSMN index where to start
 * @param indent_level  Current indentation level
 * 
 * @return Negative value corresponding to value in JSMNITER_ERR_* if error during printing
 */
int print_iterator(const char *psz, jsmntok_t *jsmn_tokens, unsigned int jsmn_len, unsigned int parser_pos, unsigned int indent_level) {
  jsmntok_t *jsmn_identifier = NULL;
  jsmntok_t *jsmn_value = NULL;
  unsigned int index = 0;
  int return_value;

  /* iterator_hint makes it possible to indicate where Array/Object ends so the
     jsmn_iterator_next function doesn't need to loop until it finds the end */
  unsigned int iterator_hint = 0;

  jsmn_iterator_t iterator;

  if ((return_value = jsmn_iterator_init(&iterator, jsmn_tokens, jsmn_len, parser_pos)) < 0)
    return return_value;

  printf("%c ", jsmn_tokens[parser_pos].type == JSMN_OBJECT ? '{' : '[');

  while((return_value = jsmn_iterator_next(&iterator, &jsmn_identifier, &jsmn_value, iterator_hint)) > 0) {
    index++;
    if (index > 1)
      printf(", ");

    if (!(indent_level >= max_indent_level)) {
      printf("\r\n%5u.%-3u%s", iterator.parser_pos, index - 1, indent_text(indent_level));
    }

    if (jsmn_identifier) {
      printf("\"");
      print_string(psz, jsmn_identifier->start, jsmn_identifier->end);
      printf("\": ");
    }

    switch(jsmn_value->type) {
      case JSMN_OBJECT:
      case JSMN_ARRAY:
        
        /* Empty Array/Object */
        if (jsmn_value->size == 0) {
          printf("%s", jsmn_value->type == JSMN_OBJECT ? "{ }" : "[ ]");
        }

        else if (indent_level < max_render_level) {
          if ((return_value = print_iterator(psz, jsmn_tokens, jsmn_len, iterator.parser_pos, indent_level + 1)) < 0)
            return return_value;

          /* print_iterator returns index for next item so use it as hint for this iterator to skip the Array/Object */
          iterator_hint = (unsigned int)return_value;
        }
        else {
          printf("%s", jsmn_value->type == JSMN_OBJECT ? "{ Object }" : "[ Array ]");
          
          /* Don't know where Array/Object ends */
          iterator_hint = 0;
        }

        break;
      case JSMN_STRING:
        printf("\"");
        print_string(psz, jsmn_value->start, jsmn_value->end);
        printf("\"");
        break;

      case JSMN_PRIMITIVE:
        print_string(psz, jsmn_value->start, jsmn_value->end);
        break;

      case JSMN_UNDEFINED:
        printf("undefined");
        break;

      default:
        printf("UNKNOWN");
        break;
    }
  }
  if (return_value < 0)
    return return_value;

  /*printf("after %d (return_value: %d)\r\n", indent_level, return_value);*/

  if (!(indent_level >= max_indent_level)) {
    printf("\r\n%9s%s%c", "", (indent_level > 0 ? indent_text(indent_level - 1) : ""), jsmn_tokens[parser_pos].type == JSMN_OBJECT ? '}' : ']');
  } else {
    printf(" %c", jsmn_tokens[parser_pos].type == JSMN_OBJECT ? '}' : ']');
  }

  /* Return parser_pos so outer function knows where this Array/Object ends */
  return (int)(iterator.parser_pos);
}


/**
 * @brief Non-recursive printing of json tree
 * @defails Uses internal stack to keep track of states for each JSON level during redering
 * 
 * @param psz           Original JSON string
 * @param jsmn_tokens   JSMN tokens
 * @param jsmn_len      JSMN token count
 * @param parser_pos    JSMN index where to start
 * 
 * @return Negative value corresponding to value in JSMNITER_ERR_* if error during printing
 */

int print_tree(const char *psz, jsmntok_t *jsmn_tokens, unsigned int jsmn_len, unsigned int start_index) {

  /* Stack used to keep information for each level during rendering, dictates absolute max render depth*/
  static struct {
    jsmn_iterator_t iterator;
    unsigned int index;
    unsigned int is_object;
  } stack[128];

  unsigned int stack_index = 0;

  jsmntok_t *jsmn_value = NULL;
  jsmntok_t *jsmn_identifier = NULL;

  int ret_value;

  /* iterator_hint makes it possible to indicate where Array/Object ends so the
     jsmn_iterator_next function doesn't need to loop until it finds the end */
  unsigned int iterator_hint = 0;

  /* Setup root item */
  stack_index = 0;
  stack[0].index = 0;
  stack[0].is_object = jsmn_tokens[0].type == JSMN_OBJECT ? 1: 0;
  if ((ret_value = jsmn_iterator_init(&stack[stack_index].iterator, jsmn_tokens, jsmn_len, start_index)) < 0)
    return ret_value;

  /* Print start item */
  printf("%5u.%-3u%s", stack[0].iterator.parser_pos, stack[0].index, indent_text(0));
  printf("%c", stack[0].is_object ? '{' : '[');

  /* Iterate over all items, only abort on error */
  while ((ret_value = jsmn_iterator_next(&stack[stack_index].iterator, &jsmn_identifier, &jsmn_value, iterator_hint)) >= 0) {
    /* No more items, need to pop stack if possible */
    if (ret_value == 0) {
      /* Back at root item, so bail out */
      if (stack_index == 0)
        break;

      /* Output the end character */
      if (stack_index + 1 < max_indent_level)
        printf( "\r\n%8s%s", "", indent_text(stack_index));
      printf(" %c", stack[stack_index].is_object ? '}' : ']');

      /* parser_pos should point at the index after the current Array/Object,
         we use it to hint to hint for the outer Array/Object where next item is */
      iterator_hint = stack[stack_index].iterator.parser_pos;

      /* Pop the stack */
      stack_index--;

      continue;
    }

    stack[stack_index].index++;
    if (stack[stack_index].index > 1) {
      printf(", ");
    }

    if (stack_index + 1 < max_indent_level) {
      printf("\r\n%5u.%-3u%s", stack[stack_index].iterator.parser_pos, stack[stack_index].index - 1, indent_text(stack_index + 1));
    }

    if (jsmn_identifier) {
      printf("\"");
      print_string(psz, jsmn_identifier->start, jsmn_identifier->end);
      printf("\": ");
    }

    switch(jsmn_value->type) {

      case JSMN_OBJECT:
      case JSMN_ARRAY:
        /* Need to iterate over child items before we can render next item so put current 
           iterator on the stack and start rendering child items */

        /* Empty Array/Object */
        if (jsmn_value->size == 0) {
          printf("%s", jsmn_value->type == JSMN_OBJECT ? "{ }" : "[ ]");
        }
            
        /* Check if we want to and can render this depth */
        else if (stack_index + 1 < max_render_level && stack_index + 1 < countof(stack)) {
          printf("%c ", jsmn_value->type == JSMN_OBJECT ? '{' : '[');

          /* Initialize new item on the stack */
          stack[stack_index + 1].index = 0;
          stack[stack_index + 1].is_object = jsmn_value->type == JSMN_OBJECT ? 1 : 0;
          if ((ret_value = jsmn_iterator_init(&stack[stack_index + 1].iterator, jsmn_tokens, jsmn_len, stack[stack_index].iterator.parser_pos)) < 0)
            return ret_value;

          /* Update stack pointer */
          stack_index++;
        }
        /* Output placeholder for this depth */
        else {
          printf("%s", jsmn_value->type == JSMN_OBJECT ? "{ Object }" : "[ Array ]");
          
          /* Don't know where Array/Object ends */
          iterator_hint = 0;
        }
        break;
      case JSMN_STRING: 
        printf("\"");
        print_string(psz, jsmn_value->start, jsmn_value->end);
        printf("\"");
        break;
      case JSMN_PRIMITIVE:
        print_string(psz, jsmn_value->start, jsmn_value->end);
        break;
      case JSMN_UNDEFINED:
        printf("undefined");
        break;
      default:
        printf("UNKNOWN");
        break;
    }
  }

  if (ret_value < 0)
    return ret_value;

  /* Render end tag */
  printf( "\r\n%8s%s %c", "", indent_text(0), stack[0].is_object ? '}' : ']');

  return 0;
}


void print_usage() {
  fprintf(stdout, "%s [<filename>] [recursive|stack|raw]\r\n", "jsonprint");
}


int main(int argc, char *argv[])
{
  jsmn_parser p;

  uint jsmn_tokens_size = 100000;
  jsmntok_t *jsmn_tokens = NULL;

  int nJSParse;

  unsigned int file_data_size = 1024*1024*2;
  unsigned int file_data_len = 0;
  char *file_data = NULL;
  char *file_name = NULL;
  char *print_type = "stack";

  if (argc < 2) {
    print_usage();
    return 0;
  }

  if ((jsmn_tokens = malloc(sizeof(jsmntok_t) * jsmn_tokens_size)) == NULL)
    goto error_out;

  if ((file_data = malloc(sizeof(char) * file_data_size)) == NULL)
    goto error_out;

  file_name = argv[1];
  if (argc >= 3)
    print_type = argv[2];

  {
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
      fprintf(stderr, "Unable to open file \"%s\"\r\n", file_name);
      goto error_out;
    }
    file_data_len = fread(file_data, sizeof(char), file_data_size, fp);
    fclose(fp);
  
    if (file_data_len == file_data_size) {
      fprintf(stderr, "Buffer to small\r\n");
      goto error_out;
    }
  }

  /* Prepare parser */
  jsmn_init(&p);

  if ((nJSParse = jsmn_parse(&p, file_data, file_data_len, jsmn_tokens, jsmn_tokens_size)) < 0) {
    fprintf(stderr, "Error parsing json %d\r\n", nJSParse);

    if (p.pos > 5) {
      fprintf(stderr, "\"%.11s\"\r\n", file_data + (p.pos - 5));
      fprintf(stderr, "%5s^\r\n", "");
    } else {
      fprintf(stderr, "\"%.12s\"\r\n", file_data);
      fprintf(stderr, "%*.s^\r\n", p.pos, "             ");
    }

    goto error_out;
  }
  fprintf(stderr, "Success parsing %d tokens\r\n", nJSParse);
  fprintf(stderr, "Memusage: %dkb (%d%%)\r\n", (sizeof(jsmntok_t) * (uint)nJSParse / 1024), ((100 * (uint)nJSParse) / jsmn_tokens_size));


  /* Different print methods */
  if (!strcmp(print_type, "recursive")) {
    int return_code;
    fprintf(stderr, "Rendering Mode: %s\r\n", print_type);
    printf("\r\n%5d.%-3u%s", 0, 0, indent_text(0));
    if ((return_code = print_iterator(file_data, jsmn_tokens, (unsigned int)nJSParse, 0, 1)) < 0) {
      fprintf(stderr, "Error Return Code: %d\r\n", return_code);
      goto error_out;
    }
    printf("\r\n");
  }

  else if (!strcmp(print_type, "stack")) {
    int return_code;
    fprintf(stderr, "Rendering Mode: %s\r\n", print_type);
    if ((return_code = print_tree(file_data, jsmn_tokens, (unsigned int)nJSParse, 0)) < 0) {
      fprintf(stderr, "Error Return Code: %d\r\n", return_code);
      goto error_out;
    }
    printf("\r\n");
  }
  
  else if (!strcmp(print_type, "raw")) {
    int i;
    fprintf(stderr, "Rendering Mode: %s\r\n", print_type);
    for (i = 0; i < nJSParse; i++) {
      jsmntok_t *t = &jsmn_tokens[i];
      printf("%5d %10.10s  start: %5d  end: %5d  size: %5d", i, type_to_text(t->type), t->start, t->end, t->size);
      if (t->type == JSMN_PRIMITIVE) {
        printf("  ");
        print_string(file_data, t->start, t->end);
      }
      else if (t->type == JSMN_STRING) {
        printf("  \"");
        print_string(file_data, t->start, min(t->end, t->start + 15));
        printf("\"");
      }
      printf("\r\n");
    }
  }
  else {
    fprintf(stderr, "Unhandled print type \"%s\"\r\n", print_type);
    print_usage();
    goto error_out;
  }

  if (jsmn_tokens) free(jsmn_tokens);
  if (file_data) free(file_data);
  
  return 0;

error_out:
  
  if (jsmn_tokens) free(jsmn_tokens);
  if (file_data) free(file_data);
  return -1;

}





