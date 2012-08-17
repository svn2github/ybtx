#include "stdafx.h"
#include "ModelHelper.h"
#include "CMath.h"
#include "CVector3.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	/**
	* @param
	*	stacks 
	*		纬度切面数目
	*	slices 
	*		精度切面数目
	*	vertex_num 
	*		返回生成球体的顶点数目
	*	indices_num
	*		返回生成球体的定点索引数目
	*	theta_start
	*		纬度起始角度(弧度)
	*	theta_end
	*		纬度结束角度(弧度)
	*	phi_start
	*		经度起始角度(弧度, 由下至上, 最下为-PI/2 )
	*	phi_end
	*		经度结束角度(弧度, 由下至上, 最上为PI/2)
	*/
	bool CreateSphere(
		float radius,		CVector3f** ppVertices, uint16** ppIndices, 
		int	  stacks,		int slices,				int* vertex_num,		int* indices_num, 
		float theta_start	= 0.0f,					float theta_end = CMath::two_pi, 
		float phi_start		= -CMath::half_pi,		float phi_end	= CMath::half_pi
		)
	{
		*ppVertices = 0;
		*ppIndices	= 0;

		int num_vertices = (stacks+1)*(slices+1);
		int num_trangles = stacks*slices*2;
		int num_indices	 = num_trangles*3;
		*vertex_num		 = num_vertices;
		*indices_num	 = num_indices;

		CVector3f* pVertices = new CVector3f[num_vertices];
		if ( !pVertices )
			return false;

		uint16* pIndices = new uint16[num_indices];
		if ( !pIndices )
		{
			SafeDelAry(pVertices);
			return false;
		}

		*ppVertices = pVertices;
		*ppIndices	= pIndices;

		const float theta_total	= theta_end - theta_start;
		const float phi_total	= phi_end - phi_start;
		const float theta_inc	= theta_total/stacks;
		const float phi_inc		= phi_total/slices;

		int	  ver_index = 0;	
		float theta		= theta_start;
		float sin_theta = 0;
		float cos_theta = 0;
		float sin_phi	= 0;
		float cos_phi	= 0;

		for ( int cur_stack = 0; cur_stack <= stacks; ++cur_stack )
		{
			float phi = phi_start;
			sin_theta = CMath::Sin(theta);
			cos_theta = CMath::Cos(theta);

			for ( int cur_slice = 0; cur_slice <= slices; ++cur_slice, ++ver_index )
			{
				sin_phi = CMath::Sin(phi);
				cos_phi	= CMath::Cos(phi);

				pVertices[ver_index].x = radius * cos_phi * cos_theta;
				pVertices[ver_index].y = radius * sin_phi;
				pVertices[ver_index].z = radius * cos_phi * sin_theta;

				phi += phi_inc;
			}

			theta += theta_inc;
		}

		int base	= 0;
		int index	= 0;

		for ( int cur_stack = 0; cur_stack < stacks; ++cur_stack )
		{
			for ( int cur_slice = 0; cur_slice < slices; ++cur_slice )
			{
				pIndices[index++] = base;
				pIndices[index++] = base + 1;
				pIndices[index++] = base + slices + 1;

				pIndices[index++] = base + 1;
				pIndices[index++] = base + slices + 2;
				pIndices[index++] = base + slices + 1;
				base++;
			}
			base++;
		}

		return true;
	}

}
