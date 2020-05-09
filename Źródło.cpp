#include <codecvt>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <random>
using namespace std;
int main(int argc, wchar_t* argsv[]) {
	setlocale(LC_ALL, "");
	printf("Content-type:text/plain\n\n");
	int len;
	char* lenstr = getenv("CONTENT_LENGTH");
	if (lenstr != NULL && (len = atoi(lenstr)) != 0) {
		char* post_data = new char[len];
		fgets(post_data, len + 1, stdin);
		setlocale(LC_ALL, "");
		char* tocomp = strtok(post_data, "|");
		char* r = strtok(NULL, "|");
		string k = "(echo " + string(r);
		r = strtok(NULL, "|");
		while (r != NULL) {
			k += " && echo " + string(r);
			r = strtok(NULL, "|");
		}
		k += ")";
		char* d = strtok(tocomp, " ");
		string a = "";
		while (d != NULL) {
			char x = static_cast<char>(stoi(d));
			a = a + x;
			d = strtok(NULL, " ");
		}
		char* output = new char[a.length()];
		strcpy(output, a.c_str());
		std::random_device device;
		std::mt19937 generator(device());
		std::uniform_int_distribution<int> distribution(1, 2137);
		wofstream f;
		f.imbue(locale(locale::empty(), new codecvt_utf8<char, 0x10ffff, generate_header>));
		string file = to_string(distribution(generator));
		f.open(file + ".cpp");
		f << output;
		f.close();
		string command = "g++ -o" + file + ".exe " + file + ".cpp" + " 2>&1";
		FILE* compiler = _popen(command.c_str(), "r");
		if (compiler == NULL) {
			cout << u8"Nieznany b³¹d kompilatora.";
			string filecpp = file + ".cpp";
			const char* fcpp = filecpp.c_str();
			remove(fcpp);
			return 0;
		}
		char buffer[1035];
		bool err = false;
		string error = "";
		while (fgets(buffer, sizeof(buffer), compiler) != NULL) {
			err = true;
			error += buffer;
		}
		if (err == true) {
			cout << u8"B³¹d kompilacji:\n" << error;
			string filecpp = file + ".cpp";
			const char* fcpp = filecpp.c_str();
			remove(fcpp);
			return 0;
		}
		command = k + " | " + file + ".exe 2>&1";
		_pclose(compiler);
		FILE* software = _popen(command.c_str(), "r");
		char buffer2[1035];
		string out = "";
		while (fgets(buffer2, sizeof(buffer2), software) != NULL) {
			out += buffer2;
		}
		cout << u8"Wynik kompilatora:\n" << out;
		_pclose(software);
		string filecpp = file + ".cpp";
		const char* fcpp = filecpp.c_str();
		string fileexe = file + ".exe";
		const char* fexe = fileexe.c_str();
		remove(fcpp);
		remove(fexe);


	} else {
		printf("Hello world");
	}
	return 0;
}