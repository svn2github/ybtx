
function goto_page(page, url)
{
	var l = url + "page=" + page;
	document.location=l;
}

function CreatePage(cur, total, size, div_name, view_num, url)
{
	var div = document.getElementById(div_name);
	var all_page = parseInt((total-1)/size) + 1;
	var prev = "<a href='#' target='_self' onclick='goto_page(" + (cur - 1) + ",\"" + url + "\")'>Previous</a>";
	if (cur == 1)
		prev = "Previous";
	var next = "<a href='#' target='_self' onclick='goto_page(" + (cur + 1) + ",\"" + url + "\")'>Next</a>";
	if (cur == all_page)
		next = "Next";
	
	var begin = cur - parseInt(view_num/2);
	if (begin < 1)
		begin = 1;
	var end = begin + view_num - 1;
	if (end > all_page)
		end = all_page;
	
	var page_str = "";
	for (var i = begin; i < end; ++i)
	{
		if ( i == cur)
			page_str = page_str + "<b>" + i + "</b>|";
		else
			page_str = page_str + "<a href='#' target='_self' onclick='goto_page(" + i + ",\"" + url + "\")'>"+i+"</a>|";
	}

	if (next == "" || next == "Next")
		page_str = page_str + "<b>" + end + "</b>";
	else
		page_str = page_str + "<a href='#' target='_self' onclick='goto_page(" + end + ",\"" + url + "\")'>"+end+"</a>";
	

	first_page = "<a href='#' target='_self' onclick='goto_page(1,\"" + url + "\")'>First</a>&nbsp;&nbsp;&nbsp;&nbsp;";
	last_page = "&nbsp;&nbsp;&nbsp;&nbsp;<a href='#' target='_self' onclick='goto_page(" + all_page + ",\"" + url + "\")'>Last</a>";
	
	page_str = "<p align=center>" + first_page + prev + "&nbsp;&nbsp;&nbsp;&nbsp;" 
		+ page_str + "&nbsp;&nbsp;&nbsp;&nbsp;" + next + last_page + "&nbsp;&nbsp;&nbsp;&nbsp;Totally " + all_page + " Pages</p>";

	div.innerHTML = page_str;
}

