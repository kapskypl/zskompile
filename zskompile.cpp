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
	name = std::to_string(time(NULL));
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

std::wstring exe(zskompile zsk) {
	std::wostringstream woss;
	std::string exename = zsk.getname() + ".exe";
	woss << "(";
	for (int i = 0; i < zsk.getinputs().size(); i++) {
		if (i > 0) {
			woss << " && ";
		}
		woss << "echo " << zsk.getinputs()[i];
	}
	woss << ") | ";
	std::wstring namews(exename.begin(), exename.end());
	woss << namews << " 2>&1";
	std::wstring command = woss.str();
	FILE* xe = _wpopen(command.c_str(), L"r");
	wchar_t buffer[10000];
	std::wstring output = L"";
	while (fgetws(buffer, sizeof(buffer), xe) != NULL) {
		output += buffer;
	}
	remove(exename.c_str());
	return output;
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
		return u8"Nieznany b³¹d kompilatora";
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
		wchar_t buffer[1035];
		std::wstring output = L"";
		std::wcout << output;
		if (comp != "OK") {
			std::cout << u8"B³¹d kompilacji:\n" << comp;
			return 0;
		}
		std::wcout << u8"Wynik dzia³ania programu:\n" << exe(zsk);
		return 0;
	}
}