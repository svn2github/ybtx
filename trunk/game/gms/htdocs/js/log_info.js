function log_info(type, obj){
	$(obj).css("font-weight",($(obj).css("font-weight")=="bold")?"normal":"bold");
	$(obj).next("div").slideToggle();
}

function role_detail_(obj){
	document.form1.serverid.value = $(obj).parents("tr").find(".server").attr("title");
	document.form1.role_name.value = $(obj).parents("span").find("a").html();
	document.form1.action_name.value = "97";
	document.form1.submit();
}

function user_detail(obj){
	document.form1.serverid.value = $(obj).parents("tr").find(".server").attr("title");
	document.form1.urs.value = $(obj).parents("span").find("a").html();
	document.form1.action_name.value = "50";
	document.form1.submit();
}

function tong_detail(obj){
	document.form1.serverid.value = $(obj).parents("tr").find(".server").attr("title");
	document.form1.str.value = $(obj).parents("span").find("a").html();
	document.form1.action_name.value = "63";
	document.form1.submit();
}

function role_filter(obj){
	var url_str = clearPageURL($(obj).parents(".panel").find("input[name='url']").val());
	var panel = $(obj).parents(".panel");
	$(panel).css("height",$(panel).height());
	$(panel).html("Loading...");
	$.ajax({
		type:"get",
		url:url_str,
		data:"role_name="+$(obj).parents("span").find("a").html()+"&flag=role",
		success:function(msg){
			$(panel).html(msg);
			$(panel).css("height","auto");
		}
	});
}

function user_filter(obj){
	var url_str = clearPageURL($(obj).parents(".panel").find("input[name='url']").val());
	var panel = $(obj).parents(".panel");
	$(panel).css("height",$(panel).height());
	$(panel).html("Loading...");
	$.ajax({
		type:"get",
		url:url_str,
		data:"role_name="+$(obj).parents("span").find("a").html()+"&flag=user",
		success:function(msg){
			$(panel).html(msg);
			$(panel).css("height","auto");
		}
	});
}

function tong_filter(obj){
	var url_str = clearPageURL($(obj).parents(".panel").find("input[name='url']").val());
	var panel = $(obj).parents(".panel");
	$(panel).css("height",$(panel).height());
	$(panel).html("Loading...");
	$.ajax({
		type:"get",
		url:url_str,
		data:"role_name="+$(obj).parents("span").find("a").html()+"&flag=tong",
		success:function(msg){
			$(panel).html(msg);
			$(panel).css("height","auto");
		}
	});
}

function role_select(obj){
	var type = "role"
	var ele = $("textarea[name='"+type+"_str']");
	var nam = $(obj).parents("span").find("a").html();
	if($(ele).val().indexOf(","+nam+",")==-1 && $(ele).val().indexOf(nam+",")!=0)
		$(ele).val($(ele).val()+nam+",");
}

function user_select(obj){
	var type = "user"
	var ele = $("textarea[name='"+type+"_str']");
	var nam = $(obj).parents("span").find("a").html();
	if($(ele).val().indexOf(","+nam+",")==-1 && $(ele).val().indexOf(nam+",")!=0)
		$(ele).val($(ele).val()+nam+",");
}