#include <io.h>
#include <iostream>
#include <vector>
#include <ranges>
#include <random>
#include <chrono>
#include <Windows.h>

#ifdef _UNICODE
#define string wstring
#define cout wcout
#define cin wcin
#define _findfirst _wfindfirst
#define _findnext _wfindnext
#define _finddata_t _wfinddata_t
#define rename _wrename
#endif

std::vector<std::string> getFileNames(std::string path = TEXT(".\\"));
std::vector<std::string> filterNeededFiles(std::vector<std::string>& input, const std::string& fileTypeNeeded);
bool RenameFiles(std::vector<std::string> source, std::string path = TEXT(".\\"));

HANDLE consoleHWnd;

int main(int argc,char** argv)
{
	consoleHWnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	std::string neededFileType;
	std::string filePath;
	if (argc <= 1) {
		std::cout << "Enter file type you need to rename:";
		std::cin >> neededFileType;
		std::cout << "Enter the path of files:";
		std::cin >> filePath;
	}
	if (neededFileType.empty()) {
		return 0;
	}
	else if (*neededFileType.begin() != '.') {
		neededFileType = TEXT('.') + neededFileType;
	}
	if (filePath.size() == 0) {
		filePath = TEXT(".\\");
	}
	else if (*(filePath.end() - 1) != TEXT('\\')) {
		filePath += TEXT('\\');
	}
	SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN);
    std::cout << "[INFO]Reading file names....\n" << std::endl;
	std::vector<std::string> fileNames = getFileNames(filePath);
	for (auto& name : fileNames) {
		std::cout << name << std::endl;
	}
	SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN);
	std::cout << "[INFO]Done\n" << "[INFO]Filter needed files\n" << std::endl;
	fileNames = filterNeededFiles(fileNames, neededFileType);
	for (auto& name : fileNames) {
		std::cout << name << std::endl;
	}
	SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN);
	std::cout << "[INFO]Done\n" << "[INFO]Start rename\n" << std::endl;
	RenameFiles(fileNames, filePath);
	SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN);
	std::cout << "[INFO]Done\n" << std::endl;
	SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

}

std::vector<std::string> getFileNames(std::string path) {
	std::vector<std::string> files;
	intptr_t hFile = 0;
	struct _finddata_t fileInfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append(TEXT("\\*")).c_str(), &fileInfo)) != -1)
	{
		do
		{
			if (!(fileInfo.attrib & _A_SUBDIR))
			{
				files.push_back(
					fileInfo.name
					//p
					//.assign(path)
					//.append("\\")
					//.append(fileInfo.name)
				);
			}
		} while (_findnext(hFile, &fileInfo) == 0);//read all file name
		_findclose(hFile);
	}
	return files;
}

std::vector<std::string> filterNeededFiles(std::vector<std::string>& input, const std::string& fileTypeNeeded)
{
	std::vector<std::string> filesInNeed;
	auto isNeededFile = [fileTypeNeeded](std::string& str) {
		return std::string(str.end() - fileTypeNeeded.length(), str.end()) == fileTypeNeeded;
	};
	for (auto& i : input | std::ranges::views::filter(isNeededFile)) {
		filesInNeed.push_back(i);
	}
	return filesInNeed;

}

bool RenameFiles(std::vector<std::string> source, std::string path)
{
	if (source.empty())
	{
		return false;
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(source.begin(), source.end(), std::default_random_engine(seed));
	for (int i = 0; i < source.size();i++) {
		std::string newName = std::format(TEXT("{:03}_"), i) + source[i];
		if (rename((path + source[i]).c_str(), (path + newName).c_str())) {
			SetConsoleTextAttribute(consoleHWnd, FOREGROUND_RED);
			std::cout << "[WARNING]\t";
			SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
			std::cout<< path + source[i] << "\n->\t" << path + newName << "\n" << "failed\n";
		}
		else {
			SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN);
			std::cout << "[INFO]\t";
			SetConsoleTextAttribute(consoleHWnd, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
			std::cout << path + source[i] << "\n->\t" << path + newName << "\n" << "success\n";
		}
	}
	std::cout << std::endl;
	return true;
}
