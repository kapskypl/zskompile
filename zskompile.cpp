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
		std::wcout.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<char, 0x10ffff, std::generate_header>));
		std::wcout << u8"<html> <head> <title>Kompilator C++ Online</title> <meta charset=\"utf-8\"><link rel=\"icon\" href=\"https://i.ibb.co/37P9yf8/title.png\"/> <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css2?family=DM+Mono:wght@300&display=swap\"> <style>";
		std::wcout << u8"*{margin:0;padding:0;font-family:\"DM Mono\",monospace}#wrapper{display:grid;grid-template-rows:100px auto 60px;grid-template-columns:80px auto auto;grid-template-areas:\"header header header\" \"menu input output\" \"footer footer footer\"}#header{grid-area:header}#menu{grid-area:menu;display:flex;flex-direction:column}#input{grid-area:input;display:grid;grid-template-rows:60px auto;grid-template-columns:auto;grid-template-areas:\"inputHeader\" \"inputWrapper\"}#output{grid-area:output;grid-template-rows:60px auto;grid-template-columns:auto;grid-template-areas:\"outputHeader\" \"outputCode\"}#footer{grid-area:footer;text-align:center;font-size:16px;line-height:30px;margin-top:10px}.headerText{font-size:32px;margin-left:10px;line-height:100px}.mode{height:40px;width:40px;margin-top:25px;margin-left:auto;margin-right:auto;border-radius:8px}.mode:hover{cursor:pointer}.img{height:20px;width:20px;padding:10px}.on{background-color:#1e272e;border:1px solid #1a2227}.off{background-color:#ecedf0;border:1px solid #dcdde1}#inputHeader{display:flex;flex-direction:row}.inputText{font-size:16px;line-height:60px;flex-grow:1;margin-left:10px}#run{height:40px;width:80px;background-color:#3742fa;border-radius:8px;border-style:none;color:#fff;font-size:16px;margin:10px}#run:hover{cursor:pointer;background-color:#4b54fd}#inputWrapper{display:flex;flex-direction:column}#inputCode{height:600px}.stdin{height:60px;font-size:16px;line-height:60px;margin-left:10px}#inputData{height:80px}#outputText{grid-area:outputHeader;font-size:16px;line-height:60px;margin-left:10px}#outputCode{grid-area:outputCode;margin-left:10px}";
		std::wcout << u8"</style></head> <body class=\"bg\"> <div id=\"wrapper\" class=\"bg\"> <div id=\"header\" class=\"bg\"> <p class=\"headerText\">Kompilator C++ Online</p></div><div id=\"menu\" class=\"bg\"> <div class=\"mode off\" onclick=\"mode()\"> <img class=\"img\" src=\"https://i.ibb.co/sWCTKCj/off.png\"> </div></div><div id=\"input\"> <div id=\"inputHeader\" class=\"bg\"> <p class=\"inputText\">input</p><button id=\"run\">Uruchom</button> </div><div id=\"inputWrapper\" class=\"bg\"> <textarea id=\"inputCode\" class=\"bg\" onkeydown=\"insertTab(this, event);\" placeholder=\"Hello World!\"></textarea> <p class=\"stdin\">stdin</p><textarea id=\"inputData\" class=\"bg\" onkeydown=\"insertTab(this, event);\"></textarea> </div></div><div id=\"output\" class=\"bg\"> <p id=\"outputText\">output</p><p id=\"outputCode\" style=\"white-space: pre-wrap;\"></p></div><div id=\"footer\" class=\"bg\">&copy;Jakub Marciniak, Dawid Pieczyñski</div></div><script>";
		std::wcout << u8"function mode(){var e=document.getElementsByClassName(\"img\"),t=document.getElementsByClassName(\"mode\");t[0].classList.contains(\"off\")?(t[0].classList.remove(\"off\"),t[0].classList.toggle(\"on\"),e[0].src=\"https://i.ibb.co/fNZKGVk/on.png\"):(t[0].classList.remove(\"on\"),t[0].classList.toggle(\"off\"),e[0].src=\"https://i.ibb.co/sWCTKCj/off.png\");var n=document.getElementsByClassName(\"bg\");for(let e=0;e<n.length;e++)t[0].classList.contains(\"off\")?(n[e].style.background=\"#fff\",n[e].style.color=\"#000\"):(n[e].style.background=\"#000\",n[e].style.color=\"#fff\")}function insertTab(e,t){if(9==(t.keyCode?t.keyCode:t.charCode?t.charCode:t.which)&&!t.shiftKey&&!t.ctrlKey&&!t.altKey){var n=e.scrollTop;if(e.setSelectionRange){var o=e.selectionStart,s=e.selectionEnd;e.value=e.value.substring(0,o)+\"\\t\"+e.value.substr(s),e.setSelectionRange(o+1,o+1),e.focus()}else e.createTextRange&&(document.selection.createRange().text=\"\\t\",t.returnValue=!1);return e.scrollTop=n,t.preventDefault&&t.preventDefault(),!1}return!0}run.addEventListener(\"click\",()=>{var e=new XMLHttpRequest;e.open(\"POST\",\"?\",!0),e.setRequestHeader(\"Content-Type\",\"application/x-www-form-urlencoded\"),e.onreadystatechange=function(){if(this.readyState===XMLHttpRequest.DONE&&200===this.status){document.querySelector(\"#outputCode\").textContent=this.responseText;var e=document.getElementById(\"outputCode\"),t=e.textContent.split(\"\\r\\n\");if(\"1\"==t[0])e.style.color=\"#ff0000\",t.shift();else{var n=t[1];e.style.color=null,t.shift();for(n=t.shift();n.length<4;)n=\"0\"+n;var o=n.substring(n.length-3,n.length);n=n.replace(o,\",\"+o),t.push(`\\r\\nCzas wykonania programu: ${n} s.`)}e.textContent=t.join(\"\\r\\n\"),run.style.backgroundColor=\"#3742fa\",run.textContent=\"Uruchom\",run.style.width=\"80px\"}},enccode=(new TextEncoder).encode(inputCode.value),encoded=\"\",enccode.forEach(e=>encoded+=`${e} `),encoded=encoded.trim(),inputSplit=inputData.value.split(\"\\n\"),inputSplit.forEach(e=>{encoded+=\"|\",encinput=(new TextEncoder).encode(e),encinput.forEach(e=>encoded+=`${e} `),encoded=encoded.trim()}),e.send(encoded),run.style.backgroundColor=\"#2ed573\",run.textContent=\"Wykonywanie...\",run.style.width=\"160px\"});";
		std::wcout << u8"</script></body></html>";
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