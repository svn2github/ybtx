#if 0

#include <string>

using namespace std;


namespace sqr
{
	class CHTTPDownloader
	{
	public:
		CHTTPDownloader();
		~CHTTPDownloader();

		bool Download(string& host, string& file);
		inline const string& GetContent() const
		{
			return m_strContent;
		}

	private:
		string m_strContent;
	};
}

#endif
