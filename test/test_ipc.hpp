#include <iostream>

#include <auss.hpp>

#include "ipc-util.hpp"

#include <cxxtest/TestSuite.h>

class testsuite_ipc_util : public CxxTest::TestSuite {
public:
	void test_pack() {
		{
			using namespace i3ipc;
			auto  buff = i3_pack(ClientMessageType::COMMAND, "exit");
			auss_t  auss;
			auss << std::hex;
			for (uint32_t  i = 0; i < buff->size; i++) {
				if (buff->data[i] < 0x10) {
					auss << '0';
				}
				auss << static_cast<uint32_t>(buff->data[i]) << ' ';
			}
			std::string  str = auss;
			str.pop_back();
			TS_ASSERT_EQUALS(str, "69 33 2d 69 70 63 04 00 00 00 00 00 00 00 65 78 69 74")
		}
	}
};
