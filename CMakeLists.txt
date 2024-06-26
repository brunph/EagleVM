# This file is automatically generated from cmake.toml - DO NOT EDIT
# See https://github.com/build-cpp/cmkr for more information

cmake_minimum_required(VERSION 3.15)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "In-tree builds are not supported. Run CMake from a separate directory: cmake -B build")
endif()

set(CMKR_ROOT_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(CMKR_ROOT_PROJECT ON)

	# Bootstrap cmkr and automatically regenerate CMakeLists.txt
	include(cmkr.cmake OPTIONAL RESULT_VARIABLE CMKR_INCLUDE_RESULT)
	if(CMKR_INCLUDE_RESULT)
		cmkr()
	endif()

	# Enable folder support
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	# Create a configure-time dependency on cmake.toml to improve IDE support
	configure_file(cmake.toml cmake.toml COPYONLY)
endif()

# Options
option(BUILD_TESTS "" OFF)

project(EagleVM
	LANGUAGES
		C
		CXX
		ASM_MASM
)

# Subdirectory: deps
set(CMKR_CMAKE_FOLDER ${CMAKE_FOLDER})
if(CMAKE_FOLDER)
	set(CMAKE_FOLDER "${CMAKE_FOLDER}/deps")
else()
	set(CMAKE_FOLDER deps)
endif()
add_subdirectory(deps)
set(CMAKE_FOLDER ${CMKR_CMAKE_FOLDER})

# Target: EagleVM
set(EagleVM_SOURCES
	"EagleVM/source/EagleVM.cpp"
	cmake.toml
)

add_executable(EagleVM)

target_sources(EagleVM PRIVATE ${EagleVM_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EagleVM_SOURCES})

target_compile_features(EagleVM PRIVATE
	cxx_std_23
)

target_include_directories(EagleVM PRIVATE
	"EagleVM/headers"
)

target_link_libraries(EagleVM PRIVATE
	EagleVMCore
)

get_directory_property(CMKR_VS_STARTUP_PROJECT DIRECTORY ${PROJECT_SOURCE_DIR} DEFINITION VS_STARTUP_PROJECT)
if(NOT CMKR_VS_STARTUP_PROJECT)
	set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT EagleVM)
endif()

# Target: EagleVMCore
set(EagleVMCore_SOURCES
	"EagleVM.Core/source/disassembler/disassembler.cpp"
	"EagleVM.Core/source/disassembler/models/basic_block.cpp"
	"EagleVM.Core/source/obfuscation/mba/math/mba_math.cpp"
	"EagleVM.Core/source/obfuscation/mba/mba.cpp"
	"EagleVM.Core/source/obfuscation/mba/mba_gen.cpp"
	"EagleVM.Core/source/obfuscation/mba/variable/mba_const.cpp"
	"EagleVM.Core/source/obfuscation/mba/variable/mba_exp.cpp"
	"EagleVM.Core/source/obfuscation/mba/variable/mba_var.cpp"
	"EagleVM.Core/source/obfuscation/mba/variable/mba_xy.cpp"
	"EagleVM.Core/source/pe/packer/pe_packer.cpp"
	"EagleVM.Core/source/pe/pe_generator.cpp"
	"EagleVM.Core/source/pe/pe_parser.cpp"
	"EagleVM.Core/source/util/section/code_label.cpp"
	"EagleVM.Core/source/util/section/function_container.cpp"
	"EagleVM.Core/source/util/section/section_manager.cpp"
	"EagleVM.Core/source/util/util.cpp"
	"EagleVM.Core/source/util/zydis_helper.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/handlers/base_handler_entry.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/handlers/inst_handler_entry.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/handlers/vm_handler_entry.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/add.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/cmp.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/dec.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/div.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/imul.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/inc.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/lea.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/mov.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/movsx.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/mul.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/pop.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/push.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/ia32_handlers/sub.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/vm_handlers/vm_enter.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/vm_handlers/vm_exit.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/vm_handlers/vm_load.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/vm_handlers/vm_pop_rflags.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/vm_handlers/vm_push_rflags.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/vm_handlers/vm_store.cpp"
	"EagleVM.Core/source/virtual_machine/handlers/vm_handlers/vm_trash_rflags.cpp"
	"EagleVM.Core/source/virtual_machine/vm_inst.cpp"
	"EagleVM.Core/source/virtual_machine/vm_inst_handlers.cpp"
	"EagleVM.Core/source/virtual_machine/vm_inst_regs.cpp"
	"EagleVM.Core/source/virtual_machine/vm_virtualizer.cpp"
	cmake.toml
)

