
function InitGasFriendTest()

	local TestTest=TestCase("test/testcase/GasFriendTest")

	function TestTest:setup()
	end
	
	function TestTest:TestHaveFriend()
		--local Friend = CFriend:new(1)
		--assert_equal(false,Friend:IsFriend(1),"")
		--assert_equal(false,Friend:IsFriend(2),'')
		
	end
	
	function TestTest:TestAddFriend()
		--local Friend = CFriend:new(1)
		--assert_equal(false, Friend:Add(1), '')
		--assert_equal(false, Friend:Add(2), '')
	end
	
	function TestTest:TestDeleteFriend()
		--local Friend = CFriend:new(1)
		--assert_equare(false, Friend:Delete(1), '')
		--assert_euqare(false, Friend:Delete(2), '')
	end
	
	function TestTest:teardown()
	end
	
end