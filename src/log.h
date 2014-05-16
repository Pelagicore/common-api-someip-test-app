#pragma once

#include "plog.h"
#include "log-console.h"

#ifdef ENABLE_DLT_LOGGING
#include "log-dlt.h"
#endif

namespace test {

typedef logging::LogContextT<
	logging::TypeSet<logging::ConsoleLogContext
#ifdef ENABLE_DLT_LOGGING
			 , logging::DltContextClass
#endif
			 >, logging::TypeSet<logging::ConsoleLogData
#ifdef ENABLE_DLT_LOGGING
					     , logging::DltContextClass::LogDataType
#endif
					     > > LogContext;

}
