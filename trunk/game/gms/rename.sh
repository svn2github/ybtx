for a in `find /home/web_fold/WebGMTools/ -name "*.php"`; 
	do mv "$a" "${a%.php}"; 
	done
