$(function(){
	$("select[name='action_name']").change(function(){
		$("#form_body").html(tabs[$("select[name='action_name'] :selected").val()]);
	});
	$("textarea[name='role_name']").change(function(){Enter2Comma($(this));});
	$("form").attr("onsubmit","return onsubmit()");
});

function onsubmit(){
	var action = document.form1.action_name.value;
	if (!action || action == "" || !Number(action)){
		return false;
	}
}

function search(action, obj){
	var str = $(obj).parents("td").find(".str").val();
	var div = $(obj).parents("td").find(".pop");
	$.ajax({
		type: "post",
		url: "/cgi-bin/gm_tools/FrameWork.php",
		data: "action_name="+action+"&UID="+$("input[name='UID']").val()+"&gameId="+$("input[name='gameId']")+"&str_match="+str,
		success:function(msg){
			$(div).html(msg);
		}
	});
}

function Enter2Comma(obj){
	var str = $(obj).val();
	str = str.replace(/\n/g,",");
	$(obj).val(str);
}