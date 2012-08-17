#pragma once

namespace sqr
{
	struct Huffman_table
	{
		uint8 Length[17];  // k =1-16 ; L[k] indicates the number of Huffman codes of length k
		uint16 minor_code[17];  // indicates the value of the smallest Huffman code of length k
		uint16 major_code[17];  // similar, but the highest code
		uint8 V[65536];  // V[k][j] = Value associated to the j-th Huffman code of length k
		// High nibble = nr of previous 0 coefficients
		// Low nibble = size (in bits) of the coefficient which will be taken from the data stream
	};

	class ITexture;

	class CJPGDecoder
	{
	public:
		CJPGDecoder( ITexture* tex, const char* filename );
		~CJPGDecoder();

		uint32	GetNeedSize();
		uint32	GetWidth();
		uint32	GetHeight();

		static	void	
				SetLod( uint32 lod_in );

	private:
		typedef void (CJPGDecoder::*decode_MCU_func)(uint32);

	private:
		bool	loadHeader( uint8* buf, uint32 size );

		bool	decode_JPEG_image( ITexture* pTex);
		
		int		init_JPG_decoding();

		void	load_quant_table( float *quant_table );

		void	load_Huffman_table( Huffman_table *HT );

		void	init_QT();

		void	calculate_mask();

		void	prepare_range_limit_table();

		void	precalculate_Cr_Cb_tables();

		void	process_Huffman_data_unit(uint8 DC_nr, uint8 AC_nr,int16 *previous_DC);

		void	resync();

		void	calculate_tabs();

		void	IDCT_transform(int16 *incoeff, uint8 *outcoeff, uint8 Q_nr);

		void	skipKbits(uint8 k);

		uint16	lookKbits(uint8 k);

		int16	getKbits(uint8 k);

		int16	get_svalue(uint8 k);

		void	convert_8x8_YCbCr_to_RGB(uint8 *Y, uint8 *Cb, uint8 *Cr, uint32 im_loc, uint32 X_image_bytes, uint8 *im_buffer);

		void	convert_8x8_YCbCr_to_RGB_tab(uint8 *Y, uint8 *Cb, uint8 *Cr, uint8 *tab, uint32 im_loc, uint32 X_image_bytes, uint8 *im_buffer);
		
		void	BYTE_p(uint32& i);

		void	WORD_p(uint32& i);

		void	decode_MCU_1x1( uint32 im_loc );
		void	decode_MCU_2x1( uint32 im_loc );
		void	decode_MCU_2x2( uint32 im_loc );
		void	decode_MCU_1x2( uint32 im_loc );


	private:
		uint8*			jpg_buf; // the buffer we use for storing the entire JPG file
		uint32			X_image;
		uint32			Y_image;

		uint8			bp; //current byte
		uint16			wp; //current word

		uint32			byte_pos; // current byte position

		uint16			X_round,Y_round; // The dimensions rounded to multiple of Hmax*8 (X_round)
		// and Ymax*8 (Y_round)

		uint8*			im_buffer; // RGBA image buffer
		uint32			X_image_bytes; // size in bytes of 1 line of the image = X_round * 4
		uint32			y_inc_value ; // 32*X_round; // used by decode_MCU_1x2,2x1,2x2

		uint8			YH,YV,CbH,CbV,CrH,CrV; // sampling factors (horizontal and vertical) for Y,Cb,Cr
		uint16			Hmax,Vmax;

		float*			QT[4]; // quantization tables, no more than 4 quantization tables (QT[0..3])
		Huffman_table	HTDC[4]; //DC huffman tables , no more than 4 (0..3)
		Huffman_table	HTAC[4]; //AC huffman tables                  (0..3)

		uint8			YQ_nr,CbQ_nr,CrQ_nr; // quantization table number for Y, Cb, Cr
		uint8			YDC_nr,CbDC_nr,CrDC_nr; // DC Huffman table number for Y,Cb, Cr
		uint8			YAC_nr,CbAC_nr,CrAC_nr; // AC Huffman table number for Y,Cb, Cr

		uint8			Restart_markers; // if 1 => Restart markers on , 0 => no restart markers
		uint16			MCU_restart; //Restart markers appears every MCU_restart MCU blocks

		int16			DCY, DCCb, DCCr; // Coeficientii DC pentru Y,Cb,Cr
		int16			DCT_coeff[64]; // Current DCT_coefficients
		uint8			Y[64], Cb[64], Cr[64]; // Y, Cb, Cr of the current 8x8 block for the 1x1 case
		uint8			Y_1[64], Y_2[64], Y_3[64], Y_4[64];
		uint8			tab_1[64],tab_2[64],tab_3[64],tab_4[64]; // tabelele de supraesantionare pt cele 4 blocuri

		int16			Cr_tab[256], Cb_tab[256]; // Precalculated Cr, Cb tables
		int16			Cr_Cb_green_tab[65536];

		// Initial conditions:
		// byte_pos = start position in the Huffman coded segment
		// WORD_get(w1); WORD_get(w2);wordval=w1;

		uint8			d_k;  // Bit displacement in memory, relative to the offset of w1
		// it's always <16
		uint16			w1, w2; // w1 = First word in memory; w2 = Second word
		uint32			wordval; // the actual used value in Huffman decoding.
		uint32			mask[17];
		

		int				shift_temp;
		uint8*			rlimit_table_mem_base;
		uint8*			rlimit_table;
		int				dest_pitch;
		static uint32	lod;
	};
}