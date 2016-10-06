# require at least autoconf 2.61
AC_PREREQ(2.61)

# Process this file with autoconf to produce a configure script.
AC_INIT([beer], [0.1.0])

# We are using C++
AC_LANG(C++)
AC_REQUIRE_CPP
AC_PROG_CXX

## Set R_HOME, respecting an environment variable if set
: ${R_HOME=$(R RHOME)}
if test -z "${R_HOME}"; then
    AC_MSG_ERROR([Could not determine R_HOME.])
fi

which cmake >/dev/null || AC_MSG_ERROR([Could not find CMake.])

## Get R compilers and flags
BEE_CC=$("${R_HOME}/bin/R" CMD config CC)
BEE_CFLAGS=$("${R_HOME}/bin/R" CMD config CFLAGS)
BEE_CPP=$("${R_HOME}/bin/R" CMD config CPP)
BEE_CPPFLAGS=$("${R_HOME}/bin/R" CMD config CPPFLAGS)
BEE_CXX=$("${R_HOME}/bin/R" CMD config CXX)
BEE_CXXFLAGS=$("${R_HOME}/bin/R" CMD config CXXFLAGS)
BEE_CXXCPP=$("${R_HOME}/bin/R" CMD config CXXCPP)

AC_MSG_NOTICE([Starting to build bee-c library])

$(cd src/3dparty/; \
	mkdir -p build/libbee; \
	cd build/libbee; \
	cmake ../../bee-c/ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX="$(pwd)/../../install" -DCMAKE_C_FLAGS="${BEE_CFLAGS}" -DCMAKE_CXX_FLAGS="${BEE_CXXFLAGS}"; \
	make > /dev/null 2>&1; \
	make install > /dev/null 2>&1;)

AC_MSG_NOTICE([Finished building bee-c library])

BEE_LIBS="$(pwd)/src/3dparty/install/lib/libbee.a"
BEE_INCL="-I$(pwd)/src/3dparty/bee-c/third_party/msgpuck"

bee_cflags="${BEE_INCL}"
bee_libs="${BEE_LIBS}"

AC_MSG_NOTICE([Starting to build msgpack library])

$(cd src/3dparty/; \
	mkdir -p build/msgpack; \
	cd build/msgpack; \
	cmake ../../msgpack-1.4.1/ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX="$(pwd)/../../install" -DMSGPACK_CXX11=ON; \
	make > /dev/null 2>&1; \
	make install > /dev/null 2>&1;)

AC_MSG_NOTICE([Finished building msgpack library])

COMMON_INCL="-I$(pwd)/src/3dparty/install/include"

AC_SUBST([PKG_CFLAGS],["${PKG_CFLAGS} ${COMMON_INCL} $bee_cflags"])
AC_SUBST([PKG_CXXFLAGS],["${PKG_CXXFLAGS} -W -Wextra ${COMMON_INCL} $bee_cflags"])
AC_SUBST([PKG_LIBS],["${PKG_LIBS} $bee_libs"])
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
