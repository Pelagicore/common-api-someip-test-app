#include "log.h"

#include "GLibCommonAPIFactory.h"

#include "TestService.h"
#include "utilLib/MainLoopApplication.h"
#include "utilLib/GlibIO.h"

namespace test {

LOG_DECLARE_DEFAULT_CONTEXT(testServiceContext, "LOG", "Default");

using namespace SomeIP_utils;

class MyService : public TestServiceImplementation {

	static const int NOTIFICATION_CYCLE_DURATION = 5000;

public:
	MyService() :
		m_timer([&]() {
				triggerBroadcast();
				onCyclic();
			}, NOTIFICATION_CYCLE_DURATION) {
	}

	void onCyclic() {
		m_enumAttribute =
			getMyAttributeAttribute() % 2 ?
			test::TestInterface::OtherEnum::VALUE_3 :
			test::TestInterface::OtherEnum::VALUE_2;
		fireMyEnumAttributeAttributeChangedNotification();
	}

	GLibTimer m_timer;

};

class TheApp : public MainLoopApplication {

	LOG_SET_CLASS_CONTEXT(testServiceContext);

public:
	TheApp() :
		MainLoopApplication(), m_timer([&]() {
						       onCyclic();
					       }, 1000), m_factory() {
	}

	void init() {

		m_service = std::make_shared<MyService>();

		bool isStubRegistrationSuccessful = m_factory.registerService(
			m_service);

		if (!isStubRegistrationSuccessful) {
			log_error() << "Error: Unable to register service!";
			throw std::exception();
		}

		log_debug() << "Service registration successful !";

	}

	void onCyclic() {
//		log_verbose("onCyclic");
		return;
	}

	std::shared_ptr<MyService> m_service;
	GLibTimer m_timer;

	GLibCommonAPIFactory m_factory;

};

}

int main(int argc, char* argv[]) {

	test::TheApp app;
	app.init();
	app.run();

	return 0;
}
