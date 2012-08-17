#include "StdAfx.h"
#include "CRecordPanel.h"
#include "RecordCtrl/CRecordCtrlImp.h"

void CRecordPanel::Update(void)
{
	AnimateListUpdate();
}

char*	CRecordPanel::GetImpTypeName(void)
{
	return GET_IMP_NAME(CRecordCtrlImp);
}

void CRecordPanel::ActivePanel()
{
	//CRecordCtrlImp *pImp = (CRecordCtrlImp*)m_pCtrl;
	//if(pImp)
	//{
	//	pImp->Active();
	//}
}

void CRecordPanel::AnimateListUpdate()
{
	CRecordCtrlImp *pImp = NULL;
	pImp = (CRecordCtrlImp*)m_pCtrl;
	//更新列表
	this->AnimateList->Items->Clear();
	unsigned int count=pImp->GetAnimateCount();
	if(count==0)
	{
		this->KeyFrameList->Clear();
		this->KeyFrameGroup->Enabled=false;
		this->AnimateCtrlGroup->Enabled=false;
		this->CamCtrlGroup->Enabled=false;
		return ;
	}

	for(unsigned int i=0;i<count;i++)
	{
		int tmpAnimateID=pImp->GetAnimateID(i);
		System::Windows::Forms::ListViewItem^ aniItem=gcnew System::Windows::Forms::ListViewItem();
		System::Int32^ ID=tmpAnimateID;
		aniItem->Text=ID->ToString();
		
		System::Windows::Forms::ListViewItem::ListViewSubItem^ aniSubItem1=gcnew System::Windows::Forms::ListViewItem::ListViewSubItem();
		System::Int32^ animateTime=pImp->GetAnimateTimlong(tmpAnimateID);
		aniSubItem1->Text=animateTime->ToString();
		aniItem->SubItems->Add(aniSubItem1);

		this->AnimateList->Items->Add(aniItem);
	}		
	//设置激活项
	if(m_LastSelectAnimate>=0 && m_LastSelectAnimate<this->AnimateList->Items->Count)
		this->AnimateList->Items[m_LastSelectAnimate]->BackColor =System::Drawing::Color::Gray;
	else
		m_LastSelectAnimate=-1;  //回复初始值
	int ActiveAnimateId=pImp->GetAnimateID(m_LastSelectAnimate);
	if(pImp->SetActiveAniamte(ActiveAnimateId))
	{
		//激活成功  更新关键帧列表
		KeyListUpdate();
		this->KeyFrameGroup->Enabled=true;
		this->AnimateCtrlGroup->Enabled=true;
	}
	else
	{
		this->KeyFrameGroup->Enabled=false;
		this->AnimateCtrlGroup->Enabled=false;
		this->CamCtrlGroup->Enabled=false;
	}
}

void CRecordPanel::KeyListUpdate()
{
	CRecordCtrlImp *pImp = NULL;
	pImp = (CRecordCtrlImp*)m_pCtrl;

	this->KeyFrameList->Items->Clear();
	int tmpActiveAnimateID=0;
	pImp->GetActiveAniamte(tmpActiveAnimateID);
	int KeyCount=pImp->GetAnimateKeyNum(tmpActiveAnimateID);
	if(KeyCount<=0)
	{
		this->CamCtrlGroup->Enabled=false;
		return ;
	}
	System::Int32^ time;
	System::Int32^ id;
	int keytime;
	int keyid;
	for(int i=0;i<KeyCount;i++)
	{
		pImp->GetKeyInfoID(i,keyid);
		id=keyid;
		pImp->GetKeyInfoTime(i,keytime);
		time=keytime;
		System::Windows::Forms::ListViewItem^ keyItem=gcnew System::Windows::Forms::ListViewItem();
		keyItem->Text=id->ToString();
		System::Windows::Forms::ListViewItem::ListViewSubItem^ keySubItem1=gcnew System::Windows::Forms::ListViewItem::ListViewSubItem();
		keySubItem1->Text=time->ToString();
		keyItem->SubItems->Add(keySubItem1);
		this->KeyFrameList->Items->Add(keyItem);
	}
	if(m_LastSelectKey>=0 && m_LastSelectKey<this->KeyFrameList->Items->Count)
		this->KeyFrameList->Items[m_LastSelectKey]->BackColor =System::Drawing::Color::Gray;
	else
		m_LastSelectKey=-1;

	if(m_LastSelectKey>=0)
	{
		//更新相机控制面板
		this->CamCtrlGroup->Enabled=true;
		CamCtrlUpdate();
	}
	else
	{
		this->CamCtrlGroup->Enabled=false;
	}
}

