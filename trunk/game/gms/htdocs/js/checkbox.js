function Checkbox(obj){
	var ele = $("textarea[name='selected_str']");
	var nam = $(obj).val();
	if(obj.checked){
		if($(ele).val().indexOf("\n"+nam+"\n")==-1 && $(ele).val().indexOf(nam+"\n")!=0)
			$(ele).val($(ele).val()+nam+"\n");
	}else{
		var list = $(ele).val();
		var sta = list.indexOf(nam+"\n");
		if(sta!=0){
			sta = list.indexOf("\n"+nam+"\n");
		}
		var end = sta + nam.length;
		var pre = list.substr(0,sta);
		var nex = list.substr(end+1);
		$(ele).val(pre+nex);
	}
}
function checkboxAll(obj,type){
	var ele = $("textarea[name='selected_str']");
	var eles = $(obj).parents("table").find("input[flag='"+type+"']");
	for(i=0;i<eles.length;i++){
		nam = $(eles[i]).val();
		if(obj.checked){
			eles[i].checked = true;
			if($(ele).val().indexOf("\n"+nam+"\n")==-1 && $(ele).val().indexOf(nam+"\n")!=0)
				$(ele).val($(ele).val()+nam+"\n");
		}else{
			eles[i].checked = false;
			var list = $(ele).val();
			var sta = list.indexOf(nam+"\n");
			if(sta!=0){
				sta = list.indexOf("\n"+nam+"\n");
			}
			if(sta!=-1){
				var end = sta + nam.length;
				var pre = list.substr(0,sta);
				var nex = list.substr(end+1);
				$(ele).val(pre+nex);
			}
		}
	}
}