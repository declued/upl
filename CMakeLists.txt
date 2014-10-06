#-----------------------------------------------------------------------

cmake_minimum_required (VERSION 2.8)
project ("UPL" CXX)

include_directories ("include")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin) 
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib) 

set (CMAKE_CONFIGURATION_TYPES "Debug;Release")
set (CMAKE_BUILD_TYPE "Debug")

if (CMAKE_COMPILER_IS_GNUCXX)
	add_definitions (-std=c++11)
endif (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------

add_library ("upl" STATIC
	"include/upl/ast.hpp"
	"include/upl/code_gen.hpp"
	"include/upl/common.hpp"
	"include/upl/definitions.hpp"
	"include/upl/errors.hpp"
	"include/upl/input.hpp"
	"include/upl/lexer.hpp"
	"include/upl/parser.hpp"
	"include/upl/symbols.hpp"
	"include/upl/tokens.hpp"
	"include/upl/vm.hpp"

	"src/upl/ast.cpp"
	"src/upl/code_gen.cpp"
	"src/upl/common.cpp"
	"src/upl/definitions.cpp"
	"src/upl/errors.cpp"
	"src/upl/input.cpp"
	"src/upl/lexer.cpp"
	"src/upl/parser.cpp"
	"src/upl/symbols.cpp"
	"src/upl/tokens.cpp"
	"src/upl/vm.cpp"
)

#-----------------------------------------------------------------------

add_executable ("uplc"
	"src/uplc/upl_compiler_main.cpp"
)

target_link_libraries ("uplc" "upl")

#-----------------------------------------------------------------------

add_executable ("playpen"
	"src/playpen/upl_playpen_main.cpp"
)

target_link_libraries ("playpen" "upl")

#-----------------------------------------------------------------------
