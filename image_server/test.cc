#include "httplib.h"

int main() {
  using namespace httplib;
  Server server;
  server.Get("/hello",[](const Request& rep,Response& resp){
      resp.set_content("<h1>hello<h1>","text/html");
      });
  server.listen("0.0.0.0",9094);
  return 0;
}
