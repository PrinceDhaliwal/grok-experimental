set(GROK_JIT_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/simple.h
    ${CMAKE_CURRENT_SOURCE_DIR}/simple.cc
    ${PROJECT_SOURCE_DIR}/src/lexer/lexer.h
    ${PROJECT_SOURCE_DIR}/src/lexer/lexer.cc
    ${PROJECT_SOURCE_DIR}/src/lexer/token.h
    ${GROK_JIT_SOURCE_FILES}
    PARENT_SCOPE
)
