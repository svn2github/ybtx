#pragma once

namespace sqr
{
	template<class T>
	struct TVector3;
	typedef TVector3<float> CVector3f;

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
	bool CreateSphere(float radius, CVector3f** ppVertices, uint16** ppIndices, 
		int stacks, int slices, int* vertex_num, int* indices_num, 
		float theta_start, float theta_end, float phi_start, float phi_end
		);

}
