#include "include/Types.h"

void CUrlDeleter::operator()(CURL* curl)
{
	if (!curl)
		return;
	curl_easy_cleanup(curl);
}

upCURL make_upCURL()
{
	return upCURL{ curl_easy_init() };
}
