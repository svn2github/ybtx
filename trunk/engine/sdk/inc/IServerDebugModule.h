#if !defined _I_SERVER_DEBUG_MODULE_H
#define _I_SERVER_DEBUG_MODULE_H

class IServerDebugModule : public IBase
{
public:
	interface_version(IServerDebugModule,000);

	virtual unsigned Init(const void* pScenePtr)=0;
	virtual const void* RenderBegin()=0;
	virtual void RenderEnd()=0;
	virtual void DrawRect( int x, int y, int w, int h, unsigned long Color )=0;
	virtual void DrawText( int x, int y, LPCTSTR pText, uint32 Color ) = 0;
	virtual void Create()=0;
};

#endif

