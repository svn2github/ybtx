#include "stdafx.h"
#include "CJPGDecoder.h"
#include "CPkgFile.h"
#include "CRectangle.h"
#include "CTexture.h"
#include "GraphicBase.h"
#include "CGraphicMallocObject.h"
#include "TSqrAllocObject.inl"

#define GenJPGErr(err) {GenErr("JPG解码错误", err); return false;}
#define SOI 0xD8
#define EOI 0xD9
#define APP0 0xE0
#define SOF 0xC0
#define DQT 0xDB
#define DHT 0xC4
#define SOS 0xDA
#define DRI 0xDD
#define COM 0xFE

#define RANGE_MASK 1023L

#define RIGHT_SHIFT(x,shft)  \
	((shift_temp = (x)) < 0 ? \
	(shift_temp >> (shft)) | ((~(0L)) << (32-(shft))) : \
	(shift_temp >> (shft)))
#define DESCALE(x,n)  RIGHT_SHIFT((x) + (1L << ((n)-1)), n)

namespace
{
	uint8 zigzag[64]=
	{   
		0, 1, 5, 6,14,15,27,28,
		2, 4, 7,13,16,26,29,42,
		3, 8,12,17,25,30,41,43,
		9,11,18,24,31,40,44,53,
		10,19,23,32,39,45,52,54,
		20,22,33,38,46,51,55,60,
		21,34,37,47,50,56,59,61,
		35,36,48,49,57,58,62,63 
	};

	int16	neg_pow2[17] = {0,-1,-3,-7,-15,-31,-63,-127,-255,-511,-1023,-2047,-4095,-8191,-16383,-32767};
	uint8	tab_1[64], tab_2[64], tab_3[64], tab_4[64]; // tabelele de supraesantionare pt cele 4 blocuri

	uint16 WORD_hi_lo(uint8 byte_high,uint8 byte_low)
	{
#if 0
		_asm 
		{
			mov ah,byte_high
				mov al,byte_low
		}
#else
		return (((int)byte_high)<<8)|(byte_low);
#endif
	}

	uint8* g_dest_buf = NULL;

	struct JPGIniter
	{
		JPGIniter()
		{
		}

		~JPGIniter()
		{
			if ( g_dest_buf )
			{
				CGraphicMallocObject::GetPool()->Dealloc(g_dest_buf);
				g_dest_buf = NULL;
			}
		}
	};
	JPGIniter inter;
}

namespace sqr
{
	uint32 CJPGDecoder::lod = 0;
	
	CJPGDecoder::CJPGDecoder( ITexture* tex, const char* filename )
		: d_k(0)
		, rlimit_table(NULL)
		, im_buffer(NULL)
		, rlimit_table_mem_base(NULL)
		, dest_pitch(0)
	{
		for (int i=0;i<=3;i++) 
			QT[i]=0;

		if ( !tex )
			return;

		CPkgFile file;
		if ( file.Open(PATH_ALIAS_RES.c_str(), filename) != eFE_SUCCESS )
		{
			ostringstream strm;
			strm<<"CJPGDecoder 打开文件错误"<<filename;
			ArtErr(strm.str());
			return;
		}

		uint32 file_size = file.Size();
		uint8* pBuf = (uint8*)CGraphicMallocObject::GetPool()->Alloc(file_size);
		file.read( pBuf, file_size );
		file.Close();
		loadHeader(pBuf, file_size);

		if ( !g_dest_buf && (tex->GetWidth() != X_round || tex->GetWidth() != Y_round) )
			g_dest_buf = (uint8*)CGraphicMallocObject::GetPool()->Alloc(X_round*Y_round*4);

		decode_JPEG_image(tex);
		CGraphicMallocObject::GetPool()->Dealloc(pBuf);
	}

	CJPGDecoder::~CJPGDecoder()
	{
		for (int i=0;i<=3;i++) 
			CGraphicMallocObject::GetPool()->Dealloc(QT[i]);

		CGraphicMallocObject::GetPool()->Dealloc(rlimit_table_mem_base);
	}

	uint32 CJPGDecoder::GetNeedSize()
	{
		if ( (X_round==X_image)&&(Y_round==Y_image) )
			return X_round*Y_round*4;
		else
			return X_image*Y_image*4;
	}

