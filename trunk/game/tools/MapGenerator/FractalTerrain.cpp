
#include "stdafx.h"

#include <math.h>
#include <time.h>


#include "FractalTerrain.h"



#define random() rand()
#define srandom(x) srand(x)

// 取随机数
static float randnum (float min, float max)
{
	int r;
	float	x;

	r = random ();
	x = (float)(r & 0x7fff) / (float)0x7fff;
	return (x * (max - min) + min);
}


#define fractRand(v) randnum (-v, v)


static int powerOf2 (int size)
{
	int i;
	int bitcount = 0;

	for (i=0; i<sizeof(int)*8; i++)
		if (size & (1<<i))
			bitcount++;
	if (bitcount == 1)
		return 1;
	else
		return 0;
}



FractalTerrain::FractalTerrain()
	: m_iGridPixel(4)
	, m_iStep(4)
	, m_iSize(128)  // 地形矩阵尺寸
	, m_iWidth(160) // 地质矩阵尺寸
	, m_fHeightScale(1.f)
	, m_fH(0.7f)
{}

FractalTerrain::~FractalTerrain()
{}

// 分配
void FractalTerrain::Alloc2DFractArray()
{
	int iSize = m_iSize + 1;

	m_Terrain = new float[iSize * iSize]; // 矩阵的显示区域与矩阵大小不同
	m_Geology = new int[m_iWidth * m_iWidth];
}

// 释放
void FractalTerrain::FreeFractArray()
{
	delete [] m_Terrain;
	delete [] m_Geology;
}

// 打印
void FractalTerrain::Dump2DFractArray()
{
	int i, j;

	for (i=0; i<m_iSize; i++)
	{
		j=0;
		printf ("[%d,%d]: ", i, j);
		for (; j<m_iSize; j++)
		{
			printf ("%.2f\t", m_Terrain[(i*m_iSize)+j]);
		}
		printf ("\n");
	}
}


// 菱锥算法
float FractalTerrain::avgDiamondVals (int i, int j, int stride, int size, int subSize)
{
	if (i == 0)
		return ((float) (m_Terrain[(i*size) + j-stride] +
                         m_Terrain[(i*size) + j+stride] +
                         m_Terrain[((subSize-stride)*size) + j] +
                         m_Terrain[((i+stride)*size) + j]) * .25f);
	else if (i == size-1)
		return ((float) (m_Terrain[(i*size) + j-stride] +
                         m_Terrain[(i*size) + j+stride] +
                         m_Terrain[((i-stride)*size) + j] +
                         m_Terrain[((0+stride)*size) + j]) * .25f);
	else if (j == 0)
		return ((float) (m_Terrain[((i-stride)*size) + j] +
                         m_Terrain[((i+stride)*size) + j] +
                         m_Terrain[(i*size) + j+stride] +
                         m_Terrain[(i*size) + subSize-stride]) * .25f);
	else if (j == size-1)
		return ((float) (m_Terrain[((i-stride)*size) + j] +
                         m_Terrain[((i+stride)*size) + j] +
                         m_Terrain[(i*size) + j-stride] +
                         m_Terrain[(i*size) + 0+stride]) * .25f);
	else
		return ((float) (m_Terrain[((i-stride)*size) + j] +
                         m_Terrain[((i+stride)*size) + j] +
                         m_Terrain[(i*size) + j-stride] +
                         m_Terrain[(i*size) + j+stride]) * .25f);
}

// 四角算法
float FractalTerrain::avgSquareVals (int i, int j, int stride, int size)
{
	return ((float) (m_Terrain[((i-stride)*size) + j-stride] +
                    m_Terrain[((i-stride)*size) + j+stride] +
                    m_Terrain[((i+stride)*size) + j-stride] +
                    m_Terrain[((i+stride)*size) + j+stride]) * .25f);
}

