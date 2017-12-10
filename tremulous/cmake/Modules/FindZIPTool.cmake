# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindZIPTool
# -------
#
# Locates the ziptool program.
#
# ::
#
#   ZIPTool_FOUND - TRUE if LibZIP was found.
#   ZIPTool_PROGRAM - full path to ziptool.

find_program( ZIPTool_PROGRAM ziptool )
mark_as_advanced( ZIPTool_PROGRAM )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( ZIPTool REQUIRED_ARGS ZIPTool_PROGRAM )
