# generate UAVOvjects from xml

set(OPENPILOT_DIR "$ENV{HOME}/src/UAV/OpenPilot" CACHE STRING "OpenPilot source tree path")
set(USE_UAVOBJGEN "" CACHE STRING "Overwrite uavobjgenerator path (discard compile)")
set(UAVOBJ_BIN "${OPENPILOT_DIR}/build/uavobjgenerator/uavobjgenerator")
set(UAVOBJ_XML_DIR "${OPENPILOT_DIR}/shared/uavobjectdefinition")
set(UAVOBJ_TEMPLATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(UAVOBJ_DEPEND "uavobjgenerator")


get_filename_component(OPENPILOT_DIR ${OPENPILOT_DIR} REALPATH)
if(NOT EXISTS "${OPENPILOT_DIR}/Makefile")
	message(FATAL_ERROR "You must specify OPENPILOT_DIR")
endif()

if(USE_UAVOBJGEN)
	get_filename_component(USE_UAVOBJGEN ${USE_UAVOBJGEN} REALPATH)
	if (NOT EXISTS ${USE_UAVOBJGEN})
		message(FATAL_ERROR "File not found " ${USE_UAVOBJGEN})
	else()
		set(UAVOBJ_BIN ${USE_UAVOBJGEN})
		set(UAVOBJ_DEPEND "")
	endif()
endif()

set(UAVOBJ_SYNTETICS_DIR "rosuavobject")

file(GLOB UAVOBJ_DEFINITIONS "${UAVOBJ_XML_DIR}/*.xml")
foreach(fl ${UAVOBJ_DEFINITIONS})
	get_filename_component(basename ${fl} NAME_WE)
	list(APPEND UAVOBJ_SYNTETICS_SOURCES "${UAVOBJ_SYNTETICS_DIR}/${basename}.cpp")
endforeach(fl)

list(APPEND UAVOBJ_SYNTETICS_SOURCES "${UAVOBJ_SYNTETICS_DIR}/uavobjectsinit.cpp")

#foreach(f ${UAVOBJ_SYNTETICS_SOURCES})
#	message(${f})
#endforeach(f)

add_custom_command(
	OUTPUT  ${UAVOBJ_SYNTETICS_SOURCES}
	COMMAND ${UAVOBJ_BIN} -rosgw ${UAVOBJ_XML_DIR} ${UAVOBJ_TEMPLATE_DIR}
	COMMENT "Generating UAVObjects"
	DEPENDS ${UAVOBJ_DEPEND}
	)

add_custom_target(uavobjgenerator
	COMMAND make -C ${OPENPILOT_DIR} QMAKE=qmake-qt4 uavobjgenerator
	COMMENT "Compiling UAVObject generator"
	)

