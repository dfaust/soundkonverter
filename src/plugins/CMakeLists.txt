file(GLOB _plugins *)

foreach(_current_plugin ${_plugins})
   if(IS_DIRECTORY ${_current_plugin})
      add_subdirectory(${_current_plugin})
   endif(IS_DIRECTORY ${_current_plugin})
endforeach(_current_plugin)
