#include "pubsub.h"

int main() {
  char article[256];
  gets(article);
  cout << retrieve_type(article) << endl;
  cout << retrieve_ogitor(article) << endl;
  cout << retrieve_org(article) << endl;
}
