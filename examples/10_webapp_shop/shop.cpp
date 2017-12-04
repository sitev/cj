#include "shop.h"

void func111() {

// { code insertion

		int a = 111;
	
// } code insertion

	
// { code insertion
}


// { code insertion

#define TEST_TEST

// } code insertion

Shop::Shop(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(14);
}


// { code insertion

void Shop::paint(WebPage *page, HttpRequest &request) {
	cout << "paint(WebPage *page, HttpRequest &request);" << endl;
	
	HttpRequest *req = &request;
	paint(page, req);
}
	
// } code insertion

void Shop::paint(WebPage *page, HttpRequest *&request) {
	int b;
	b = 5;
}

