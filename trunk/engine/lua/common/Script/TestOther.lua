engine_require "common/Script/lunit"
lunit.import "all" 

function InitTestOther( TestOther )
	--[[
	function TestOther:test_arkdofile()
		local function no_param()
			DoFile()
		end
		
		local function file_not_exsit()
			DoFile( "folder_noexsit" )
		end
		
		local brun_all =false
		local bload_all =false
		
		local brun_all = false
		local function test_dofile()
		
			assert_error( "[other][DoFile][no param]", no_param )
			assert_error( "[other][DoFile][folder test]", file_not_exsit )
		
			local rb, ri, rstr = DoFile( "common/Script/folder/return_ibs" )
			assert_false( rb, 				"[other][DoFile][return]" )
			assert_equal( 456131, ri, 		"[other][DoFile][return]" )
			assert_equal( "mesh", rstr,		"[other][DoFile][return]" )
		
			assert_equal( "folder1", DoFile( "common/Script/folder/folder1/folder1" ), 			"[other][DoFile][folder test]" )
			assert_equal( "folder2", DoFile( "common/Script/folder/folder2/folder2" ),			"[other][DoFile][folder test]" )
			assert_equal( "folder3", DoFile( "common/Script/folder/folder1/folder3/folder3" ), 			"[other][DoFile][folder test]" )
			assert_equal( "folder4", DoFile( "common/Script/folder/folder1/folder4/folder4" ), 	"[other][DoFile][folder test]" )
			assert_equal( "testfolder", DoFile( "common/Script/testfolder" ), 	"[other][DoFile][folder test]" )
			brun_all = true
		end
		
		local function test_loadfile()

			local r, error = LoadFile( "no exsit" )
			assert_equal( nil, r, "[other][LoadFile]" )
			assert_not_equal( nil, error, "[other][LoadFile][msg]" )
		
			r, error = LoadFile( "common/Script/folder/folder1/folder1" )
			assert_equal( "folder1", r(),  "[other][LoadFile]" )		
			assert_equal( nil, error, "[other][LoadFile][msg]" )
			bload_all = true
		end
			
		test_dofile()
		test_loadfile()
	end
	--]]
end