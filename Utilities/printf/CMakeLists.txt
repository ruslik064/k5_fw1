
add_library(printf INTERFACE) 
# target_include_directories(printf SYSTEM INTERFACE .)
target_sources(printf INTERFACE printf.c)
