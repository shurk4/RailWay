# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "RelWithDebInfo")
  file(REMOVE_RECURSE
  "CMakeFiles\\RailWay_ControlPanel_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\RailWay_ControlPanel_autogen.dir\\ParseCache.txt"
  "RailWay_ControlPanel_autogen"
  )
endif()
