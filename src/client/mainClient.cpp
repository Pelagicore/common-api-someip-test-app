#include "log.h"

#include "utilLib/MainLoopApplication.h"
#include "utilLib/GlibIO.h"

#include "TestService.h"
#include "test/TestInterfaceProxy.h"

#include "GLibCommonAPIFactory.h"

namespace test {

LOG_DECLARE_DEFAULT_CONTEXT(testServiceContext, "test", "test");

bool bStopStress = false;

class TestCaseRunner {

	typedef std::function<bool (size_t)> TestFunction;

public:
	TestCaseRunner(const TestFunction& testFunction) :
		m_testFunction(testFunction), m_thread(startTestFunction,
						       &m_testFunction) {
	}

	static void startTestFunction(const TestFunction* f) {
		int count = 0;
		while (true) {
			bool testReturnValue = (*f)( rand() );
			assert(testReturnValue == true);
			count++;
			usleep(1000);
		}
	}

	TestFunction m_testFunction;
	std::thread m_thread;

};

class TheApp : public SomeIP_utils::MainLoopApplication {

	LOG_DECLARE_CLASS_CONTEXT("MAIN", "Main context");

public:
	TheApp() :
		SomeIP_utils::MainLoopApplication(), m_timer([&]() {
								     onCyclic();
							     }, 800) {
	}

	void callTestMethodBlocking() {
		CommonAPI::CallStatus callStatus;
		int32_t val1;
		int32_t val2;
		getProxy().TestMethod(0x01234567, "My String", callStatus, val1, val2);
		log_info("Result from method : callStatus: %i, val1:%X, val2:%X",
			 static_cast<int>(callStatus), val1, val2);
	}

	class MyVisitor {
public:
		template<typename _Type>
		void operator()(const _Type&) const {
			log_verbose("%s", __PRETTY_FUNCTION__);
		}

		void operator()(const int16_t& v) const {
			log_verbose() << "Received uint16_t : " << v;
		}

		void operator()(const std::string& v) const {
			log_verbose() << "Received string: " << v;
		}

	};

	void startTests() {
		using namespace std::placeholders;
		//		new TestCaseRunner(std::bind(&TheApp::testInt, this, _1));
		//		new TestCaseRunner(std::bind(&TheApp::testStruct, this, _1));
		//		new TestCaseRunner(std::bind(&TheApp::testEnum, this, _1));
		//		new TestCaseRunner(std::bind(&TheApp::testString, this, _1));
		//		new TestCaseRunner(std::bind(&TheApp::testArrayOfStruct, this, _1));
		//		new TestCaseRunner(std::bind(&TheApp::testUnion, this, _1));
	}

	void init() {

		proxy = m_commonAPIGlibFactory.buildProxy<test::TestInterfaceProxy>();

		getProxy().getProxyStatusEvent().subscribe(
			[&](const CommonAPI::AvailabilityStatus & availability) {
				if ( getProxy().isAvailable() ) {
					log_info() << "Proxy is available";
					startTests();
				} else {
					log_info() << "Proxy is NOT available";
				}
			});

		getProxy().getMyBroadcastEvent().subscribe(
			[&](const std::string & value) {
				static int i;
				if (++i % 10000 == 0)
					log_info() << i << " notifications received";
			});

		getProxy().getMyBroadcastEvent().subscribe(
			[&](const std::string & value) {
				static int i;
				if (++i % 10000 == 0)
					log_info() << i << " notifications received";
			});

		getProxy().getMyAttributeAttribute().getChangedEvent().subscribe(
			[&](const uint32_t &value) {
				log_info() << "Notification received MyAttribute : " << value;

				CommonAPI::CallStatus callStatus;
				int32_t value2;
				getProxy().getMyAttributeAttribute().
				getValue(
					callStatus, value2);
				log_info() << "MyAttribute value: " << value2;
			});

		getProxy().getBroadcastWithStructEvent().subscribe(
			[&](const TestInterface::MyStruct & value) {
				log_verbose(
					"Notification received int:%i string:%s bool:%i",
					value.intField, value.stringField.c_str(),
					value.boolField);
			});

		getProxy().getMyBroadcastWithUnionEvent().subscribe(
			[&](const TestInterface::MyUnion & value) {
				MyVisitor myVisitor;
				CommonAPI::ApplyVoidVisitor<MyVisitor,
							    TestInterface::MyUnion,
							    int16_t,
							    std::string>::visit(
					myVisitor,
					value);
			});

		TestInterface::ArrayOfStruct inputArray;
		TestInterface::MyStruct element;
		for (size_t i = 0; i < 2; i++) {
			element.intField = i;
			element.boolField = 1;
			element.stringField = "FSFSF";
			element.enumField = TestInterface::OtherEnum::VALUE_2;
			element.subStructField.intField = 54;
			element.subStructField.stringField = "fdsfs";
			inputArray.push_back(element);
		}

	}

