#ifndef _CTCandidateWindowHanja_
#define _CTCandidateWindowHanja_

#include "tcandidatewindow.h"
#include <string>
#include <vector>

/**
* 한국어일경우 한자 변환시 사용되는 Candidate Window class 
* - 연관클래스 CTIme
*
* @Author		최종진
*
* @Date			2005/8/30		
*/
class CTCandidateWindowHanja : public CTCandidateWindow
{
public:
	CTCandidateWindowHanja(void);
	virtual ~CTCandidateWindowHanja(void);
	virtual void Draw();
	virtual void AddCandidate( const char* pszCandidate );
	virtual void Clear();

protected:
	std::vector< std::string >	m_Candidates;
};
#endif