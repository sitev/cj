SiteManager from "cms" {
}

WebModule from "cms" {
	void setOptionsFromDB(int moduleId);
}

WebPage from "cms" {
}

HttpRequest from "cms" {
}

Shop : WebModule {
	string uuid;
	int userId;

	Shop(SiteManager manager) {
		setOptionsFromDB(14);
	}
	
	@cpph {
	virtual void paint(WebPage *page, HttpRequest &request);
	}
	
	@cpp {
void Shop::paint(WebPage *page, HttpRequest &request) {
	cout << "paint(WebPage *page, HttpRequest &request);" << endl;
	
	HttpRequest *req = &request;
	paint(page, req);
}
	}
	
	void paint(WebPage page, HttpRequest &request) {
		uuid = request->header.COOKIE.getValue("uuid");
		userId = manager->getUserId(uuid);
	}
}

