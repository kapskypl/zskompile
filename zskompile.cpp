#include "zskompile.h"

std::wstring zskompile::dectowstring(char* in) {
	std::wostringstream woss;
	woss.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<char, 0x10ffff, std::generate_header>));
	char* next_temp;
	char* temp = strtok_s(in, " ", &next_temp);
	while (temp != NULL) {
		char x = static_cast<char>(atoi(temp));
		woss << x;
		temp = strtok_s(NULL, " ", &next_temp);
	}
	return woss.str();
};

std::wstring zskompile::getcode() {
	return this->code;
}

std::vector<std::wstring> zskompile::getinputs() {
	return this->inputs;
}

std::string zskompile::getname() {
	return this->name;
}

zskompile::zskompile(char* in) {
	unsigned __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	name = std::to_string(now);
	char* next_temp;
	char* temp = strtok_s(in, "|", &next_temp);
	int i = 0;
	while (temp != NULL) {
		if (i == 0) {
			this->code = dectowstring(temp);
		}
		else {
			this->inputs.push_back(dectowstring(temp));
		}
		i++;
		temp = strtok_s(NULL, "|", &next_temp);
	}
};
bool isGET() {
	char* buf = nullptr;
	size_t sizeb = 0;
	errno_t e = _dupenv_s(&buf, &sizeb, "CONTENT_LENGTH");
	if (e == 0 && buf != nullptr) {
		free(buf);
		return false;
	}
	else {
		free(buf);
		return true;
	}
}

void run(std::wstring command) {
	_wpopen(command.c_str(), L"r");
}

std::wstring exe(zskompile zsk) {
	unsigned __int64 d1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::wostringstream woss;
	std::string outname = zsk.getname() + ".out";
	std::string exename = zsk.getname() + ".exe";
	if (zsk.getinputs().size() > 0) {
		woss << "(";
		for (int i = 0; i < zsk.getinputs().size(); i++) {
			if (i > 0) {
				woss << " && ";
			}
			woss << "echo " << zsk.getinputs()[i];
		}
		woss << ") | ";
	}
	std::wstring outws(outname.begin(), outname.end());
	std::wstring namews(exename.begin(), exename.end());
	woss << namews << " >" << outws << " 2>&1";
	std::wstring command = woss.str();
	int n1 = time(NULL);
	int n2 = time(NULL);
	int completed = 0;
	std::future<void> f = std::async(run, command);
	while (n2 - n1 <= 10) {
		std::string ifcmd = "tasklist /FI \"IMAGENAME eq " + exename + "\" | find /c /v \"\"";
		FILE* ifrun = _popen(ifcmd.c_str(), "r");
		char buffer[1035];
		std::string ifout = "";
		while (fgets(buffer, sizeof(buffer), ifrun) != NULL) {
			ifout += buffer;
		}
		_pclose(ifrun);
		if (ifout == "1\n") {
			completed = 1;
			break;
		}
		n2 = time(NULL);
	}
	if (completed == 0) {
		std::string killcmd = "taskkill /F /IM " + exename;
		system(killcmd.c_str());
	}
	std::wstring opt;
	std::wifstream oof(outname.c_str(), std::ios::binary | std::ios::ate);
	int file_size = oof.tellg();
	oof.close();
	if (file_size > 2097152) {
		std::wostringstream wssss;
		wssss << u8"1\nOdpowiedŸ za du¿a na wys³anie (" << std::to_wstring(file_size) << L"B)";
		opt = wssss.str();
	}
	else {
		std::wstringstream z;
		std::wifstream oof2(outname.c_str());
		unsigned __int64 d2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		z << L"0\n" << std::to_wstring(d2-d1) << L"\n" << oof2.rdbuf();
		opt = z.str();
		oof2.close();
	}
	remove(outname.c_str());
	remove(exename.c_str());
	return opt;
}

std::string compiler(zskompile zsk) {
	std::string cppname = zsk.getname() + ".cpp";
	std::wofstream f;
	f.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<char, 0x10ffff, std::generate_header>));
	f.open(cppname);
	f << zsk.getcode();
	f.close();
	std::string command = "g++ -o" + zsk.getname() + ".exe " + cppname + " 2>&1";
	FILE* comp = _popen(command.c_str(), "r");
	if (comp == NULL) {
		remove(cppname.c_str());
		return u8"1\nNieznany b³¹d kompilatora.";
	}
	char buffer[1035];
	bool err = false;
	std::string error = "";
	while (fgets(buffer, sizeof(buffer), comp) != NULL) {
		err = true;
		error += buffer;
	}
	if (err == true) {
		remove(cppname.c_str());
		return error;
	}
	_pclose(comp);
	remove(cppname.c_str());
	return "OK";
}

char* post() {
	char* buf = nullptr;
	size_t sz = 0;
	errno_t e = _dupenv_s(&buf, &sz, "CONTENT_LENGTH");
	int l = atoi(buf) + 1;
	char* postdata = new char[l];
	fgets(postdata, l, stdin);
	free(buf);
	return postdata;
}

int main() {
	setlocale(LC_ALL, "");
	if (isGET()) {
		// Frontend
		std::cout << "Content-Type: text/html\n\n";
		std::cout << "JD";
	}
	else {
		// Backend
		std::cout << "Content-Type: text/plain\n\n";
		zskompile zsk(post());
		std::string comp = compiler(zsk);
		std::wstring output = L"";
		std::wcout << output;
		if (comp != "OK") {
			std::cout << u8"1\n" << comp;
			return 0;
		}
		std::wcout << exe(zsk);
		return 0;
	}
}