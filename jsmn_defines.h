#ifndef JSMN_DEFINES
#define JSMN_DEFINES

/*!
 * If nothing is defined, the default definitions are JSMN_PARENT_LINKS and   *
 *   JSMN_NEXT_SIBLING with a jsmntok field size of 4 bytes (unsigned int). *
 * This will parse one json object in a buffer at a time and return after a   *
 *   successful json object parse. To check if there is more data in the      *
 *   buffer that hasn't been parsed, run jsmn_eof.                           !*/

/*! @def JSMN_PARENT_LINKS
 * @brief Adds a parent field to the token
 *
 * This decreases the initial time required to parse a json buffer and
 *   simplifies the post-processing of token array by adding a link to the id of
 *   a token's parent.
 * This is enabled by default and highly recommended.
 */

/*! @def JSMN_NEXT_SIBLING
 * @brief Adds a next_sibling field to the token
 *
 * This simplifies the post-processing of token array by adding a link to the id
 *   of a token's next sibling.
 * This is enabled by default and highly recommended.
 */

/*! @def JSMN_UTF8
 * @brief Add UTF-8 functionality
 *
 * This allows for stricter parsing of json strings and also allows for the
 *   conversion of escaped characters (\uXXXX) to UTF-8 and back.
 */

/*! @def JSMN_LOW_MEMORY
 * @brief Enables defintions that reduce jsmn's memory footprint for small
 *        devices and doesn't enable definitions that increase it's footprint.
 *
 * This enables definitions that reduce jsmn's memory footprint at the cost of
 *   CPU usage. This is useful for small devices that don't parse json objects
 *   often and have restrictive memory requirements.
 */

/*! @def JSMN_SHORT_TOKENS
 * @brief Changes the tokens field size from a uint32_t to a uint16_t
 *
 * This reduces the jsmntok size by half by changing jsmntok field sizes
 *   from an unsigned int to an unsigned short. NOTE: This reduces the maximum
 *   possible json string length from 4,294,967,295 to 65,535 minus the size of
 *   jsmnerr.
 */

/*! @def JSMN_PERMISSIVE
 * @brief Enables all PERMISSIVE definitions
 *
 * Enables JSMN_PERMISSIVE_KEY, JSMN_PERMISSIVE_PRIMITIVE, and
 *   JSMN_MULTIPLE_JSON
 */

/*! @def JSMN_PERMISSIVE_KEY
 * @brief Allows PRIMITIVEs to be OBJECT KEYs
 */

/*! @def JSMN_PERMISSIVE_PRIMITIVE
 * @brief Allows PRIMITIVEs to be any contiguous value
 *
 * This allows PRIMIVITEs to be any contiguous value that does not contain a
 *   character that has a special meaning to json (`{}[]",:`). NOTE: There is no
 *   validation of JSMN_PRI_MINUS, JSNM_PRI_DECIMAL, or JSMN_PRI_EXPONENT;
 *   everything is the base type JSMN_PRIMITIVE.
 */

/*! @def JSMN_MULTIPLE_JSON
 * @brief Allows multiple json objects in a complete buffer
 *
 * This allows jsmn to parse multiple json objects in a single buffer.
 *   NOTE: If a single json object is malformed jsmn_parse will return with
 *   an error.
 */

/*! @def JSMN_MULTIPLE_JSON_FAIL
 * @brief Fails if there is more than one json object in a buffer.
 */

#ifndef JSMN_API
# ifdef JSMN_STATIC
#  define JSMN_API static
# else
#  define JSMN_API extern
# endif
#endif

#ifndef JSMN_LOW_MEMORY

# ifndef JSMN_PARENT_LINKS
#  define JSMN_PARENT_LINKS
# endif
# ifndef JSMN_NEXT_SIBLING
#  define JSMN_NEXT_SIBLING
# endif

#else

# ifndef JSMN_SHORT_TOKENS
#  define JSMN_SHORT_TOKENS
# endif

#endif

#ifdef JSMN_PERMISSIVE
# ifndef JSMN_PERMISSIVE_KEY
#  define JSMN_PERMISSIVE_KEY
# endif
# ifndef JSMN_PERMISSIVE_PRIMITIVE
#  define JSMN_PERMISSIVE_PRIMITIVE
# endif
# ifndef JSMN_MULTIPLE_JSON
#  define JSMN_MULTIPLE_JSON
# endif
#endif

#ifdef JSMN_MULTIPLE_JSON_FAIL
# undef JSMN_MULTIPLE_JSON
#endif

#if (defined(__linux__) || defined(__APPLE__) || defined(ARDUINO))
# define JSMN_EXPORT __attribute__((visibility("default")))
# define JSMN_LOCAL  __attribute__((visibility("hidden")))
#elif (defined(_WIN32))
# define JSMN_EXPORT __declspec(dllexport)
# define JSMN_LOCAL
#else
# define JSMN_EXPORT
# define JSMN_LOCAL
#endif

#endif /* JSMN_DEFINES */
