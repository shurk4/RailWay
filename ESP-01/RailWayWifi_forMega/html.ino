void showLoginPage(bool wrongLogin) {
  Serial.println("showLoginPage");
  String htmlCode("<!DOCTYPE html>"
                  "<html>"
                  "<head>"
                  "<meta name=\"viewport\" content=\"width=device-widtg, initial-scale=1.0\">"
                  "<meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">"
                  "<title>shurk ESP-01 web interface - LogIn</title>"
                  "<style>"
                  "#redText {"
                  "text-align: right;"
                  "color: red; "
                  "font-size: 0.8em;"
                  "}"
                  "#header {"
                  "width: calc(100% - 22px);"
                  "position: absolute;"
                  "background-color: #303050;"
                  "color: #ffffff; "
                  "font-size: 2em;"
                  "user-select: none;"
                  "padding-left: 22px;"
                  "padding-bottom: 5px;"
                  "border-top-left-radius: 5px;"
                  "border-top-right-radius: 5px;"
                  "}"
                  "#module {"
                  "display:grid;"
                  "border-width: 5px;"
                  "border-style:outset;"
                  "border-radius: 10px;"
                  "border-color: #303050;"
                  "margin: 10px 10px;"
                  "padding: 10px 10px;"
                  "position: absolute;"
                  "top: calc(50% - 15px);"
                  "left: 50%;"
                  "transform: translate(-50%,-50%);"
                  "vertical-align: middle;"
                  "}"
                  "#loginModule {"
                  "margin-top: 43px;"
                  "display:grid;"
                  "}"
                  "#loginForm, #button {"
                  "margin: 5px 10px; "
                  "font-size: 1.5em;"
                  "border-width: 2px;"
                  "border-style:solid;"
                  "border-radius: 10px;"
                  "border-color: #303050;"
                  "text-align:center;"
                  "}"
                  "#button"
                  "{"
                  "background-color: #303050;"
                  "color: #ffffff;"
                  "}"
                  "#button:hover"
                  "{"
                  "background-color: #303070;"
                  "}"
                  "</style>"
                  "</head>"
                  "<body>"
                  "<div id=\"module\">"
                  "<div id=\"header\">Login:");

  if (wrongLogin) htmlCode += "<a id=\"redText\">wrong name or pass</a>";

  htmlCode += "</div>"
              "<form action='/login' method='POST' id=\"loginModule\">"
              "<input type=\"text\" placeholder=\"Name\" name=\"USERNAME\" maxlength=\"30\" id=\"loginForm\">"
              "<input type=\"password\" placeholder=\"Password\" name=\"PASSWORD\" maxlength=\"30\" id=\"loginForm\">"
              "<input type='submit' name='SUBMIT' value='Submit' id=\"button\">"
              "</form>"
              "</div>"
              "</body>"
              "</html>";

  server.send(200, "text/html", htmlCode);
}
// Показывает запрошеную страницу. Принимает строку с кодом для изменяемой части страницы, например <body> String page </bodу> и строку со использованным в странице стилем(если стиль не указан в шаблоне)
// headerSize, middleSize и footerSize хранят количество символов хедера и футера, templSize считает считает общее количество символов для резервирования размера отправляемой страницы(для ускорения подсчёта)
void showPage(String &page, String &style) {
  Serial.println("showPage");
  int pageSize = page.length();
  int styleSize = style.length();
  String cookie = server.header("Cookie");
  int headerSize = 1255;
  String header("<!DOCTYPE html>"
                "<html lang=\"en\">"
                "<head>"
                "<meta charset=\"UTF-8\">"
                "<title>");
  header += moduleName;
  header += "</title><style>";

  String middle("</style>"
                "</head>"
                "<body>"
                "<div class=\"name\">");
  middle += moduleName;
  middle += "</div>";
  middle += "<header>";
  middle += "<nav>";
  middle += "<a class=\"menuButton home\"href=\"/\">Home</a>";
  if (cookie.indexOf("name=admin") != -1) {
    middle += "<a class=\"menuButton settings\"href=\"/settings\">Settings</a>";
  }
  middle += "<a class=\"menuButton\"href=\"/login?DISCONNECT=YES\">LogOut</a>";
  middle += "</nav>";
  middle += "</header>";
  middle += "<main>";
  int middleSize = middle.length();


  String footer("</main>"
                "<footer><a class=\"mail\"href=\"mailto:shurk4@gmail.com\">shurk4@gmail.com</a></footer>"
                "</body>"
                "</html>");
  int footerSize = footer.length();

  int templSize;  // Подсчёт размера строки страницы

  if (page.length() > 5000) {
    templSize = styleSize + headerSize + middleSize;
    header.reserve(templSize);
    header += baseCSS + style + middle;
    server.setContentLength(header.length() + page.length() + footer.length());
    server.send(200, "text/html", header);
    server.sendContent(page);
    server.sendContent(footer);
  } else {
    templSize = styleSize + headerSize + middleSize + pageSize + footerSize + 100;  // Подсчёт размера строки страницы
    header.reserve(templSize);
    header += baseCSS + style + middle + page + footer;
    server.send(200, "text/html", header);
  }
}

void showHomePage() {
  String page("<div class=\"bodyPart\">"
              "<label class=\"line\">Двери</label>"
              "<div class=\"item\">"
              "<a href=\"/door1\" class=\"button\">1</a>"
              "<a href=\"/door2\" class=\"button\">2</a>"
              "</div></div>"
);

  showPage(page, homeCSS);
}

