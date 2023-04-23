#include "stdafx.h"
#include "include/TaskPerformer.h"
#include "include/Request.h"
#include "tomykaira/Base64.hpp"

TaskPerformer::TaskPerformer(const Json::Value& task)
{
	m_QueueId = task.get("queue_id", 0).asInt();
	m_UseFiles = task.get("use_files", false).asBool();
	m_InputFile = task.get("input_file", "input.dat").asString();
	m_OutputFile = task.get("output_file", "output.rez").asString();
	m_Extension = task.get("extensions", "").asString();
	m_TimeLimit = task.get("time_limit", 0).asInt();
	m_MemoryLimit = task.get("memory_limit", 0).asInt();
	macaron::Base64::Decode(task.get("text", "").asString(), m_Text);

	const Json::Value tests = task.get("tests", {});
	if (tests.isArray())
	{
		const size_t count = tests.size();
		m_Tests.resize(count);
		for (size_t i = 0; i < count; i++)
		{
			if (const Json::Value val = tests.get(i, {}))
			{
				if (const std::string input = val.get("input", "").asString(); !input.empty())
					macaron::Base64::Decode(input, m_Tests[i].first);
				if (const std::string output = val.get("input", "").asString(); !output.empty())
					macaron::Base64::Decode(output, m_Tests[i].second);
			}
		}
	}
}

void TaskPerformer::Run(Request request)
{
	request.Post()["queue_id"] = std::to_string(m_QueueId);
	request.Post()["state"] = "0";
	request.Perform();
}