// 算法框架
void FractalTerrain::Fill2DFractArray(int seedValue)
{
	int i, j;
	int stride;
	int oddline;
	int subSize;
	float ratio, scale;

	int size = m_iSize;
	if (!powerOf2(size) || (size==1))
		return;

	subSize = size;
	++size;

	// 初始化随机数
	srandom (seedValue);

	// 设置粒度
	ratio = (float) pow (2.f, -m_fH);
	scale = m_fHeightScale * ratio;

	// 初始化四角数据
	m_Terrain[(0*size)+0] = 0.f;
	m_Terrain[(subSize*size)+0] = 0.f;
	m_Terrain[(subSize*size)+subSize] = 0.f;
	m_Terrain[(0*size)+subSize] = 0.f;


	stride = subSize / 2;
	while (stride)
	{
		// 通过四角计算中点值
		for (i=stride; i<subSize; i+=stride)
		{
			for (j=stride; j<subSize; j+=stride)
			{
				m_Terrain[(i * size) + j] = scale * fractRand (.5f) + avgSquareVals (i, j, stride, size);
				j += stride;
			}
			i += stride;
		}


		// 计算各凌锥中心
		oddline = 0;
		for (i=0; i<subSize; i+=stride)
		{
			oddline = (oddline == 0);
			for (j=0; j<subSize; j+=stride)
			{
				if ((oddline) && !j)
					j+=stride;

				// 计算凌锥中点值
				m_Terrain[(i * size) + j] = scale * fractRand (.5f) + avgDiamondVals (i, j, stride, size, subSize);

				// To wrap edges seamlessly, copy edge values around to other side of array
				if (i==0)
					m_Terrain[(subSize*size) + j] = m_Terrain[(i * size) + j];
				if (j==0)
					m_Terrain[(i*size) + subSize] = m_Terrain[(i * size) + j];

				j+=stride;
			}
		}

		// reduce random number range.
		scale *= ratio;
		stride >>= 1;
	}
}


// 二次平滑
void FractalTerrain::SecondSmooth(float *Terrain, int iSize)
{
	int i, j;
	int x, y;
	float average = 0;
	for (i=1; i<iSize-1; ++i)
	{
		x = i * iSize;
		for (j=1; j<iSize-1; ++j)
		{
			y = j;
			average = ( Terrain[(x-1)+(j-1)] + Terrain[(x)+(j-1)] + Terrain[(x+1)+(j-1)] +
                        Terrain[(x-1)+(j)]   + Terrain[(x)+(j)]   + Terrain[(x+1)+(j)]   +
						Terrain[(x-1)+(j+1)] + Terrain[(x)+(j+1)] + Terrain[(x+1)+(j+1)] ) / 9;
			m_Terrain[(x)+(j)] = average;
		}
	}
}




void FractalTerrain::DisplayTerrain(CPaintDC *dc)
{
	int i, j;
	int x = 0;
	int y = 0;
	DWORD dwColor = 0;
	float fColor = 0.0f;
	int iSize = m_iSize + 1;

	for (i=0; i<m_iSize; i++)
	{
		for (j=0; j<m_iSize; j++)
		{
			fColor = m_Terrain[(i*iSize)+j]; // iSize改为(iSize-1)或(iSize+1)都会有丰富的变化

			if (fColor<0)
				dwColor = (DWORD)(64 - abs(fColor*100));
			else
				dwColor = (DWORD)(fColor*100 + 64);

			x = i*m_iGridPixel;
			y = j*m_iGridPixel;

			CBrush brush;
			brush.CreateSolidBrush(RGB(dwColor,dwColor,dwColor)); // 0黑色代表低地

			//dc->SelectObject(&brush);
			//dc->Rectangle(x, y, x+m_iGridPixel, y+m_iGridPixel);

			CRect rect(x, y, x+m_iGridPixel, y+m_iGridPixel);
			dc->FillRect(&rect, &brush);
		}
	}

}


// 地质生成

// 随机散播种子初始化地图数组
void FractalTerrain::InitGeology()
{
	int i, j;

	srand((int)time(0));
	// 随机初始化数组
	for (i=0; i<m_iWidth; ++i)
	{
		for (j=0; j<m_iWidth; ++j)
		{
			m_Geology[(i*m_iWidth)+j] = (1+(int)(10.0*rand()/(RAND_MAX+1.0)));
		}
	}
}