void showSettingsPage() {
  Serial.println("showSettingsPage");
  IPAddress ip(WiFi.localIP());
  String ip1;
  String ip2;
  String ip3;
  String ip4;
  ipToPartsString(ip.toString(), ip1, ip2, ip3, ip4);

  String page("<div class=\"bodyPart\">"
              "<label class=\"modLabel\">Настройка сети в режиме клиент</label>"
              "<div class=\"item\">"
              "<form method=\"post\" action=\"setClientIp\" class=\"config\">"
              "<div class=\"contentForm\" id=\"inForm\">"
              "<label>ip:</label>"
              "<spacer></spacer>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"");
              page += ip1;
              page += 
              "\" name='ip1'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip2;
              page += 
              "\" name='ip2'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip3;
              page += 
              "\" name='ip3'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip4;

              ip = WiFi.dnsIP();              
              ipToPartsString(ip.toString(), ip1, ip2, ip3, ip4);

              page += 
              "\" name='ip4'>"
              "</div>"
              "<div class=\"contentForm\" id=\"inForm\">"
              "<label>mask:</label>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip1;
              page +=
              "\" name='mask1'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip2;
              page +=
              "\" name='mask2'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip3;
              page +=
              "\" name='mask3'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip4;

              ip = WiFi.gatewayIP();
              ipToPartsString(ip.toString(), ip1, ip2, ip3, ip4);

              page +=
              "\" name='mask4'>"
              "</div>"
              "<div class=\"contentForm\" id=\"inForm\">"
              "<label>gw:</label>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip1;
              page +=
              "\" name='gw1'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip2;
              page +=
              "\" name='gw2'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip3;
              page +=
              "\" name='gw3'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip4;

              ip = WiFi.subnetMask();
              ipToPartsString(ip.toString(), ip1, ip2, ip3, ip4);

              page +=
              "\" name='gw4'>"
              "</div>"
              "<div class=\"contentForm\" id=\"inForm\">"
              "<label>dns:</label>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip1;
              page +=
              "\" name='dns1'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip2;
              page +=
              "\" name='dns2'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip3;
              page +=
              "\" name='dns3'>"
              "<input class=\"ip\" type=\"number\" min=\"0\" max=\"255\" value=\"";
              page += ip4;
              page +=
              "\" name='dns4'>"
              "</div>"
              "<input class=\"contentForm\" type='submit' value=\"Применить\">"
              "</form>"
              "</div>"
              "<label class=\"modLabel\">Настройки ESP</label>"
              "<div class=\"item\">"
              "<form method=POST action=\"setAdminPass\" class=\"modLabel\">"
              "<input class=\"contentForm\" type=\"password\" placeholder=\"admin password\" name='pass' length=\"10\">"
              "<input class=\"contentForm\" type='submit' value=\"Задать\">"
              "</form>"
              "<form method=POST action=\"setUserPass\" class=\"modLabel\">"
              "<input class=\"contentForm\" type=\"password\" placeholder=\"user password\" name='pass' length=\"10\">"
              "<input class=\"contentForm\" type='submit' value=\"Задать\">"
              "</form>"
              "<hr align=\"center\" width=100% size=\"2\" color=\"#303050\" />"
              "<div class=\"modLabel\">"
              "<form class=\"contentForm\" action=\"/restart\" method=\"POST\"><input class=\"contentForm\" type=\"submit\" value=\"Перезапустить ESP\"></form>"
              "<form class=\"contentForm\" action=\"/reset\" method=\"POST\"><input class=\"contentForm\" type=\"submit\" value=\"Сбросить настройки\"></form>"
              "</div>"
              "</div>"
              "</div>"
              "<div class=\"bodyPart\">"
              "<label class=\"modLabel\">Настройки режима точки доступа</label>"
              "<div class=\"item\" id=\"ap\">"
              "<div class=\"modLabel\">"
              "<form method=POST action=\"setAPsid\" class=\"contentForm\"id=\"inForm\">"
              "<input class=\"contentForm\" type=\"input\" placeholder=\"WiFi AP Name: ";
  page.reserve(7000);
  page += apsid;
  page +=
    "\" name='ssid' length=\"10\">"
    "<input class=\"contentForm\" type='submit' value=\"Задать\">"
    "</form>"
    "<form method=\"post\" action=\"resetAPsid\">"
    "<input class=\"contentForm\" type=\"submit\" value=\"Сбросить\">"
    "</form>"
    "</div>"
    "<div class=\"modLabel\">"
    "<form method=POST action=\"setAPPass\" class=\"contentForm\" id=\"inForm\">"
    "<input class=\"contentForm\" type=\"password\" placeholder=\"WiFi AP password: ";
  page += appass;
  page +=
    "\" name='pass' length=\"10\">"
    "<input class=\"contentForm\" type='submit' value=\"Задать\">"
    "</form>"
    "<form method=\"post\" action=\"resetAPPass\">"
    "<input class=\"contentForm\" type=\"submit\" value=\"Сбросить\">"
    "</form>"
    "</div>"
    "</div>"
    "<div class=\"modLabel\">"
    "<label>Доступные сети WiFi:</label>"
    "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"Обновить\"></form>"
    "</div>"
    "<form method=POST action='setWiFi'>"
    "<div class=\"wifi\">";

  // 153 symbols
  String pageEnd("</div>"
                 "<div class=\"modLabel\"><input class=\"contentForm\" type=\"password\" placeholder=\"Password\" name='pass' length=\"10\">"
                 "<input class=\"contentForm\" type='submit' value=\"Подключиться\">"
                 "</div>"
                 "</form>"
                 "</div>");

  page += wifiSsidList;  // Список устанавливается в wifi.ino - void setAvailibleNetworks()
  page += pageEnd;
  showPage(page, settingsCSS);
}
