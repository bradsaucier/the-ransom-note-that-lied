cmake_minimum_required(VERSION 3.24)

get_filename_component(REPOSITORY_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(REQUIRED_FILES
  "README.md"
  "CMakeLists.txt"
  "portfolio/project-one/Green_Pace_Secure_Development_Policy.docx"
  "portfolio/project-two/Green_Pace_Security_Policy_Presentation.pptx"
  "portfolio/project-two/Green_Pace_Security_Policy_Presentation_Script.docx"
  "portfolio/coding/CS405_Original_GoogleTest_Suite.zip"
  "portfolio/coding/Ransom_Note_That_Lied_Source.zip"
  "portfolio/reflection/Portfolio_Reflection.txt"
  "portfolio/reflection/Security_Before_the_Finish_Line.pdf"
)

foreach(RELATIVE_PATH IN LISTS REQUIRED_FILES)
  set(FULL_PATH "${REPOSITORY_ROOT}/${RELATIVE_PATH}")
  if(NOT EXISTS "${FULL_PATH}")
    message(FATAL_ERROR "Required portfolio file is missing: ${RELATIVE_PATH}")
  endif()
  file(SIZE "${FULL_PATH}" FILE_SIZE)
  if(FILE_SIZE EQUAL 0)
    message(FATAL_ERROR "Required portfolio file is empty: ${RELATIVE_PATH}")
  endif()
endforeach()

set(CODING_ARCHIVES
  "portfolio/coding/CS405_Original_GoogleTest_Suite.zip"
  "portfolio/coding/Ransom_Note_That_Lied_Source.zip"
)

foreach(RELATIVE_PATH IN LISTS CODING_ARCHIVES)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar tf "${REPOSITORY_ROOT}/${RELATIVE_PATH}"
    RESULT_VARIABLE ARCHIVE_RESULT
    OUTPUT_QUIET
    ERROR_VARIABLE ARCHIVE_ERROR
  )
  if(NOT ARCHIVE_RESULT EQUAL 0)
    message(FATAL_ERROR "Invalid ZIP archive ${RELATIVE_PATH}: ${ARCHIVE_ERROR}")
  endif()
endforeach()

message(STATUS "Portfolio artifact validation passed")