void CRecordPanel::CamCtrlUpdate()
{
	CRecordCtrlImp *pImp = NULL;
	pImp = (CRecordCtrlImp*)m_pCtrl;

	if(m_LastSelectKey<0)
		return;
	if(this->KeyFrameShow->Checked)
	{
		float posx,posy,posz,dirx,diry,dirz,speed,fovy;
		int time;
		pImp->GetKeyInfoPos(m_LastSelectKey,posx,posy,posz);
		pImp->GetKeyInfoDir(m_LastSelectKey,dirx,diry,dirz);
		pImp->GetKeyInfoSpeed(m_LastSelectKey,speed);
		pImp->GetKeyInfoFov(m_LastSelectKey,fovy);
		pImp->GetKeyInfoTime(m_LastSelectKey,time);
		System::Single^ tmpPosx;
		System::Single^ tmpPosy;
		System::Single^ tmpPosz;
		System::Single^ tmpDirx;
		System::Single^ tmpDiry;
		System::Single^ tmpDirz;
		System::Single^ tmpSpeed;
		System::Single^ tmpFovy;
		System::Int32^	tmpTime;
		tmpPosx=posx;
		tmpPosy=posy;
		tmpPosz=posz;
		tmpDirx=dirx;
		tmpDiry=diry;
		tmpDirz=dirz;
		tmpSpeed=speed;
		tmpFovy=fovy;
		tmpTime=time;
		this->CamPosXEdit->Text=tmpPosx->ToString();
		this->CamPosYEdit->Text=tmpPosy->ToString();
		this->CamPosZEdit->Text=tmpPosz->ToString();
		this->CamDirXEdit->Text=tmpDirx->ToString();
		this->CamDirYEdit->Text=tmpDiry->ToString();
		this->CamDirZEdit->Text=tmpDirz->ToString();
		this->CamSpeedEdit->Text=tmpSpeed->ToString();
		this->CamFovEdit->Text=tmpFovy->ToString();
		this->CamTimeEdit->Text=tmpTime->ToString();
	}
}

System::Void CRecordPanel::AddAnimate(System::Object^  sender, System::EventArgs^  e) 
{
	CRecordCtrlImp *pImp = (CRecordCtrlImp*)m_pCtrl;
	static int i=10;
	i++;
	pImp->InsertAnimate(i);
	m_LastSelectAnimate=pImp->GetAnimateCount()-1;
	this->Update();
}

System::Void CRecordPanel::DelAnimate(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_LastSelectAnimate<0 || this->AnimateList->Items->Count<=0)
		return ;
	CRecordCtrlImp *pImp = (CRecordCtrlImp*)m_pCtrl;
	int tmpAnimateID=pImp->GetAnimateID(m_LastSelectAnimate);
	pImp->DelAnimate(tmpAnimateID);
	m_LastSelectAnimate=(m_LastSelectAnimate-1)>0?(m_LastSelectAnimate-1):0;
	this->Update();
}

System::Void CRecordPanel::AddKeyFrame(System::Object^  sender, System::EventArgs^  e) 
{
	this->AddKeyPanel->Visible=true;
}

System::Void CRecordPanel::btKeyTimeY_Click(System::Object^  sender, System::EventArgs^  e)
{
	CRecordCtrlImp *pImp = (CRecordCtrlImp*)m_pCtrl;
	System::String ^ str=this->EditKeytime->Text;
	if(str!=L"")
	{
		int time=System::Convert::ToInt32(str);
		if(time>0)
		{
			if(pImp->InsertKeyInfo(time))
			{
				int id=0;
				pImp->GetActiveAniamte(id);
				m_LastSelectKey=pImp->GetAnimateKeyNum(id)-1;
			}
			this->Update();
		}
	}
	this->EditKeytime->Text=L"";
	this->AddKeyPanel->Visible=false;
}

System::Void CRecordPanel::btKeyTimeN_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->EditKeytime->Text=L"";
	this->AddKeyPanel->Visible=false;
}

