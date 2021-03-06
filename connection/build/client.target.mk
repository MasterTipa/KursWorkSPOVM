# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := client
DEFS_Debug := \
	'-DNODE_GYP_MODULE_NAME=client' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_IMMINENT_DEPRECATION_WARNINGS' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DBUILDING_NODE_EXTENSION' \
	'-DDEBUG' \
	'-D_DEBUG' \
	'-DV8_ENABLE_CHECKS'

# Flags passed to all source files.
CFLAGS_Debug := \
	-fPIC \
	-pthread \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-m64 \
	-fPIC \
	-g \
	-O0

# Flags passed to only C files.
CFLAGS_C_Debug :=

# Flags passed to only C++ files.
CFLAGS_CC_Debug := \
	-fno-rtti \
	-std=gnu++1y \
	-std=c++17

INCS_Debug := \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/include/node \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/src \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/openssl/config \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/openssl/openssl/include \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/uv/include \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/zlib \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/v8/include \
	-I$(srcdir)/.

DEFS_Release := \
	'-DNODE_GYP_MODULE_NAME=client' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_IMMINENT_DEPRECATION_WARNINGS' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DBUILDING_NODE_EXTENSION'

# Flags passed to all source files.
CFLAGS_Release := \
	-fPIC \
	-pthread \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-m64 \
	-fPIC \
	-O3 \
	-fno-omit-frame-pointer

# Flags passed to only C files.
CFLAGS_C_Release :=

# Flags passed to only C++ files.
CFLAGS_CC_Release := \
	-fno-rtti \
	-std=gnu++1y \
	-std=c++17

INCS_Release := \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/include/node \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/src \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/openssl/config \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/openssl/openssl/include \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/uv/include \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/zlib \
	-I/home/master/.electron-gyp/.cache/node-gyp/8.3.0/deps/v8/include \
	-I$(srcdir)/.

OBJS := \
	$(obj).target/$(TARGET)/node_wrap.o \
	$(obj).target/$(TARGET)/client_pop3.o \
	$(obj).target/$(TARGET)/client_smtp.o \
	$(obj).target/$(TARGET)/message.o \
	$(obj).target/$(TARGET)/ssl_socket.o \
	$(obj).target/$(TARGET)/tcp_socket.o \
	$(obj).target/$(TARGET)/base64.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug := \
	-pthread \
	-rdynamic \
	-m64

LDFLAGS_Release := \
	-pthread \
	-rdynamic \
	-m64

LIBS := \
	-lnode \
	-lssl -lcrypto -lpthread

$(obj).target/client.node: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).target/client.node: LIBS := $(LIBS)
$(obj).target/client.node: TOOLSET := $(TOOLSET)
$(obj).target/client.node: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,solink_module)

all_deps += $(obj).target/client.node
# Add target alias
.PHONY: client
client: $(builddir)/client.node

# Copy this to the executable output path.
$(builddir)/client.node: TOOLSET := $(TOOLSET)
$(builddir)/client.node: $(obj).target/client.node FORCE_DO_CMD
	$(call do_cmd,copy)

all_deps += $(builddir)/client.node
# Short alias for building this executable.
.PHONY: client.node
client.node: $(obj).target/client.node $(builddir)/client.node

# Add executable to "all" target.
.PHONY: all
all: $(builddir)/client.node