	bool CJPGDecoder::loadHeader( uint8* buf, uint32 buf_size )
	{
		if (buf==NULL)
			GenJPGErr("Not enough memory for loading file");

		uint8 vers,units;
		uint16 Xdensity,Ydensity,Xthumbnail,Ythumbnail;
		uint16 length;
		float *qtable;
		uint32 old_byte_pos;
		Huffman_table *htable;
		uint32 j;
		uint8 precision,comp_id,nr_components;
		uint8 QT_info,HT_info;
		uint8 SOS_found,SOF_found;

		jpg_buf = buf;

		if ((jpg_buf[0]!=0xFF)||(jpg_buf[1]!=SOI)) 
			GenJPGErr("Not a JPG file ?\n");
		if ((jpg_buf[2]!=0xFF)||(jpg_buf[3]!=APP0)) 
			GenJPGErr("Invalid JPG file.");
		if ((jpg_buf[6]!='J')||
			(jpg_buf[7]!='F')||
			(jpg_buf[8]!='I')||
			(jpg_buf[9]!='F')||
			(jpg_buf[10]!=0) ) 
			GenJPGErr("Invalid JPG file.");

		init_JPG_decoding();
		byte_pos = 11;

		BYTE_p(byte_pos);
		vers=bp;
		if (vers!=1) 
			GenJPGErr("JFIF version not supported");
		BYTE_p(byte_pos); // vers_lo=bp;
		BYTE_p(byte_pos);  
		units=bp;
		// if (units!=0) GenJPGErr("JPG format not supported");
		//;//	printf("units = %d\n", units);
		WORD_p(byte_pos); Xdensity=wp; WORD_p(byte_pos); Ydensity=wp;
		// if ((Xdensity!=1)||(Ydensity!=1)) GenJPGErr("JPG format not supported");
		//;  //{printf("X density = %d\n",Xdensity); printf("Y density = %d\n",Ydensity);}
		BYTE_p(byte_pos);Xthumbnail=bp;BYTE_p(byte_pos);Ythumbnail=bp;
		if ((Xthumbnail!=0)||(Ythumbnail!=0))
			GenJPGErr(" Cannot process JFIF thumbnailed files\n");
		// Start decoding process
		SOS_found=0; 
		SOF_found=0; 
		Restart_markers=0;
		while ((byte_pos<buf_size)&&!SOS_found)
		{
			BYTE_p(byte_pos);
			if (bp!=0xFF) 
				continue;
			// A marker was found
			BYTE_p(byte_pos);
			switch(bp)
			{
			case DQT: 
				WORD_p(byte_pos); 
				length=wp; // length of the DQT marker
				for (j=0;j<(uint32)(wp-2);)
				{
					old_byte_pos=byte_pos;
					BYTE_p(byte_pos); QT_info=bp;
					if ((QT_info>>4)!=0)
						GenJPGErr("16 bit quantization table not supported");
					qtable=QT[QT_info&0xF];
					load_quant_table(qtable);
					j+=byte_pos-old_byte_pos;
				}
				break;
			case DHT: 
				WORD_p(byte_pos); 
				length=wp;
				for (j=0;j<(uint32)(wp-2);)
				{
					old_byte_pos = byte_pos;
					BYTE_p(byte_pos); 
					HT_info = bp;
					if ((HT_info&0x10)!=0) 
						htable=&HTAC[HT_info&0xF];
					else 
						htable = &HTDC[HT_info&0xF];
					load_Huffman_table(htable);
					j+=byte_pos-old_byte_pos;
				}
				break;
			case COM: 
				WORD_p(byte_pos); 
				length=wp;
				byte_pos+=wp-2;
				break;
			case DRI: 
				Restart_markers=1;
				WORD_p(byte_pos); 
				length=wp; //should be = 4
				WORD_p(byte_pos);  
				MCU_restart=wp;
				if (MCU_restart==0) 
					Restart_markers=0;
				break;
			case SOF: 
				WORD_p(byte_pos); 
				length=wp; //should be = 8+3*3=17
				BYTE_p(byte_pos); 
				precision=bp;
				if (precision!=8) 
					GenJPGErr("Only 8 bit precision supported");

				WORD_p(byte_pos); 
				Y_image=wp; 
				WORD_p(byte_pos); 
				X_image=wp;
				BYTE_p(byte_pos); 
				nr_components=bp;
				if (nr_components!=3) 
					GenJPGErr("Only truecolor JPGS supported");

				for (j=1;j<=3;j++)
				{
					BYTE_p(byte_pos); 
					comp_id=bp;
					if ((comp_id==0)||(comp_id>3)) GenJPGErr("Only YCbCr format supported");
					switch (comp_id)
					{
					case 1: // Y
						BYTE_p(byte_pos); 
						YH=bp>>4;
						YV=bp&0xF;
						BYTE_p(byte_pos); 
						YQ_nr=bp;
						break;
					case 2: // Cb
						BYTE_p(byte_pos); 
						CbH=bp>>4;
						CbV=bp&0xF;
						BYTE_p(byte_pos); 
						CbQ_nr=bp;
						break;
					case 3: // Cr
						BYTE_p(byte_pos); 
						CrH=bp>>4;
						CrV=bp&0xF;
						BYTE_p(byte_pos); 
						CrQ_nr=bp;
						break;
					}
				}
				SOF_found=1;
				break;
			case SOS: 
				WORD_p(byte_pos); 
				length=wp; //should be = 6+3*2=12
				BYTE_p(byte_pos); 
				nr_components=bp;
				if (nr_components!=3) 
					GenJPGErr("Invalid SOS marker");

				for (j=1;j<=3;j++)
				{
					BYTE_p(byte_pos); 
					comp_id=bp;
					if ((comp_id==0)||(comp_id>3)) 
						GenJPGErr("Only YCbCr format supported");

					switch (comp_id)
					{
					case 1: // Y
						BYTE_p(byte_pos); 
						YDC_nr=bp>>4;
						YAC_nr=bp&0xF;
						break;
					case 2: // Cb
						BYTE_p(byte_pos); 
						CbDC_nr=bp>>4;
						CbAC_nr=bp&0xF;
						break;
					case 3: // Cr
						BYTE_p(byte_pos); 
						CrDC_nr=bp>>4;
						CrAC_nr=bp&0xF;
						break;
					}
				}
				BYTE_p(byte_pos); 
				BYTE_p(byte_pos); 
				BYTE_p(byte_pos); // Skip 3 bytes
				SOS_found=1;
				break;
			case 0xFF:
				break; // do nothing for 0xFFFF, sequence of consecutive 0xFF are for
				// filling purposes and should be ignored
			default:  
				WORD_p(byte_pos); 
				length=wp;
				byte_pos+=wp-2; //skip unknown marker
				break;
			}
		}
		if (!SOS_found) 
			GenJPGErr("Invalid JPG file. No SOS marker found.");

		if (!SOF_found) 
			GenJPGErr("Progressive JPEGs not supported");

		if ((CbH>YH)||(CrH>YH)) 
			GenJPGErr("Vertical sampling factor for Y should be >= sampling factor for Cb,Cr");

		if ((CbV>YV)||(CrV>YV)) 
			GenJPGErr("Horizontal sampling factor for Y should be >= sampling factor for Cb,Cr");

		if ((CbH>=2)||(CbV>=2)) 
			GenJPGErr("Cb sampling factors should be = 1");

		if ((CrV>=2)||(CrV>=2)) 
			GenJPGErr("Cr sampling factors should be = 1");

		// Restricting sampling factors for Y,Cb,Cr should give us 4 possible cases for sampling factors
		// YHxYV,CbHxCbV,CrHxCrV: 2x2,1x1,1x1;  1x2,1x1,1x1; 2x1,1x1,1x1;
		// and 1x1,1x1,1x1 = no upsampling needed

		Hmax=YH,Vmax=YV;
		Ast(X_image%(Hmax*8)==0);
		X_round=X_image;
		//if ( X_image%(Hmax*8)==0) 
		//	X_round=X_image; // X_round = Multiple of Hmax*8
		//else 
		//	X_round=(X_image/(Hmax*8)+1)*(Hmax*8);
		
		Ast(Y_image%(Vmax*8)==0);
		Y_round=Y_image;
		//if ( Y_image%(Vmax*8)==0) 
		//	Y_round=Y_image; // Y_round = Multiple of Vmax*8
		//else 
		//	Y_round=(Y_image/(Vmax*8)+1)*(Vmax*8);

		return true;
	}

