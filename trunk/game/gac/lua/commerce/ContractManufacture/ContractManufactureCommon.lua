CContractManufactureCommon = class (SQRDialog)

function CContractManufactureCommon:SetContractManuOrderDetail(tbl, index, wndTbl)
    local productType = tbl[index][1]
    local productName = tbl[index][2]
    
    local materialInfoTbl = {}
    for i=1, 3 do
        if  tbl[index][3][i] ~= nil and tbl[index][3][i][3] ~= 0 and tbl[index][3][i][1] ~= 0 then
            local materialNode = {tbl[index][3][i][1], tbl[index][3][i][2], tbl[index][3][i][3]} 
            table.insert(materialInfoTbl, materialNode)
        end
    end
    
    local productIconWnd = wndTbl[1]
    local productNameWnd = wndTbl[2]
    local materialIconWnd1 = wndTbl[3]
    local materialIconWnd2 = wndTbl[4]
    local materialIconWnd3 = wndTbl[5]

    productIconWnd:SetFocus()
    materialIconWnd1:SetFocus()
    materialIconWnd2:SetFocus()
    materialIconWnd3:SetFocus()
    
    local materialIconWndTbl = {materialIconWnd1, materialIconWnd2, materialIconWnd3}
    
    local productIcon = g_ItemInfoMgr:GetItemInfo(productType, productName, "SmallIcon")
    local material1Icon = g_ItemInfoMgr:GetItemInfo(materialInfoTbl[1][1], materialInfoTbl[1][2], "SmallIcon")
    local displayProductName = g_ItemInfoMgr:GetItemLanInfo(productType, productName,"DisplayName")
    
    g_LoadIconFromRes(productIcon, productIconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
    g_SetWndMultiToolTips(productIconWnd,productType,productName,0 ,1)     
    productNameWnd:SetWndText(displayProductName)

    g_LoadIconFromRes(material1Icon, materialIconWnd1, -1, IP_ENABLE, IP_CLICKDOWN)
    
    g_SetWndMultiToolTips(materialIconWnd1,materialInfoTbl[1][1],materialInfoTbl[1][2],0 ,1)  
        
        
    for i=1, 3 do
        local materialIconWnd = materialIconWndTbl[i]
        if materialInfoTbl[i] ~= nil then
            local materialType = materialInfoTbl[i][1]
            local materialName = materialInfoTbl[i][2]
            local materialCount= materialInfoTbl[i][3]
            local materialIcon = g_ItemInfoMgr:GetItemInfo(materialType, materialName, "SmallIcon")
            g_LoadIconFromRes(materialIcon, materialIconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
            g_SetWndMultiToolTips(materialIconWnd,materialType,materialName,0 ,1)  
            materialIconWnd:ShowWnd(true)           
        else
            materialIconWnd:SetMouseOverDescAfter("")
            materialIconWnd:ShowWnd(false)
        end
    end
end