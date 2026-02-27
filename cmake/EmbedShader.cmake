file(READ ${INPUT_FILE} SHADER_CONTENTS)
set(HEADER_CONTENTS "/* Generated from ${INPUT_FILE} */\n#pragma once\n\nconst char* ${VAR_NAME} = R\"(\n${SHADER_CONTENTS}\n)\";")
file(WRITE ${OUTPUT_FILE} "${HEADER_CONTENTS}")