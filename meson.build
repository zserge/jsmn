project('jsmn', 'c')

jsmn_inc = include_directories('.')

jsmn_lib = static_library(
  'scaljsmnparser',
  'jsmn.c',
  include_directories: jsmn_inc
)

jsmn_dep = declare_dependency(
  link_with: jsmn_lib,
  include_directories: jsmn_inc
)
