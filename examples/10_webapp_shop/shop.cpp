#include "shop.h"

Shop::Shop(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(14);
}

void Shop::paint(int page, int request) {
}

