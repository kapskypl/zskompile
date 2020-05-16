function mode() {
    var img = document.getElementsByClassName("img");
    var mode = document.getElementsByClassName("mode");
    if (mode[0].classList.contains("off")) {
        mode[0].classList.remove("off");
        mode[0].classList.toggle("on");
        img[0].src = "https://i.ibb.co/fNZKGVk/on.png";
    } else {
        mode[0].classList.remove("on");
        mode[0].classList.toggle("off");
        img[0].src = "https://i.ibb.co/sWCTKCj/off.png";
    }
    var bg = document.getElementsByClassName("bg");
    for (let i = 0; i < bg.length; i++) {
        if (mode[0].classList.contains("off")) {
            bg[i].style.background = "#fff";
            bg[i].style.color = "#000";
        } else {
            bg[i].style.background = "#000";
            bg[i].style.color = "#fff";
        }
    }
}

run.addEventListener("click", () => {
    var a = new XMLHttpRequest();
    a.open("POST", "?", true);
    a.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    a.onreadystatechange = function() {
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            document.querySelector("#outputCode").textContent = this.responseText;
            var pCheck = document.getElementById("outputCode");
            var pc = pCheck.textContent.split("\r\n");
            if (pc[0] == "1") {
                pCheck.style.color = "#ff0000";
                pc.shift();
            } else {
                var milli = pc[1];
                pCheck.style.color = null;
                pc.shift();
                var milli = pc.shift();
                while (milli.length < 4) {
                    milli = "0" + milli;
                }
                var strtmp = milli.substring(milli.length - 3, milli.length);
                milli = milli.replace(strtmp, "," + strtmp);
                pc.push(`\r\nCzas wykonania programu: ${milli} s.`);
            }
            pCheck.textContent = pc.join("\r\n");
            run.style.backgroundColor = "#3742fa";
            run.textContent = "Uruchom";
            run.style.width = "80px";
        }
    }
    enccode = new TextEncoder().encode(inputCode.value);
    encoded = "";
    enccode.forEach((x) => encoded += `${x} `);
    encoded = encoded.trim();
    inputSplit = inputData.value.split("\n")
    inputSplit.forEach((x) => {
        encoded += "|";
        encinput = new TextEncoder().encode(x)
        encinput.forEach((k) => encoded += `${k} `)
        encoded = encoded.trim();
    })
    a.send(encoded);
    run.style.backgroundColor = "#2ed573";
    run.textContent = "Wykonywanie...";
    run.style.width = "160px";
});

function insertTab(o, e) {		
    var kC = e.keyCode ? e.keyCode : e.charCode ? e.charCode : e.which;
    if (kC == 9 && !e.shiftKey && !e.ctrlKey && !e.altKey) {
        var oS = o.scrollTop;
        if (o.setSelectionRange) {
            var sS = o.selectionStart;	
            var sE = o.selectionEnd;
            o.value = o.value.substring(0, sS) + "\t" + o.value.substr(sE);
            o.setSelectionRange(sS + 1, sS + 1);
            o.focus();
        } else if (o.createTextRange) {
            document.selection.createRange().text = "\t";
            e.returnValue = false;
        }
        o.scrollTop = oS;
        if (e.preventDefault) {
            e.preventDefault();
        }
        return false;
    }
    return true;
}
