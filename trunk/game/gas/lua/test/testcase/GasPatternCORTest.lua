
function InitGasPatternCORTest()
	local TestTest=TestCase("test/testcase/GasPatternCORTest")
	
	local CTestEventHandler = class(CPtCORHandler)
	
	function CTestEventHandler:Ctor()
		self.m_Count = 0
	end
	
	function CTestEventHandler:OnCOREvent(Subject,Id, Arg)
		self.m_Count = self.m_Count + 1
	end
	
	function TestTest:setup()
	end

	function TestTest:TestAttach()
		local EventHandler = CTestEventHandler:new()
		local Subject = CPtCORSubject:new()
		Subject:Attach(EventHandler,1)
		Subject:Notify(1,nil)
		assert_equal(1,EventHandler.m_Count,"" )
		Subject:Detach(EventHandler,1)
		Subject:Notify(1, nil)
		assert_equal(1,EventHandler.m_Count,"" )
	end
	
	
	function TestTest:TestDetachHandler()
		local EventHandler = CTestEventHandler:new()
		local Subject = CPtCORSubject:new()
		Subject:Attach(EventHandler,1)
		Subject:Notify(1, nil)
		assert_equal(1,EventHandler.m_Count,"" )
		EventHandler:DetachAll()
		Subject:Notify(1, nil)
		assert_equal(1,EventHandler.m_Count,"" )
	end
	
	function TestTest:TestAttachMore()
		local EventHandler1 = CTestEventHandler:new()
		local EventHandler2 = CTestEventHandler:new()
		local Subject = CPtCORSubject:new()
		Subject:Attach(EventHandler1,1)
		Subject:Attach(EventHandler2,1)
		Subject:Notify(1, nil)
		assert_equal(1,EventHandler1.m_Count,"" )
		assert_equal(1,EventHandler2.m_Count,"" )
		Subject:Detach(EventHandler1, 1)
		Subject:Detach(EventHandler2, 1)	
	end
	
	function TestTest:TestJoinMore()
		local EventHandler1 = CTestEventHandler:new()
		local EventHandler2 = CTestEventHandler:new()
		local Subject1 = CPtCORSubject:new()
		local Subject2 = CPtCORSubject:new()
		
		Subject1:Attach(EventHandler1,1)
		Subject1:Attach(EventHandler2,1)
		Subject2:Attach(EventHandler1,1)
		Subject2:Attach(EventHandler2,1)
		
		Subject1:Notify(1, nil)
		Subject2:Notify(1, nil)
		assert_equal(2,EventHandler1.m_Count,"" )
		assert_equal(2,EventHandler2.m_Count,"" )
		
		Subject1:Detach(EventHandler1,1)
		Subject1:Detach(EventHandler2,1)
		Subject2:Detach(EventHandler1,1)
		Subject2:Detach(EventHandler2,1)
	end
	
	function TestTest:TestOther()
		local EventHandler1 = CTestEventHandler:new()
		local Subject1 = CPtCORSubject:new()

		Subject1:Attach(EventHandler1,1)
		Subject1:Notify(1, nil)
		assert_equal(1,EventHandler1.m_Count,"" )
		--EventHandler1 一定要手动解除关系，否则会有问题
		--在EventHandler1 的Update函数中不能设置Subject1为nil，否则后果自负
		EventHandler1:DetachAll()
		EventHandler1 = nil
		Subject1:Notify(1, nil)
		Subject1:Notify(1, nil)
	end
	
	function TestTest:teardown()
	end
end