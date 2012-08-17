all: install
	
wan: install_wan
	

install:
	cp php/cgi/* /home/web_fold/cgi-bin/LogFree/
	cp php/conf/config.php.template /home/web_fold/conf/LogFree/config.php
	chmod +x /home/web_fold/cgi-bin/LogFree/*
	cp php/lib/* /home/web_fold/lib/LogFree/
	cp -rf htdocs/js /var/www/html/
	cp -rf htdocs/css /var/www/html/
	cp -rf htdocs/img /var/www/html/
	sed -i "s,in_or_out,ÄÚÍø,g" /home/web_fold/conf/LogFree/*

#install_wan:
#	cp php/cgi/* /home/web_fold/cgi-bin/LogFree_WAN/
#	cp php/conf/config.php.template /home/web_fold/conf/LogFree_WAN/config.php
#	chmod +x /home/web_fold/cgi-bin/LogFree_WAN/*
#	cp php/lib/* /home/web_fold/lib/LogFree_WAN/
#	cp -rf htdocs/js /var/www/html/
#	cp -rf htdocs/css /var/www/html/
#	cp -rf htdocs/img /var/www/html/
#	cp -rf htdocs/LogFree.html /var/www/html/
#	sed -i "s,LogFree,LogFree_WAN,g" /home/web_fold/cgi-bin/LogFree_WAN/*
#	sed -i "s,LogFree,LogFree_WAN,g" /home/web_fold/lib/LogFree_WAN/*
#	sed -i "s,LogFree,LogFree_WAN,g" /home/web_fold/conf/LogFree_WAN/*
#	sed -i "s,in_or_out,ÍâÍø,g" /home/web_fold/conf/LogFree_WAN/*
