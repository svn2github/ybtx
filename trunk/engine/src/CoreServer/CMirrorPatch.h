#pragma once


namespace sqr
{
	class CMirrorPatchCmd;

	class CMirrorPatch
	{
	public:
		virtual void AddCmd(CMirrorPatchCmd* pCmd) = 0;
		virtual ~CMirrorPatch(){}
	};

}
