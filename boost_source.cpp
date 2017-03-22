#include "boost_include.h"

//system
#if BOOST_SYSTEM_USE
#include <libs/system/src/error_code.cpp>
#endif

//filesystem
#if BOOST_FILESYSTEM_USE
#include <libs/filesystem/src/codecvt_error_category.cpp>
#include <libs/filesystem/src/path_traits.cpp>
#include <libs/filesystem/src/unique_path.cpp>
#include <libs/filesystem/src/utf8_codecvt_facet.cpp>
#include <libs/filesystem/src/path.cpp>
#include <libs/filesystem/src/operations.cpp>
#include <libs/filesystem/src/portability.cpp>
#include <libs/filesystem/src/windows_file_codecvt.cpp>
#endif

//datetime
#if BOOST_DATE_TIME_USE
#include <libs/date_time/src/gregorian/greg_names.hpp>
#include <libs/date_time/src/gregorian/date_generators.cpp>
#include <libs/date_time/src/gregorian/greg_month.cpp>
#include <libs/date_time/src/gregorian/greg_weekday.cpp>
#include <libs/date_time/src/gregorian/gregorian_types.cpp>
#endif

//thread
#if BOOST_THREAD_USE
#ifdef _MSC_VER
namespace boost{
    void tss_cleanup_implemented() {}
}
#include <libs/thread/src/win32/thread.cpp>
#include <libs/thread/src/win32/tss_dll.cpp>
#include <libs/thread/src/win32/tss_pe.cpp>
#else
#include <libs/thread/src/pthread/thread.cpp>
#include <libs/thread/src/pthread/once.cpp>
#endif
#endif

//python
#if BOOST_PYTHON_USE
#include<libs/python/src/numeric.cpp>
#include<libs/python/src/list.cpp>
#include<libs/python/src/long.cpp>
#include<libs/python/src/dict.cpp>
#include<libs/python/src/tuple.cpp>
#include<libs/python/src/str.cpp>
#include<libs/python/src/slice.cpp>

#include<libs/python/src/converter/from_python.cpp>
#include<libs/python/src/converter/registry.cpp>
#include<libs/python/src/converter/type_id.cpp>
#include<libs/python/src/object/enum.cpp>
#include<libs/python/src/object/class.cpp>
#include<libs/python/src/object/function.cpp>
#include<libs/python/src/object/inheritance.cpp>
#include<libs/python/src/object/life_support.cpp>
#include<libs/python/src/object/pickle_support.cpp>
#include<libs/python/src/errors.cpp>
#include<libs/python/src/module.cpp>
#include<libs/python/src/converter/builtin_converters.cpp>
#include<libs/python/src/converter/arg_to_python_base.cpp>
#include<libs/python/src/object/iterator.cpp>
#include<libs/python/src/object/stl_iterator.cpp>
#include<libs/python/src/object_protocol.cpp>
#include<libs/python/src/object_operators.cpp>
#include<libs/python/src/wrapper.cpp>
#include<libs/python/src/import.cpp>
#include<libs/python/src/exec.cpp>
#include<libs/python/src/object/function_doc_signature.cpp>
#endif
