#pragma once

#include "log.h"
#include "test/TestInterfaceServiceAbstract.h"

namespace test {

class TestServiceImplementation : public TestInterfaceServiceAbstract {

	LOG_DECLARE_CLASS_CONTEXT("Test", "TestServiceImplementation");

public:
	TestServiceImplementation() {
	}

	void takeArrayOfStructReturnArrayOfStruct(test::TestInterface::ArrayOfStruct input,
						  test::TestInterface::ArrayOfStruct& output) {
		//	override

		log_verbose() << "methodWithArrayOfStruct with array length : " << input.size();

		test::TestInterface::MyStruct s;
		s.intField = 0xAABBCCDD;
		s.stringField = "ABDC";
		s.boolField = true;
		s.enumField = test::TestInterface::OtherEnum::VALUE_3;
		s.subStructField.intField = 543;
		s.subStructField.stringField = "subStructField.stringField";

		int16_t v = 0x3456;
		s.unionField = v;

		if (input.size() > 0)
			s.unionField = input[0].unionField;

		output.push_back(s);
		output.push_back(s);
	}

	void TakeIntReturnIntWait(int32_t input, TestTypes::TestError& methodError, int32_t& output) override {
		log_info("sleeping 5 seconds");
		sleep(5);
		TakeIntReturnInt(input, output);
	}

	void TakeIntReturnInt(int32_t input, int32_t& output) override {
		output = input + 5;
	}

	void TakeStringReturnString(std::string input, std::string& output) override {
		output = input;
		output += " / Answer string";
	}

	void TakeBoolReturnBool(bool input, bool& output) override {
		output = !input;
	}

	void takeUnionReturnUnion(TestInterface::MyUnion input, TestInterface::MyUnion& output) override {
		output = input;
	}

	void takeStructReturnStruct(TestInterface::MyStruct input, TestInterface::MyStruct& output) override {
		output = input;
	}

	void takeEnumReturnEnum(TestInterface::OtherEnum input, TestInterface::OtherEnum& output) override {
		output = input;
	}

	void TestMethod(int32_t input, std::string stringParam, int32_t& val1, int32_t& val2) override {
		//		log_debug("TestMethod with params : 0x%X, string : %s", input, stringParam.c_str());
		val1 = 0x01234567;
		val2 = 0x89ABCDEF;
	}

	void OtherTestMethod(std::string& greeting, int32_t& identifier) override {
		log_info("OtherTestMethod()");
		//		fireMyBroadcastEvent("ABCDEFGHI");
	}

	static constexpr const char* LONG_STRING =
		"This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. This is a long string. ";

	void triggerBroadcast() override {
		//		log_debug("triggerBroadcast()");
		fireMyBroadcastEvent(LONG_STRING);
		setMyAttributeAttribute(getMyAttributeAttribute() + 1);

		TestInterface::MyStruct myStruct;
		myStruct.intField = getMyAttributeAttribute();
		myStruct.stringField = "The value of the string is ";
		//		myStruct.stringField + = getMyAttributeAttribute();
		myStruct.boolField = ( (getMyAttributeAttribute() % 2) == 0 );
		myStruct.enumField = TestInterface::OtherEnum::VALUE_2;
		myStruct.subStructField.intField = 543;
		myStruct.subStructField.stringField = "This is the sub-struct string";
		fireBroadcastWithStructEvent(myStruct);

		TestInterface::MyUnion param;
		if (getMyAttributeAttribute() % 2 == 0) {
			int16_t paramValue = getMyAttributeAttribute();
			param = paramValue;
			fireMyBroadcastWithUnionEvent(param);
		} else {
			std::string stringValue = "Now, this is a string";
			param = stringValue;
			fireMyBroadcastWithUnionEvent(param);
		}

		m_myStructAttribute.unionField = param;

		m_myStructAttribute.enumField =
			getMyAttributeAttribute() %
			2 ? test::TestInterface::OtherEnum::VALUE_3 : test::TestInterface::OtherEnum::VALUE_2;

		log_debug("fireMyStructAttributeAttributeChangedNotification");
		m_myStructAttribute.stringField += "r";

		if (m_myStructAttribute.stringField.length() > 20) {
			m_arrayOfStructAttribute.resize(0);
			m_myStructAttribute.stringField = "";
		} else {
			TestInterface::MyStruct arrayElement;
			arrayElement.stringField = "Item in array";
			arrayElement.intField = 3432;
			m_arrayOfStructAttribute.push_back(arrayElement);
		}

		// This call kicks us out from DBUS, for some unknown reason
		//		fireMyArrayOfStructAttributeChangedNotification();

		m_myStructAttribute.boolField = !m_myStructAttribute.boolField;
		m_myStructAttribute.intField++;

		auto& s = m_myStructAttribute.subStructField.stringField;
		if (s.size() == 0)
			s = "This is the sub-struct string";
		s = s.substr(0, s.size() - 1);

		fireMyStructAttributeAttributeChangedNotification();
	}

	void methodWithUnion(TestInterface::MyUnion inParam, TestInterface::MyUnion& outParam) override {
		log_info( "methodWithUnion input type:%i", inParam.getValueType() );
		std::string s = "This is a string sent as variant";
		outParam = s;
	}

	void methodWithStruct(test::TestInterface::MyStruct input, test::TestInterface::MyStruct& output) override {
		log_info("methodWithStruct with values : int:0x%X, string: %s, bool:%i", input.intField, input.stringField.c_str(),
			 input.boolField);

		output.intField = 0xAABBCCDD;
		output.stringField = "ABDC";
		output.boolField = true;
	}

	void setMyAttributeAttribute(int32_t value) override {
		m_myAttribute = value;
		fireMyAttributeAttributeChangedNotification();
	}

	const int32_t& getMyAttributeAttribute() override {
		return m_myAttribute;
	}

	void setMyStructAttributeAttribute(TestInterface::MyStruct value) override {
		//		m_myAttribute = value;
		//		fireMyAttributeAttributeChangedNotification();
	}

	const TestInterface::MyStruct& getMyStructAttributeAttribute() override {
		return m_myStructAttribute;
	}

	const TestInterface::OtherEnum& getMyEnumAttributeAttribute() override {
		return m_enumAttribute;
	}

	void setMyEnumAttributeAttribute(TestInterface::OtherEnum value) override {
	}

	void methodWithMap(TestInterface::MyMap param) override {
	}

	void setMyArrayOfStructAttribute(TestInterface::ArrayOfStruct array) {
		//	override
	}

	void setMyArrayOfIntAttribute(TestTypes::ArrayOfInt array) override {
	}

	const TestInterface::ArrayOfStruct& getMyArrayOfStructAttribute() {
		return m_arrayOfStructAttribute;
	}

	const TestTypes::ArrayOfInt& getMyArrayOfIntAttribute() {
		return m_arrayOfIntAttribute;
	}

protected:
	int32_t m_myAttribute = 0;
	TestInterface::MyStruct m_myStructAttribute;
	TestInterface::OtherEnum m_enumAttribute = TestInterface::OtherEnum::VALUE_1;
	TestInterface::ArrayOfStruct m_arrayOfStructAttribute;
	TestTypes::ArrayOfInt m_arrayOfIntAttribute;

};

}
