#include "pubsub.h"
// using namespace std;

int main() {
  // char article[256];
  // gets(article);
  // cout << retrieve_type(article) << endl;
  // cout << retrieve_ogitor(article) << endl;
  // cout << retrieve_org(article) << endl;
  //
  // cout << endl << sub_article_valid(article) << endl;
  // cout << endl << pub_article_valid(article) << endl;
  //
  char buf[1024];
  memset(buf, 0, sizeof buf);
  string self_IP = "127.0.0.1";
  int self_port = 5105;
  string combined_str;
  // ​[“Register;RPC;IP;Port;ProgramID;Version”]
  combined_str = "Register;RPC;" + self_IP + ";" +
                 to_string(self_port) + ";0x20000001;1";
  strncpy(buf, combined_str.c_str(), 1024);
  printf("%s\n", buf);
}
