#include "stdafx.h"
#include "SQRChart.h"
#include "SQRGuiManager.h"
#include "StringHelper.h"
#include "CColor.inl"

#include "TSqrAllocator.inl"

DYNAMIC_CREATE_WND(SQRChart, SQRControl)

SQRChart::SQRChart()
{
}
SQRChart::~SQRChart()
{
	Clear();
}
int32 SQRChart::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD | WS_IGNOREWHEEL;
	int32 re = SQRWnd::Create( newParam );
	return re;
}
void SQRChart::DrawWndBackground()
{
	m_beBGDrawed = true;
	if(m_graphicItemList.empty() == false)
	{
		for(UIVector<SQRChartGraphicItem*>::iterator it = m_graphicItemList.begin();
			it != m_graphicItemList.end(); ++it)
		{
			if((*it)->GetVisible())
				(*it)->OnRender();
		}
	}
}
void SQRChart::DrawWndText()
{
}


void SQRChart::Clear()
{
	for(UIVector<HGRAPHICITEM>::iterator it = m_graphicItemList.begin();
		it != m_graphicItemList.end(); ++it)
	{
		(*it)->Clear();
		delete (*it);
	}
	m_graphicItemList.clear();
}

bool SQRChart::IsHasEventState(EventStateMask Mask)
{
	return SQRWnd::IsHasEventState(Mask);
}

bool SQRChart::ObjHasEventState(EventStateMask Mask)
{
	return SQRChart::IsHasEventState(Mask);
}

HGRAPHICITEM SQRChart::AppendGraphicItem( EGraphicItemType type )
{
	switch(type)
	{
	case EGraphicItem_LineGraph:
		{
			SQRChartLineGraph* item = new SQRChartLineGraph(this);
			m_graphicItemList.push_back(item);
			return item;
		}
	case EGraphicItem_Histogram:
		{
			SQRChartHistogram* item = new SQRChartHistogram(this);
			m_graphicItemList.push_back(item);
			return item;
		}
	default:
		return NULL;
	}
}

HGRAPHICITEM SQRChart::GetGraphicItem( int index )
{
	if(index >= 0 && index < static_cast<int>(Count()))
		return m_graphicItemList[index];
	return NULL;
}

size_t SQRChart::Count() const
{
	return m_graphicItemList.size();
}

