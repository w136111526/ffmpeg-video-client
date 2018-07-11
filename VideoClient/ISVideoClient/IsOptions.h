#pragma once

#include <iostream>
class CIsOptions
{
	friend class CIsSystem;
public:
	int GetDetectMinFace() const { return m_nDetectMinFace; }
	void SetDetectMinFace(int nDetectMinFace) { m_nDetectMinFace = nDetectMinFace; }

	int GetDetectMaxFace() const { return m_nDetectMaxFace; }
	void SetDetectMaxFace(int nDetectMaxFace) { m_nDetectMaxFace = nDetectMaxFace; }

	int GetDetectSpeed() const { return m_nDetectSpeed; }
	void SetDetectSpeed(int nDetectSpeed) { m_nDetectSpeed = nDetectSpeed; }

	int GetSortType() const { return m_nSortType; }
	void SetSortType(int nSortType) { m_nSortType = nSortType; }

	int GetAlgoType() const { return m_nAlgoType; }
	void SetAlgoType(int nAlgoType) { m_nAlgoType = nAlgoType; }

	bool GetUseNetDog() const { return m_bUseNetDog; }
	void SetUseNetDog(bool bUseNetDog) { m_bUseNetDog = bUseNetDog; }

	std::string GetSN() const { return m_strSN; }
	void SetSN(std::string strSN) { m_strSN = strSN; }

	std::string GetLIC() const { return m_strLIC; }
	void SetLIC(std::string strLIC) { m_strLIC = strLIC; }

	int GetSeq() const { return m_nSeq; }
	void	SetSeq(int Seq) { m_nSeq = Seq; }

	int GetExposure() const { return m_nExposure; }
	void SetExposure(int nExposure) { m_nExposure = nExposure; }

	vector<string> GetUrlList() const { return m_vectUrlList; }
	void SetUrlList(vector<string> vectUrlList) { m_vectUrlList.swap(vectUrlList); }

	//! Load/Save
	int Load();
	int Save();

private:
	CIsOptions();
	~CIsOptions();

	//!AlgoParam
	int									m_nDetectMinFace;				
	int									m_nDetectMaxFace;
	int									m_nDetectSpeed;				//1:Slow;2:Normal;3:Faster
	int									m_nSortType;						//1:None;2:Ascending;3:Descending
	int									m_nAlgoType;					//1:TypeA;2:TypeB;3:TypeC(v9007)
	bool								m_bUseNetDog;					//网络狗
	std::string							m_strSN;								//序列号
	std::string							m_strLIC;							//许可号

	//Camera&Video
	int									m_nVideoType;						//视频类型
	int									m_nSeq;								//摄像头编号
	int									m_nExposure;						//曝光值-13 - 0

	vector<string>						m_vectUrlList;
};