	bool CJPGDecoder::decode_JPEG_image( ITexture* pTex )
	{
		pTex->SetLod(lod);

		TEXLOCKED_RECT lockRect;
		if ( !g_dest_buf )
		{
			if ( FAILED( pTex->Lock(0, &lockRect, NULL) ) )
				GenJPGErr("Lock texture failed.");

			dest_pitch = lockRect.Pitch;
			im_buffer = (uint8*)lockRect.pBits;
		}
		else
		{
			im_buffer = g_dest_buf;
			dest_pitch = X_round*4;
		}

		decode_MCU_func decode_MCU;

		uint16 x_mcu_cnt,y_mcu_cnt;
		uint32 nr_mcu;
		uint16 X_MCU_nr,Y_MCU_nr; // Nr de MCU-uri
		uint32 MCU_dim_x; //dimensiunea in bufferul imagine a unui MCU pe axa x
		uint32 im_loc_inc; // = 7 * X_round * 4 sau 15*X_round*4;
		uint32 im_loc; //locatia in bufferul imagine

		byte_pos-=2;
		resync();

		y_inc_value = 8*dest_pitch;
		calculate_tabs(); // Calcul tabele de supraesantionare, tinand cont de YH si YV

		if ((YH==1)&&(YV==1)) 
			decode_MCU = &CJPGDecoder::decode_MCU_1x1;
		else 
		{
			if (YH==2)
			{
				if (YV==2) 
					decode_MCU = &CJPGDecoder::decode_MCU_2x2;
				else 
					decode_MCU = &CJPGDecoder::decode_MCU_2x1;
			}
			else 
				decode_MCU = &CJPGDecoder::decode_MCU_1x2;
		}
		MCU_dim_x=Hmax*8*4;

		Y_MCU_nr=Y_round/(Vmax*8); // nr of MCUs on Y axis
		X_MCU_nr=X_round/(Hmax*8); // nr of MCUs on X axis

		X_image_bytes=X_round*4; im_loc_inc = (Vmax*8-1) * dest_pitch;
		nr_mcu=0; im_loc=0; // memory location of the current MCU
		for (y_mcu_cnt=0;y_mcu_cnt<Y_MCU_nr;y_mcu_cnt++)
		{
			for (x_mcu_cnt=0;x_mcu_cnt<X_MCU_nr;x_mcu_cnt++)
			{
				(this->*decode_MCU)(im_loc);
				if ((Restart_markers)&&((nr_mcu+1)%MCU_restart==0)) resync();
				nr_mcu++;
				im_loc+=MCU_dim_x;
			}
			im_loc+=im_loc_inc;
		}

		if ( !g_dest_buf )
		{
			pTex->Unlock(0);
			pTex->AutoGenMip();
		}
		else
		{
			pTex->FillFromMemory(g_dest_buf, TF_XRGB32, X_round, CIRect(0, 0, X_round, Y_round), CIRect(0, 0, pTex->GetWidth(), pTex->GetHeight()));
		}

		return true;
	}	