System::Void CRecordPanel::ModifyKeyFrame(System::Object^  sender, System::EventArgs^  e)
{
	//被隐藏掉了 功能做到了相机里面
}

System::Void CRecordPanel::DekKeyFrame(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_LastSelectKey<0 || this->KeyFrameList->Items->Count<=0)
		return ;
	CRecordCtrlImp *pImp = (CRecordCtrlImp*)m_pCtrl;
	pImp->DelKeyInfo(m_LastSelectKey);
	m_LastSelectKey=(m_LastSelectKey-1)>0?(m_LastSelectKey-1):0;
	this->Update();
}

System::Void CRecordPanel::SetLastKeyFrame(System::Object^  sender, System::EventArgs^  e) 
{
}

System::Void CRecordPanel::PlayAnimate(System::Object^  sender, System::EventArgs^  e) 
{
	CRecordCtrlImp *pImp = (CRecordCtrlImp*)m_pCtrl;
	if(pImp)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		if(pImp->GetCurState() == RECORD_PAUSE)
		{
			type.custom_type=MGT_RECORD_PLAY;
			this->AnimatePlay->Text=L"暂停";
		}
		else if(pImp->GetCurState() == RECORD_STOP)
		{
			type.custom_type=MGT_RECORD_PLAY;
			this->AnimatePlay->Text=L"暂停";
		}
		else
		{
			type.custom_type=MGT_RECORD_PAUSE;
			this->AnimatePlay->Text=L"播放";
		}
		info._iinfo=0;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

System::Void CRecordPanel::StopAnimate(System::Object^  sender, System::EventArgs^  e) 
{
	CRecordCtrlImp *pImp = (CRecordCtrlImp*)m_pCtrl;
	if(pImp)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		type.custom_type=MGT_RECORD_STOP;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
	this->AnimateStop->Text=L"停止";
	this->AnimatePlay->Text=L"播放";
}

System::Void CRecordPanel::ReplayAnimate(System::Object^  sender, System::EventArgs^  e) 
{
	CRecordCtrlImp *pImp = (CRecordCtrlImp*)m_pCtrl;
	if(pImp)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		type.custom_type=MGT_RECORD_REPLAY;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
	this->AnimateReplay->Text=L"重播";
}

System::Void CRecordPanel::CamCtrlPos_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
}
System::Void CRecordPanel::CamCtrltTarget_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
}

System::Void CRecordPanel::KeyFrameShow_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	m_PreView=this->KeyFrameShow->Checked;
}

System::Void CRecordPanel::AnimateList_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_LastSelectAnimate<this->AnimateList->Items->Count && m_LastSelectAnimate>=0)
	{
		this->AnimateList->Items[m_LastSelectAnimate]->BackColor=System::Drawing::SystemColors::Window;
	}

	if(this->AnimateList->SelectedIndices->Count>0)
	{
		m_LastSelectAnimate=this->AnimateList->SelectedIndices[0];
		this->AnimateList->Items[m_LastSelectAnimate]->BackColor =System::Drawing::Color::Gray;
	}
	CRecordCtrlImp *pImp = NULL;
	pImp = (CRecordCtrlImp*)m_pCtrl;
	int tmpAnimateID=pImp->GetAnimateID(m_LastSelectAnimate);
	if(pImp->SetActiveAniamte(tmpAnimateID))
		KeyListUpdate();
}

System::Void CRecordPanel::KeyFrameList_Click(System::Object^  sender, System::EventArgs^  e)
{
	if(m_LastSelectKey<this->KeyFrameList->Items->Count && m_LastSelectKey>=0)
	{
		this->KeyFrameList->Items[m_LastSelectKey]->BackColor=System::Drawing::SystemColors::Window;
	}

	if(this->KeyFrameList->SelectedIndices->Count>0)
	{
		m_LastSelectKey=this->KeyFrameList->SelectedIndices[0];
		this->KeyFrameList->Items[m_LastSelectKey]->BackColor =System::Drawing::Color::Gray;
	}
	if(m_LastSelectKey>=0)
	{
		this->CamCtrlGroup->Enabled=true;
		CamCtrlUpdate();
	}
	else
	{
		this->CamCtrlGroup->Enabled=false;
	}

}

