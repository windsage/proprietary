AM_CFLAGS = -Wall -g3 -D __linux__ -Werror

requiredlibs = -lnl-3 -lnl-genl-3 -lpthread -lxml2

vendor_cmd_tool_SOURCES = nl_cmd_wrapper.c ven_cmd_tool.c
vendor_cmd_tool_LDADD = $(requiredlibs)

bin_PROGRAMS = vendor_cmd_tool

vendor_cmd_tool_CFLAGS = $(LIBXML_CFLAGS)