	uint32 CJPGDecoder::GetWidth()
	{
		return X_image;
	}

	uint32 CJPGDecoder::GetHeight()
	{
		return Y_image;
	}

	int CJPGDecoder::init_JPG_decoding()
	{
		byte_pos=0;
		init_QT();
		calculate_mask();
		prepare_range_limit_table();
		precalculate_Cr_Cb_tables();
		return 1; //for future error check
	}

	void CJPGDecoder::load_quant_table( float *quant_table )
	{
		float scalefactor[8] =
		{	
			1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
			1.0f, 0.785694958f, 0.541196100f, 0.275899379f
		};
		uint8 j,row,col;
		// Load quantization coefficients from JPG file, scale them for DCT and reorder
		// from zig-zag order
		for (j=0;j<=63;j++) 
			quant_table[j] = jpg_buf[byte_pos+zigzag[j]];

		j=0;
		for (row=0;row<=7;row++)
		{
			for (col=0;col<=7;col++) 
			{
				quant_table[j] *= scalefactor[row] * scalefactor[col];
				j++;
			}
		}
		byte_pos+=64;
	}

	void CJPGDecoder::load_Huffman_table( Huffman_table *HT )
	{
		uint8 k,j;
		uint32 code;

		for (j=1;j<=16;j++) 
		{
			BYTE_p(byte_pos);
			HT->Length[j]=bp;
		}
		
		for (k=1;k<=16;k++)
		{
			for (j=0;j<HT->Length[k];j++) 
			{
				BYTE_p(byte_pos);
				HT->V[WORD_hi_lo(k,j)]=bp;
			}
		}

		code=0;
		for (k=1;k<=16;k++) 
		{
			HT->minor_code[k] = (uint16)code;
			for (j=1;j<=HT->Length[k];j++) 
				code++;
			
			HT->major_code[k]=(uint16)(code-1);
			code*=2;
			if (HT->Length[k]==0) 
			{
				HT->minor_code[k]=0xFFFF;
				HT->major_code[k]=0;
			}
		}
	}

	void CJPGDecoder::init_QT()
	{
		uint8 i;
		for (i=0;i<=3;i++) 
			QT[i]=(float *)CGraphicMallocObject::GetPool()->Alloc(sizeof(float)*64);
	}

	void CJPGDecoder::calculate_mask()
	{
		uint8 k;
		uint32 tmpdv;
		//precalculated bit mask
		for (k=0;k<=16;k++) 
		{ 
			tmpdv=0x10000;
			mask[k]=(tmpdv>>k)-1;
		} 
	}

	/* Allocate and fill in the sample_range_limit table */
	void CJPGDecoder::prepare_range_limit_table()
	{
		int j;
		rlimit_table_mem_base = (uint8 *)CGraphicMallocObject::GetPool()->Alloc(5 * 256L + 128);
		/* First segment of "simple" table: limit[x] = 0 for x < 0 */
		memset((void *)rlimit_table_mem_base,0,256);
		rlimit_table = rlimit_table_mem_base + 256;	/* allow negative subscripts of simple table */
		/* Main part of "simple" table: limit[x] = x */
		for (j = 0; j < 256; j++) 
			rlimit_table[j] = j;
		/* End of simple table, rest of first half of post-IDCT table */
		for (j = 256; j < 640; j++) 
			rlimit_table[j] = 255;
		/* Second half of post-IDCT table */
		memset((void *)(rlimit_table + 640),0,384);
		for (j = 0; j < 128 ; j++) 
			rlimit_table[j+1024] = j;
	}

	void CJPGDecoder::precalculate_Cr_Cb_tables()
	{
		uint16 k;
		uint16 Cr_v,Cb_v;
		for (k=0;k<=255;k++) 
			Cr_tab[k]=(int16)((k-128.0)*1.402);
		
		for (k=0;k<=255;k++) 
			Cb_tab[k]=(int16)((k-128.0)*1.772);

		for (Cr_v=0;Cr_v<=255;Cr_v++)
			for (Cb_v=0;Cb_v<=255;Cb_v++)
				Cr_Cb_green_tab[((uint16)(Cr_v)<<8)+Cb_v] = (int)(-0.34414*(Cb_v-128.0)-0.71414*(Cr_v-128.0));

	}

