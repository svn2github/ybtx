function setCookie(sectionid, htmlName, serverID)
{
	var expires = new Date();
	expires.setTime(expires.getTime() + 36 * 30 * 24 * 60 * 60 * 1000);
	/*   三十六个月 x 一个月当作 30 天 x 一天 24 小时
   	x 一小时 60 分 x 一分 60 秒 x 一秒 1000 毫秒 */
	
	document.cookie ='section=' + sectionid + ' htm=' + htmlName + ' id=' + serverID + ' ;expires=' + expires.toGMTString();
}

function getCookie(cookieName)
{
  var cookieString = document.cookie;
  var start = cookieString.indexOf(cookieName + '=');
  // 加上等号的原因是避免在某些 Cookie 的值里有
  // 与 cookieName 一样的字符串。
  if (start == -1) // 找不到
    return null;
  start += cookieName.length + 1;
  var end = cookieString.indexOf(' ', start);
  if (end == -1)
  	end = cookieString.indexOf(';', start);
  if (end == -1) return cookieString.substring(start);
  return cookieString.substring(start, end);
}

function doColor(id, color, bg) 
{
	var item;
	for(var i =1; i<=document.anchors.length; i++)
	{
		item = document.getElementById(i+'');
    		item.style.color = "#000000";

		item.style.backgroundColor = "";
	}
	if(id==null) return;
	item = document.getElementById(id+'');
	item.style.color = color;
	item.style.backgroundColor = bg;
}

function SetRightHtml()
{
	var rightHtml;
	if(getCookie('htm') == null)
		rightHtml='southern.htm';
	else
		rightHtml=getCookie('htm')+'.htm';
	top.frames['serverlist'].location.replace(rightHtml);
}

function ClickLink(id)
{
	top.location.href = document.getElementById(id+'').href;
}

function OnMouseDown(sectionid, area, obj)
{
	doColor(obj.id,'blue','red')
	setCookie(sectionid ,area, String(obj.id) )
}

function OnLoad()
{
	if(getCookie('section') == 1)
	{
		doColor(getCookie('id'), 'blue', 'red');
		ClickLink(getCookie('id'));
	}
	else
	{
		doColor(1, 'blue', 'red');
		ClickLink(1);
	}
}
