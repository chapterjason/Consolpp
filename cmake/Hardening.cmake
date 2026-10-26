include(CheckPIESupported)

check_pie_supported(OUTPUT_VARIABLE CONSOLPP_PIE_ERROR LANGUAGES CXX)
if(NOT CMAKE_CXX_LINK_PIE_SUPPORTED)
    message(FATAL_ERROR "Position independent executables are not supported: ${CONSOLPP_PIE_ERROR}")
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(release "$<CONFIG:Release,RelWithDebInfo>")
set(linux "$<PLATFORM_ID:Linux>")

# Global, so dependencies linked into the binary are hardened too.
# Some toolchains predefine _FORTIFY_SOURCE, hence the -U before redefining it.
add_compile_options(
    "$<${release}:-U_FORTIFY_SOURCE;-D_FORTIFY_SOURCE=3>"
    "$<${release}:-fstack-protector-strong>"
    "$<$<AND:${release},${linux}>:-fstack-clash-protection>"
)
add_link_options(
    "$<$<AND:${release},${linux}>:LINKER:-z,relro,-z,now,-z,noexecstack>"
)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
    add_compile_options("$<${release}:-fcf-protection=full>")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_options("$<${release}:-mbranch-protection=standard>")
endif()

target_compile_options(consolpp_options INTERFACE "$<${release}:-fstrict-flex-arrays=3>")

unset(release)
unset(linux)
