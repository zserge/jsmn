###################################################################################
#                                                                                 #
# NAME: test/meson.build                                                          #
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
# Meson: Add unit test exe.
#
##
test_exe = executable(meson.project_name() + '-test', 
    sources: 'tests.c', 
    dependencies: jsmn_dep)

test('jsmn unit test', test_exe)