add_library(EagleVMCore)

target_sources(EagleVMCore PRIVATE ${EagleVMCore_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EagleVMCore_SOURCES})

target_compile_features(EagleVMCore PUBLIC
	cxx_std_23
)

if(MSVC) # msvc
	target_compile_options(EagleVMCore PUBLIC
		"/MP"
		"/permissive-"
		"/sdl"
		"/W4"
		"/Zc:inline"
		"/Zc:wchar_t"
		"$<$<CONFIG:Debug>:/FC;/JMC>"
		"$<$<NOT:$<CONFIG:Debug>>:/Gy;/O1;/Os>"
	)
endif()

target_include_directories(EagleVMCore PUBLIC
	"EagleVM.Core/headers"
)

target_link_libraries(EagleVMCore PUBLIC
	Zydis
	bigint
)

if(MSVC) # msvc
	target_link_options(EagleVMCore PUBLIC
		"/INCREMENTAL:NO"
	)
endif()

# Target: EagleVMStub
set(EagleVMStub_SOURCES
	"EagleVM.Stub/EagleVMStub.cpp"
	"EagleVM.Stub/dllmain.cpp"
	cmake.toml
)

add_library(EagleVMStub SHARED)

target_sources(EagleVMStub PRIVATE ${EagleVMStub_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EagleVMStub_SOURCES})

target_compile_features(EagleVMStub PUBLIC
	cxx_std_23
)

target_include_directories(EagleVMStub PUBLIC
	"EagleVM.Stub/Headers"
)

# Target: EagleVMSandbox
set(EagleVMSandbox_SOURCES
	"EagleVM.Sandbox/EagleVMSandbox.cpp"
	"EagleVM.Sandbox/xorstr.hpp"
	cmake.toml
)

add_executable(EagleVMSandbox)

target_sources(EagleVMSandbox PRIVATE ${EagleVMSandbox_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EagleVMSandbox_SOURCES})

target_compile_features(EagleVMSandbox PRIVATE
	cxx_std_23
)

target_link_libraries(EagleVMSandbox PRIVATE
	EagleVMStub
)

target_link_options(EagleVMSandbox PRIVATE
	"/DEBUG:FULL"
)

get_directory_property(CMKR_VS_STARTUP_PROJECT DIRECTORY ${PROJECT_SOURCE_DIR} DEFINITION VS_STARTUP_PROJECT)
if(NOT CMKR_VS_STARTUP_PROJECT)
	set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT EagleVMSandbox)
endif()

# Target: EagleVMTests
set(EagleVMTests_SOURCES
	"EagleVM.Tests/source/main.cpp"
	"EagleVM.Tests/source/run_container.cpp"
	"EagleVM.Tests/source/util.cpp"
	"EagleVM.Tests/headers/run_container.h"
	"EagleVM.Tests/headers/util.h"
	cmake.toml
)

add_executable(EagleVMTests)

target_sources(EagleVMTests PRIVATE ${EagleVMTests_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EagleVMTests_SOURCES})

target_compile_features(EagleVMTests PRIVATE
	cxx_std_23
)

target_include_directories(EagleVMTests PRIVATE
	"EagleVM.Tests/headers"
)

target_link_libraries(EagleVMTests PRIVATE
	EagleVMCore
	nlohmann_json
	Zydis
)

get_directory_property(CMKR_VS_STARTUP_PROJECT DIRECTORY ${PROJECT_SOURCE_DIR} DEFINITION VS_STARTUP_PROJECT)
if(NOT CMKR_VS_STARTUP_PROJECT)
	set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT EagleVMTests)
endif()

