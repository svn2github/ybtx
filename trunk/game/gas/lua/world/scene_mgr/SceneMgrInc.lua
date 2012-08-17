CSceneMgr = class()

-- m_tbl_MetaScenes 表里存放已经创建的 MetaScene(静态数据), 用 SceneName 索引    
CSceneMgr.m_tbl_MetaScene= {}    --存储场景数据,场景属性
CSceneMgr.m_tbl_AreaMetaScene = {} --存储区域场景 属性

-- m_tbl_CoreScenes 表里存放已经创建的 CoreScene(动态数据), 用 ID 索引
CSceneMgr.m_tbl_Scene = {}
CSceneMgr.m_tbl_CoreSceneIndex = {}

--存放所有主场景的数据库Id, 改多服后虽然一个服务器只加载不同的部分主场景
--由于主场景的id和所在服务器id是固定的,所以为了方便每个服务器都保存一份所有主场景的id(其中包括花语平原)
CSceneMgr.m_tbl_MainScene_name = {}   --[1]id [2]name [3]Process [4]serverId
CSceneMgr.m_tbl_MainScene_Id = {} 