// 平滑地图色块
void FractalTerrain::SmoothTerrain()
{
	int row_offset = 0;
	int width = m_iWidth;
	int passes = m_iStep;

	while (passes > 0)
	{
		for ( int square_size = width; square_size > 1; square_size /= 2 )
		{
			for ( int x1 = row_offset; x1 < width; x1 += square_size )
			{
				for ( int y1 = row_offset; y1 < width; y1 += square_size )
				{
					// Get the four corner points.
					int x2 = (x1 + square_size) % width;
					int y2 = (y1 + square_size) % width;

					int i1 = m_Geology[x1*m_iWidth + y1];
					int i2 = m_Geology[x2*m_iWidth + y1];
					int i3 = m_Geology[x1*m_iWidth + y2];
					int i4 = m_Geology[x2*m_iWidth + y2];

					// Obtain new points by averaging the corner points.
					int p1 = ((i1 * 9) + (i2 * 3) + (i3 * 3) + (i4))     / 16;
					int p2 = ((i1 * 3) + (i2 * 9) + (i3)     + (i4 * 3)) / 16; 
					int p3 = ((i1 * 3) + (i2)     + (i3 * 9) + (i4 * 3)) / 16;
					int p4 = ((i1)     + (i2 * 3) + (i3 * 3) + (i4 * 9)) / 16;

					int x3 = (x1 + square_size/4) % width;
					int y3 = (y1 + square_size/4) % width;
					int x4 = (x3 + square_size/2) % width;
					int y4 = (y3 + square_size/2) % width;

					m_Geology[x3*m_iWidth + y3] = p1;
					m_Geology[x4*m_iWidth + y3] = p2;
					m_Geology[x3*m_iWidth + y4] = p3;
					m_Geology[x4*m_iWidth + y4] = p4;
				}
			}
			row_offset = square_size/4;  // set offset for next row
		}
		passes--;
	}
}


void FractalTerrain::DisplayGeology(CPaintDC *dc)
{
	// 地形辅助信息
	DWORD dwTerrainColor = 0;
	float fTerrainColor = 0.0f;
	int iSize = m_iSize + 1;

	// 生成色块图形
	DWORD dwGridColor = 0;
	DWORD dwColor = 0;
	dc->MoveTo(0, 0);
	int i, j;
	int x = 0;
	int y = 0;
	for (i=0; i<m_iWidth; ++i)
	{
		for (j=0; j<m_iWidth; ++j)
		{
			// 截取矩阵
			if (i>=m_iSize || j>=m_iSize)
			{
				x += m_iGridPixel;
				continue;
			}

			// 地形辅助信息
			fTerrainColor = m_Terrain[(i*iSize)+j]; // iSize改为(iSize-1)或(iSize+1)都会有丰富的变化

			if (fTerrainColor<0)
				dwTerrainColor = (DWORD)(64 - abs(fTerrainColor*100));
			else
				dwTerrainColor = (DWORD)(fTerrainColor*100 + 64);

			// 填充
			dwColor = (int)m_Geology[i*m_iWidth + j];

			CBrush brush;


			if (dwTerrainColor < 55)
			{
				// 谷地
				switch (dwColor)
				{
					case 1:
						brush.CreateSolidBrush(RGB(100,100,200)); // 蓝
						break;
					case 2:
					case 3:
						brush.CreateSolidBrush(RGB(200,200,100)); // 黄
						break;
					case 4:
						brush.CreateSolidBrush(RGB(0,255,0)); // 绿
						break;
					default:
						brush.CreateSolidBrush(RGB(100,100,200)); // 蓝
				}
			}
			else if (dwTerrainColor >= 55 && dwTerrainColor < 70)
			{
				// 平原
				switch (dwColor)
				{
					case 1:
						brush.CreateSolidBrush(RGB(200,200,100)); // 黄
						break;
					case 2:
						brush.CreateSolidBrush(RGB(0,200,0)); // 青
						break;
					case 3:
					case 4:
					default:
						brush.CreateSolidBrush(RGB(0,255,0)); // 绿
				}
			}
			else
			{
				//brush.CreateSolidBrush(RGB(200,0,0)); // 红
				// Origin
				switch (dwColor)
				{
					case 1:
						brush.CreateSolidBrush(RGB(100,100,200)); // 蓝
						break;
					case 2:
					case 3:
						brush.CreateSolidBrush(RGB(0,200,0)); // 青
						break;
					case 4:
						brush.CreateSolidBrush(RGB(0,255,0)); // 绿
						break;
					default:
						brush.CreateSolidBrush(RGB(200,200,100)); // 黄
				}
			}

			//dc->SelectObject(&brush);
			//dc->Rectangle(x, y, x+m_iPixPerGrid, y+m_iPixPerGrid);

			CRect rect(x, y, x+m_iGridPixel/2, y+m_iGridPixel/2);
			dc->FillRect(&rect, &brush);
			x += m_iGridPixel;
		}
		x = 0;
		y += m_iGridPixel;
	}
}


// 入口函数
void FractalTerrain::CreateTerrian(CPaintDC *dc)
{
	int seedValue = (int)time(0);

	Alloc2DFractArray();

	// 地形
	Fill2DFractArray(seedValue);
	SecondSmooth(m_Terrain , (m_iSize+1));
	DisplayTerrain(dc);

	// 地质
	InitGeology();
	SmoothTerrain();
	DisplayGeology(dc);

	FreeFractArray();
}



