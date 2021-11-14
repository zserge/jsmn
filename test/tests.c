#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "testutil.h"

int test_jsmn_test_suite_i_(void) {
  /* i_number_double_huge_neg_exp.json */
  check(parse("[123.456e-789]", 2, 2,
              JSMN_ARRAY,  0, 14, 1,
              JSMN_PRIMITIVE, "123.456e-789"));

  /* i_number_huge_exp.json */
  check(parse("[0.4e00669999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999969999999006]", 2, 2,
              JSMN_ARRAY,  0, 137, 1,
              JSMN_PRIMITIVE, "0.4e00669999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999969999999006"));

  /* i_number_neg_int_huge_exp.json */
  check(parse("[-1e+9999]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_PRIMITIVE, "-1e+9999"));

  /* i_number_pos_double_huge_exp.json */
  check(parse("[1.5e+9999]", 2, 2,
              JSMN_ARRAY,  0, 11, 1,
              JSMN_PRIMITIVE, "1.5e+9999"));

  /* i_number_real_neg_overflow.json */
  check(parse("[-123123e100000]", 2, 2,
              JSMN_ARRAY,  0, 16, 1,
              JSMN_PRIMITIVE, "-123123e100000"));

  /* i_number_real_pos_overflow.json */
  check(parse("[123123e100000]", 2, 2,
              JSMN_ARRAY,  0, 15, 1,
              JSMN_PRIMITIVE, "123123e100000"));

  /* i_number_real_underflow.json */
  check(parse("[123e-10000000]", 2, 2,
              JSMN_ARRAY,  0, 15, 1,
              JSMN_PRIMITIVE, "123e-10000000"));

  /* i_number_too_big_neg_int.json */
  check(parse("[-123123123123123123123123123123]", 2, 2,
              JSMN_ARRAY,  0, 33, 1,
              JSMN_PRIMITIVE, "-123123123123123123123123123123"));

  /* i_number_too_big_pos_int.json */
  check(parse("[100000000000000000000]", 2, 2,
              JSMN_ARRAY,  0, 23, 1,
              JSMN_PRIMITIVE, "100000000000000000000"));

  /* i_number_very_big_negative_int.json */
  check(parse("[-237462374673276894279832749832423479823246327846]", 2, 2,
              JSMN_ARRAY,  0, 51, 1,
              JSMN_PRIMITIVE, "-237462374673276894279832749832423479823246327846"));

  /* i_object_key_lone_2nd_surrogate.json */
  check(parse("{\"\\uDFAA\":0}", 3, 3,
              JSMN_OBJECT, 0, 12, 1,
              JSMN_STRING, "\\uDFAA", 1,
              JSMN_PRIMITIVE, "0"));

  /* i_string_1st_surrogate_but_2nd_missing.json */
  check(parse("[\"\\uDADA\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\uDADA", 0));

  /* i_string_1st_valid_surrogate_2nd_invalid.json */
  check(parse("[\"\\uD888\\u1234\"]", 2, 2,
              JSMN_ARRAY,  0, 16, 1,
              JSMN_STRING, "\\uD888\\u1234", 0));

  /* i_string_incomplete_surrogate_and_escape_valid.json */
  check(parse("[\"\\uD800\\n\"]", 2, 2,
              JSMN_ARRAY,  0, 12, 1,
              JSMN_STRING, "\\uD800\\n", 0));

  /* i_string_incomplete_surrogate_pair.json */
  check(parse("[\"\\uDd1ea\"]", 2, 2,
              JSMN_ARRAY,  0, 11, 1,
              JSMN_STRING, "\\uDd1ea", 0));

  /* i_string_incomplete_surrogates_escape_valid.json */
  check(parse("[\"\\uD800\\uD800\\n\"]", 2, 2,
              JSMN_ARRAY,  0, 18, 1,
              JSMN_STRING, "\\uD800\\uD800\\n", 0));

  /* i_string_invalid_lonely_surrogate.json */
  check(parse("[\"\\ud800\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\ud800", 0));

  /* i_string_invalid_surrogate.json */
  check(parse("[\"\\ud800abc\"]", 2, 2,
              JSMN_ARRAY,  0, 13, 1,
              JSMN_STRING, "\\ud800abc", 0));

  /* i_string_invalid_utf-8.json */
  check(parse("[\"ˇ\"]", 2, 2,
              JSMN_ARRAY,  0, 5, 1,
              JSMN_STRING, "ˇ", 0));

  /* i_string_inverted_surrogates_U+1D11E.json */
  check(parse("[\"\\uDd1e\\uD834\"]", 2, 2,
              JSMN_ARRAY,  0, 16, 1,
              JSMN_STRING, "\\uDd1e\\uD834", 0));

  /* i_string_iso_latin_1.json */
  check(parse("[\"È\"]", 2, 2,
              JSMN_ARRAY,  0, 5, 1,
              JSMN_STRING, "È", 0));

  /* i_string_lone_second_surrogate.json */
  check(parse("[\"\\uDFAA\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\uDFAA", 0));

  /* i_string_lone_utf8_continuation_byte.json */
  check(parse("[\"Å\"]", 2, 2,
              JSMN_ARRAY,  0, 5, 1,
              JSMN_STRING, "Å", 0));

  /* i_string_not_in_unicode_range.json */
  check(parse("[\"Ùøøø\"]", 2, 2,
              JSMN_ARRAY,  0, 8, 1,
              JSMN_STRING, "Ùøøø", 0));

  /* i_string_overlong_sequence_2_bytes.json */
  check(parse("[\"¿Ø\"]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_STRING, "¿Ø", 0));

  /* i_string_overlong_sequence_6_bytes.json */
  check(parse("[\"¸Éøøøø\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "¸Éøøøø", 0));

  /* i_string_overlong_sequence_6_bytes_null.json */
  check(parse("[\"¸ÄÄÄÄÄ\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "¸ÄÄÄÄÄ", 0));

  /* i_string_truncated-utf-8.json */
  check(parse("[\"‡ˇ\"]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_STRING, "‡ˇ", 0));

  /* i_string_utf16BE_no_BOM.json failed to parse. */

  /* i_string_utf16LE_no_BOM.json failed to parse. */

  /* i_string_UTF-16LE_with_BOM.json failed to parse. */

  /* i_string_UTF-8_invalid_sequence.json */
  check(parse("[\"Êó•—à˙\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "Êó•—à˙", 0));

  /* i_string_UTF8_surrogate_U+D800.json */
  check(parse("[\"Ì†Ä\"]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_STRING, "Ì†Ä", 0));

  /* i_structure_500_nested_arrays.json */
  check(parse("[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]", 500, 500,
              JSMN_ARRAY,    0,1000, 1, JSMN_ARRAY,    1, 999, 1, JSMN_ARRAY,    2, 998, 1, JSMN_ARRAY,    3, 997, 1, JSMN_ARRAY,    4, 996, 1,
              JSMN_ARRAY,    5, 995, 1, JSMN_ARRAY,    6, 994, 1, JSMN_ARRAY,    7, 993, 1, JSMN_ARRAY,    8, 992, 1, JSMN_ARRAY,    9, 991, 1,
              JSMN_ARRAY,   10, 990, 1, JSMN_ARRAY,   11, 989, 1, JSMN_ARRAY,   12, 988, 1, JSMN_ARRAY,   13, 987, 1, JSMN_ARRAY,   14, 986, 1,
              JSMN_ARRAY,   15, 985, 1, JSMN_ARRAY,   16, 984, 1, JSMN_ARRAY,   17, 983, 1, JSMN_ARRAY,   18, 982, 1, JSMN_ARRAY,   19, 981, 1,
              JSMN_ARRAY,   20, 980, 1, JSMN_ARRAY,   21, 979, 1, JSMN_ARRAY,   22, 978, 1, JSMN_ARRAY,   23, 977, 1, JSMN_ARRAY,   24, 976, 1,
              JSMN_ARRAY,   25, 975, 1, JSMN_ARRAY,   26, 974, 1, JSMN_ARRAY,   27, 973, 1, JSMN_ARRAY,   28, 972, 1, JSMN_ARRAY,   29, 971, 1,
              JSMN_ARRAY,   30, 970, 1, JSMN_ARRAY,   31, 969, 1, JSMN_ARRAY,   32, 968, 1, JSMN_ARRAY,   33, 967, 1, JSMN_ARRAY,   34, 966, 1,
              JSMN_ARRAY,   35, 965, 1, JSMN_ARRAY,   36, 964, 1, JSMN_ARRAY,   37, 963, 1, JSMN_ARRAY,   38, 962, 1, JSMN_ARRAY,   39, 961, 1,
              JSMN_ARRAY,   40, 960, 1, JSMN_ARRAY,   41, 959, 1, JSMN_ARRAY,   42, 958, 1, JSMN_ARRAY,   43, 957, 1, JSMN_ARRAY,   44, 956, 1,
              JSMN_ARRAY,   45, 955, 1, JSMN_ARRAY,   46, 954, 1, JSMN_ARRAY,   47, 953, 1, JSMN_ARRAY,   48, 952, 1, JSMN_ARRAY,   49, 951, 1,
              JSMN_ARRAY,   50, 950, 1, JSMN_ARRAY,   51, 949, 1, JSMN_ARRAY,   52, 948, 1, JSMN_ARRAY,   53, 947, 1, JSMN_ARRAY,   54, 946, 1,
              JSMN_ARRAY,   55, 945, 1, JSMN_ARRAY,   56, 944, 1, JSMN_ARRAY,   57, 943, 1, JSMN_ARRAY,   58, 942, 1, JSMN_ARRAY,   59, 941, 1,
              JSMN_ARRAY,   60, 940, 1, JSMN_ARRAY,   61, 939, 1, JSMN_ARRAY,   62, 938, 1, JSMN_ARRAY,   63, 937, 1, JSMN_ARRAY,   64, 936, 1,
              JSMN_ARRAY,   65, 935, 1, JSMN_ARRAY,   66, 934, 1, JSMN_ARRAY,   67, 933, 1, JSMN_ARRAY,   68, 932, 1, JSMN_ARRAY,   69, 931, 1,
              JSMN_ARRAY,   70, 930, 1, JSMN_ARRAY,   71, 929, 1, JSMN_ARRAY,   72, 928, 1, JSMN_ARRAY,   73, 927, 1, JSMN_ARRAY,   74, 926, 1,
              JSMN_ARRAY,   75, 925, 1, JSMN_ARRAY,   76, 924, 1, JSMN_ARRAY,   77, 923, 1, JSMN_ARRAY,   78, 922, 1, JSMN_ARRAY,   79, 921, 1,
              JSMN_ARRAY,   80, 920, 1, JSMN_ARRAY,   81, 919, 1, JSMN_ARRAY,   82, 918, 1, JSMN_ARRAY,   83, 917, 1, JSMN_ARRAY,   84, 916, 1,
              JSMN_ARRAY,   85, 915, 1, JSMN_ARRAY,   86, 914, 1, JSMN_ARRAY,   87, 913, 1, JSMN_ARRAY,   88, 912, 1, JSMN_ARRAY,   89, 911, 1,
              JSMN_ARRAY,   90, 910, 1, JSMN_ARRAY,   91, 909, 1, JSMN_ARRAY,   92, 908, 1, JSMN_ARRAY,   93, 907, 1, JSMN_ARRAY,   94, 906, 1,
              JSMN_ARRAY,   95, 905, 1, JSMN_ARRAY,   96, 904, 1, JSMN_ARRAY,   97, 903, 1, JSMN_ARRAY,   98, 902, 1, JSMN_ARRAY,   99, 901, 1,
              JSMN_ARRAY,  100, 900, 1, JSMN_ARRAY,  101, 899, 1, JSMN_ARRAY,  102, 898, 1, JSMN_ARRAY,  103, 897, 1, JSMN_ARRAY,  104, 896, 1,
              JSMN_ARRAY,  105, 895, 1, JSMN_ARRAY,  106, 894, 1, JSMN_ARRAY,  107, 893, 1, JSMN_ARRAY,  108, 892, 1, JSMN_ARRAY,  109, 891, 1,
              JSMN_ARRAY,  110, 890, 1, JSMN_ARRAY,  111, 889, 1, JSMN_ARRAY,  112, 888, 1, JSMN_ARRAY,  113, 887, 1, JSMN_ARRAY,  114, 886, 1,
              JSMN_ARRAY,  115, 885, 1, JSMN_ARRAY,  116, 884, 1, JSMN_ARRAY,  117, 883, 1, JSMN_ARRAY,  118, 882, 1, JSMN_ARRAY,  119, 881, 1,
              JSMN_ARRAY,  120, 880, 1, JSMN_ARRAY,  121, 879, 1, JSMN_ARRAY,  122, 878, 1, JSMN_ARRAY,  123, 877, 1, JSMN_ARRAY,  124, 876, 1,
              JSMN_ARRAY,  125, 875, 1, JSMN_ARRAY,  126, 874, 1, JSMN_ARRAY,  127, 873, 1, JSMN_ARRAY,  128, 872, 1, JSMN_ARRAY,  129, 871, 1,
              JSMN_ARRAY,  130, 870, 1, JSMN_ARRAY,  131, 869, 1, JSMN_ARRAY,  132, 868, 1, JSMN_ARRAY,  133, 867, 1, JSMN_ARRAY,  134, 866, 1,
              JSMN_ARRAY,  135, 865, 1, JSMN_ARRAY,  136, 864, 1, JSMN_ARRAY,  137, 863, 1, JSMN_ARRAY,  138, 862, 1, JSMN_ARRAY,  139, 861, 1,
              JSMN_ARRAY,  140, 860, 1, JSMN_ARRAY,  141, 859, 1, JSMN_ARRAY,  142, 858, 1, JSMN_ARRAY,  143, 857, 1, JSMN_ARRAY,  144, 856, 1,
              JSMN_ARRAY,  145, 855, 1, JSMN_ARRAY,  146, 854, 1, JSMN_ARRAY,  147, 853, 1, JSMN_ARRAY,  148, 852, 1, JSMN_ARRAY,  149, 851, 1,
              JSMN_ARRAY,  150, 850, 1, JSMN_ARRAY,  151, 849, 1, JSMN_ARRAY,  152, 848, 1, JSMN_ARRAY,  153, 847, 1, JSMN_ARRAY,  154, 846, 1,
              JSMN_ARRAY,  155, 845, 1, JSMN_ARRAY,  156, 844, 1, JSMN_ARRAY,  157, 843, 1, JSMN_ARRAY,  158, 842, 1, JSMN_ARRAY,  159, 841, 1,
              JSMN_ARRAY,  160, 840, 1, JSMN_ARRAY,  161, 839, 1, JSMN_ARRAY,  162, 838, 1, JSMN_ARRAY,  163, 837, 1, JSMN_ARRAY,  164, 836, 1,
              JSMN_ARRAY,  165, 835, 1, JSMN_ARRAY,  166, 834, 1, JSMN_ARRAY,  167, 833, 1, JSMN_ARRAY,  168, 832, 1, JSMN_ARRAY,  169, 831, 1,
              JSMN_ARRAY,  170, 830, 1, JSMN_ARRAY,  171, 829, 1, JSMN_ARRAY,  172, 828, 1, JSMN_ARRAY,  173, 827, 1, JSMN_ARRAY,  174, 826, 1,
              JSMN_ARRAY,  175, 825, 1, JSMN_ARRAY,  176, 824, 1, JSMN_ARRAY,  177, 823, 1, JSMN_ARRAY,  178, 822, 1, JSMN_ARRAY,  179, 821, 1,
              JSMN_ARRAY,  180, 820, 1, JSMN_ARRAY,  181, 819, 1, JSMN_ARRAY,  182, 818, 1, JSMN_ARRAY,  183, 817, 1, JSMN_ARRAY,  184, 816, 1,
              JSMN_ARRAY,  185, 815, 1, JSMN_ARRAY,  186, 814, 1, JSMN_ARRAY,  187, 813, 1, JSMN_ARRAY,  188, 812, 1, JSMN_ARRAY,  189, 811, 1,
              JSMN_ARRAY,  190, 810, 1, JSMN_ARRAY,  191, 809, 1, JSMN_ARRAY,  192, 808, 1, JSMN_ARRAY,  193, 807, 1, JSMN_ARRAY,  194, 806, 1,
              JSMN_ARRAY,  195, 805, 1, JSMN_ARRAY,  196, 804, 1, JSMN_ARRAY,  197, 803, 1, JSMN_ARRAY,  198, 802, 1, JSMN_ARRAY,  199, 801, 1,
              JSMN_ARRAY,  200, 800, 1, JSMN_ARRAY,  201, 799, 1, JSMN_ARRAY,  202, 798, 1, JSMN_ARRAY,  203, 797, 1, JSMN_ARRAY,  204, 796, 1,
              JSMN_ARRAY,  205, 795, 1, JSMN_ARRAY,  206, 794, 1, JSMN_ARRAY,  207, 793, 1, JSMN_ARRAY,  208, 792, 1, JSMN_ARRAY,  209, 791, 1,
              JSMN_ARRAY,  210, 790, 1, JSMN_ARRAY,  211, 789, 1, JSMN_ARRAY,  212, 788, 1, JSMN_ARRAY,  213, 787, 1, JSMN_ARRAY,  214, 786, 1,
              JSMN_ARRAY,  215, 785, 1, JSMN_ARRAY,  216, 784, 1, JSMN_ARRAY,  217, 783, 1, JSMN_ARRAY,  218, 782, 1, JSMN_ARRAY,  219, 781, 1,
              JSMN_ARRAY,  220, 780, 1, JSMN_ARRAY,  221, 779, 1, JSMN_ARRAY,  222, 778, 1, JSMN_ARRAY,  223, 777, 1, JSMN_ARRAY,  224, 776, 1,
              JSMN_ARRAY,  225, 775, 1, JSMN_ARRAY,  226, 774, 1, JSMN_ARRAY,  227, 773, 1, JSMN_ARRAY,  228, 772, 1, JSMN_ARRAY,  229, 771, 1,
              JSMN_ARRAY,  230, 770, 1, JSMN_ARRAY,  231, 769, 1, JSMN_ARRAY,  232, 768, 1, JSMN_ARRAY,  233, 767, 1, JSMN_ARRAY,  234, 766, 1,
              JSMN_ARRAY,  235, 765, 1, JSMN_ARRAY,  236, 764, 1, JSMN_ARRAY,  237, 763, 1, JSMN_ARRAY,  238, 762, 1, JSMN_ARRAY,  239, 761, 1,
              JSMN_ARRAY,  240, 760, 1, JSMN_ARRAY,  241, 759, 1, JSMN_ARRAY,  242, 758, 1, JSMN_ARRAY,  243, 757, 1, JSMN_ARRAY,  244, 756, 1,
              JSMN_ARRAY,  245, 755, 1, JSMN_ARRAY,  246, 754, 1, JSMN_ARRAY,  247, 753, 1, JSMN_ARRAY,  248, 752, 1, JSMN_ARRAY,  249, 751, 1,
              JSMN_ARRAY,  250, 750, 1, JSMN_ARRAY,  251, 749, 1, JSMN_ARRAY,  252, 748, 1, JSMN_ARRAY,  253, 747, 1, JSMN_ARRAY,  254, 746, 1,
              JSMN_ARRAY,  255, 745, 1, JSMN_ARRAY,  256, 744, 1, JSMN_ARRAY,  257, 743, 1, JSMN_ARRAY,  258, 742, 1, JSMN_ARRAY,  259, 741, 1,
              JSMN_ARRAY,  260, 740, 1, JSMN_ARRAY,  261, 739, 1, JSMN_ARRAY,  262, 738, 1, JSMN_ARRAY,  263, 737, 1, JSMN_ARRAY,  264, 736, 1,
              JSMN_ARRAY,  265, 735, 1, JSMN_ARRAY,  266, 734, 1, JSMN_ARRAY,  267, 733, 1, JSMN_ARRAY,  268, 732, 1, JSMN_ARRAY,  269, 731, 1,
              JSMN_ARRAY,  270, 730, 1, JSMN_ARRAY,  271, 729, 1, JSMN_ARRAY,  272, 728, 1, JSMN_ARRAY,  273, 727, 1, JSMN_ARRAY,  274, 726, 1,
              JSMN_ARRAY,  275, 725, 1, JSMN_ARRAY,  276, 724, 1, JSMN_ARRAY,  277, 723, 1, JSMN_ARRAY,  278, 722, 1, JSMN_ARRAY,  279, 721, 1,
              JSMN_ARRAY,  280, 720, 1, JSMN_ARRAY,  281, 719, 1, JSMN_ARRAY,  282, 718, 1, JSMN_ARRAY,  283, 717, 1, JSMN_ARRAY,  284, 716, 1,
              JSMN_ARRAY,  285, 715, 1, JSMN_ARRAY,  286, 714, 1, JSMN_ARRAY,  287, 713, 1, JSMN_ARRAY,  288, 712, 1, JSMN_ARRAY,  289, 711, 1,
              JSMN_ARRAY,  290, 710, 1, JSMN_ARRAY,  291, 709, 1, JSMN_ARRAY,  292, 708, 1, JSMN_ARRAY,  293, 707, 1, JSMN_ARRAY,  294, 706, 1,
              JSMN_ARRAY,  295, 705, 1, JSMN_ARRAY,  296, 704, 1, JSMN_ARRAY,  297, 703, 1, JSMN_ARRAY,  298, 702, 1, JSMN_ARRAY,  299, 701, 1,
              JSMN_ARRAY,  300, 700, 1, JSMN_ARRAY,  301, 699, 1, JSMN_ARRAY,  302, 698, 1, JSMN_ARRAY,  303, 697, 1, JSMN_ARRAY,  304, 696, 1,
              JSMN_ARRAY,  305, 695, 1, JSMN_ARRAY,  306, 694, 1, JSMN_ARRAY,  307, 693, 1, JSMN_ARRAY,  308, 692, 1, JSMN_ARRAY,  309, 691, 1,
              JSMN_ARRAY,  310, 690, 1, JSMN_ARRAY,  311, 689, 1, JSMN_ARRAY,  312, 688, 1, JSMN_ARRAY,  313, 687, 1, JSMN_ARRAY,  314, 686, 1,
              JSMN_ARRAY,  315, 685, 1, JSMN_ARRAY,  316, 684, 1, JSMN_ARRAY,  317, 683, 1, JSMN_ARRAY,  318, 682, 1, JSMN_ARRAY,  319, 681, 1,
              JSMN_ARRAY,  320, 680, 1, JSMN_ARRAY,  321, 679, 1, JSMN_ARRAY,  322, 678, 1, JSMN_ARRAY,  323, 677, 1, JSMN_ARRAY,  324, 676, 1,
              JSMN_ARRAY,  325, 675, 1, JSMN_ARRAY,  326, 674, 1, JSMN_ARRAY,  327, 673, 1, JSMN_ARRAY,  328, 672, 1, JSMN_ARRAY,  329, 671, 1,
              JSMN_ARRAY,  330, 670, 1, JSMN_ARRAY,  331, 669, 1, JSMN_ARRAY,  332, 668, 1, JSMN_ARRAY,  333, 667, 1, JSMN_ARRAY,  334, 666, 1,
              JSMN_ARRAY,  335, 665, 1, JSMN_ARRAY,  336, 664, 1, JSMN_ARRAY,  337, 663, 1, JSMN_ARRAY,  338, 662, 1, JSMN_ARRAY,  339, 661, 1,
              JSMN_ARRAY,  340, 660, 1, JSMN_ARRAY,  341, 659, 1, JSMN_ARRAY,  342, 658, 1, JSMN_ARRAY,  343, 657, 1, JSMN_ARRAY,  344, 656, 1,
              JSMN_ARRAY,  345, 655, 1, JSMN_ARRAY,  346, 654, 1, JSMN_ARRAY,  347, 653, 1, JSMN_ARRAY,  348, 652, 1, JSMN_ARRAY,  349, 651, 1,
              JSMN_ARRAY,  350, 650, 1, JSMN_ARRAY,  351, 649, 1, JSMN_ARRAY,  352, 648, 1, JSMN_ARRAY,  353, 647, 1, JSMN_ARRAY,  354, 646, 1,
              JSMN_ARRAY,  355, 645, 1, JSMN_ARRAY,  356, 644, 1, JSMN_ARRAY,  357, 643, 1, JSMN_ARRAY,  358, 642, 1, JSMN_ARRAY,  359, 641, 1,
              JSMN_ARRAY,  360, 640, 1, JSMN_ARRAY,  361, 639, 1, JSMN_ARRAY,  362, 638, 1, JSMN_ARRAY,  363, 637, 1, JSMN_ARRAY,  364, 636, 1,
              JSMN_ARRAY,  365, 635, 1, JSMN_ARRAY,  366, 634, 1, JSMN_ARRAY,  367, 633, 1, JSMN_ARRAY,  368, 632, 1, JSMN_ARRAY,  369, 631, 1,
              JSMN_ARRAY,  370, 630, 1, JSMN_ARRAY,  371, 629, 1, JSMN_ARRAY,  372, 628, 1, JSMN_ARRAY,  373, 627, 1, JSMN_ARRAY,  374, 626, 1,
              JSMN_ARRAY,  375, 625, 1, JSMN_ARRAY,  376, 624, 1, JSMN_ARRAY,  377, 623, 1, JSMN_ARRAY,  378, 622, 1, JSMN_ARRAY,  379, 621, 1,
              JSMN_ARRAY,  380, 620, 1, JSMN_ARRAY,  381, 619, 1, JSMN_ARRAY,  382, 618, 1, JSMN_ARRAY,  383, 617, 1, JSMN_ARRAY,  384, 616, 1,
              JSMN_ARRAY,  385, 615, 1, JSMN_ARRAY,  386, 614, 1, JSMN_ARRAY,  387, 613, 1, JSMN_ARRAY,  388, 612, 1, JSMN_ARRAY,  389, 611, 1,
              JSMN_ARRAY,  390, 610, 1, JSMN_ARRAY,  391, 609, 1, JSMN_ARRAY,  392, 608, 1, JSMN_ARRAY,  393, 607, 1, JSMN_ARRAY,  394, 606, 1,
              JSMN_ARRAY,  395, 605, 1, JSMN_ARRAY,  396, 604, 1, JSMN_ARRAY,  397, 603, 1, JSMN_ARRAY,  398, 602, 1, JSMN_ARRAY,  399, 601, 1,
              JSMN_ARRAY,  400, 600, 1, JSMN_ARRAY,  401, 599, 1, JSMN_ARRAY,  402, 598, 1, JSMN_ARRAY,  403, 597, 1, JSMN_ARRAY,  404, 596, 1,
              JSMN_ARRAY,  405, 595, 1, JSMN_ARRAY,  406, 594, 1, JSMN_ARRAY,  407, 593, 1, JSMN_ARRAY,  408, 592, 1, JSMN_ARRAY,  409, 591, 1,
              JSMN_ARRAY,  410, 590, 1, JSMN_ARRAY,  411, 589, 1, JSMN_ARRAY,  412, 588, 1, JSMN_ARRAY,  413, 587, 1, JSMN_ARRAY,  414, 586, 1,
              JSMN_ARRAY,  415, 585, 1, JSMN_ARRAY,  416, 584, 1, JSMN_ARRAY,  417, 583, 1, JSMN_ARRAY,  418, 582, 1, JSMN_ARRAY,  419, 581, 1,
              JSMN_ARRAY,  420, 580, 1, JSMN_ARRAY,  421, 579, 1, JSMN_ARRAY,  422, 578, 1, JSMN_ARRAY,  423, 577, 1, JSMN_ARRAY,  424, 576, 1,
              JSMN_ARRAY,  425, 575, 1, JSMN_ARRAY,  426, 574, 1, JSMN_ARRAY,  427, 573, 1, JSMN_ARRAY,  428, 572, 1, JSMN_ARRAY,  429, 571, 1,
              JSMN_ARRAY,  430, 570, 1, JSMN_ARRAY,  431, 569, 1, JSMN_ARRAY,  432, 568, 1, JSMN_ARRAY,  433, 567, 1, JSMN_ARRAY,  434, 566, 1,
              JSMN_ARRAY,  435, 565, 1, JSMN_ARRAY,  436, 564, 1, JSMN_ARRAY,  437, 563, 1, JSMN_ARRAY,  438, 562, 1, JSMN_ARRAY,  439, 561, 1,
              JSMN_ARRAY,  440, 560, 1, JSMN_ARRAY,  441, 559, 1, JSMN_ARRAY,  442, 558, 1, JSMN_ARRAY,  443, 557, 1, JSMN_ARRAY,  444, 556, 1,
              JSMN_ARRAY,  445, 555, 1, JSMN_ARRAY,  446, 554, 1, JSMN_ARRAY,  447, 553, 1, JSMN_ARRAY,  448, 552, 1, JSMN_ARRAY,  449, 551, 1,
              JSMN_ARRAY,  450, 550, 1, JSMN_ARRAY,  451, 549, 1, JSMN_ARRAY,  452, 548, 1, JSMN_ARRAY,  453, 547, 1, JSMN_ARRAY,  454, 546, 1,
              JSMN_ARRAY,  455, 545, 1, JSMN_ARRAY,  456, 544, 1, JSMN_ARRAY,  457, 543, 1, JSMN_ARRAY,  458, 542, 1, JSMN_ARRAY,  459, 541, 1,
              JSMN_ARRAY,  460, 540, 1, JSMN_ARRAY,  461, 539, 1, JSMN_ARRAY,  462, 538, 1, JSMN_ARRAY,  463, 537, 1, JSMN_ARRAY,  464, 536, 1,
              JSMN_ARRAY,  465, 535, 1, JSMN_ARRAY,  466, 534, 1, JSMN_ARRAY,  467, 533, 1, JSMN_ARRAY,  468, 532, 1, JSMN_ARRAY,  469, 531, 1,
              JSMN_ARRAY,  470, 530, 1, JSMN_ARRAY,  471, 529, 1, JSMN_ARRAY,  472, 528, 1, JSMN_ARRAY,  473, 527, 1, JSMN_ARRAY,  474, 526, 1,
              JSMN_ARRAY,  475, 525, 1, JSMN_ARRAY,  476, 524, 1, JSMN_ARRAY,  477, 523, 1, JSMN_ARRAY,  478, 522, 1, JSMN_ARRAY,  479, 521, 1,
              JSMN_ARRAY,  480, 520, 1, JSMN_ARRAY,  481, 519, 1, JSMN_ARRAY,  482, 518, 1, JSMN_ARRAY,  483, 517, 1, JSMN_ARRAY,  484, 516, 1,
              JSMN_ARRAY,  485, 515, 1, JSMN_ARRAY,  486, 514, 1, JSMN_ARRAY,  487, 513, 1, JSMN_ARRAY,  488, 512, 1, JSMN_ARRAY,  489, 511, 1,
              JSMN_ARRAY,  490, 510, 1, JSMN_ARRAY,  491, 509, 1, JSMN_ARRAY,  492, 508, 1, JSMN_ARRAY,  493, 507, 1, JSMN_ARRAY,  494, 506, 1,
              JSMN_ARRAY,  495, 505, 1, JSMN_ARRAY,  496, 504, 1, JSMN_ARRAY,  497, 503, 1, JSMN_ARRAY,  498, 502, 1, JSMN_ARRAY,  499, 501, 0));

  /* i_structure_UTF-8_BOM_empty_object.json failed to parse. */

  return 0;
}

int test_jsmn_test_suite_n_(void) {
#ifndef JSMN_PERMISSIVE
  /* n_array_1_true_without_comma.json */
  check(query("[1 true]", JSMN_ERROR_INVAL));

  /* n_array_a_invalid_utf8.json */
  check(query("[aÂ]", JSMN_ERROR_INVAL));

  /* n_array_colon_instead_of_comma.json */
  check(query("[\"\": 1]", JSMN_ERROR_INVAL));

  /* n_array_comma_after_close.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("[\"\"],", JSMN_ERROR_INVAL));
#else
  check(query("[\"\"],", 2));
#endif

  /* n_array_comma_and_number.json */
  check(query("[,1]", JSMN_ERROR_INVAL));

  /* n_array_double_comma.json */
  check(query("[1,,2]", JSMN_ERROR_INVAL));

  /* n_array_double_extra_comma.json */
  check(query("[\"x\",,]", JSMN_ERROR_INVAL));

  /* n_array_extra_close.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("[\"x\"]]", JSMN_ERROR_INVAL));
#else
  check(query("[\"x\"]]", 2));
#endif

  /* n_array_extra_comma.json */
  check(query("[\"\",]", JSMN_ERROR_INVAL));

  /* n_array_incomplete_invalid_value.json */
  check(query("[x", JSMN_ERROR_INVAL));

  /* n_array_incomplete.json */
  check(query("[\"x\"", JSMN_ERROR_PART));

  /* n_array_inner_array_no_comma.json */
  check(query("[3[4]]", JSMN_ERROR_INVAL));

  /* n_array_invalid_utf8.json */
  check(query("[ˇ]", JSMN_ERROR_INVAL));

  /* n_array_items_separated_by_semicolon.json */
  check(query("[1:2]", JSMN_ERROR_INVAL));

  /* n_array_just_comma.json */
  check(query("[,]", JSMN_ERROR_INVAL));

  /* n_array_just_minus.json */
  check(query("[-]", JSMN_ERROR_INVAL));

  /* n_array_missing_value.json */
  check(query("[   , \"\"]", JSMN_ERROR_INVAL));

  /* n_array_newlines_unclosed.json */
  check(query("[\"a\",\n4\n,1,", JSMN_ERROR_PART));

  /* n_array_number_and_comma.json */
  check(query("[1,]", JSMN_ERROR_INVAL));

  /* n_array_number_and_several_commas.json */
  check(query("[1,,]", JSMN_ERROR_INVAL));

  /* n_array_spaces_vertical_tab_formfeed.json */
  check(query("[\"a\"\f]", JSMN_ERROR_INVAL));

  /* n_array_star_inside.json */
  check(query("[*]", JSMN_ERROR_INVAL));

  /* n_array_unclosed.json */
  check(query("[\"\"", JSMN_ERROR_PART));

  /* n_array_unclosed_trailing_comma.json */
  check(query("[1,", JSMN_ERROR_PART));

  /* n_array_unclosed_with_new_lines.json */
  check(query("[1,\n1\n,1", JSMN_ERROR_PART));

  /* n_array_unclosed_with_object_inside.json */
  check(query("[{}", JSMN_ERROR_PART));

  /* n_incomplete_false.json */
  check(query("[fals]", JSMN_ERROR_INVAL));

  /* n_incomplete_null.json */
  check(query("[nul]", JSMN_ERROR_INVAL));

  /* n_incomplete_true.json */
  check(query("[tru]", JSMN_ERROR_INVAL));

  /* n_multidigit_number_then_00.json has a null byte in it. */

  /* n_number_0.1.2.json */
  check(query("[0.1.2]", JSMN_ERROR_INVAL));

  /* n_number_-01.json */
  check(query("[-01]", JSMN_ERROR_INVAL));

  /* n_number_0.3e+.json */
  check(query("[0.3e+]", JSMN_ERROR_INVAL));

  /* n_number_0.3e.json */
  check(query("[0.3e]", JSMN_ERROR_INVAL));

  /* n_number_0_capital_E+.json */
  check(query("[0E+]", JSMN_ERROR_INVAL));

  /* n_number_0_capital_E.json */
  check(query("[0E]", JSMN_ERROR_INVAL));

  /* n_number_0.e1.json */
  check(query("[0.e1]", JSMN_ERROR_INVAL));

  /* n_number_0e+.json */
  check(query("[0e+]", JSMN_ERROR_INVAL));

  /* n_number_0e.json */
  check(query("[0e]", JSMN_ERROR_INVAL));

  /* n_number_1_000.json */
  check(query("[1 000.0]", JSMN_ERROR_INVAL));

  /* n_number_1.0e+.json */
  check(query("[1.0e+]", JSMN_ERROR_INVAL));

  /* n_number_1.0e-.json */
  check(query("[1.0e-]", JSMN_ERROR_INVAL));

  /* n_number_1.0e.json */
  check(query("[1.0e]", JSMN_ERROR_INVAL));

  /* n_number_-1.0..json */
  check(query("[-1.0.]", JSMN_ERROR_INVAL));

  /* n_number_1eE2.json */
  check(query("[1eE2]", JSMN_ERROR_INVAL));

  /* n_number_+1.json */
  check(query("[+1]", JSMN_ERROR_INVAL));

  /* n_number_.-1.json */
  check(query("[.-1]", JSMN_ERROR_INVAL));

  /* n_number_2.e+3.json */
  check(query("[2.e+3]", JSMN_ERROR_INVAL));

  /* n_number_2.e-3.json */
  check(query("[2.e-3]", JSMN_ERROR_INVAL));

  /* n_number_2.e3.json */
  check(query("[2.e3]", JSMN_ERROR_INVAL));

  /* n_number_.2e-3.json */
  check(query("[.2e-3]", JSMN_ERROR_INVAL));

  /* n_number_-2..json */
  check(query("[-2.]", JSMN_ERROR_INVAL));

  /* n_number_9.e+.json */
  check(query("[9.e+]", JSMN_ERROR_INVAL));

  /* n_number_expression.json */
  check(query("[1+2]", JSMN_ERROR_INVAL));

  /* n_number_hex_1_digit.json */
  check(query("[0x1]", JSMN_ERROR_INVAL));

  /* n_number_hex_2_digits.json */
  check(query("[0x42]", JSMN_ERROR_INVAL));

  /* n_number_infinity.json */
  check(query("[Infinity]", JSMN_ERROR_INVAL));

  /* n_number_+Inf.json */
  check(query("[+Inf]", JSMN_ERROR_INVAL));

  /* n_number_Inf.json */
  check(query("[Inf]", JSMN_ERROR_INVAL));

  /* n_number_invalid+-.json */
  check(query("[0e+-1]", JSMN_ERROR_INVAL));

  /* n_number_invalid-negative-real.json */
  check(query("[-123.123foo]", JSMN_ERROR_INVAL));

  /* n_number_invalid-utf-8-in-bigger-int.json */
  check(query("[123Â]", JSMN_ERROR_INVAL));

  /* n_number_invalid-utf-8-in-exponent.json */
  check(query("[1e1Â]", JSMN_ERROR_INVAL));

  /* n_number_invalid-utf-8-in-int.json */
  check(query("[0Â]", JSMN_ERROR_INVAL));

  /* n_number_++.json */
  check(query("[++1234]", JSMN_ERROR_INVAL));

  /* n_number_minus_infinity.json */
  check(query("[-Infinity]", JSMN_ERROR_INVAL));

  /* n_number_minus_sign_with_trailing_garbage.json */
  check(query("[-foo]", JSMN_ERROR_INVAL));

  /* n_number_minus_space_1.json */
  check(query("[- 1]", JSMN_ERROR_INVAL));

  /* n_number_-NaN.json */
  check(query("[-NaN]", JSMN_ERROR_INVAL));

  /* n_number_NaN.json */
  check(query("[NaN]", JSMN_ERROR_INVAL));

  /* n_number_neg_int_starting_with_zero.json */
  check(query("[-012]", JSMN_ERROR_INVAL));

  /* n_number_neg_real_without_int_part.json */
  check(query("[-.123]", JSMN_ERROR_INVAL));

  /* n_number_neg_with_garbage_at_end.json */
  check(query("[-1x]", JSMN_ERROR_INVAL));

  /* n_number_real_garbage_after_e.json */
  check(query("[1ea]", JSMN_ERROR_INVAL));

  /* n_number_real_with_invalid_utf8_after_e.json */
  check(query("[1eÂ]", JSMN_ERROR_INVAL));

  /* n_number_real_without_fractional_part.json */
  check(query("[1.]", JSMN_ERROR_INVAL));

  /* n_number_starting_with_dot.json */
  check(query("[.123]", JSMN_ERROR_INVAL));

  /* n_number_U+FF11_fullwidth_digit_one.json */
  check(query("[Ôºë]", JSMN_ERROR_INVAL));

  /* n_number_with_alpha_char.json */
  check(query("[1.8011670033376514H-308]", JSMN_ERROR_INVAL));

  /* n_number_with_alpha.json */
  check(query("[1.2a-3]", JSMN_ERROR_INVAL));

  /* n_number_with_leading_zero.json */
  check(query("[012]", JSMN_ERROR_INVAL));

  /* n_object_bad_value.json */
  check(query("[\"x\", truth]", JSMN_ERROR_INVAL));

  /* n_object_bracket_key.json */
  check(query("{[: \"x\"}", JSMN_ERROR_INVAL));

  /* n_object_comma_instead_of_colon.json */
  check(query("{\"x\", null}", JSMN_ERROR_INVAL));

  /* n_object_double_colon.json */
  check(query("{\"x\"::\"b\"}", JSMN_ERROR_INVAL));

  /* n_object_emoji.json */
  check(query("{üá®üá≠}", JSMN_ERROR_INVAL));

  /* n_object_garbage_at_end.json */
  check(query("{\"a\":\"a\" 123}", JSMN_ERROR_INVAL));

  /* n_object_key_with_single_quotes.json */
  check(query("{key: 'value'}", JSMN_ERROR_INVAL));

  /* n_object_lone_continuation_byte_in_key_and_trailing_comma.json */
  check(query("{\"π\":\"0\",}", JSMN_ERROR_INVAL));

  /* n_object_missing_colon.json */
  check(query("{\"a\" b}", JSMN_ERROR_INVAL));

  /* n_object_missing_key.json */
  check(query("{:\"b\"}", JSMN_ERROR_INVAL));

  /* n_object_missing_semicolon.json */
  check(query("{\"a\" \"b\"}", JSMN_ERROR_INVAL));

  /* n_object_missing_value.json */
  check(query("{\"a\":", JSMN_ERROR_PART));

  /* n_object_no-colon.json */
  check(query("{\"a\"", JSMN_ERROR_PART));

  /* n_object_non_string_key_but_huge_number_instead.json */
  check(query("{9999E9999:1}", JSMN_ERROR_INVAL));

  /* n_object_non_string_key.json */
  check(query("{1:1}", JSMN_ERROR_INVAL));

  /* n_object_repeated_null_null.json */
  check(query("{null:null,null:null}", JSMN_ERROR_INVAL));

  /* n_object_several_trailing_commas.json */
  check(query("{\"id\":0,,,,,}", JSMN_ERROR_INVAL));

  /* n_object_single_quote.json */
  check(query("{'a':0}", JSMN_ERROR_INVAL));

  /* n_object_trailing_comma.json */
  check(query("{\"id\":0,}", JSMN_ERROR_INVAL));

  /* n_object_trailing_comment.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{\"a\":\"b\"}/**/", JSMN_ERROR_INVAL));
#else
  check(query("{\"a\":\"b\"}/**/", 3));
#endif

  /* n_object_trailing_comment_open.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{\"a\":\"b\"}/**//", JSMN_ERROR_INVAL));
#else
  check(query("{\"a\":\"b\"}/**//", 3));
#endif

  /* n_object_trailing_comment_slash_open_incomplete.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{\"a\":\"b\"}/", JSMN_ERROR_INVAL));
#else
  check(query("{\"a\":\"b\"}/", 3));
#endif

  /* n_object_trailing_comment_slash_open.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{\"a\":\"b\"}//", JSMN_ERROR_INVAL));
#else
  check(query("{\"a\":\"b\"}//", 3));
#endif

  /* n_object_two_commas_in_a_row.json */
  check(query("{\"a\":\"b\",,\"c\":\"d\"}", JSMN_ERROR_INVAL));

  /* n_object_unquoted_key.json */
  check(query("{a: \"b\"}", JSMN_ERROR_INVAL));

  /* n_object_unterminated-value.json */
  check(query("{\"a\":\"a", JSMN_ERROR_PART));

  /* n_object_with_single_string.json */
  check(query("{ \"foo\" : \"bar\", \"a\" }", JSMN_ERROR_INVAL));

  /* n_object_with_trailing_garbage.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{\"a\":\"b\"}#", JSMN_ERROR_INVAL));
#else
  check(query("{\"a\":\"b\"}#", 3));
#endif

  /* n_single_space.json */
  check(query(" ", JSMN_ERROR_INVAL));

  /* n_string_1_surrogate_then_escape.json */
  check(query("[\"\\uD800\\\"]", JSMN_ERROR_PART));

  /* n_string_1_surrogate_then_escape_u1.json */
  check(query("[\"\\uD800\\u1\"]", JSMN_ERROR_INVAL));

  /* n_string_1_surrogate_then_escape_u1x.json */
  check(query("[\"\\uD800\\u1x\"]", JSMN_ERROR_INVAL));

  /* n_string_1_surrogate_then_escape_u.json */
  check(query("[\"\\uD800\\u\"]", JSMN_ERROR_INVAL));

  /* n_string_accentuated_char_no_quotes.json */
  check(query("[√©]", JSMN_ERROR_INVAL));

  /* n_string_backslash_00.json has a null byte in it. */

  /* n_string_escaped_backslash_bad.json */
  check(query("[\"\\\\\\\"]", JSMN_ERROR_PART));

  /* n_string_escaped_ctrl_char_tab.json has a null byte in it. */

  /* n_string_escaped_emoji.json */
  check(query("[\"\\üåÄ\"]", JSMN_ERROR_INVAL));

  /* n_string_escape_x.json */
  check(query("[\"\\x00\"]", JSMN_ERROR_INVAL));

  /* n_string_incomplete_escaped_character.json */
  check(query("[\"\\u00A\"]", JSMN_ERROR_INVAL));

  /* n_string_incomplete_escape.json */
  check(query("[\"\\\"]", JSMN_ERROR_PART));

  /* n_string_incomplete_surrogate_escape_invalid.json */
  check(query("[\"\\uD800\\uD800\\x\"]", JSMN_ERROR_INVAL));

  /* n_string_incomplete_surrogate.json */
  check(query("[\"\\uD834\\uDd\"]", JSMN_ERROR_INVAL));

  /* n_string_invalid_backslash_esc.json */
  check(query("[\"\\a\"]", JSMN_ERROR_INVAL));

  /* n_string_invalid_unicode_escape.json */
  check(query("[\"\\uqqqq\"]", JSMN_ERROR_INVAL));

  /* n_string_invalid_utf8_after_escape.json */
  check(query("[\"\\Â\"]", JSMN_ERROR_INVAL));

  /* n_string_invalid-utf-8-in-escape.json */
  check(query("[\"\\uÂ\"]", JSMN_ERROR_INVAL));

  /* n_string_leading_uescaped_thinspace.json */
  check(query("[\\u0020\"asd\"]", JSMN_ERROR_INVAL));

  /* n_string_no_quotes_with_bad_escape.json */
  check(query("[\\n]", JSMN_ERROR_INVAL));

  /* n_string_single_doublequote.json */
  check(query("\"", JSMN_ERROR_PART));

  /* n_string_single_quote.json */
  check(query("['single quote']", JSMN_ERROR_INVAL));

  /* n_string_single_string_no_double_quotes.json */
  check(query("abc", JSMN_ERROR_INVAL));

  /* n_string_start_escape_unclosed.json */
  check(query("[\"\\", JSMN_ERROR_PART));

  /* n_string_unescaped_crtl_char.json has a null byte in it. */

  /* n_string_unescaped_newline.json */
  check(query("[\"new\nline\"]", JSMN_ERROR_INVAL));

  /* n_string_unescaped_tab.json */
  check(query("[\"\t\"]", JSMN_ERROR_INVAL));

  /* n_string_unicode_CapitalU.json */
  check(query("\"\\UA66D\"", JSMN_ERROR_INVAL));

  /* n_string_with_trailing_garbage.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("\"\"x", JSMN_ERROR_INVAL));
#else
  check(query("\"\"x", 1));
#endif

  /* n_structure_angle_bracket_..json */
  check(query("<.>", JSMN_ERROR_INVAL));

  /* n_structure_angle_bracket_null.json */
  check(query("[<null>]", JSMN_ERROR_INVAL));

  /* n_structure_array_trailing_garbage.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("[1]x", JSMN_ERROR_INVAL));
#else
  check(query("[1]x", 2));
#endif

  /* n_structure_array_with_extra_array_close.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("[1]]", JSMN_ERROR_INVAL));
#else
  check(query("[1]]", 2));
#endif

  /* n_structure_array_with_unclosed_string.json */
  check(query("[\"asd]", JSMN_ERROR_PART));

  /* n_structure_ascii-unicode-identifier.json */
  check(query("a√•", JSMN_ERROR_INVAL));

  /* n_structure_capitalized_True.json */
  check(query("[True]", JSMN_ERROR_INVAL));

  /* n_structure_close_unopened_array.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("1]", JSMN_ERROR_INVAL));
#else
  check(query("1]", 1));
#endif

  /* n_structure_comma_instead_of_closing_brace.json */
  check(query("{\"x\": true,", JSMN_ERROR_PART));

  /* n_structure_double_array.json */
#if defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("[][]", JSMN_ERROR_INVAL));
#elif defined(JSMN_MULTIPLE_JSON)
  check(query("[][]", 2));
#else
  check(query("[][]", 1));
#endif

  /* n_structure_end_array.json */
  check(query("]", JSMN_ERROR_INVAL));

  /* n_structure_incomplete_UTF8_BOM.json */
  check(query("Ôª{}", JSMN_ERROR_INVAL));

  /* n_structure_lone-invalid-utf-8.json */
  check(query("Â", JSMN_ERROR_INVAL));

  /* n_structure_lone-open-bracket.json */
  check(query("[", JSMN_ERROR_PART));

  /* n_structure_no_data.json */
  check(query("", JSMN_ERROR_INVAL));

  /* n_structure_null-byte-outside-string.json has a null byte in it. */

  /* n_structure_number_with_trailing_garbage.json */
  check(query("2@", JSMN_ERROR_INVAL));

  /* n_structure_object_followed_by_closing_object.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{}}", JSMN_ERROR_INVAL));
#else
  check(query("{}}", 1));
#endif

  /* n_structure_object_unclosed_no_value.json */
  check(query("{\"\":", JSMN_ERROR_PART));

  /* n_structure_object_with_comment.json */
  check(query("{\"a\":/*comment*/\"b\"}", JSMN_ERROR_INVAL));

  /* n_structure_object_with_trailing_garbage.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{\"a\": true} \"x\"", JSMN_ERROR_INVAL));
#else
  check(query("{\"a\": true} \"x\"", 3));
#endif

  /* n_structure_open_array_apostrophe.json */
  check(query("['", JSMN_ERROR_INVAL));

  /* n_structure_open_array_comma.json */
  check(query("[,", JSMN_ERROR_INVAL));

  /* n_structure_open_array_object.json is too big. */

  /* n_structure_open_array_open_object.json */
  check(query("[{", JSMN_ERROR_PART));

  /* n_structure_open_array_open_string.json */
  check(query("[\"a", JSMN_ERROR_PART));

  /* n_structure_open_array_string.json */
  check(query("[\"a\"", JSMN_ERROR_PART));

  /* n_structure_open_object_close_array.json */
  check(query("{]", JSMN_ERROR_BRACKETS));

  /* n_structure_open_object_comma.json */
  check(query("{,", JSMN_ERROR_INVAL));

  /* n_structure_open_object.json */
  check(query("{", JSMN_ERROR_PART));

  /* n_structure_open_object_open_array.json */
  check(query("{[", JSMN_ERROR_INVAL));

  /* n_structure_open_object_open_string.json */
  check(query("{\"a", JSMN_ERROR_PART));

  /* n_structure_open_object_string_with_apostrophes.json */
  check(query("{'a'", JSMN_ERROR_INVAL));

  /* n_structure_open_open.json */
  check(query("[\"\\{[\"\\{[\"\\{[\"\\{", JSMN_ERROR_INVAL));

  /* n_structure_single_eacute.json */
  check(query("È", JSMN_ERROR_INVAL));

  /* n_structure_single_star.json */
  check(query("*", JSMN_ERROR_INVAL));

  /* n_structure_trailing_#.json */
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{\"a\":\"b\"}#{}", JSMN_ERROR_INVAL));
#else
  check(query("{\"a\":\"b\"}#{}", 3));
#endif

  /* n_structure_U+2060_word_joined.json */
  check(query("[‚Å†]", JSMN_ERROR_INVAL));

  /* n_structure_uescaped_LF_before_string.json */
  check(query("[\\u000A\"\"]", JSMN_ERROR_INVAL));

  /* n_structure_unclosed_array.json */
  check(query("[1", JSMN_ERROR_PART));

  /* n_structure_unclosed_array_partial_null.json */
  check(query("[ false, nul", JSMN_ERROR_PART));

  /* n_structure_unclosed_array_unfinished_false.json */
  check(query("[ true, fals", JSMN_ERROR_PART));

  /* n_structure_unclosed_array_unfinished_true.json */
  check(query("[ false, tru", JSMN_ERROR_PART));

  /* n_structure_unclosed_object.json */
  check(query("{\"asd\":\"asd\"", JSMN_ERROR_PART));

  /* n_structure_unicode-identifier.json */
  check(query("√•", JSMN_ERROR_INVAL));

  /* n_structure_UTF8_BOM_no_data.json */
  check(query("Ôªø", JSMN_ERROR_INVAL));

  /* n_structure_whitespace_formfeed.json */
  check(query("[\f]", JSMN_ERROR_INVAL));

  /* n_structure_whitespace_U+2060_word_joiner.json */
  check(query("[‚Å†]", JSMN_ERROR_INVAL));

#endif /* JSMN_PERMISSIVE */
  return 0;
}

int test_jsmn_test_suite_y_(void) {
  /* y_array_arraysWithSpaces.json */
  check(parse("[[]   ]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_ARRAY,  1, 3, 0));

  /* y_array_empty.json */
  check(parse("[]", 1, 1,
              JSMN_ARRAY,  0, 2, 0));

  /* y_array_empty-string.json */
  check(parse("[\"\"]", 2, 2,
              JSMN_ARRAY,  0, 4, 1,
              JSMN_STRING, "", 0));

  /* y_array_false.json */
  check(parse("[false]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_PRIMITIVE, "false"));

  /* y_array_heterogeneous.json */
  check(parse("[null, 1, \"1\", {}]", 5, 5,
              JSMN_ARRAY,  0, 18, 4,
              JSMN_PRIMITIVE, "null",
              JSMN_PRIMITIVE, "1",
              JSMN_STRING, "1", 0,
              JSMN_OBJECT, 15, 17, 0));

  /* y_array_null.json */
  check(parse("[null]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "null"));

  /* y_array_with_1_and_newline.json */
  check(parse("[1\n]", 2, 2,
              JSMN_ARRAY,  0, 4, 1,
              JSMN_PRIMITIVE, "1"));

  /* y_array_with_leading_space.json */
  check(parse(" [1]", 2, 2,
              JSMN_ARRAY,  1, 4, 1,
              JSMN_PRIMITIVE, "1"));

  /* y_array_with_several_null.json */
  check(parse("[1,null,null,null,2]", 6, 6,
              JSMN_ARRAY,  0, 20, 5,
              JSMN_PRIMITIVE, "1",
              JSMN_PRIMITIVE, "null",
              JSMN_PRIMITIVE, "null",
              JSMN_PRIMITIVE, "null",
              JSMN_PRIMITIVE, "2"));

  /* y_array_with_trailing_space.json */
  check(parse("[2] ", 2, 2,
              JSMN_ARRAY,  0, 3, 1,
              JSMN_PRIMITIVE, "2"));

  /* y_number_0e+1.json */
  check(parse("[0e+1]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "0e+1"));

  /* y_number_0e1.json */
  check(parse("[0e1]", 2, 2,
              JSMN_ARRAY,  0, 5, 1,
              JSMN_PRIMITIVE, "0e1"));

  /* y_number_after_space.json */
  check(parse("[ 4]", 2, 2,
              JSMN_ARRAY,  0, 4, 1,
              JSMN_PRIMITIVE, "4"));

  /* y_number_double_close_to_zero.json */
  check(parse("[-0.000000000000000000000000000000000000000000000000000000000000000000000000000001]", 2, 2,
              JSMN_ARRAY,  0, 83, 1,
              JSMN_PRIMITIVE, "-0.000000000000000000000000000000000000000000000000000000000000000000000000000001"));

  /* y_number_int_with_exp.json */
  check(parse("[20e1]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "20e1"));

  /* y_number.json */
  check(parse("[123e65]", 2, 2,
              JSMN_ARRAY,  0, 8, 1,
              JSMN_PRIMITIVE, "123e65"));

  /* y_number_minus_zero.json */
  check(parse("[-0]", 2, 2,
              JSMN_ARRAY,  0, 4, 1,
              JSMN_PRIMITIVE, "-0"));

  /* y_number_negative_int.json */
  check(parse("[-123]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "-123"));

  /* y_number_negative_one.json */
  check(parse("[-1]", 2, 2,
              JSMN_ARRAY,  0, 4, 1,
              JSMN_PRIMITIVE, "-1"));

  /* y_number_negative_zero.json */
  check(parse("[-0]", 2, 2,
              JSMN_ARRAY,  0, 4, 1,
              JSMN_PRIMITIVE, "-0"));

  /* y_number_real_capital_e.json */
  check(parse("[1E22]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "1E22"));

  /* y_number_real_capital_e_neg_exp.json */
  check(parse("[1E-2]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "1E-2"));

  /* y_number_real_capital_e_pos_exp.json */
  check(parse("[1E+2]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "1E+2"));

  /* y_number_real_exponent.json */
  check(parse("[123e45]", 2, 2,
              JSMN_ARRAY,  0, 8, 1,
              JSMN_PRIMITIVE, "123e45"));

  /* y_number_real_fraction_exponent.json */
  check(parse("[123.456e78]", 2, 2,
              JSMN_ARRAY,  0, 12, 1,
              JSMN_PRIMITIVE, "123.456e78"));

  /* y_number_real_neg_exp.json */
  check(parse("[1e-2]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "1e-2"));

  /* y_number_real_pos_exponent.json */
  check(parse("[1e+2]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "1e+2"));

  /* y_number_simple_int.json */
  check(parse("[123]", 2, 2,
              JSMN_ARRAY,  0, 5, 1,
              JSMN_PRIMITIVE, "123"));

  /* y_number_simple_real.json */
  check(parse("[123.456789]", 2, 2,
              JSMN_ARRAY,  0, 12, 1,
              JSMN_PRIMITIVE, "123.456789"));

  /* y_object_basic.json */
  check(parse("{\"asd\":\"sdf\"}", 3, 3,
              JSMN_OBJECT, 0, 13, 1,
              JSMN_STRING, "asd", 1,
              JSMN_STRING, "sdf", 0));

  /* y_object_duplicated_key_and_value.json */
  check(parse("{\"a\":\"b\",\"a\":\"b\"}", 5, 5,
              JSMN_OBJECT, 0, 17, 2,
              JSMN_STRING, "a", 1,
              JSMN_STRING, "b", 0,
              JSMN_STRING, "a", 1,
              JSMN_STRING, "b", 0));

  /* y_object_duplicated_key.json */
  check(parse("{\"a\":\"b\",\"a\":\"c\"}", 5, 5,
              JSMN_OBJECT, 0, 17, 2,
              JSMN_STRING, "a", 1,
              JSMN_STRING, "b", 0,
              JSMN_STRING, "a", 1,
              JSMN_STRING, "c", 0));

  /* y_object_empty.json */
  check(parse("{}", 1, 1,
              JSMN_OBJECT, 0, 2, 0));

  /* y_object_empty_key.json */
  check(parse("{\"\":0}", 3, 3,
              JSMN_OBJECT, 0, 6, 1,
              JSMN_STRING, "", 1,
              JSMN_PRIMITIVE, "0"));

  /* y_object_escaped_null_in_key.json */
  check(parse("{\"foo\\u0000bar\": 42}", 3, 3,
              JSMN_OBJECT, 0, 20, 1,
              JSMN_STRING, "foo\\u0000bar", 1,
              JSMN_PRIMITIVE, "42"));

  /* y_object_extreme_numbers.json */
  check(parse("{ \"min\": -1.0e+28, \"max\": 1.0e+28 }", 5, 5,
              JSMN_OBJECT, 0, 35, 2,
              JSMN_STRING, "min", 1,
              JSMN_PRIMITIVE, "-1.0e+28",
              JSMN_STRING, "max", 1,
              JSMN_PRIMITIVE, "1.0e+28"));

  /* y_object.json */
  check(parse("{\"asd\":\"sdf\", \"dfg\":\"fgh\"}", 5, 5,
              JSMN_OBJECT, 0, 26, 2,
              JSMN_STRING, "asd", 1,
              JSMN_STRING, "sdf", 0,
              JSMN_STRING, "dfg", 1,
              JSMN_STRING, "fgh", 0));

  /* y_object_long_strings.json */
  check(parse("{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}", 8, 8,
              JSMN_OBJECT, 0, 108, 2,
              JSMN_STRING, "x", 1,
              JSMN_ARRAY,  5, 57, 1,
              JSMN_OBJECT, 6, 56, 1,
              JSMN_STRING, "id", 1,
              JSMN_STRING, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0,
              JSMN_STRING, "id", 1,
              JSMN_STRING, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0));

  /* y_object_simple.json */
  check(parse("{\"a\":[]}", 3, 3,
              JSMN_OBJECT, 0, 8, 1,
              JSMN_STRING, "a", 1,
              JSMN_ARRAY,  5, 7, 0));

  /* y_object_string_unicode.json */
  check(parse("{\"title\":\"\\u041f\\u043e\\u043b\\u0442\\u043e\\u0440\\u0430 \\u0417\\u0435\\u043c\\u043b\\u0435\\u043a\\u043e\\u043f\\u0430\" }", 3, 3,
              JSMN_OBJECT, 0, 110, 1,
              JSMN_STRING, "title", 1,
              JSMN_STRING, "\\u041f\\u043e\\u043b\\u0442\\u043e\\u0440\\u0430 \\u0417\\u0435\\u043c\\u043b\\u0435\\u043a\\u043e\\u043f\\u0430", 0));

  /* y_object_with_newlines.json */
  check(parse("{\n\"a\": \"b\"\n}", 3, 3,
              JSMN_OBJECT, 0, 12, 1,
              JSMN_STRING, "a", 1,
              JSMN_STRING, "b", 0));

  /* y_string_1_2_3_bytes_UTF-8_sequences.json */
  check(parse("[\"\\u0060\\u012a\\u12AB\"]", 2, 2,
              JSMN_ARRAY,  0, 22, 1,
              JSMN_STRING, "\\u0060\\u012a\\u12AB", 0));

  /* y_string_accepted_surrogate_pair.json */
  check(parse("[\"\\uD801\\udc37\"]", 2, 2,
              JSMN_ARRAY,  0, 16, 1,
              JSMN_STRING, "\\uD801\\udc37", 0));

  /* y_string_accepted_surrogate_pairs.json */
  check(parse("[\"\\ud83d\\ude39\\ud83d\\udc8d\"]", 2, 2,
              JSMN_ARRAY,  0, 28, 1,
              JSMN_STRING, "\\ud83d\\ude39\\ud83d\\udc8d", 0));

  /* y_string_allowed_escapes.json */
  check(parse("[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]", 2, 2,
              JSMN_ARRAY,  0, 20, 1,
              JSMN_STRING, "\\\"\\\\\\/\\b\\f\\n\\r\\t", 0));

  /* y_string_backslash_and_u_escaped_zero.json */
  check(parse("[\"\\\\u0000\"]", 2, 2,
              JSMN_ARRAY,  0, 11, 1,
              JSMN_STRING, "\\\\u0000", 0));

  /* y_string_backslash_doublequotes.json */
  check(parse("[\"\\\"\"]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_STRING, "\\\"", 0));

  /* y_string_comments.json */
  check(parse("[\"a/*b*/c/*d//e\"]", 2, 2,
              JSMN_ARRAY,  0, 17, 1,
              JSMN_STRING, "a/*b*/c/*d//e", 0));

  /* y_string_double_escape_a.json */
  check(parse("[\"\\\\a\"]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_STRING, "\\\\a", 0));

  /* y_string_double_escape_n.json */
  check(parse("[\"\\\\n\"]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_STRING, "\\\\n", 0));

  /* y_string_escaped_control_character.json */
  check(parse("[\"\\u0012\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u0012", 0));

  /* y_string_escaped_noncharacter.json */
  check(parse("[\"\\uFFFF\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\uFFFF", 0));

  /* y_string_in_array.json */
  check(parse("[\"asd\"]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_STRING, "asd", 0));

  /* y_string_in_array_with_leading_space.json */
  check(parse("[ \"asd\"]", 2, 2,
              JSMN_ARRAY,  0, 8, 1,
              JSMN_STRING, "asd", 0));

  /* y_string_last_surrogates_1_and_2.json */
  check(parse("[\"\\uDBFF\\uDFFF\"]", 2, 2,
              JSMN_ARRAY,  0, 16, 1,
              JSMN_STRING, "\\uDBFF\\uDFFF", 0));

  /* y_string_nbsp_uescaped.json */
  check(parse("[\"new\\u00A0line\"]", 2, 2,
              JSMN_ARRAY,  0, 17, 1,
              JSMN_STRING, "new\\u00A0line", 0));

  /* y_string_nonCharacterInUTF-8_U+10FFFF.json */
  check(parse("[\"Ùèøø\"]", 2, 2,
              JSMN_ARRAY,  0, 8, 1,
              JSMN_STRING, "Ùèøø", 0));

  /* y_string_nonCharacterInUTF-8_U+FFFF.json */
  check(parse("[\"Ôøø\"]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_STRING, "Ôøø", 0));

  /* y_string_null_escape.json */
  check(parse("[\"\\u0000\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u0000", 0));

  /* y_string_one-byte-utf-8.json */
  check(parse("[\"\\u002c\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u002c", 0));

  /* y_string_pi.json */
  check(parse("[\"œÄ\"]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_STRING, "œÄ", 0));

  /* y_string_reservedCharacterInUTF-8_U+1BFFF.json */
  check(parse("[\"õøø\"]", 2, 2,
              JSMN_ARRAY,  0, 8, 1,
              JSMN_STRING, "õøø", 0));

  /* y_string_simple_ascii.json */
  check(parse("[\"asd \"]", 2, 2,
              JSMN_ARRAY,  0, 8, 1,
              JSMN_STRING, "asd ", 0));

  /* y_string_space.json */
  check(parse("\" \"", 1, 1,
              JSMN_STRING, " ", 0));

  /* y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json */
  check(parse("[\"\\uD834\\uDd1e\"]", 2, 2,
              JSMN_ARRAY,  0, 16, 1,
              JSMN_STRING, "\\uD834\\uDd1e", 0));

  /* y_string_three-byte-utf-8.json */
  check(parse("[\"\\u0821\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u0821", 0));

  /* y_string_two-byte-utf-8.json */
  check(parse("[\"\\u0123\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u0123", 0));

  /* y_string_u+2028_line_sep.json */
  check(parse("[\"‚Ä®\"]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_STRING, "‚Ä®", 0));

  /* y_string_u+2029_par_sep.json */
  check(parse("[\"‚Ä©\"]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_STRING, "‚Ä©", 0));

  /* y_string_uescaped_newline.json */
  check(parse("[\"new\\u000Aline\"]", 2, 2,
              JSMN_ARRAY,  0, 17, 1,
              JSMN_STRING, "new\\u000Aline", 0));

  /* y_string_uEscape.json */
  check(parse("[\"\\u0061\\u30af\\u30EA\\u30b9\"]", 2, 2,
              JSMN_ARRAY,  0, 28, 1,
              JSMN_STRING, "\\u0061\\u30af\\u30EA\\u30b9", 0));

  /* y_string_unescaped_char_delete.json */
  check(parse("[\"\"]", 2, 2,
              JSMN_ARRAY,  0, 5, 1,
              JSMN_STRING, "", 0));

  /* y_string_unicode_2.json */
  check(parse("[\"‚çÇ„à¥‚çÇ\"]", 2, 2,
              JSMN_ARRAY,  0, 13, 1,
              JSMN_STRING, "‚çÇ„à¥‚çÇ", 0));

  /* y_string_unicodeEscapedBackslash.json */
  check(parse("[\"\\u005C\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u005C", 0));

  /* y_string_unicode_escaped_double_quote.json */
  check(parse("[\"\\u0022\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u0022", 0));

  /* y_string_unicode.json */
  check(parse("[\"\\uA66D\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\uA66D", 0));

  /* y_string_unicode_U+10FFFE_nonchar.json */
  check(parse("[\"\\uDBFF\\uDFFE\"]", 2, 2,
              JSMN_ARRAY,  0, 16, 1,
              JSMN_STRING, "\\uDBFF\\uDFFE", 0));

  /* y_string_unicode_U+1FFFE_nonchar.json */
  check(parse("[\"\\uD83F\\uDFFE\"]", 2, 2,
              JSMN_ARRAY,  0, 16, 1,
              JSMN_STRING, "\\uD83F\\uDFFE", 0));

  /* y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json */
  check(parse("[\"\\u200B\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u200B", 0));

  /* y_string_unicode_U+2064_invisible_plus.json */
  check(parse("[\"\\u2064\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\u2064", 0));

  /* y_string_unicode_U+FDD0_nonchar.json */
  check(parse("[\"\\uFDD0\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\uFDD0", 0));

  /* y_string_unicode_U+FFFE_nonchar.json */
  check(parse("[\"\\uFFFE\"]", 2, 2,
              JSMN_ARRAY,  0, 10, 1,
              JSMN_STRING, "\\uFFFE", 0));

  /* y_string_utf8.json */
  check(parse("[\"‚Ç¨ùÑû\"]", 2, 2,
              JSMN_ARRAY,  0, 11, 1,
              JSMN_STRING, "‚Ç¨ùÑû", 0));

  /* y_string_with_del_character.json */
  check(parse("[\"aa\"]", 2, 2,
              JSMN_ARRAY,  0, 7, 1,
              JSMN_STRING, "aa", 0));

  /* y_structure_lonely_false.json */
  check(parse("false", 1, 1,
              JSMN_PRIMITIVE, "false"));

  /* y_structure_lonely_int.json */
  check(parse("42", 1, 1,
              JSMN_PRIMITIVE, "42"));

  /* y_structure_lonely_negative_real.json */
  check(parse("-0.1", 1, 1,
              JSMN_PRIMITIVE, "-0.1"));

  /* y_structure_lonely_null.json */
  check(parse("null", 1, 1,
              JSMN_PRIMITIVE, "null"));

  /* y_structure_lonely_string.json */
  check(parse("\"asd\"", 1, 1,
              JSMN_STRING, "asd", 0));

  /* y_structure_lonely_true.json */
  check(parse("true", 1, 1,
              JSMN_PRIMITIVE, "true"));

  /* y_structure_string_empty.json */
  check(parse("\"\"", 1, 1,
              JSMN_STRING, "", 0));

  /* y_structure_trailing_newline.json */
  check(parse("[\"a\"]\n", 2, 2,
              JSMN_ARRAY,  0, 5, 1,
              JSMN_STRING, "a", 0));

  /* y_structure_true_in_array.json */
  check(parse("[true]", 2, 2,
              JSMN_ARRAY,  0, 6, 1,
              JSMN_PRIMITIVE, "true"));

  /* y_structure_whitespace_array.json */
  check(parse(" [] ", 1, 1,
              JSMN_ARRAY,  1, 3, 0));

  return 0;
}


int test_empty(void) {
  check(parse("{}", 1, 1, JSMN_OBJECT, 0, 2, 0));
  check(parse("[]", 1, 1, JSMN_ARRAY, 0, 2, 0));
  check(parse("[{},{}]", 3, 3, JSMN_ARRAY, 0, 7, 2, JSMN_OBJECT, 1, 3, 0,
              JSMN_OBJECT, 4, 6, 0));
  return 0;
}

int test_object(void) {
  check(parse("{\"a\":0}", 3, 3, JSMN_OBJECT, 0, 7, 1, JSMN_STRING, "a", 1,
              JSMN_PRIMITIVE, "0"));
  check(parse("{\"a\":[]}", 3, 3, JSMN_OBJECT, 0, 8, 1, JSMN_STRING, "a", 1,
              JSMN_ARRAY, 5, 7, 0));
  check(parse("{\"a\":[1,2]}", 5, 5, JSMN_OBJECT, 0, 11, 1, JSMN_STRING, "a", 1,
              JSMN_ARRAY, 5, 10, 2, JSMN_PRIMITIVE, "1", JSMN_PRIMITIVE, "2"));
  check(parse("{\"a\":{},\"b\":{}}", 5, 5, JSMN_OBJECT, -1, -1, 2, JSMN_STRING,
              "a", 1, JSMN_OBJECT, -1, -1, 0, JSMN_STRING, "b", 1, JSMN_OBJECT,
              -1, -1, 0));
  check(parse("{\n \"Day\": 26,\n \"Month\": 9,\n \"Year\": 12\n }", 7, 7,
              JSMN_OBJECT, -1, -1, 3, JSMN_STRING, "Day", 1, JSMN_PRIMITIVE,
              "26", JSMN_STRING, "Month", 1, JSMN_PRIMITIVE, "9", JSMN_STRING,
              "Year", 1, JSMN_PRIMITIVE, "12"));
  check(parse("{\"a\": 0, \"b\": \"c\"}", 5, 5, JSMN_OBJECT, -1, -1, 2,
              JSMN_STRING, "a", 1, JSMN_PRIMITIVE, "0", JSMN_STRING, "b", 1,
              JSMN_STRING, "c", 0));

#ifndef JSMN_PERMISSIVE
  check(query("{\"a\"\n0}", JSMN_ERROR_INVAL));
  check(query("{\"a\", 0}", JSMN_ERROR_INVAL));
  check(query("{\"a\": {2}}", JSMN_ERROR_INVAL));
  check(query("{\"a\": {2: 3}}", JSMN_ERROR_INVAL));
  check(query("{\"a\": {\"a\": 2 3}}", JSMN_ERROR_INVAL));
  check(query("{\"a\"}", JSMN_ERROR_INVAL));
  check(query("{\"a\": 1, \"b\"}", JSMN_ERROR_INVAL));
  check(query("{\"a\",\"b\":1}", JSMN_ERROR_INVAL));
  check(query("{\"a\":1,}", JSMN_ERROR_INVAL));
  check(query("{\"a\":\"b\":\"c\"}", JSMN_ERROR_INVAL));
  check(query("{,}", JSMN_ERROR_INVAL));

  check(query("{\"a\":}", JSMN_ERROR_INVAL));
  check(query("{\"a\" \"b\"}", JSMN_ERROR_INVAL));
  check(query("{\"a\" ::::: \"b\"}", JSMN_ERROR_INVAL));
  check(query("{\"a\": [1 \"b\"]}", JSMN_ERROR_INVAL));
  check(query("{\"a\"\"\"}", JSMN_ERROR_INVAL));
  check(query("{\"a\":1\"\"}", JSMN_ERROR_INVAL));
  check(query("{\"a\":1\"b\":1}", JSMN_ERROR_INVAL));
  check(query("{\"a\":\"b\", \"c\":\"d\", {\"e\": \"f\"}}", JSMN_ERROR_INVAL));
#endif
  return 0;
}

int test_array(void) {
  check(query("[10}", JSMN_ERROR_BRACKETS));
  check(query("[1,,3]", JSMN_ERROR_INVAL));
  check(parse("[10]", 2, 2, JSMN_ARRAY, -1, -1, 1, JSMN_PRIMITIVE, "10"));
  check(query("{\"a\": 1]", JSMN_ERROR_BRACKETS));
  check(query("[\"a\": 1]", JSMN_ERROR_INVAL));
  return 0;
}

int test_primitive(void) {
  check(parse("{\"boolVar\" : true }", 3, 3, JSMN_OBJECT, -1, -1, 1,
              JSMN_STRING, "boolVar", 1, JSMN_PRIMITIVE, "true"));
  check(parse("{\"boolVar\" : false }", 3, 3, JSMN_OBJECT, -1, -1, 1,
              JSMN_STRING, "boolVar", 1, JSMN_PRIMITIVE, "false"));
  check(parse("{\"nullVar\" : null }", 3, 3, JSMN_OBJECT, -1, -1, 1,
              JSMN_STRING, "nullVar", 1, JSMN_PRIMITIVE, "null"));
  check(parse("{\"intVar\" : 12}", 3, 3, JSMN_OBJECT, -1, -1, 1, JSMN_STRING,
              "intVar", 1, JSMN_PRIMITIVE, "12"));
  check(parse("{\"floatVar\" : 12.345}", 3, 3, JSMN_OBJECT, -1, -1, 1,
              JSMN_STRING, "floatVar", 1, JSMN_PRIMITIVE, "12.345"));
  return 0;
}

int test_string(void) {
  check(parse("{\"strVar\" : \"hello world\"}", 3, 3, JSMN_OBJECT, -1, -1, 1,
              JSMN_STRING, "strVar", 1, JSMN_STRING, "hello world", 0));
  check(parse("{\"strVar\" : \"escapes: \\/\\r\\n\\t\\b\\f\\\"\\\\\"}", 3, 3,
              JSMN_OBJECT, -1, -1, 1, JSMN_STRING, "strVar", 1, JSMN_STRING,
              "escapes: \\/\\r\\n\\t\\b\\f\\\"\\\\", 0));
  check(parse("{\"strVar\": \"\"}", 3, 3, JSMN_OBJECT, -1, -1, 1, JSMN_STRING,
              "strVar", 1, JSMN_STRING, "", 0));
  check(parse("{\"a\":\"\\uAbcD\"}", 3, 3, JSMN_OBJECT, -1, -1, 1, JSMN_STRING,
              "a", 1, JSMN_STRING, "\\uAbcD", 0));
  check(parse("{\"a\":\"str\\u0000\"}", 3, 3, JSMN_OBJECT, -1, -1, 1,
              JSMN_STRING, "a", 1, JSMN_STRING, "str\\u0000", 0));
  check(parse("{\"a\":\"\\uFFFFstr\"}", 3, 3, JSMN_OBJECT, -1, -1, 1,
              JSMN_STRING, "a", 1, JSMN_STRING, "\\uFFFFstr", 0));
  check(parse("{\"a\":[\"\\u0280\"]}", 4, 4, JSMN_OBJECT, -1, -1, 1,
              JSMN_STRING, "a", 1, JSMN_ARRAY, -1, -1, 1, JSMN_STRING,
              "\\u0280", 0));

  check(query("{\"a\":\"str\\uFFGFstr\"}", JSMN_ERROR_INVAL));
  check(query("{\"a\":\"str\\u@FfF\"}", JSMN_ERROR_INVAL));
  check(query("{\"a\":[\"\\u028\"]}", JSMN_ERROR_INVAL));
  return 0;
}

int test_partial_string(void) {
  jsmnint_t r;
  jsmntok_t tok[5];
  jsmn_parser p;
  jsmn_init(&p);

  const char *js = "{\"x\": \"va\\\\ue\", \"y\": \"value y\"}";

  size_t i;
  for (i = 1; i <= strlen(js); i++) {
    r = jsmn_parse(&p, js, i, tok, sizeof(tok) / sizeof(tok[0]));
    if (i == strlen(js)) {
      check(r == 5);
      check(tokeq(js, tok, 5, JSMN_OBJECT, -1, -1, 2, JSMN_STRING, "x", 1,
                  JSMN_STRING, "va\\\\ue", 0, JSMN_STRING, "y", 1, JSMN_STRING,
                  "value y", 0));
    } else {
      check(r == (jsmnint_t)JSMN_ERROR_PART);
    }
  }
  return 0;
}

int test_partial_array(void) {
#ifndef JSMN_PERMISSIVE
  jsmnint_t r;
  jsmntok_t tok[10];
  jsmn_parser p;
  jsmn_init(&p);

  const char *js = "[ 1, true, [123, \"hello\"]]";

  size_t i;
  for (i = 1; i <= strlen(js); i++) {
    r = jsmn_parse(&p, js, i, tok, sizeof(tok) / sizeof(tok[0]));
    if (i == strlen(js)) {
      check(r == 6);
      check(tokeq(js, tok, 6, JSMN_ARRAY, -1, -1, 3, JSMN_PRIMITIVE, "1",
                  JSMN_PRIMITIVE, "true", JSMN_ARRAY, -1, -1, 2, JSMN_PRIMITIVE,
                  "123", JSMN_STRING, "hello", 0));
    } else {
      check(r == (jsmnint_t)JSMN_ERROR_PART);
    }
  }
#endif
  return 0;
}

int test_array_nomem(void) {
  jsmnint_t r;
  jsmntok_t toksmall[10], toklarge[10];
  jsmn_parser p;

  const char *js = "  [ 1, true, [123, \"hello\"]]";

  size_t i;
  for (i = 0; i < 6; i++) {
    jsmn_init(&p);
    memset(toksmall, 0, sizeof(toksmall));
    memset(toklarge, 0, sizeof(toklarge));
    r = jsmn_parse(&p, js, strlen(js), toksmall, i);
    check(r == (jsmnint_t)JSMN_ERROR_NOMEM);

    memcpy(toklarge, toksmall, sizeof(toksmall));

    r = jsmn_parse(&p, js, strlen(js), toklarge, 10);
    check(r >= 0);
    check(tokeq(js, toklarge, 4, JSMN_ARRAY, -1, -1, 3, JSMN_PRIMITIVE, "1",
                JSMN_PRIMITIVE, "true", JSMN_ARRAY, -1, -1, 2, JSMN_PRIMITIVE,
                "123", JSMN_STRING, "hello", 0));
  }
  return 0;
}

int test_unquoted_keys(void) {
#ifdef JSMN_PERMISSIVE
  jsmnint_t r;
  jsmntok_t tok[10];
  jsmn_parser p;
  jsmn_init(&p);

  const char *js = "key1: \"value\"\nkey2 : 123";

  r = jsmn_parse(&p, js, strlen(js), tok, 10);
  check(r >= 0);
  check(tokeq(js, tok, 4, JSMN_PRIMITIVE, "key1", JSMN_STRING, "value", 0,
              JSMN_PRIMITIVE, "key2", JSMN_PRIMITIVE, "123"));
#endif
  return 0;
}

int test_issue_22(void) {
  const char *js =
      "{ \"height\":10, \"layers\":[ { \"data\":[6,6], \"height\":10, "
      "\"name\":\"Calque de Tile 1\", \"opacity\":1, \"type\":\"tilelayer\", "
      "\"visible\":true, \"width\":10, \"x\":0, \"y\":0 }], "
      "\"orientation\":\"orthogonal\", \"properties\": { }, \"tileheight\":32, "
      "\"tilesets\":[ { \"firstgid\":1, \"image\":\"..\\/images\\/tiles.png\", "
      "\"imageheight\":64, \"imagewidth\":160, \"margin\":0, "
      "\"name\":\"Tiles\", "
      "\"properties\":{}, \"spacing\":0, \"tileheight\":32, \"tilewidth\":32 "
      "}], "
      "\"tilewidth\":32, \"version\":1, \"width\":10 }";
  check(query(js, 61));
  return 0;
}

int test_issue_27(void) {
  const char *js =
      "{ \"name\" : \"Jack\", \"age\" : 27 } { \"name\" : \"Anna\", ";
#ifndef JSMN_MULTIPLE_JSON
  check(parse(js, 5, 5,
              JSMN_OBJECT, -1, -1, 2,
              JSMN_STRING, "name", 1,
              JSMN_STRING, "Jack", 0,
              JSMN_STRING, "age", 1,
              JSMN_PRIMITIVE, "27"));
#else
  check(query(js, JSMN_ERROR_PART));
#endif
  return 0;
}

int test_input_length(void) {
  jsmnint_t r;
  jsmntok_t tokens[10];
  jsmn_parser p;
  jsmn_init(&p);

  const char *js = "{\"a\": 0}garbage";

  r = jsmn_parse(&p, js, 8, tokens, 10);
  check(r == 3);
  check(tokeq(js, tokens, 3, JSMN_OBJECT, -1, -1, 1, JSMN_STRING, "a", 1,
              JSMN_PRIMITIVE, "0"));
  return 0;
}

int test_count(void) {
  check(query("{}", 1));
  check(query("[]", 1));
  check(query("[[]]", 2));
  check(query("[[], []]", 3));
  check(query("[[], []]", 3));
  check(query("[[], [[]], [[], []]]", 7));
  check(query("[\"a\", [[], []]]", 5));
  check(query("[[], \"[], [[]]\", [[]]]", 5));
  check(query("[1, 2, 3]", 4));
  check(query("[1, 2, [3, \"a\"], null]", 7));
  check(query("[}", JSMN_ERROR_BRACKETS));
  check(query("{]", JSMN_ERROR_BRACKETS));
  return 0;
}

int test_nonstrict(void) {
#ifdef JSMN_PERMISSIVE
  const char *js;
  js = "a: 0garbage";
  check(parse(js, 2, 2, JSMN_PRIMITIVE, "a", JSMN_PRIMITIVE, "0garbage"));

  js = "Day : 26\nMonth : Sep\n\nYear: 12";
  check(parse(js, 6, 6, JSMN_PRIMITIVE, "Day", JSMN_PRIMITIVE, "26",
              JSMN_PRIMITIVE, "Month", JSMN_PRIMITIVE, "Sep", JSMN_PRIMITIVE,
              "Year", JSMN_PRIMITIVE, "12"));

  /* nested {s don't cause a parse error. */
  js = "\"key {1\": 1234";
  check(parse(js, 2, 2, JSMN_STRING, "key {1", 1, JSMN_PRIMITIVE, "1234"));
#endif
  return 0;
}

int test_unmatched_brackets(void) {
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("\"key 1\": 1234}", JSMN_ERROR_INVAL));
#else
  check(parse("\"key 1\": 1234}", 1, 1, JSMN_STRING, "key 1", 0));
#endif
  check(query("{\"key 1\": 1234", JSMN_ERROR_PART));
#if defined(JSMN_MULTIPLE_JSON) || defined(JSMN_MULTIPLE_JSON_FAIL)
  check(query("{\"key 1\": 1234}}", JSMN_ERROR_INVAL));
  check(query("\"key 1\"}: 1234", JSMN_ERROR_INVAL));
#else
  check(parse("{\"key 1\": 1234}}", 3, 3, JSMN_OBJECT, 0, 15, 1,
        JSMN_STRING, "key 1", 1, JSMN_PRIMITIVE, "1234"));
  check(parse("\"key 1\"}: 1234", 1, 1, JSMN_STRING, "key 1", 0));
#endif
  check(parse("{\"key {1\": 1234}", 3, 3,
              JSMN_OBJECT, 0, 16, 1,
              JSMN_STRING, "key {1", 1,
              JSMN_PRIMITIVE, "1234"));
  check(query("{\"key 1\":{\"key 2\": 1234}", JSMN_ERROR_PART));
  return 0;
}

int test_object_key(void) {
  check(parse("{\"key\": 1}", 3, 3,
              JSMN_OBJECT, 0, 10, 1,
              JSMN_STRING, "key", 1,
              JSMN_PRIMITIVE, "1"));
#ifndef JSMN_PERMISSIVE
  check(query("{true: 1}", JSMN_ERROR_INVAL));
  check(query("{1: 1}", JSMN_ERROR_INVAL));
  check(query("{{\"key\": 1}: 2}", JSMN_ERROR_INVAL));
  check(query("{[1,2]: 2}", JSMN_ERROR_INVAL));
#endif
  return 0;
}

int test_simple(void) {
  check(parse("{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}", 13, 13,
              JSMN_OBJECT, 0, 95, 4,
              JSMN_STRING, "user", 1,
              JSMN_STRING, "johndoe", 0,
              JSMN_STRING, "admin", 1,
              JSMN_PRIMITIVE, "false",
              JSMN_STRING, "uid", 1,
              JSMN_PRIMITIVE, "1000",
              JSMN_STRING, "groups", 1,
              JSMN_ARRAY, 58, 94, 4,
              JSMN_STRING, "users", 0,
              JSMN_STRING, "wheel", 0,
              JSMN_STRING, "audio", 0,
              JSMN_STRING, "video", 0));
  return 0;
}

int main(void) {
  test(test_empty, "test for a empty JSON objects/arrays");
  test(test_object, "test for a JSON objects");
  test(test_array, "test for a JSON arrays");
  test(test_primitive, "test primitive JSON data types");
  test(test_string, "test string JSON data types");

  test(test_partial_string, "test partial JSON string parsing");
  test(test_partial_array, "test partial array reading");
  test(test_array_nomem, "test array reading with a smaller number of tokens");
  test(test_unquoted_keys, "test unquoted keys (like in JavaScript)");
  test(test_input_length, "test strings that are not null-terminated");
  test(test_issue_22, "test issue #22");
  test(test_issue_27, "test issue #27");
  test(test_count, "test tokens count estimation");
  test(test_nonstrict, "test for non-strict mode");
  test(test_unmatched_brackets, "test for unmatched brackets");
  test(test_object_key, "test for key type");
  test(test_simple, "test for jsmn string from simple.c");

  test(test_jsmn_test_suite_i_, "test jsmn test suite implementation");
  test(test_jsmn_test_suite_n_, "test jsmn test suite should fail");
  test(test_jsmn_test_suite_y_, "test jsmn test suite should pass");

  printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);
  return (test_failed > 0);
}