	void CJPGDecoder::resync()
	{
		// byte_pos  = pozitionat pe restart marker
		byte_pos+=2;
		BYTE_p(byte_pos);
		if (bp==0xFF) 
			byte_pos++; // skip 00
		w1=WORD_hi_lo(bp, 0);
		BYTE_p(byte_pos);
		if (bp==0xFF) 
			byte_pos++; // skip 00
		w1+=bp;
		BYTE_p(byte_pos);
		if (bp==0xFF) 
			byte_pos++; // skip 00
		w2=WORD_hi_lo(bp, 0);
		BYTE_p(byte_pos);
		if (bp==0xFF) 
			byte_pos++; // skip 00
		w2+=bp;
		wordval=w1; d_k=0; // Reinit bitstream decoding
		DCY=0; DCCb=0; DCCr=0; // Init DC coefficients
	}

	void CJPGDecoder::calculate_tabs()
	{
		uint8 tab_temp[256];
		uint8 x,y;

		// Tabelul de supraesantionare 16x16
		for (y=0;y<16;y++)
			for (x=0;x<16;x++)
				tab_temp[y*16+x] = (y/YV)* 8 + x/YH;

		// Din el derivam tabelele corespunzatoare celor 4 blocuri de 8x8 pixeli
		for (y=0;y<8;y++)
		{
			for (x=0;x<8;x++)
				tab_1[y*8+x]=tab_temp[y*16+x];
			for (x=8;x<16;x++)
				tab_2[y*8+(x-8)]=tab_temp[y*16+x];
		}
		for (y=8;y<16;y++)
		{
			for (x=0;x<8;x++)
				tab_3[(y-8)*8+x]=tab_temp[y*16+x];
			for (x=8;x<16;x++)
				tab_4[(y-8)*8+(x-8)]=tab_temp[y*16+x];
		}
	}

	void CJPGDecoder::process_Huffman_data_unit(uint8 DC_nr, uint8 AC_nr,int16 *previous_DC)
	{
		// Process one data unit. A data unit = 64 DCT coefficients
		// Data is decompressed by Huffman decoding, then the array is dezigzag-ed
		// The result is a 64 DCT coefficients array: DCT_coeff
		uint8 nr,k,j,EOB_found;
		register uint16 tmp_Hcode;
		uint8 size_val,count_0;
		uint16 *min_code,*maj_code; // min_code[k]=minimum code of length k, maj_code[k]=similar but maximum
		uint16 *max_val, *min_val;
		uint8 *huff_values;
		int16 DCT_tcoeff[64];
		uint8 byte_temp;

		// Start Huffman decoding
		// First the DC coefficient decoding
		min_code=HTDC[DC_nr].minor_code;
		maj_code=HTDC[DC_nr].major_code;
		huff_values=HTDC[DC_nr].V;

		for (nr = 0; nr < 64 ; nr++) 
			DCT_tcoeff[nr] = 0; //Initialize DCT_tcoeff

		nr=0;// DC coefficient

		min_val = &min_code[1]; 
		max_val = &maj_code[1];
		for (k=1;k<=16;k++) 
		{
			tmp_Hcode=lookKbits(k);
			//	   max_val = &maj_code[k]; min_val = &min_code[k];
			if ( (tmp_Hcode<=*max_val)&&(tmp_Hcode>=*min_val) ) 
			{ //Found a valid Huffman code
				skipKbits(k);
				size_val = huff_values[WORD_hi_lo(k,(uint8)(tmp_Hcode-*min_val))];
				if (size_val==0) 
					DCT_tcoeff[0] = *previous_DC;
				else 
				{
					DCT_tcoeff[0] = *previous_DC + getKbits(size_val);
					*previous_DC = DCT_tcoeff[0];
				}
				break;
			}
			min_val++; max_val++;
		}

		// Second, AC coefficient decoding
		min_code=HTAC[AC_nr].minor_code;
		maj_code=HTAC[AC_nr].major_code;
		huff_values=HTAC[AC_nr].V;

		nr=1; // AC coefficient
		EOB_found=0;
		while ( (nr<=63)&&(!EOB_found) )
		{
			max_val = &maj_code[1]; min_val =&min_code[1];
			for (k=1;k<=16;k++)
			{
				tmp_Hcode=lookKbits(k);
				//	   max_val = &maj_code[k]; &min_val = min_code[k];
				if ( (tmp_Hcode<=*max_val)&&(tmp_Hcode>=*min_val) )
				{
					skipKbits(k);
					byte_temp=huff_values[WORD_hi_lo(k,(uint8)(tmp_Hcode-*min_val))];
					size_val=byte_temp&0xF;
					count_0=byte_temp>>4;
					if (size_val==0) {if (count_0==0) EOB_found=1;
					else if (count_0==0xF) nr+=16; //skip 16 zeroes
					}
					else
					{
						nr+=count_0; //skip count_0 zeroes
						DCT_tcoeff[nr++]=getKbits(size_val);
					}
					break;
				}
				min_val++; max_val++;
			}
			if (k>16) nr++;  // This should not occur
		}
		for (j=0;j<=63;j++) DCT_coeff[j]=DCT_tcoeff[zigzag[j]]; // Et, voila ... DCT_coeff
	}
	
