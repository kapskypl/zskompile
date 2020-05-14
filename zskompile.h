#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <codecvt>
#include <vector>
#include <fstream>
#include <chrono>
#include <future>


class zskompile {
	public:
		zskompile(char* in);
		std::wstring getcode();
		std::string getname();
		std::vector<std::wstring> getinputs();
	private:
		std::string name;
		std::wstring code;
		std::vector<std::wstring> inputs;
		std::wstring dectowstring(char* in);
};