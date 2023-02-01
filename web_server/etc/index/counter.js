setInterval(loadDoc, 2000);
function loadDoc() {
  const xhttp = new XMLHttpRequest();
  xhttp.onload = function() {
    document.getElementById("demo").innerHTML =
    this.responseText;
    }
    xhttp.open("GET", "/ajax_info");
    xhttp.send();
}