	void CJPGDecoder::IDCT_transform(int16 *incoeff, uint8 *outcoeff, uint8 Q_nr)
		// Fast float IDCT transform
	{
		uint8 x;
		int8 y;
		int16 *inptr;
		uint8 *outptr;
		float workspace[64];
		float *wsptr;//Workspace pointer
		float *quantptr; // Quantization table pointer
		float dcval;
		float tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
		float tmp10,tmp11,tmp12,tmp13;
		float z5,z10,z11,z12,z13;
		uint8 *range_limit=rlimit_table+128;
		// Pass 1: process COLUMNS from input and store into work array.
		wsptr=workspace;
		inptr=incoeff;
		quantptr=QT[Q_nr];
		for (y=0;y<=7;y++)
		{
			if( (inptr[8]|inptr[16]|inptr[24]|inptr[32]|inptr[40]|inptr[48]|inptr[56])==0)
			{
				// AC terms all zero (in a column)
				dcval=inptr[0]*quantptr[0];
				wsptr[0]  = dcval;
				wsptr[8]  = dcval;
				wsptr[16] = dcval;
				wsptr[24] = dcval;
				wsptr[32] = dcval;
				wsptr[40] = dcval;
				wsptr[48] = dcval;
				wsptr[56] = dcval;
				inptr++;quantptr++;wsptr++;//advance pointers to next column
				continue ;
			}
			//Even part
			tmp0 = inptr[0] *quantptr[0];
			tmp1 = inptr[16]*quantptr[16];
			tmp2 = inptr[32]*quantptr[32];
			tmp3 = inptr[48]*quantptr[48];

			tmp10 = tmp0 + tmp2;// phase 3
			tmp11 = tmp0 - tmp2;

			tmp13 = tmp1 + tmp3;// phases 5-3
			tmp12 = (tmp1 - tmp3) * 1.414213562f - tmp13; // 2*c4

			tmp0 = tmp10 + tmp13;// phase 2
			tmp3 = tmp10 - tmp13;
			tmp1 = tmp11 + tmp12;
			tmp2 = tmp11 - tmp12;

			// Odd part
			tmp4 = inptr[8] *quantptr[8];
			tmp5 = inptr[24]*quantptr[24];
			tmp6 = inptr[40]*quantptr[40];
			tmp7 = inptr[56]*quantptr[56];

			z13 = tmp6 + tmp5;// phase 6
			z10 = tmp6 - tmp5;
			z11 = tmp4 + tmp7;
			z12 = tmp4 - tmp7;

			tmp7 = z11 + z13;// phase 5
			tmp11= (z11 - z13) * 1.414213562f; // 2*c4

			z5 = (z10 + z12) * 1.847759065f; // 2*c2
			tmp10 = 1.082392200f * z12 - z5; // 2*(c2-c6)
			tmp12 = -2.613125930f * z10 + z5;// -2*(c2+c6)

			tmp6 = tmp12 - tmp7;// phase 2
			tmp5 = tmp11 - tmp6;
			tmp4 = tmp10 + tmp5;

			wsptr[0]  = tmp0 + tmp7;
			wsptr[56] = tmp0 - tmp7;
			wsptr[8]  = tmp1 + tmp6;
			wsptr[48] = tmp1 - tmp6;
			wsptr[16] = tmp2 + tmp5;
			wsptr[40] = tmp2 - tmp5;
			wsptr[32] = tmp3 + tmp4;
			wsptr[24] = tmp3 - tmp4;
			inptr++;
			quantptr++;
			wsptr++;//advance pointers to the next column
		}

		//  Pass 2: process ROWS from work array, store into output array.
		// Note that we must descale the results by a factor of 8 = 2^3
		outptr=outcoeff;
		wsptr=workspace;
		for (x=0;x<=7;x++)
		{
			// Even part
			tmp10 = wsptr[0] + wsptr[4];
			tmp11 = wsptr[0] - wsptr[4];

			tmp13 = wsptr[2] + wsptr[6];
			tmp12 =(wsptr[2] - wsptr[6]) * 1.414213562f - tmp13;

			tmp0 = tmp10 + tmp13;
			tmp3 = tmp10 - tmp13;
			tmp1 = tmp11 + tmp12;
			tmp2 = tmp11 - tmp12;

			// Odd part
			z13 = wsptr[5] + wsptr[3];
			z10 = wsptr[5] - wsptr[3];
			z11 = wsptr[1] + wsptr[7];
			z12 = wsptr[1] - wsptr[7];

			tmp7 = z11 + z13;
			tmp11= (z11 - z13) * 1.414213562f;

			z5 = (z10 + z12) * 1.847759065f; // 2*c2
			tmp10 = 1.082392200f * z12 - z5;  // 2*(c2-c6)
			tmp12 = -2.613125930f * z10 + z5; // -2*(c2+c6)

			tmp6 = tmp12 - tmp7;
			tmp5 = tmp11 - tmp6;
			tmp4 = tmp10 + tmp5;

			// Final output stage: scale down by a factor of 8
			outptr[0] = range_limit[(DESCALE((int) (tmp0 + tmp7), 3)) & 1023L];
			outptr[7] = range_limit[(DESCALE((int) (tmp0 - tmp7), 3)) & 1023L];
			outptr[1] = range_limit[(DESCALE((int) (tmp1 + tmp6), 3)) & 1023L];
			outptr[6] = range_limit[(DESCALE((int) (tmp1 - tmp6), 3)) & 1023L];
			outptr[2] = range_limit[(DESCALE((int) (tmp2 + tmp5), 3)) & 1023L];
			outptr[5] = range_limit[(DESCALE((int) (tmp2 - tmp5), 3)) & 1023L];
			outptr[4] = range_limit[(DESCALE((int) (tmp3 + tmp4), 3)) & 1023L];
			outptr[3] = range_limit[(DESCALE((int) (tmp3 - tmp4), 3)) & 1023L];

			wsptr+=8;//advance pointer to the next row
			outptr+=8;
		}
	}

