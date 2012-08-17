--- @brief 根据配置文件向CTreeCtrl添加数据节点
--- @param tree_ctrl 要插入节点的目标树
--- @param cfg 配置数据，格式参见下面的说明
--- @param [parent_node] 将新节点作为该节点的子节点，缺省为插入到树的根下
--- @return 插入的节点表
--[[
-- 以下为实例数据配置的格式：
tree_config_data=
{
	{text="武器", sub={
		{text="防具"},
		{text="锁甲"},
		{text="皮甲", sub={
			{text="布甲"},
			{text="头部"},
			{text="肩部"},
			{text="胸部"},
			{text="手部"},	
			{text="脚部"},
		}}
	}},
	{text="药剂", head_img=nil, tail_img=nil, data=nil, param=0}},
	{text="宝石"},
	{text="其他"},
}
]]

CTreeCtrlNode = class ()

function CTreeCtrlNode.g_AddTreeCtrlNode(tree_ctrl, cfg, parent_node)
	if cfg == nil or type(cfg) ~= "table" then
		return nil
	end
	nodelist = {}
	for i=1, #cfg do
		-- make single node
		local curNode = nil
		if cfg[i].text ~= nil or cfg[i].head_img ~= nil or cfg[i].tail_img ~= nil or cfg[i].data ~= nil or cfg[i].param ~= nil then
			curNode = tree_ctrl:InsertNode(parent_node, cfg[i].text, cfg[i].head_img, cfg[i].tail_img, cfg[i].data, cfg[i].param)
		end
		if curNode == nil then
			curNode = parent_node
		end
		-- make sub nodes
		if cfg[i].sub ~= nil then
			CTreeCtrlNode.g_AddTreeCtrlNode(tree_ctrl, cfg[i].sub, curNode)
		end		
		table.insert(nodelist, curNode)
	end
	return nodelist
end

function CTreeCtrlNode.CTreeCtrl_ClearNode(tree_ctrl)
	for i = 1,tree_ctrl:GetChildNodeCount(tree_ctrl) do
		tree_ctrl:DeleteNode(tree_ctrl:GetFirstChildNode(tree_ctrl))
	end
end