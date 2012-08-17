#pragma once

namespace sqr
{
	enum MSG_SOURCE
	{
		MGS_SYSCOM		= 0x00, //全局
		MGS_SYSMSG		= 0x01, //局部
		MGS_KEYMSG		= 0x02,
		MGS_MOUSEMSG	= 0x03,
		MGS_PADMSG		= 0x04,
		MGS_UIMSG		= 0x05,
		MGS_CUSTOM		= 0x06,
		MGS_NULL		= 0x07,
	};

	enum MSG_SYSTEM_TYPE //局部消息
	{
		MGT_SYS_IDLE		= 0x00,
		MGT_SYS_INIT		= 0x01,
		MGT_SYS_END			= 0x02,
		MGT_SYS_PAUSE		= 0x03,
		MGT_SYS_RUN			= 0x04,
		MGT_SYS_RESIZE		= 0x05,
		MGT_SYS_SELCHANGE	= 0x06,
		MGT_SYS_RENDER		= 0x07,
		MGT_SYS_ADDBONE		= 0x08,
		MGT_SYS_SELBONE     = 0x09,
	};

	enum MSG_KEY_TYPE
	{
		MGT_KEY_DOWN	= 0x00,
		MGT_KEY_UP		= 0x01,
	};

	enum MSG_MOUSE_TYPE
	{
		MGT_MOUSE_MOVE		= 0x00,
		MGT_MOUSE_UP		= 0x01,
		MGT_MOUSE_DOWN		= 0x02,
		MGT_MOUSE_DCLICK	= 0x03,
		MGT_MOUSE_WHEEL		= 0x04,

		MGT_MOUSE_LBUTTON	= 0x10,
		MGT_MOUSE_RBUTTON	= 0x20,
		MGT_MOUSE_MBUTTON	= 0x40,

		MGT_MOUSE_LBNUP		= 0x11,
		MGT_MOUSE_LBNDOWN	= 0x12,
		MGT_MOUSE_LBNDCLK	= 0x13,
		MGT_MOUSE_RBNUP		= 0x21,
		MGT_MOUSE_RBNDOWN	= 0x22,
		MGT_MOUSE_RBNDCLK	= 0x23,
		MGT_MOUSE_MBNUP		= 0x41,
		MGT_MOUSE_MBNDOWN	= 0x42,
		MGT_MOUSE_MBNDCLK	= 0x43,
	};

	enum MSG_UI_TYPE
	{
		MGT_UI_SIZE			= 0x00,
		MGT_UI_CREATE		= 0x01,
		MGT_UI_PAINT		= 0x02,
		MGT_UI_CLOSE		= 0x03,
		MGT_UI_QUIT			= 0x04,
		MGT_UI_NULL			= 0xff,
	};

	enum MSG_KEY_INF
	{
		MGI_KEY_Back		= 0x08,	//8(Back)
		MGI_KEY_Tab			= 0x09,	//9(Tab)

		MGI_KEY_ShiftKey	= 0x10,	//16(ShiftKey)
		MGI_KEY_CtrlKey		= 0x11,	//17(ControlKey)
		MGI_KEY_Menu		= 0x12,	//18(AltKey)
		MGI_KEY_Pause		= 0x13,	//19(Pause)
		MGI_KEY_Capital		= 0x14,	//20(Capital)

		MGI_KEY_Escape		= 0x1B,	//27(Escape)

		MGI_KEY_Space		= 0x20,	//32(Space)
		MGI_KEY_Prior		= 0x21,	//33(Prior)
		MGI_KEY_Next		= 0x22,	//34(Next)
		MGI_KEY_End			= 0x23,	//35(End)
		MGI_KEY_Home		= 0x24,	//36(Home)
		MGI_KEY_Left		= 0x25,	//37(Left)
		MGI_KEY_Up			= 0x26,	//38(Up)
		MGI_KEY_Right		= 0x27,	//39(Right)
		MGI_KEY_Down		= 0x28,	//40(Down)

		MGI_KEY_PrScr	= 0x2C,	//44(Print Screen)
		MGI_KEY_Insert	= 0x2D,	//45(Insert)
		MGI_KEY_Delete	= 0x2E,	//46(Delete)

		MGI_KEY_D0		= 0x30,	//48(0)
		MGI_KEY_D1		= 0x31,	//49(1)
		MGI_KEY_D2		= 0x32,	//50(2)
		MGI_KEY_D3		= 0x33,	//51(3)
		MGI_KEY_D4		= 0x34,	//52(4)
		MGI_KEY_D5		= 0x35,	//53(5)
		MGI_KEY_D6		= 0x36,	//54(6)
		MGI_KEY_D7		= 0x37,	//55(7)
		MGI_KEY_D8		= 0x38,	//56(8)
		MGI_KEY_D9		= 0x39,	//57(9)

