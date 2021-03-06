INCLUDE(${QT_USE_FILE})

QT4_WRAP_CPP(
  MOC_SRCS 
  GraphLayoutFilterPanel.h
  )

SET(
  UI_FORMS
  GraphLayoutFilterPanel.ui
  )

QT4_WRAP_UI(
  UI_BUILT_SOURCES
  ${UI_FORMS}
  )

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${OverViewCore_BINARY_DIR}
  ${OverViewCore_SOURCE_DIR}
  ${PARAVIEW_INCLUDE_DIRS}
  ${ParaView_BINARY_DIR}/VTK/GUISupport/Qt
  ${ParaView_SOURCE_DIR}/VTK/GUISupport/Qt
  ${QtWidgets_BINARY_DIR}
  ${QtWidgets_SOURCE_DIR}
  ${VTK_INCLUDE_DIR}
  ${pqComponents_BINARY_DIR}
  ${pqComponents_SOURCE_DIR}
  ${pqCore_BINARY_DIR}
  ${pqCore_SOURCE_DIR}
  )

ADD_PARAVIEW_OBJECT_PANEL(
  IFACES IFACE_SRCS
  CLASS_NAME GraphLayoutFilterPanel
  XML_NAME GraphLayoutFilter
  XML_GROUP filters
  )

ADD_PARAVIEW_PLUGIN(
  GraphLayoutFilterPanelPlugin "1.0"
  GUI_INTERFACES ${IFACES}
  SOURCES ${MOC_SRCS} ${UI_BUILT_SOURCES} ${IFACE_SRCS} GraphLayoutFilterPanel.cxx
  )

TARGET_LINK_LIBRARIES(
  GraphLayoutFilterPanelPlugin
  OverViewCore
  )