	void CJPGDecoder::skipKbits(uint8 k)
	{
		uint8 b_high,b_low;
		d_k+=k;
		if (d_k>=16) 
		{ 
			d_k-=16;
			w1=w2;
			// Get the next word in w2
			BYTE_p(byte_pos);
			if (bp!=0xFF) b_high=bp;
			else 
			{
				if (jpg_buf[byte_pos]==0) 
					byte_pos++; //skip 00
				else 
					byte_pos--; // stop byte_pos pe restart marker
				b_high=0xFF;
			}
			BYTE_p(byte_pos);
			if (bp!=0xFF) 
				b_low=bp;
			else 
			{
				if (jpg_buf[byte_pos]==0) 
					byte_pos++; //skip 00
				else 
					byte_pos--; // stop byte_pos pe restart marker
				b_low=0xFF;
			}
			w2=WORD_hi_lo(b_high,b_low);
		}

		wordval = ((uint32)(w1)<<16) + w2;
		wordval <<= d_k;
		wordval >>= 16;
	}

	uint16 CJPGDecoder::lookKbits(uint8 k)
	{
		return (wordval>>(16-k));
	}

	int16 CJPGDecoder::getKbits(uint8 k)
	{
		int16 signed_wordvalue;
		signed_wordvalue=get_svalue(k);
		skipKbits(k);
		return signed_wordvalue;
	}

	int16 CJPGDecoder::get_svalue(uint8 k)
	{
#if 0
		_asm {
			xor ecx, ecx
				mov cl,k
				mov eax,[wordval]
			shl eax,cl
				shr eax, 16
				dec cl
				bt eax,ecx
				jc end_macro
signed_value:inc cl
			 mov ebx,[start_neg_pow2]
			add ax,word ptr [ebx+ecx*2]
end_macro:
		}
#else
		// k>0 always
		// Takes k bits out of the BIT stream (wordval), and makes them a signed value
		unsigned char cl_=k;
		unsigned int ebx_,eax_;
		unsigned int tempv=1;

		ebx_ = 0;
		eax_=((wordval<<cl_)>>16);
		cl_--;
		tempv=(tempv<<cl_);
		if(tempv&eax_)
		{
			return (int16)eax_;
		}
		cl_++;
		eax_+=neg_pow2[cl_];
		return (int16)eax_;
#endif	
	}

	// Functia (ca optimizare) poate fi apelata si fara parametrii Y,Cb,Cr
	// Stim ca va fi apelata doar in cazul 1x1
	void CJPGDecoder::convert_8x8_YCbCr_to_RGB(uint8 *Y, uint8 *Cb, uint8 *Cr, uint32 im_loc, uint32 X_image_bytes, uint8 *im_buffer)
	{
		uint32 x,y;
		uint8 im_nr;
		uint8 *Y_val = Y, *Cb_val = Cb, *Cr_val = Cr;
		uint8 *ibuffer = im_buffer + im_loc;

		for (y=0;y<8;y++)
		{
			im_nr=0;
			for (x=0;x<8;x++)
			{
				ibuffer[im_nr++] = rlimit_table[*Y_val + Cb_tab[*Cb_val]]; //B
				ibuffer[im_nr++] = rlimit_table[*Y_val + Cr_Cb_green_tab[WORD_hi_lo(*Cr_val,*Cb_val)]]; //G
				ibuffer[im_nr++] = rlimit_table[*Y_val + Cr_tab[*Cr_val]]; // R
				
				Y_val++; Cb_val++; Cr_val++; im_nr++;
			}
			ibuffer+=dest_pitch;
		}
	}

