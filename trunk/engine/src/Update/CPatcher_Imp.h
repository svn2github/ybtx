#pragma once


namespace sqr
{
	class IAsyncUpdateHandler;

	class CPatcher_Imp
	{
	public:
		CPatcher_Imp();
		~CPatcher_Imp();

		int YbtxPatchExecute(IAsyncUpdateHandler* pAsyncUpdateHandler, const char *PatchFile); // 新的升级程序
		int decompress (void *in , void *out);

	private:
		// 更新包文件头
		struct UpdateFileHead
		{
			DWORD flag; //标志 'DTUD' 标记DTBZ表示用bzip2
			DWORD ver;  //版本号
			DWORD decompress_len; //下面数据解压后的长度
		};

		// 资源更新节点(某一个ADDF或MODI)
		struct UpdateSec
		{
			DWORD flag;
			DWORD file_len;
			DWORD uDiffLen;     // Diff长度, ADDF模式下为0
			DWORD pathname_len;
		};

		// MODI内部块节点
		struct ModiChipNode
		{
			DWORD uSkip;
			DWORD uSize;
		};
	};

}
