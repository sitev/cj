#pragma once

namespace cms {
#include "cms.h"

class Shop : public WebModule {
public:
	String uuid;
	int userId;
	String p1;
	String p2;
	String p3;
	String p4;
	Shop(SiteManager *manager);
	
// { code insertion

	virtual void paint(WebPage *page, HttpRequest &request);
	
// } code insertion

	void paint(WebPage *page, HttpRequest *&request);
	void paintIndex(WebPage *page, HttpRequest *&request);
	void paintCategory(WebPage *page, HttpRequest *&request);
	void paintCategoryIndex(WebPage *page, HttpRequest *&request);
	void paintCategoryEdit(WebPage *page, HttpRequest *&request);
	void paintCategoryRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject);
	void paintCategorySub(WebTemplate *tpl, int categoryId, int level, bool isShowOnlyProject);
	void ajax(WebPage *page, HttpRequest *&request);
	void ajaxAcceptCategory(WebPage *page, HttpRequest *&request);
	void ajaxAddCategory(WebPage *page, HttpRequest *&request);
	void ajaxDeleteCategory(WebPage *page, HttpRequest *&request);
};

}