System::Void CRecordPanel::CamCtrModify_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_LastSelectKey<0)
		return ;
	//修改相机各个参数
	float fov=0;
	float speed=0;
	float pos_x=0;
	float pos_y=0;
	float pos_z=0;
	float dir_x=0;
	float dir_y=0;
	float dir_z=0;
	int time=0;

	System::String^ str=this->CamFovEdit->Text;
	if( !System::String::IsNullOrEmpty(str) )
		fov=System::Convert::ToSingle(str);
	str=this->CamSpeedEdit->Text;
	if( !System::String::IsNullOrEmpty(str) )
		speed=System::Convert::ToSingle(str);
	str=this->CamPosXEdit->Text;
	if( !System::String::IsNullOrEmpty(str) )
		pos_x=System::Convert::ToSingle(str);
	str=this->CamPosYEdit->Text;
	if( !System::String::IsNullOrEmpty(str) )
		pos_y=System::Convert::ToSingle(str);
	str=this->CamPosZEdit->Text;
	if( !System::String::IsNullOrEmpty(str) )
		pos_z=System::Convert::ToSingle(str);
	str=this->CamDirXEdit->Text;
	if( !System::String::IsNullOrEmpty(str) )
		dir_x=System::Convert::ToSingle(str);
	str=this->CamDirYEdit->Text;
	if( !System::String::IsNullOrEmpty(str) )
		dir_y=System::Convert::ToSingle(str);
	str=this->CamDirZEdit->Text;
	if( !System::String::IsNullOrEmpty(str) )
		dir_z=System::Convert::ToSingle(str);
	str=this->CamTimeEdit->Text;
	if(!System::String::IsNullOrEmpty(str))
		time=System::Convert::ToUInt32(str);

	CRecordCtrlImp* pImp = (CRecordCtrlImp*)m_pCtrl;
	
	pImp->SetKeyInfoPos(m_LastSelectKey,pos_x,pos_y,pos_z);
	pImp->SetKeyInfoDir(m_LastSelectKey,dir_x,dir_y,dir_z);
	pImp->SetKeyInfoSpeed(m_LastSelectKey,speed);
	pImp->SetKeyInfoFov(m_LastSelectKey,fov);
	pImp->SetKeyInfoTime(m_LastSelectKey,time);
	pImp->ResetKeyList();
	this->Update();
}

//正则表达式啊 

// \d	0---9
// +	匹配前面子表达式1次或多次
// ?	匹配前面子表达式0次或1次
// *	匹配前面子表达式0次或多次
// ^	正则表达式开始
// $	正则表达式结束

//  解析 ^(\\d)+(\\.)?(\\d)*$

//  \d	[0-9]数字					
//  +	数字重复出现1次或多次			359
//  \.	转义成字符( .)					359.
//	?	匹配前面(.)出现0次或一次		359		359.
//	\d	[0-9]数字						359.2
//	*	匹配前面数字出现0次或者多次		359.	359.2	359.24

System::Void CRecordPanel::CamFovEdit_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->CamFovEdit->Text))
		this->CamFovEdit->Text=L"";
}

System::Void CRecordPanel::CamSpeedEdit_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->CamSpeedEdit->Text))
		this->CamSpeedEdit->Text=L"";
}

System::Void CRecordPanel::CamPosXEdit_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->CamPosXEdit->Text))
		this->CamPosXEdit->Text=L"";
}

System::Void CRecordPanel::CamPosYEdit_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->CamPosYEdit->Text))
		this->CamPosYEdit->Text=L"";
}

System::Void CRecordPanel::CamPosZEdit_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->CamPosZEdit->Text))
		this->CamPosZEdit->Text=L"";
}

System::Void CRecordPanel::CamDirXEdit_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->CamDirXEdit->Text))
		this->CamDirXEdit->Text=L"";
}

System::Void CRecordPanel::CamDirYEdit_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->CamDirYEdit->Text))
		this->CamDirYEdit->Text=L"";
}

System::Void CRecordPanel::CamDirZEdit_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->CamDirZEdit->Text))
		this->CamDirZEdit->Text=L"";
}

System::Void CRecordPanel::EditKeytime_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\-)?(\\d)+(\\.)?(\\d)*$");
	if(!reg->IsMatch(this->EditKeytime->Text))
		this->EditKeytime->Text=L"";
}

System::Void CRecordPanel::CamTimeEdit_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\d)+$");
	if(!reg->IsMatch(this->CamTimeEdit->Text))
		this->CamTimeEdit->Text=L"";
}
