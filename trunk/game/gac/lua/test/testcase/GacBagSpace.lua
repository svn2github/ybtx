gac_gas_require "item/item_info_mgr/ItemInfoMgr"

function InitGacBagSpace()
	local TestTest= TestCase("TestGacBagSpace")
	function TestTest:setup()
	end
	
	function TestTest:TestCreate()
		local Bag = CBagSpace.Init(10)
		assert_equal( 10, Bag:Size(), "" )
		Bag = nil
	end
	
	function TestTest:TestAddAndDel()
		local Bag = CBagSpace.Init(10)
		local OneItem = CItem:new(1," À≈ÆÕº",1,0)
		Bag:AddItemByPos(1,OneItem)
		local OneGrid = Bag:GetGrid(1)
		local nSize = OneGrid:Size()
		assert_equal( 1, nSize, "" )
		OneGrid:DelUnique(1)
		nSize = OneGrid:Size()
		assert_equal( 0, nSize, "" )
		
		local OneItem = CItem:new(1,"¥Û—©«Ú",1,0) --FoldLimit = 1
		Bag:AddItemByPos(1,OneItem)
		Bag:DelNotPileByPos(1)
		nSize = OneGrid:Size()
		assert_equal( 0, nSize, "" )
		Bag = nil
	end

	function TestTest:TestGetWherePlace()
		local Bag = CBagSpace.Init(10)
		local OneItem = CItem:new(1,"–°—©«Ú",1,0) --CItem:new(GlobalID,BigID,Index),FoldLimit=10
		local TwoItem = CItem:new(1,"–°—©«Ú",2,0) --FoldLimit = 10
		
		Bag:AddItemByPos(1,OneItem)
		Bag:AddItemByPos(1,TwoItem)
		function func1()
			Bag:GetWherePlace(1,"–°—©«Ú",12)
		end
		assert_error( "No Error", func1)
		
		local nNum = Bag:GetWherePlace(1,"–°—©«Ú",9)
		assert_equal( 2, nNum, "" )
		
		local nNum = Bag:GetWherePlace(1,"–°—©«Ú",8)
		assert_equal( 1, nNum, "" )
		
		local nNum = Bag:GetWherePlace(1,"–°—©«Ú",7)
		assert_equal( 1, nNum, "" )
		
		local nNum = Bag:GetWherePlace(1,"–°—©«Ú",1)
		assert_equal( 1, nNum, "" )
	end
	
	function TestTest:TestAllFull()
		local Bag = CBagSpace.Init(2)
		local OneItem = CItem:new(1," À≈ÆÕº",1,0) 
		local TwoItem = CItem:new(1," À≈ÆÕº",2,0)
		local ThreeItem = CItem:new(1," À≈ÆÕº",3,0)
		Bag:AddItemByPos(1,OneItem)
		Bag:AddItemByPos(2,TwoItem)
		Bag:AddItemByPos(2,ThreeItem)
		local nNum = Bag:GetWherePlace(1,"–°—©«Ú",1)
		assert_equal( nil, nNum, "" )
	end
	
	function TestTest:TestAddTest()
		local Bag = CBagSpace.Init(2)
		local OneItem = CItem:new(1,"–°—©«Ú",1,0) --CItem:new(GlobalID,BigID,Index),FoldLimit=10
		local OneItem_Add = CItem:new(1,"–°—©«Ú",2,0)--FoldLimit=10
		
		local nNum = Bag:AddItemByPos(1,OneItem)
		assert_equal( 1, nNum, "" )
		nNum = Bag:AddItemByPos(1,OneItem_Add)
		assert_equal( 1, nNum, "" )
		
		local OneItem = CItem:new(1,"¥Û—©«Ú",1,0) --CItem:new(GlobalID,BigID,Index),FoldLimit=1
		local OneItem_Add = CItem:new(1,"¥Û—©«Ú",2,0)--FoldLimit=1
		local nNum = Bag:AddItemByPos(2,OneItem)
		assert_equal( 1, nNum, "" )
		nNum = Bag:AddItemByPos(2,OneItem_Add)
		assert_equal( 0, nNum, "" )
	end
	
	function TestTest:TestExchange()
		local Bag = CBagSpace.Init(2)
		local OneItem = CItem:new(1," À≈ÆÕº",1,0)
		
		local TwoItem = CItem:new(1,"–°—©«Ú",2,0) 
		
		local nNum = Bag:AddItemByPos(1,OneItem)
		assert_equal( 1, nNum, "" )
		
		nNum = Bag:AddItemByPos(2,TwoItem)
		assert_equal( 1, nNum, "" )
		
		Bag:ExChangeTwoGrid(1,2)
		--∏Ò◊”Ωªªª
		local OneGrid = Bag:GetGrid(1)
		local nBigID,nIndex = OneGrid:GetType()
		assert_equal( "–°—©«Ú", nIndex, "" )
		
		local OneGrid = Bag:GetGrid(2)
		local nBigID,nIndex = OneGrid:GetType()
		assert_equal( " À≈ÆÕº", nIndex, "" )
	end

	function TestTest:teardown()
	end
end