		MGI_KEY_A		= 0x41,	//65(A)
		MGI_KEY_B		= 0x42,	//66(B)
		MGI_KEY_C		= 0x43,	//67(C)
		MGI_KEY_D		= 0x44,	//68(D)
		MGI_KEY_E		= 0x45,	//69(E)
		MGI_KEY_F		= 0x46,	//70(F)
		MGI_KEY_G		= 0x47,	//71(G)
		MGI_KEY_H		= 0x48,	//72(H)
		MGI_KEY_I		= 0x49,	//73(I)
		MGI_KEY_J		= 0x4A,	//74(J)
		MGI_KEY_K		= 0x4B,	//75(K)
		MGI_KEY_L		= 0x4C,	//76(L)
		MGI_KEY_M		= 0x4D,	//77(M)
		MGI_KEY_N		= 0x4E,	//78(N)
		MGI_KEY_O		= 0x4F,	//79(O)
		MGI_KEY_P		= 0x50,	//80(P)
		MGI_KEY_Q		= 0x51,	//81(Q)
		MGI_KEY_R		= 0x52,	//82(R)
		MGI_KEY_S		= 0x53,	//83(S)
		MGI_KEY_T		= 0x54,	//84(T)
		MGI_KEY_U		= 0x55,	//85(U)
		MGI_KEY_V		= 0x56,	//86(V)
		MGI_KEY_W		= 0x57,	//87(W)
		MGI_KEY_X		= 0x58,	//88(X)
		MGI_KEY_Y		= 0x59,	//89(Y)
		MGI_KEY_Z		= 0x5A,	//90(Z)
		MGI_KEY_LWin	= 0x5B,	//91(LWin)
		MGI_KEY_RWin	= 0x5C,	//92(RWin)
		MGI_KEY_Apps	= 0x5D,	//93(Apps)

		MGI_KEY_NumPad0	= 0x60,	//96(0)
		MGI_KEY_NumPad1	= 0x61,	//97(1)
		MGI_KEY_NumPad2	= 0x62,	//98(2)
		MGI_KEY_NumPad3	= 0x63,	//99(3)
		MGI_KEY_NumPad4	= 0x64,	//100(4)
		MGI_KEY_NumPad5	= 0x65,	//101(5)
		MGI_KEY_NumPad6	= 0x66,	//102(6)
		MGI_KEY_NumPad7	= 0x67,	//103(7)
		MGI_KEY_NumPad8	= 0x68,	//104(8)
		MGI_KEY_NumPad9	= 0x69,	//105(9)
		MGI_KEY_Multiply	= 0x6A,	//106(*)
		MGI_KEY_Add			= 0x6B,	//107(+)

		MGI_KEY_Subtract	= 0x6D,	//109(-)
		MGI_KEY_Decimal		= 0x6E,	//110(.)
		MGI_KEY_Divide		= 0x6F,	//111(/)

		MGI_KEY_F1		= 0x70,	//112(F1)
		MGI_KEY_F2		= 0x71,	//113(F2)
		MGI_KEY_F3		= 0x72,	//114(F3)
		MGI_KEY_F4		= 0x73,	//115(F4)
		MGI_KEY_F5		= 0x74,	//116(F5)
		MGI_KEY_F6		= 0x75,	//117(F6)
		MGI_KEY_F7		= 0x76,	//118(F7)
		MGI_KEY_F8		= 0x77,	//119(F8)
		MGI_KEY_F9		= 0x78,	//120(F9)
		MGI_KEY_F10		= 0x79,	//121(F10)
		MGI_KEY_F11		= 0x7A,	//122(F11)
		MGI_KEY_F12		= 0x7B,	//123(F12)

		MGI_KEY_NumLock			= 0x90,	//144(NumLock)
		MGI_KEY_Scroll			= 0x91,	//145(Scroll)

		MGI_KEY_Semicolon		= 0xBA,	//186(;)
		MGI_KEY_Equals			= 0xBB,	//187(=)
		MGI_KEY_Comma			= 0xBC,	//188(,)
		MGI_KEY_Minus			= 0xBD,	//189(-)
		MGI_KEY_Period			= 0xBE,	//190(.)
		MGI_KEY_Question		= 0xBF,	//191(/)

		MGI_KEY_OpenBrackets	= 0xDB,	//219([)
		MGI_KEY_Pipe			= 0xDC,	//220(\)
		MGI_KEY_CloseBrackets	= 0xDD,	//221(])
		MGI_KEY_Quotes			= 0xDE,	//222(')
	};

	struct MSG_MOUSE_INF
	{
		unsigned short	X;
		unsigned short	Y;
	};
}