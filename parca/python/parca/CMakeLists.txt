find_package(PythonInterp)

set(SOURCES
  __init__.py
  __main__.py
  _base.py
  _util.py
  _custom_matrices.py
  soap_server.py
)

if(APPLE)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.stdout.write('/Library/Python/'+sys.version[0:3]+'/site-packages/parca')"
        OUTPUT_VARIABLE PYTHON_INSTALL_DIR
    )
elseif(WIN32)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import sys; import sysconfig; sys.stdout.write(sysconfig.get_path('platlib')+'/parca')"
        OUTPUT_VARIABLE PYTHON_INSTALL_DIR
    )
else()
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import sys; import sysconfig; sys.stdout.write(sysconfig.get_path('platlib').replace(sys.prefix,'${CMAKE_INSTALL_PREFIX}')+'/parca')"
        OUTPUT_VARIABLE PYTHON_INSTALL_DIR
    )
endif(APPLE)

message("Pure python modules will be installed in "${PYTHON_INSTALL_DIR})

function (py_source target filename)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${PYTHON_EXECUTABLE} -c "import py_compile; py_compile.compile('${CMAKE_CURRENT_SOURCE_DIR}/${filename}','${CMAKE_CURRENT_BINARY_DIR}/${filename}c')"
        VERBATIM
    )
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${PYTHON_EXECUTABLE} -OO -c "import py_compile; py_compile.compile('${CMAKE_CURRENT_SOURCE_DIR}/${filename}','${CMAKE_CURRENT_BINARY_DIR}/${filename}o')"
        VERBATIM
    )
    install(FILES ${filename} ${CMAKE_CURRENT_BINARY_DIR}/${filename}c ${CMAKE_CURRENT_BINARY_DIR}/${filename}o DESTINATION ${PYTHON_INSTALL_DIR})
endfunction(py_source)

add_custom_target(
    py_parca
    ALL
    SOURCES ${SOURCES}
)

foreach (py ${SOURCES})
    py_source(py_parca ${py})
endforeach(py)
