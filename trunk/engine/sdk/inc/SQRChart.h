#pragma once
#include "SQRControl.h"

namespace sqr
{
	class SQRChartGraphicItem;
	class SQRChartLineGraph;
	class SQRChartHistogram;
	typedef SQRChartGraphicItem* HGRAPHICITEM;

	/// @brief 图表类。目前支持折线图和柱状图
	class GUI_API SQRChart : public SQRControl
	{
		DYNAMIC_DECLARE_WND(SQRChart)
	public:
		enum EGraphicItemType
		{
			EGraphicItem_LineGraph = 0,
			EGraphicItem_Histogram = 1,
		};
	public:
		SQRChart();
		virtual ~SQRChart();
	public: // 从基类继承的虚函数
		virtual int32 Create( const WndCreateParam& param );
		virtual void DrawWndBackground();
		virtual void DrawWndText();
		virtual void OnMouseMove( uint32 nFlags, int32 x, int32 y );
	public: // 逻辑相关
		void Clear();
		size_t Count() const;
		HGRAPHICITEM AppendGraphicItem(EGraphicItemType type);
		HGRAPHICITEM GetGraphicItem(int index);

		static bool		IsHasEventState(EventStateMask Mask);
		virtual bool	ObjHasEventState(EventStateMask Mask);
	private:
		UIVector<HGRAPHICITEM> m_graphicItemList;
	};

	class SQRChartGraphicItem : public virtual CDynamicObject
	{
		friend class SQRChart;
	protected:
		typedef float						node_type;
		typedef UIVector<node_type>		container_type;
		typedef container_type::iterator	iterator;
	protected:
		container_type m_data;				///< 存贮y值的数组
		bool m_visible;						///< 可见性标记
		SQRChart* m_phost;					///< 父容器的引用
	public:
		SQRChartGraphicItem(SQRChart* phost = NULL);
		virtual ~SQRChartGraphicItem();
	public:
		size_t Count()const;
		bool Empty()const;
		bool GetVisible()const;
		void SetVisible(bool visilbe);
		virtual void Clear();
		SQRChart* GetHost();
	protected:
		virtual void OnRender() {}
		virtual void OnMouseMove(uint32 hostIndex, uint32 nFlags, int32 x, int32 y );
	};

	class SQRChartLineGraph : public SQRChartGraphicItem
	{
	private:
		float m_zoomY;						///< y轴缩放系数
		float m_deltaY;						///< y轴偏移
		float m_deltaX;						///< x轴偏移
		float m_spanX;						///< 水平方向的间隔距离
		uint32 m_color;						///< 线条颜色
	public:
		SQRChartLineGraph(SQRChart* phost = NULL);
		~SQRChartLineGraph();
	public: // 属性操作
		void AppendData(float v);
		uint32 GetColor()const;
		void SetColor(uint32 color);
		float GetZoomY()const;
		void SetZoomY(float zoomY);
		float GetDeltaY()const;
		void SetDeltaY(float deltaY);
		float GetDeltaX()const;
		void SetDeltaX(float deltaX);
		float GetSpanX()const;
		void SetSpanX(float spanX);
	private:
		int HitCheck(int32 x, int32 y);
	protected: // 虚函数
		virtual void OnRender();
		virtual void OnMouseMove(uint32 hostIndex, uint32 nFlags, int32 x, int32 y);
	protected: // 回调函数
		/// @brief 用于显示ToolTips的内容
		virtual void OnHitPoint(int32 hostIndex, int32 x, int32 y, int index, float value);

	};
	class SQRChartHistogram : public SQRChartGraphicItem
	{
	private:
		float m_zoomY;								///< y轴缩放系数
		float m_itemWidth;							///< 柱状图的绘制宽度
		float m_spanX;								///< 水平方向的间隔距离
		UIVector< pair<uint32, uint32> > m_color;		///< 对应y值的颜色数组，first为填充颜色，second为文字颜色
	public:
		SQRChartHistogram(SQRChart* phost = NULL);
		virtual ~SQRChartHistogram();
	public: // 属性操作
		/// @brief 向容器追加数据
		/// @param colorText 文字标签的颜色，0表示colorFill的反色
		void AppendData(float v, uint32 colorFill, uint32 colorText = 0);
		float GetZoomY()const;
		void SetZoomY(float zoomY);
		float GetItemWidth()const;
		void SetItemWidth(float itemWidth);
		float GetSpanX()const;
		void SetSpanX(float spanX);
	protected: // 虚函数
		virtual void OnRender();
		virtual void Clear();
	};
}