	void onCyclic() {
		CommonAPI::CallStatus callStatus;
		int32_t val1;
		int32_t val2;
		getProxy().TestMethod(0x3456, "My String", callStatus, val1, val2);
		log_debug() << "Result from method : " << val1 << val2;
	}

	size_t random(int max) {
		size_t value = rand();
		return value;
	}

	std::string buildTestString(size_t randomNumber) {
		std::string result = "Test String";
		for (size_t i = 0; i < (randomNumber % 30); i++)
			result += "TestString ";

		return result;
	}

	bool testInt(size_t randomNumber) {
		int32_t inputValue = randomNumber;
		int32_t outputValue;
		CommonAPI::CallStatus callStatus;
		getProxy().TakeIntReturnInt(inputValue, callStatus, outputValue);

		int32_t referenceOutputValue;
		m_localService.TakeIntReturnInt(inputValue, referenceOutputValue);

		return (outputValue == referenceOutputValue);
	}

	bool testString(size_t randomNumber) {
		std::string inputValue = "TestString ";
		uint8_t count = rand();
		for (size_t i = 0; i < count; i++)
			inputValue += "TestString ";

		std::string outputValue;
		CommonAPI::CallStatus callStatus;
		getProxy().TakeStringReturnString(inputValue, callStatus, outputValue);

		std::string referenceOutputValue;
		m_localService.TakeStringReturnString(inputValue, referenceOutputValue);

		return (outputValue == referenceOutputValue);
	}

	bool testEnum(size_t randomNumber) {

		typedef TestInterface::OtherEnum Type;

		Type inputValue = static_cast<Type>(randomNumber);

		Type outputValue;
		CommonAPI::CallStatus callStatus;
		getProxy().takeEnumReturnEnum(inputValue, callStatus, outputValue);

		Type referenceOutputValue;
		m_localService.takeEnumReturnEnum(inputValue, referenceOutputValue);

		return (outputValue == referenceOutputValue);
	}

	bool testUnion(size_t randomNumber) {

		typedef TestInterface::MyUnion Type;

		Type inputValue;

		switch (randomNumber % 2) {

		case 0 : {
			int16_t v = randomNumber;
			inputValue = v;
		}
		break;

		case 1 : {
			auto v = buildTestString(randomNumber);
			inputValue = v;
		}
		break;

		default :
			assert(false);
			break;

		}

		Type outputValue;
		CommonAPI::CallStatus callStatus;
		getProxy().takeUnionReturnUnion(inputValue, callStatus, outputValue);

		Type referenceOutputValue;
		m_localService.takeUnionReturnUnion(inputValue, referenceOutputValue);

		return (outputValue == referenceOutputValue);
	}

	bool testStruct(size_t randomNumber) {

		typedef TestInterface::MyStruct Type;

		Type inputValue;
		inputValue.boolField = ( (randomNumber % 2) == 0 );
		inputValue.enumField = TestInterface::OtherEnum::VALUE_2;
		inputValue.intField = randomNumber;
		inputValue.stringField = buildTestString(randomNumber);

		int16_t v = 25674;
		inputValue.unionField = v;
		if (inputValue.boolField)
			inputValue.unionField = std::string("fdsfsf");

		inputValue.subStructField.intField = randomNumber * 2;
		inputValue.subStructField.stringField = buildTestString(randomNumber);

		Type outputValue;
		CommonAPI::CallStatus callStatus;
		getProxy().takeStructReturnStruct(inputValue, callStatus, outputValue);

		Type referenceOutputValue;
		m_localService.takeStructReturnStruct(inputValue, referenceOutputValue);

		return (outputValue == referenceOutputValue);
	}

	void stressWithSyncRequests() {
		int counter = 0;
		while (!bStopStress) {
			if ( getProxy().isAvailable() ) {

				for (int i = 0; i < 100; i++) {
					CommonAPI::CallStatus callStatus;
					getProxy().triggerBroadcast(callStatus);
					if (++counter % 10000 == 0)
						log_info("stressWithSyncRequests running");
				}

				usleep(100000);
			}
		}
	}

	TestInterfaceProxyBase& getProxy() {
		return *proxy;
	}

	SomeIP_utils::GLibTimer m_timer;
	TestServiceImplementation m_localService;

	GLibCommonAPIFactory m_commonAPIGlibFactory;

	std::shared_ptr<TestInterfaceProxyBase> proxy;

};

}

int main(int argc, char* argv[]) {

	test::TheApp app;
	app.init();
	app.run();

	return 0;
}
