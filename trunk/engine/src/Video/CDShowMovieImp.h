#pragma once

class CMemStream;

namespace sqr
{
    struct DirectShowData;
	class ITexture;
    //播放视频类（direct show）
	class  CDShowMovieImp : public CBaseMovieImp
    {
    public:
        CDShowMovieImp();

        virtual ~CDShowMovieImp();
 
        // 加载内存视频
        virtual HRESULT LoadMemMovie(const char* name);
		// 加载url视频
		virtual HRESULT LoadUrlMovie(const char* name);
        // 获取视频的空间大小
        void GetMovieDimensions(float &width, float &height);
		// 得到视频的宽度
		float GetMovieWidth(void);
		// 得到视频的高度
		float GetMovieHeight(void);
        // 卸载
        void UnloadMovie();
        // 暂停
        virtual void Pause();
        // 播放
        virtual void Play();
        // 回放
        virtual void Rewind();
        // 停止
        virtual void Stop();
        // 是否播放中
        virtual bool IsPlayingMovie();
		// 是否播放完成
		virtual bool IsPlayCompleted(void);
        // 更新视频贴图
        virtual void UpdateMovieTexture();
		//得到当前进度
		virtual float GetProgress(void);
		// 清除
		void CleanTextureContents();
    protected:
        // 贴图宽度
        int mTexWidth;
        // 贴图高度
        int mTexHeight;
        // DirectShow结构体
        DirectShowData* dsdata;
        // 水平镜像
        bool mHorizontalMirroring;
		// 内存文件
		CMemStream* m_pMemStream;
		// 上次加载的视频文件名称
		std::string m_strPreName;
    };
}