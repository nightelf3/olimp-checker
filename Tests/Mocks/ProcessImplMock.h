#pragma once

#include "gmock/gmock.h"
#include "include/Interfaces/IProcessImpl.h"

struct ProcessImplMock : public IProcessImpl
{
	MOCK_METHOD(ProcessResponse, Run, (ProcessData data), (override));
};