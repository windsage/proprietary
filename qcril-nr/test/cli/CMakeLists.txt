cmake_minimum_required(VERSION 2.8.9)

set(TARGET_TELSDK_CONSOLE_APP qcrilnr-console-app)

set(TELSDK_CONSOLE_APP_SOURCES
   Phone/PhoneMenu.cpp
   
   Call/CallMenu.cpp

   SimCardServices/SimCardServicesMenu.cpp
   SimCardServices/Card/CardServicesMenu.cpp
   SimCardServices/SapCard/SapCardServicesMenu.cpp

   Sms/SmsMenu.cpp

   ECall/ECallMenu.cpp
   ECall/MsdSettings.cpp

   Data/DataMenu.cpp
   Data/ConfigParser.cpp

   Phone/ServingSystemMenu.cpp

   Phone/NetworkMenu.cpp
   Ims/ImsMenu.cpp
   RILCall/RILCallMenu.cpp
   Modules/ModuleMenu.cpp
   TelSdkConsoleApp.cpp
   Utils.cpp

   Oemhook/OemhookMenu.cpp
   Oemhook/OemhookUtils.cpp
   Oemhook/csg/CsgMenu.cpp
   Oemhook/nv/NvMenu.cpp
   Oemhook/Qtuner/QtunerMenu.cpp

   console_app_framework/ConsoleAppCommand.cpp
   console_app_framework/ConsoleApp.cpp

   Utils.cpp
)

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${telematics-apps_SOURCE_DIR}/common
)

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi-framework)

# set global variables
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_executable (${TARGET_TELSDK_CONSOLE_APP} ${TELSDK_CONSOLE_APP_SOURCES})
target_link_libraries(${TARGET_TELSDK_CONSOLE_APP}
          qcrilClient qcrilInterfaces
          rilqmimiscservices
          qmi_cci
          qmi_encdec
          )

# install to target
install ( TARGETS ${TARGET_TELSDK_CONSOLE_APP}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR} )
