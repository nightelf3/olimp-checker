#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "include/Process.h"
#include "Utils.hpp"

using namespace ::testing;

TEST(TestProcess, CustomInput)
{
	Process process;
	process.Input("echo Trololo && exit");
	EXPECT_TRUE(process.Run({ "C:\\Windows\\System32\\cmd.exe" }));
	EXPECT_THAT(process.MoveOutput(), HasSubstr("Trololo"));
}

TEST(TestProcess, CustomParams)
{
	Process process;
	EXPECT_TRUE(process.Run({ { "C:\\Windows\\System32\\ping.exe" }, "-n 1 127.0.0.1" }));
	EXPECT_THAT(process.MoveOutput(), HasSubstr("127.0.0.1:"));
}

TEST(TestProcess, SysRestirctions)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "SysRestirctions.cpp";
	std::string code = R"---(
		#include <windows.h>
		int main() { ExitWindowsEx(EWX_LOGOFF, NULL); return 0; }
	)---";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.SysRestrictions(true);
	EXPECT_TRUE(process.Run(exePath));
}

TEST(TestProcess, CppFileInput)
{
	const std::string kInput = "Trololo\nLine2";
	const FileGuard inFilePath = std::filesystem::temp_directory_path() / "CppFileInput.dat";
	const FileGuard outFilePath = std::filesystem::temp_directory_path() / "CppFileInput.rez";
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CppFileInput.cpp";
	std::string code = R"---(
		#include <fstream>
		int main()
		{
			std::ifstream in("CppFileInput.dat");
			std::ofstream out("CppFileInput.rez");
			std::string line;
			while (std::getline(in, line))
				out << line << std::endl;
			return 0;
		}
	)---";
	FileGuard exePath = CompileCode(std::move(code), srcPath);
	exePath.PreventDeletion();
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	EXPECT_FALSE(inFilePath.Exists());
	EXPECT_FALSE(outFilePath.Exists());

	Process process;
	process.ReadWriteFiles(inFilePath, outFilePath);
	process.Input(kInput);
	EXPECT_TRUE(process.Run(exePath));
	EXPECT_THAT(process.MoveOutput(), HasSubstr(kInput));

	EXPECT_TRUE(inFilePath.Exists());
	EXPECT_TRUE(outFilePath.Exists());
}

TEST(TestProcess, CppTimeLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CppTimeLimit.cpp";
	std::string code = "int main() { for (volatile unsigned i = 999999; i >= 0; i--); return 0; }";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000);
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::TimeLimit);
}

TEST(TestProcess, CppMemoryLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CppMemoryLimit.cpp";
	std::string code = "int main() { volatile long long* mem = new long long[1000000]; return 0; }";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000); // just in case
	process.MemoryLimit(2);
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::MemoryLimit);
}

TEST(TestProcess, CppRuntimeError)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CppRuntimeError.cpp";
	std::string code = "int main() { throw 0; return 0; }";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::RuntimeError);
}

TEST(TestProcess, PasFileInput)
{
	const std::string kInput = "Trololo\nLine2";
	const FileGuard inFilePath = std::filesystem::temp_directory_path() / "PasFileInput.dat";
	const FileGuard outFilePath = std::filesystem::temp_directory_path() / "PasFileInput.rez";
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PasFileInput.pas";
	std::string code = R"---(
		Var S : String;
				F : TextFile;
				O : TextFile;
		begin
			Assign (F,'PasFileInput.dat');
			Assign (O,'PasFileInput.rez');
			Reset (F);
			Rewrite(O);
			While not Eof(f) do
			Begin
				Readln(F,S);
				Writeln(O, S);
			end;
			Close (O);
			Close (F);
		end.
	)---";
	FileGuard exePath = CompileCode(std::move(code), srcPath);
	exePath.PreventDeletion();
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	EXPECT_FALSE(inFilePath.Exists());
	EXPECT_FALSE(outFilePath.Exists());

	Process process;
	process.ReadWriteFiles(inFilePath, outFilePath);
	process.Input(kInput);
	EXPECT_TRUE(process.Run(exePath));
	EXPECT_THAT(process.MoveOutput(), HasSubstr(kInput));

	EXPECT_TRUE(inFilePath.Exists());
	EXPECT_TRUE(outFilePath.Exists());
}

TEST(TestProcess, PasTimeLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PasTimeLimit.pas";
	std::string code = R"---(
		begin
			While True do
				Begin
				end;
		end.
	)---";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000);
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::TimeLimit);
}

TEST(TestProcess, PasMemoryLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PasMemoryLimit.pas";
	std::string code = R"---(
		var sieve: array of longWord;
		begin
			setLength(sieve, 1000000);
		end.
	)---";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000); // just in case
	process.MemoryLimit(2);
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::MemoryLimit);
}

TEST(TestProcess, PasRuntimeError)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PasRuntimeError.pas";
	std::string code = R"---(
		var number, zero : Integer;
		begin
			number := 1 div zero;
		end.
	)---";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::RuntimeError);
}

TEST(TestProcess, PyFileInput)
{
	const std::string kInput = "Trololo\nLine2";
	const FileGuard inFilePath = std::filesystem::temp_directory_path() / "PyFileInput.dat";
	const FileGuard outFilePath = std::filesystem::temp_directory_path() / "PyFileInput.rez";
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PyFileInput.py";
	std::string code = R"---(fr = open("PyFileInput.dat", "r")
fw = open("PyFileInput.rez", "w")
fw.write(fr.read()))---";
	FileGuard exePath = CompileCode(std::move(code), srcPath);
	exePath.PreventDeletion();
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	EXPECT_FALSE(inFilePath.Exists());
	EXPECT_FALSE(outFilePath.Exists());

	Process process;
	process.ReadWriteFiles(inFilePath, outFilePath);
	process.Input(kInput);
	EXPECT_TRUE(process.Run(exePath));
	EXPECT_THAT(process.MoveOutput(), HasSubstr(kInput));

	EXPECT_TRUE(inFilePath.Exists());
	EXPECT_TRUE(outFilePath.Exists());
}

