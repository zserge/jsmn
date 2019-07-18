###################################################################################
#                                                                                 #
# NAME: example/meson.build                                                       #
#                                                                                 #
# AUTHOR: Serge Zaitsev.                                                          #
#                                                                                 #
# WRITTEN BY: Michael Brockus.                                                    #
#                                                                                 #
# CONTACT: <mailto:michael@squidfarts.com>                                        #
#                                                                                 #
# NOTICES:                                                                        #
#                                                                                 #
# License: MIT                                                                    #
#                                                                                 #
###################################################################################



##
#
# Meson: Add example executables.
#
##
exe_example_1 = executable('jsondump', 
    sources: 'jsondump.c', 
    dependencies: jsmn_dep)

exe_example_2 = executable('simple', 
    sources: 'simple.c',
    dependencies: jsmn_dep)

test('jsmn json-dump example test', exe_example_1)
test('jsmn simple example test', exe_example_2)
