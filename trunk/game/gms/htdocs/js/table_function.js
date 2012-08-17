function order(obj){
	var i = $(obj).parent().children().index($(obj));
	var head = obj.parentNode;
	$(obj).siblings(".order,.asc,.desc").attr("class","order");
	var arr = head.parentNode.childNodes;
	var n = 0;
	var array = new Array();
	for(var f=0;f<arr.length-1;f++){
		if(arr[f+1].nodeType==1){
			var item = new Array();
			var node = arr[f+1];
			var t = 0;
			var j = 0;
			while(t<=i){
				if(node.childNodes[j].nodeType==1){
					item[0] = node.childNodes[j].firstChild.nodeValue;
					t++;
				}
				j++;
			}
			item[1] = arr[f+1];
			array[n++] = item;
		}
	}
	if($(obj).attr("class")=="order" || $(obj).attr("class")=="desc"){
		obj.className = "asc";
		array.sort(function compare(a,b){return a[0]-b[0];});
	}else{
		obj.className = "desc";
		array.sort(function compare(a,b){return b[0]-a[0];});
	}
	var table = obj.parentNode.parentNode;
	for(var ti = table.length; ti>1;ti--)
		table.children[ti-1].removeNode(true);
	for(var f=0;f<array.length;f++){
		var ele = array[f][1];
		if(f%2)
			ele.className = "even";
		else
			ele.className = "";
		table.appendChild(ele);
	}
}