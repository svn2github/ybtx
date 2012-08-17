enum ECharType
{
	eRLM = 0,	//人类男
	eRLW = 1,	//人类女
	eSRM = 2,	//兽人男
	eSRW = 3,	//兽人女
	eJLM = 4,	//精灵男
	eJLW = 5,	//精灵女
	eARM = 6,	//矮人男
	eARW = 7,	//矮人女
	eOTHER = 8,	//其他
	eCharTypeCount,
};

enum ECharItemType
{
	// 		ItemList			m_Head;
	// 		ItemList			m_Face;
	// 		ItemList			m_Hair;
	// 		ItemList			m_Shoulder;
	// 		ItemList			m_Body;
	// 		ItemList			m_Mantle;
	// 		ItemList			m_Cuff;
	// 		ItemList			m_Boot;
	// 		ItemList			m_MainWeapon;
	// 		ItemList			m_SubWeapon;


	ITEM_HEAD		= 0, //头盔
	ITEM_FACE		= 1, //相貌
	ITEM_HAIR		= 2, //头发
	ITEM_SHOULDER	= 3, //护肩
	ITEM_BODY		= 4, //身体
	ITEM_MANTLE		= 5, //披风
	ITEM_CUFF		= 6, //护腕
	ITEM_BOOT		= 7, //靴子
	ITEM_MAINWEAPON = 8, //主手
	ITEM_SUBWEAPON  = 9, //副手
	ITEM_COUNT		= 10,
};

