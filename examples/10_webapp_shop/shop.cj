SiteManager from "cms" {
}

WebModule from "cms" {
	void setOptionsFromDB(int moduleId);
}


Shop : WebModule {

	Shop(SiteManager manager) {
		setOptionsFromDB(14);
	}
	
	void paint(int page, int request) {
	
	}
}