	void CJPGDecoder::convert_8x8_YCbCr_to_RGB_tab(uint8 *Y, uint8 *Cb, uint8 *Cr, uint8 *tab, uint32 im_loc, uint32 X_image_bytes, uint8 *im_buffer)
		// Functia (ca optimizare) poate fi apelata si fara parametrii Cb,Cr
	{
		uint32 x,y;
		uint8 nr, im_nr;
		uint8 Y_val,Cb_val,Cr_val;
		uint8 *ibuffer = im_buffer + im_loc;

		nr=0;
		for (y=0;y<8;y++)
		{
			im_nr=0;
			for (x=0;x<8;x++)
			{
				Y_val=Y[nr];
				Cb_val=Cb[tab[nr]]; Cr_val=Cr[tab[nr]]; // reindexare folosind tabelul
				// de supraesantionare precalculat
				ibuffer[im_nr++] = rlimit_table[Y_val + Cb_tab[Cb_val]]; //B
				ibuffer[im_nr++] = rlimit_table[Y_val + Cr_Cb_green_tab[WORD_hi_lo(Cr_val,Cb_val)]]; //G
				ibuffer[im_nr++] = rlimit_table[Y_val + Cr_tab[Cr_val]]; // R
				nr++; im_nr++;
			}
			ibuffer+=dest_pitch;
		}
	}

	void CJPGDecoder::BYTE_p(uint32& i)
	{
		bp = jpg_buf[(i)++];
	}

	void CJPGDecoder::WORD_p(uint32& i)
	{
		wp = (((uint16)(jpg_buf[(i)]))<<8) + jpg_buf[(i)+1]; 
		(i)+=2;
	}

	void CJPGDecoder::decode_MCU_1x1(uint32 im_loc)
	{
		// Y
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y,YQ_nr);
		// Cb
		process_Huffman_data_unit(CbDC_nr,CbAC_nr,&DCCb);
		IDCT_transform(DCT_coeff,Cb,CbQ_nr);
		// Cr
		process_Huffman_data_unit(CrDC_nr,CrAC_nr,&DCCr);
		IDCT_transform(DCT_coeff,Cr,CrQ_nr);

		convert_8x8_YCbCr_to_RGB(Y,Cb,Cr,im_loc,X_image_bytes,im_buffer);
	}

	void CJPGDecoder::decode_MCU_2x1(uint32 im_loc)
	{
		// Y
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y_1,YQ_nr);
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y_2,YQ_nr);
		// Cb
		process_Huffman_data_unit(CbDC_nr,CbAC_nr,&DCCb);
		IDCT_transform(DCT_coeff,Cb,CbQ_nr);
		// Cr
		process_Huffman_data_unit(CrDC_nr,CrAC_nr,&DCCr);
		IDCT_transform(DCT_coeff,Cr,CrQ_nr);

		convert_8x8_YCbCr_to_RGB_tab(Y_1,Cb,Cr,tab_1,im_loc,X_image_bytes,im_buffer);
		convert_8x8_YCbCr_to_RGB_tab(Y_2,Cb,Cr,tab_2,im_loc+32,X_image_bytes,im_buffer);
	}

	void CJPGDecoder::decode_MCU_2x2(uint32 im_loc)
	{
		// Y
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y_1,YQ_nr);
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y_2,YQ_nr);
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y_3,YQ_nr);
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y_4,YQ_nr);
		// Cb
		process_Huffman_data_unit(CbDC_nr,CbAC_nr,&DCCb);
		IDCT_transform(DCT_coeff,Cb,CbQ_nr);
		// Cr
		process_Huffman_data_unit(CrDC_nr,CrAC_nr,&DCCr);
		IDCT_transform(DCT_coeff,Cr,CrQ_nr);

		convert_8x8_YCbCr_to_RGB_tab(Y_1,Cb,Cr,tab_1,im_loc,X_image_bytes,im_buffer);
		convert_8x8_YCbCr_to_RGB_tab(Y_2,Cb,Cr,tab_2,im_loc+32,X_image_bytes,im_buffer);
		convert_8x8_YCbCr_to_RGB_tab(Y_3,Cb,Cr,tab_3,im_loc+y_inc_value,X_image_bytes,im_buffer);
		convert_8x8_YCbCr_to_RGB_tab(Y_4,Cb,Cr,tab_4,im_loc+y_inc_value+32,X_image_bytes,im_buffer);
	}

	void CJPGDecoder::decode_MCU_1x2(uint32 im_loc)
	{
		// Y
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y_1,YQ_nr);
		process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
		IDCT_transform(DCT_coeff,Y_2,YQ_nr);
		// Cb
		process_Huffman_data_unit(CbDC_nr,CbAC_nr,&DCCb);
		IDCT_transform(DCT_coeff,Cb,CbQ_nr);
		// Cr
		process_Huffman_data_unit(CrDC_nr,CrAC_nr,&DCCr);
		IDCT_transform(DCT_coeff,Cr,CrQ_nr);

		convert_8x8_YCbCr_to_RGB_tab(Y_1,Cb,Cr,tab_1,im_loc,X_image_bytes,im_buffer);
		convert_8x8_YCbCr_to_RGB_tab(Y_2,Cb,Cr,tab_3,im_loc+y_inc_value,X_image_bytes,im_buffer);
	}

	void CJPGDecoder::SetLod( uint32 lod_in )
	{
		lod = lod_in;
	}

}