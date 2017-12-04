#include "shop.h"

void func111() {
inta= 111;
}

#defineTEST_TEST
Shop::Shop(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(14);
}

voidShop::paint(WebPage*page, HttpRequest&request){
	cout<<"paint(WebPage *page, HttpRequest &request);"<<endl;
	HttpRequest*req= &request;
	paint(page, req);
}
void Shop::paint111(WebPage *page, HttpRequest *&request) {
	int b;
	b = 5;
}