void SQRChart::OnMouseMove( uint32 nFlags, int32 x, int32 y )
{
	if(m_graphicItemList.empty())
		return;
	for(size_t i = 0; i < m_graphicItemList.size(); i++)
	{
		HGRAPHICITEM item = m_graphicItemList[i];
		if(item->GetVisible())
			item->OnMouseMove(i, nFlags, x, y);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQRChartGraphicItem::SQRChartGraphicItem(SQRChart* phost)
: m_visible(true)
, m_phost(phost)
{
}
SQRChartGraphicItem::~SQRChartGraphicItem()
{
}
size_t SQRChartGraphicItem::Count() const
{
	return m_data.size();
}

bool SQRChartGraphicItem::Empty() const
{
	return m_data.empty();
}

bool SQRChartGraphicItem::GetVisible() const
{
	return m_visible;
}

void SQRChartGraphicItem::SetVisible( bool visilbe )
{
	m_visible = visilbe;
}

void SQRChartGraphicItem::Clear()
{
	m_data.clear();
}
SQRChart* SQRChartGraphicItem::GetHost()
{
	return m_phost;
}
void SQRChartGraphicItem::OnMouseMove(uint32 hostIndex, uint32 nFlags, int32 x, int32 y )
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQRChartLineGraph::SQRChartLineGraph(SQRChart* phost) 
: SQRChartGraphicItem(phost)
, m_zoomY(1.0f)
, m_deltaY(0.0f)
, m_deltaX(0)
, m_spanX(20.0f)
, m_color(CColor::Blue)
{
}
SQRChartLineGraph::~SQRChartLineGraph()
{
}
void SQRChartLineGraph::AppendData(float v)
{
	m_data.push_back(v);
}
void SQRChartLineGraph::OnRender()
{
	if(GetVisible() == false || Empty())
		return;

	// 绘制常数计算
	CFRect wndRect;
	m_phost->GetWndRect(wndRect);
	const float height = wndRect.bottom - wndRect.top;
	//const float zoomSize = m_phost->m_hWnd->m_pGUI->GetZoomSize();
	const PointGroup default_pg = {CVector3f(0.0f, 0.0f, 0.0f), GetColor()};

	// 绘制数据准备
	UIVector<PointGroup> pgList(Count(), default_pg);
	for(size_t i = 0; i < Count(); i++)
	{
		pgList[i].vPoint.x = (wndRect.left + i * m_spanX + m_deltaX);// * zoomSize;
		pgList[i].vPoint.y = (wndRect.top  + height - m_data[i] * m_zoomY - m_deltaY);// * zoomSize;
	}

	// 数据点的绘制
	for(UIVector<PointGroup>::const_iterator it = pgList.begin(); it != pgList.end(); ++it)
	{
		CFRect rect(it->vPoint.x - 2, it->vPoint.y - 2, it->vPoint.x + 2, it->vPoint.y + 2);
		m_phost->GetGraphic()->DrawRect(rect, m_color, NULL);
	}

	// 线的绘制
	if(pgList.size() > 1)
	{
		m_phost->GetGraphic()->DrawLine(&(pgList[0]), static_cast<int32>(pgList.size()), true, m_phost->GetWndHandle()->m_eRS );
	}
}

void SQRChartLineGraph::SetColor( uint32 color )
{
	m_color = color;
}

uint32 SQRChartLineGraph::GetColor() const
{
	return m_color;
}
float SQRChartLineGraph::GetZoomY()const
{
	return m_zoomY;
}
void SQRChartLineGraph::SetZoomY(float zoomY)
{
	m_zoomY = zoomY;
}
float SQRChartLineGraph::GetDeltaY()const
{
	return m_deltaY;
}
void SQRChartLineGraph::SetDeltaY(float deltaY)
{
	m_deltaY = deltaY;
}
float SQRChartLineGraph::GetDeltaX()const
{
	return m_deltaX;
}
void SQRChartLineGraph::SetDeltaX(float deltaX)
{
	m_deltaX = deltaX;
}
float SQRChartLineGraph::GetSpanX()const
{
	return m_spanX;
}
void SQRChartLineGraph::SetSpanX(float spanX)
{
	m_spanX = spanX;
}
int SQRChartLineGraph::HitCheck(int32 x, int32 y)
{
	x -=  static_cast<int32>(m_deltaX);
	const float delta = 2.0f; // 鼠标捕获的误差半径
	//const float zoomSize = m_phost->m_hWnd->m_pGUI->GetZoomSize();
	const float zoomSize = 1.0f;
	const float span = GetSpanX() * zoomSize;
	size_t point_x = 0;
	// x方向检测
	if(span - delta * 2 > 0)
	{
		// |<---- span --->|
		// |++++-------++++|
		// | A |   C   | B |
		const float r = fmod(static_cast<float>(x), span);
		
		if(r < delta) // 落在前命中区间A
			point_x = static_cast<int>(x / span);
		else if(r > (GetSpanX() - delta)) // 落在后命中区间B
			point_x = static_cast<int>(x / span) + 1;
		else // 落在C区间
			return -1;
	}
	else // delta区间有交叠
	{
		point_x = static_cast<int>(x / span);
		if(fmod(static_cast<float>(x), span) > 0.5f)
			++point_x;
	}
	// y方向检测
	if(point_x < Count())
	{
		CFRect wndRect;
		m_phost->GetWndRect(wndRect);
		const float height = wndRect.bottom - wndRect.top;
		const float value_y = (height - m_data[point_x] * m_zoomY - m_deltaY) * zoomSize;
		if(fabs(value_y - y) < delta)
			return point_x;
	}
	return -1;
}
void SQRChartLineGraph::OnMouseMove(uint32 hostIndex, uint32 nFlags, int32 x, int32 y)
{
	const int index = HitCheck(x, y);
	if(index >= 0)
	{
		//TRUSERLINE("lk-yangmingzhe", "mouse x,y=%d,%d point x,y=%d,%f", x, y, index, m_data[index]);
		OnHitPoint(hostIndex, x, y, index, m_data[index]);
	}
	else
	{
		m_phost->SetMouseOverDesc("");
	}
}
void SQRChartLineGraph::OnHitPoint(int32 hostIndex, int32 x, int32 y, int index, float value)
{
	CALL_CLASS_CALLBACK_5(hostIndex, x, y, index, value)
	m_phost->SetMouseOverDesc(format("mouse x,y=%d,%d  value x,y=%d,%f", x, y, index, m_data[index]).c_str());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQRChartHistogram::SQRChartHistogram(SQRChart* phost) 
: SQRChartGraphicItem(phost)
, m_zoomY(1.0f)
, m_itemWidth(10.0f)
, m_spanX(20.0f)
{
}
SQRChartHistogram::~SQRChartHistogram()
{
}
void SQRChartHistogram::AppendData(float v, uint32 colorFill, uint32 colorText)
{
	m_data.push_back(v);
	if(colorText == 0)
		colorText = CColor(colorFill).Reverse();
	m_color.push_back(make_pair(colorFill, colorText));
	Ast(m_data.size() == m_color.size());
}
void SQRChartHistogram::Clear()
{
	m_color.clear();
	SQRChartGraphicItem::Clear();
}
float SQRChartHistogram::GetZoomY()const
{
	return m_zoomY;
}
void SQRChartHistogram::SetZoomY(float zoomY)
{
	m_zoomY = zoomY;
}
float SQRChartHistogram::GetItemWidth()const
{
	return m_itemWidth;
}
void SQRChartHistogram::SetItemWidth(float itemWidth)
{
	m_itemWidth = itemWidth;
}
float SQRChartHistogram::GetSpanX()const
{
	return m_spanX;
}
void SQRChartHistogram::SetSpanX(float spanX)
{
	m_spanX = spanX;
}

void SQRChartHistogram::OnRender()
{
	if(GetVisible() == false || Empty())
		return;
	Ast(m_color.size() == m_data.size());
	CFRect wndRect;
	m_phost->GetWndRect(wndRect);
	const float height = wndRect.bottom - wndRect.top;
	//const float zoomSize = m_phost->m_hWnd->m_pGUI->GetZoomSize();
	
	// 绘制填充矩形
	CFRect rect;
	for(size_t i = 0; i < Count(); i++)
	{
		rect.left = (wndRect.left + i * m_spanX);// * zoomSize;
		rect.top = (wndRect.top  + height - m_data[i] * m_zoomY);// * zoomSize;
		rect.right = (wndRect.left + i * m_spanX + m_itemWidth);// * zoomSize;
		rect.bottom = float(wndRect.top  + height);// * zoomSize;
		m_phost->GetGraphic()->DrawRect(rect, m_color[i].first, NULL);
	}
	// 绘制文字标注
	//   0不绘制，柱过低时将字画在上面
	wstring str;
	for(size_t i = 0; i < Count(); i++)
	{
		if(m_data[i] == 0)
			continue;
		format(str, L"%d", static_cast<int>(m_data[i]));
		const float x = ((i * m_spanX + (m_itemWidth - str.size() * m_phost->GetFontSize() * 0.5f) * 0.5f));
		float y = 0.0f;
		if(m_data[i] * m_zoomY <= m_phost->GetFontSize() + 8.0f)
			y = height - (m_data[i] * m_zoomY - 2.0f);
		else
			y = height - ((m_data[i] * m_zoomY + m_phost->GetFontSize()) * 0.5f);
		m_phost->DrawText(str, x, y, m_color[i].second, 0, false, NULL);
	}
}


