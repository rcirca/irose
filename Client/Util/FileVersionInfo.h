#ifndef _FILEVERSION_INFO_
#define _FILEVERSION_INFO_


class CFileVersionInfo
{
public:
	int				m_iMajorVersion;
	int				m_MinorVersion;
	int				m_BuildNumber;
	int				m_RevisionNumber;	

	std::string		m_strFileVersion;
	std::string		m_strLibToolFileVersion;

public:
	CFileVersionInfo(void);
	~CFileVersionInfo(void);


	bool	GetVersionInfo( const char* pFileName );
};

#endif //_FILEVERSION_INFO_