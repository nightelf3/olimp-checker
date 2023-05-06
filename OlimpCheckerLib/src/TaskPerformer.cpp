#include "stdafx.h"
#include "include/TaskPerformer.h"
#include "include/Process.h"
#include "include/Request.h"
#include "tomykaira/Base64.hpp"

TaskPerformer::TaskPerformer(const Json::Value& task)
{
	m_QueueId = task.get("queue_id", 0).asInt();
	m_UseFiles = task.get("use_files", false).asBool();
	m_InputFile = task.get("input_file", "input.dat").asString();
	m_OutputFile = task.get("output_file", "output.rez").asString();
	m_TimeLimit = task.get("time_limit", 0).asInt();
	m_MemoryLimit = task.get("memory_limit", 0).asInt();

	const Json::Value tests = task.get("tests", {});
	if (tests.isArray())
	{
		const Json::ArrayIndex count = tests.size();
		m_Tests.resize(count);
		for (Json::ArrayIndex i = 0; i < count; i++)
		{
			if (const Json::Value val = tests.get(i, {}))
			{
				if (const std::string input = val.get("input", "").asString(); !input.empty())
					macaron::Base64::Decode(input, m_Tests[i].first);
				if (const std::string output = val.get("output", "").asString(); !output.empty())
					macaron::Base64::Decode(output, m_Tests[i].second);
			}
		}
	}

	std::string text;
	if (const std::string error = macaron::Base64::Decode(task.get("text", "").asString(), text); error.empty())
	{
		const std::string extension = task.get("extension", "").asString();
		std::filesystem::path path = Compiler::MakeFilePath(task.get("username", "").asString(), task.get("task_id", "").asString(), m_QueueId, extension);
		m_Compiler = std::make_unique<Compiler>(std::move(text), std::move(path), Compiler::MakeImplFromExtension(extension));
	}
}

void TaskPerformer::Run(Request request)
{
	request.Post()["queue_id"] = std::to_string(m_QueueId);

	if (!m_Compiler)
	{
		// return task back to the queue
		std::cerr << "Compiler is not valid" << std::endl;
		return ChangeState(request, TaskState::NoAction);
	}

	ChangeState(request, TaskState::Compiling);
	if (!m_Compiler->Run())
	{
		if (std::string error = m_Compiler->Error(); !error.empty())
		{
			// add compilation error to the message
			request.Post()["message"] = macaron::Base64::Encode(error);
			return ChangeState(request, TaskState::CompilingError);
		}
		else
		{
			// return task back to the queue
			std::cerr << "Internal compilation error" << std::endl;
			return ChangeState(request, TaskState::NoAction);
		}
	}

	ChangeState(request, TaskState::InProgress);

	size_t nTest = 1;
	std::vector<std::pair<size_t, TaskState>> testStates;
	testStates.reserve(m_Tests.size());

	for (auto& [input, output] : m_Tests)
	{
		Process process;
		process.Input(input);
		process.TimeLimit(m_TimeLimit);
		process.MemoryLimit(m_MemoryLimit);

		TaskState state = TaskState::Succeed;
		if (process.Run(m_Compiler->ExecutablePath(), m_Compiler->ExecutableParams()))
		{
			if (!input.empty() && process.Output().empty())
			{
				ChangeState(request, TaskState::InvalidOutputStream);
				return;
			}

			std::istringstream expected{ output };
			std::istringstream data{ process.MoveOutput() };
			while (expected)
			{
				std::string value1, value2;
				expected >> value1;
				data >> value2;
				if (value1 != value2)
				{
					state = TaskState::ResponseError;
					break;
				}
			}
		}
		else
		{
			switch (process.Code())
			{
			case ProcessCode::TimeLimit:
				state = TaskState::OverTime;
				break;
			case ProcessCode::MemoryLimit:
				state = TaskState::OverMemory;
				break;
			default:
			case ProcessCode::Failed:
				std::cerr << "WARNING: unknown process failing on test #" << nTest << " for: " << m_Compiler->ExecutablePath().string() << std::endl;
			case ProcessCode::RuntimeError:
				state = TaskState::RuntimeError;
				break;
			}
		}

		if (state != TaskState::Succeed)
			testStates.emplace_back(nTest, state);

		nTest++;
	}

	if (testStates.empty())
	{
		ChangeState(request, TaskState::Succeed);
	}
	else
	{
		std::ostringstream tests;
		std::ostringstream states;
		tests << testStates[0].first;
		states << (int)testStates[0].second;
		for (size_t i = 1; i < testStates.size(); i++)
		{
			tests << "," << testStates[i].first;
			states << "," << (int)testStates[i].second;
		}
		request.Post()["states"] = states.str();
		request.Post()["message"] = tests.str();
		request.Perform();
	}
}

void TaskPerformer::ChangeState(Request& request, TaskState state)
{
	request.Post()["states"] = std::to_string((int)state);
	request.Perform();
}
