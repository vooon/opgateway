# generate UAVOvjects from xml

set(OPENPILOT_DIR "$ENV{HOME}/src/OpenPilot" CACHE STRING "OpenPilot source tree path")
set(UAVOBJ_BIN "${OPENPILOT_DIR}/build/uavobjgenerator/uavobjgenerator")
set(UAVOBJ_XML_DIR "${OPENPILOT_DIR}/shared/uavobjectdefinition")
set(UAVOBJ_TEMPLATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")


get_filename_component(OPENPILOT_DIR ${OPENPILOT_DIR} REALPATH)
if(NOT EXISTS "${OPENPILOT_DIR}/Makefile")
	message(FATAL_ERROR "You must specify OPENPILOT_DIR")
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

add_custom_command(#TARGET uavobj_syntetics_gen
	OUTPUT  ${UAVOBJ_SYNTETICS_SOURCES}
	COMMAND ${UAVOBJ_BIN} -rosgw ${UAVOBJ_XML_DIR} ${UAVOBJ_TEMPLATE_DIR}
	COMMENT "Generating UAVObjects"
	DEPENDS uavobjgenerator
	)
#add_dependencies(uavobjects uavobj_syntetics_gen)

add_custom_target(uavobjgenerator
	COMMAND make -C ${OPENPILOT_DIR} QMAKE=qmake-qt4 uavobjgenerator
	COMMENT "Compiling UAVObject generator"
	)
#add_dependencies(uavobj_syntetics_gen uavobjgenerator)

