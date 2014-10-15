#pragma once

#include "ivi-logging.h"
#include "ivi-logging-console.h"

#ifdef ENABLE_DLT_LOGGING
#include "ivi-logging-dlt.h"
#endif

namespace someip {
namespace test {

#ifdef DISABLE_LOGGING

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

#else

typedef logging::LogContextT<
	logging::TypeSet<>,
	logging::TypeSet<> > LogContext;

#endif

}
}
