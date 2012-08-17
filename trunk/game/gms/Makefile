all:

install:
	mkdir -p $(phpfold)cgi-bin/gm_tools/
	mkdir -p $(phpfold)lib/gm_tools/
	mkdir -p $(phpfold)lang/gm_tools/$(language)/
	mkdir -p $(phpfold)conf/gm_tools/
	mkdir -p $(crontab)
	mkdir -p $(webfold)
	install cgi/FrameWork/FrameWork.php         $(phpfold)cgi-bin/gm_tools/
	install cgi/FrameWork/InterFace.php         $(phpfold)cgi-bin/gm_tools/
	install cgi/FrameWork/ActionPage.php        $(phpfold)cgi-bin/gm_tools/
	install cgi/FrameWork/image.php             $(phpfold)cgi-bin/gm_tools/
	install cgi/FrameWork/download.php          $(phpfold)cgi-bin/gm_tools/
	install cgi/FrameWork/ShowNotice.php        $(phpfold)cgi-bin/gm_tools/
	install cgi/FrameWork/Sell_GetList.php      $(phpfold)cgi-bin/gm_tools/
	install cgi/FrameWork/Sell_GetSN.php        $(phpfold)cgi-bin/gm_tools/
	install cgi/FrameWork/Sell_BuyError.php.php $(phpfold)cgi-bin/gm_tools/
	install tools/*.php                         $(phpfold)cgi-bin/gm_tools/  
	install cgi/Management/*.php                $(phpfold)lib/gm_tools/
	install cgi/RoleInfo/*.php                  $(phpfold)lib/gm_tools/
	install cgi/TongInfo/*.php                  $(phpfold)lib/gm_tools/
	install cgi/TaskInfo/*.php                  $(phpfold)lib/gm_tools/
	install cgi/DataMonitor/*.php               $(phpfold)lib/gm_tools/
	install cgi/FrameWork/Auth.php              $(phpfold)lib/gm_tools/
	install cgi/FrameWork/Lib.php               $(phpfold)lib/gm_tools/
	install lang/$(language)/*.php              $(phpfold)lang/gm_tools/$(language)/
	install conf/Config.php.template            $(phpfold)conf/gm_tools/
	install conf/ServerList.php.template        $(phpfold)conf/gm_tools/
	cp -rf cron/*                               $(crontab)  
	cp -rf htdocs/js                            $(webfold)
	cp -rf htdocs/css                           $(webfold)

	#Following lines are used only for testing.
	#install cgi/FrameWork/gm_index.php         $(phpfold)cgi-bin/gm_tools/

init:
