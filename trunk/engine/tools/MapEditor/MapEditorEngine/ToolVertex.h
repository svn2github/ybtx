#pragma once
#include "CEventOwner.h"
#include "TSingleton.h"

namespace sqr
{
	class CLogicBrush;

	class CToolVertex: public Singleton<CToolVertex>
	{
	private:
		HWND m_hVertexView;

		CLogicBrush *m_pLogicBrush;
		DWORD	    m_dwColor;
		float       m_fVertexStrength;

		void OperatorVertexColor();
	public:

		CToolVertex();
		~CToolVertex();
		void OnEvent(const CMsg& msg);
		void Render();
		void Intialize(HWND hVertexView);

		void IncBrushSizeInX();
		void DecBrushSizeInX();

		void IncBrushSizeInZ();
		void DecBrushSizeInZ();

		void PickColor();
		void RevertColor();
		void DeleteVertexColor();

		void SetGridsEffectObject(const bool bEffectObject);
		void SetAllObjectsIsEffectByVertexColor(const bool bEffectObject);

		///bDiffuse = true 还原所有的顶点diffuse颜色到初始灰色, bDiffuse = false还原所有的顶点specular颜色到初始灰色
		void RevertVertexColorToIntial(bool bDiffuse);

		///给所有顶点着同一色，index = 0 表示是diffuse， index = 2表示是specular
		void SetAllVertexToColor(DWORD color, int index = 0);

		inline void SetVertexColor(const DWORD dw)
		{
			m_dwColor = dw;
		}

		inline DWORD GetVertexColor() const
		{
			return m_dwColor;
		}

		inline void SetVertexStrength(const float f)
		{
			m_fVertexStrength = f;
		}

		inline float GetVertexStrength() const
		{
			return m_fVertexStrength;
		}
	};
}