TEST(TestProcess, PyTimeLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PyTimeLimit.py";
	std::string code = R"---(
x = 0
while True:
    x += 1
	)---";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000);
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::TimeLimit);
}

TEST(TestProcess, PyMemoryLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PyMemoryLimit.py";
	std::string code = "new_list1M = [0] * 1000000";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000); // just in case
	process.MemoryLimit(5);
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::MemoryLimit);
}

TEST(TestProcess, PyRuntimeError)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PyRuntimeError.py";
	std::string code = "print(\"Hello World!\");sd";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::RuntimeError);
}

TEST(TestProcess, CSharpFileInput)
{
	const std::string kInput = "Trololo\nLine2";
	const FileGuard inFilePath = std::filesystem::temp_directory_path() / "CSharpFileInput.dat";
	const FileGuard outFilePath = std::filesystem::temp_directory_path() / "CSharpFileInput.rez";
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CSharpFileInput.cs";
	std::string code = R"---(
		using System;
		using System.IO;
		public class HelloWorld
		{
			public static void Main(string[] args)
			{
				string[] lines = File.ReadAllLines("CSharpFileInput.dat");
				using (StreamWriter outputFile = new StreamWriter("CSharpFileInput.rez"))
				{
					foreach (string line in lines)
						outputFile.WriteLine(line);
				}
			}
		}
	)---";
	FileGuard exePath = CompileCode(std::move(code), srcPath);
	exePath.PreventDeletion();
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	EXPECT_FALSE(inFilePath.Exists());
	EXPECT_FALSE(outFilePath.Exists());

	Process process;
	process.ReadWriteFiles(inFilePath, outFilePath);
	process.Input(kInput);
	EXPECT_TRUE(process.Run(exePath));
	EXPECT_THAT(process.MoveOutput(), HasSubstr(kInput));

	EXPECT_TRUE(inFilePath.Exists());
	EXPECT_TRUE(outFilePath.Exists());
}

TEST(TestProcess, CSharpTimeLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CSharpTimeLimit.cs";
	std::string code = R"---(
		public class HelloWorld
		{
			public static void Main(string[] args)
			{
				int a = 0;
				while (true)
					a++;
			}
		}
	)---";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000);
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::TimeLimit);
}

TEST(TestProcess, CSharpMemoryLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CSharpMemoryLimit.cs";
	std::string code = R"---(
		public class HelloWorld
		{
			public static void Main(string[] args)
			{
				byte[] buffer = new byte[1024*1024*10];
			}
		}
	)---";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000); // just in case
	process.MemoryLimit(5);
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::MemoryLimit);
}

TEST(TestProcess, CSharpRuntimeError)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CSharpRuntimeError.cs";
	std::string code = R"---(
		using System;
		public class HelloWorld
		{
			public static void Main(string[] args)
			{
				throw new InvalidOperationException("Test Exception");
			}
		}
	)---";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::RuntimeError);
}

TEST(TestProcess, JavaFileInput)
{
	const std::string kInput = "Trololo\nLine2";
	const FileGuard inFilePath = std::filesystem::temp_directory_path() / "JavaFileInput.dat";
	const FileGuard outFilePath = std::filesystem::temp_directory_path() / "JavaFileInput.rez";
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "JavaFileInput.java";
	std::string code = R"---(
		import java.io.*;
		class Program {
			public static void main(String [] args) {
				try {
					String fileName = "JavaFileInput.dat";
					String fileOutput = "JavaFileInput.rez";

					FileInputStream inputStream = new FileInputStream(fileName);
					FileWriter fileWriter = new FileWriter(fileOutput);
					BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

					byte[] buffer = new byte[1000];
					while(inputStream.read(buffer) != -1) {
						bufferedWriter.write(new String(buffer));
						bufferedWriter.newLine();
					}

					inputStream.close();
					bufferedWriter.close();
				}
				catch(FileNotFoundException ex) {
				}
				catch(IOException ex) {
				}
			}
		}
	)---";
	FileGuard exePath = CompileCode(std::move(code), srcPath);
	exePath.PreventDeletion();
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	EXPECT_FALSE(inFilePath.Exists());
	EXPECT_FALSE(outFilePath.Exists());

	Process process;
	process.ReadWriteFiles(inFilePath, outFilePath);
	process.Input(kInput);
	EXPECT_TRUE(process.Run(exePath));
	EXPECT_THAT(process.MoveOutput(), HasSubstr(kInput));

	EXPECT_TRUE(inFilePath.Exists());
	EXPECT_TRUE(outFilePath.Exists());
}

TEST(TestProcess, JavaTimeLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "JavaTimeLimit.java";
	std::string code = "class Program { public static void main(String [] args) { while (true); } }";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000);
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::TimeLimit);
}

TEST(TestProcess, JavaMemoryLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "JavaMemoryLimit.java";
	std::string code = "class Program { public static void main(String [] args) { byte[] buffer = new byte[1024*1024*10]; } }";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000); // just in case
	process.MemoryLimit(5);
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::MemoryLimit);
}

TEST(TestProcess, JavaRuntimeError)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "JavaRuntimeError.java";
	std::string code = "class Program { public static void main(String [] args) { int i = 0; int k = 100 / i; } }";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	EXPECT_FALSE(process.Run(exeParams));
	EXPECT_EQ(process.Code(), ProcessCode::RuntimeError);
}