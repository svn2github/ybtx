function topage(obj){
	var url_str = clearPageURL($(obj).parents(".panel").find("input[name='url']").val());
	var panel = $(obj).parents(".panel");
	$(panel).css("height",$(panel).height());
	$(panel).html("Loading...");
	$.ajax({
		type:"get",
		url:url_str,
		data:"page="+$(obj).val(),
		success:function(msg){
			$(panel).html(msg);
			$(panel).css("height","auto");
		}
	});
}

function toFilter(obj,field,val){
	var url_str = clearPageURL(clearFilterURL($(obj).parents(".panel").find("input[name='url']").val()));
	var panel = $(obj).parents(".panel");
	$(panel).css("height",$(panel).height());
	$(panel).html("Loading...");
	var v = "";
	if(typeof(val) == "undefined"){
		v = $(obj).html();
	}else{
		v = val;
	}
	$.ajax({
		type:"get",
		url:url_str,
		data:"page_field="+field+"&page_value="+v,
		success:function(msg){
			$(panel).html(msg);
			$(panel).css("height","auto");
		}
	});
}

function toNoFilter(obj){
	var url_str = clearPageURL(clearFilterURL($(obj).parents(".panel").find("input[name='url']").val()));
	var panel = $(obj).parents(".panel");
	
	$(panel).css("height",$(panel).height());
	$(panel).html("Loading...");
	$.ajax({
		type:"get",
		url:url_str,
		data:"page=1",
		success:function(msg){
			$(panel).html(msg);
			$(panel).css("height","auto");
		}
	});
}

function clearFilterURL(url_str){
	var hasField = true;
	while(hasField){
		var i1 = url_str.indexOf("&page_field=");
		if(i1 != -1){
			var i2 = url_str.indexOf("&",i1+7);
			if(i2 != -1){
				url_str = url_str.substr(0,i1) + url_str.substr(i2);
			}else{
				url_str = url_str.substr(0,i1)
			}
		}else{
			i1 = url_str.indexOf("page_field=");
			if(i1 != -1){
				var i2 = url_str.indexOf("&",i1+6);
				if(i2 != -1){
					url_str = url_str.substr(0,i1) + url_str.substr(i2+1);
				}else{
					url_str = url_str.substr(0,i1)
				}
			}else{
				hasField = false;
			}
		}
	}
	
	var hasValue = true;
	while(hasValue){
		var i1 = url_str.indexOf("&page_value=");
		if(i1 != -1){
			var i2 = url_str.indexOf("&",i1+7);
			if(i2 != -1){
				url_str = url_str.substr(0,i1) + url_str.substr(i2);
			}else{
				url_str = url_str.substr(0,i1)
			}
		}else{
			i1 = url_str.indexOf("page_value=");
			if(i1 != -1){
				var i2 = url_str.indexOf("&",i1+6);
				if(i2 != -1){
					url_str = url_str.substr(0,i1) + url_str.substr(i2+1);
				}else{
					url_str = url_str.substr(0,i1)
				}
			}else{
				hasValue = false;
			}
		}
	}
	
	var hasFlag = true;
	while(hasFlag){
		var i1 = url_str.indexOf("&flag=");
		if(i1 != -1){
			var i2 = url_str.indexOf("&",i1+7);
			if(i2 != -1){
				url_str = url_str.substr(0,i1) + url_str.substr(i2);
			}else{
				url_str = url_str.substr(0,i1)
			}
		}else{
			i1 = url_str.indexOf("flag=");
			if(i1 != -1){
				var i2 = url_str.indexOf("&",i1+6);
				if(i2 != -1){
					url_str = url_str.substr(0,i1) + url_str.substr(i2+1);
				}else{
					url_str = url_str.substr(0,i1)
				}
			}else{
				hasFlag = false;
			}
		}
	}
	
	var hasName = true;
	while(hasName){
		var i1 = url_str.indexOf("&role_name=");
		if(i1 != -1){
			var i2 = url_str.indexOf("&",i1+7);
			if(i2 != -1){
				url_str = url_str.substr(0,i1) + url_str.substr(i2);
			}else{
				url_str = url_str.substr(0,i1)
			}
		}else{
			i1 = url_str.indexOf("role_name=");
			if(i1 != -1){
				var i2 = url_str.indexOf("&",i1+6);
				if(i2 != -1){
					url_str = url_str.substr(0,i1) + url_str.substr(i2+1);
				}else{
					url_str = url_str.substr(0,i1)
				}
			}else{
				hasName = false;
			}
		}
	}
	return url_str;
}

function clearPageURL(url_str){
	var hasPage = true;
	while(hasPage){
		var i1 = url_str.indexOf("&page=");
		if(i1 != -1){
			var i2 = url_str.indexOf("&",i1+7);
			if(i2 != -1){
				url_str = url_str.substr(0,i1) + url_str.substr(i2);
			}else{
				url_str = url_str.substr(0,i1)
			}
		}else{
			i1 = url_str.indexOf("page=");
			if(i1 != -1){
				var i2 = url_str.indexOf("&",i1+6);
				if(i2 != -1){
					url_str = url_str.substr(0,i1) + url_str.substr(i2+1);
				}else{
					url_str = url_str.substr(0,i1)
				}
			}else{
				hasPage = false;
			}
		}
	}
	return url_str;
}