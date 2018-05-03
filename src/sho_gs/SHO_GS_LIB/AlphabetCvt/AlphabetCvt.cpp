// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
// SQL 쿼리문을 만들때 사용되는 문자열들을 조사하여 특정 문자를 다른문자로 대체하거나
// 변환한다.


#include "AlphabetCvt.h"

// 알파벳과 ' 을 바꾼다. a->b, z->a, A->B, Z->A, '->`
char Alphabet2[] = {"                                       `                         BCDEFGHIJKLMNOPQRSTUVWXYZA      bcdefghijklmnopqrstuvwxyza"};
// 알파벳만 원상복구한다.
char Alphabet0[] = {"                                                                 ZABCDEFGHIJKLMNOPQRSTUVWXY      zabcdefghijklmnopqrstuvwxy"};

void AlphabetCvt(char *p, int len)
{
	//return;	// Japan

	char *pp;
	for(int i=0; i<len; ++i)
	{
		pp = p + i;

		// America
		if(*pp == '\'' || *pp == '_' || *pp == '!' || *pp == '-' || 
			*pp == '#' || *pp == '$' || *pp == '%' || *pp == '^' || *pp == '&' || 
			*pp == '*' || *pp == '(' || *pp == ')' || *pp == '\\' || *pp == ';' ||
			*pp == '|' || *pp == '{' || *pp == '}' )
		{
			*pp = Alphabet2[*pp];
		}

		// 알파벳과 ' 을 바꾼다. a->b, z->a, A->B, Z->A, '->`
		//if(('a' <= *pp && *pp <= 'z') || ('A' <= *pp && *pp <= 'Z') || *pp == '\'')
		//if(*pp == '\'' || *pp == '-')		// 임시로 ' 문자만 `로 바꾼다.
		/*
		if(*pp == '\'' || *pp == '-' || *pp == '_' || *pp == '!' || *pp == '@' || 
			*pp == '#' || *pp == '$' || *pp == '%' || *pp == '^' || *pp == '&' || 
			*pp == '*' || *pp == '(' || *pp == ')' || *pp == '\\' || *pp == ';' ||
			*pp == '|' || *pp == '{' || *pp == '}' )
		{
			*pp = Alphabet2[*pp];
		}
		if(*pp == '\'' || *pp == '-' || *pp == '_')		// 임시로
		{
			*pp = Alphabet2[*pp];
		}
		*/


		// Korea, Philippine
		//if(*pp == '\'' || *pp == '_' || *pp == '!' || 
		//	*pp == '#' || *pp == '$' || *pp == '%' || *pp == '^' || *pp == '&' || 
		//	*pp == '*' || *pp == '(' || *pp == ')' || *pp == '\\' || *pp == ';' ||
		//	*pp == '|' || *pp == '{' || *pp == '}' )
		//{
		//	*pp = Alphabet2[*pp];
		//}
	}
}

void AlphabetCvt2(char *p, int len)
{
	char *pp;
	for(int i=0; i<len; ++i)
	{
		pp = p + i;

		// 알파벳만 원상복구한다.
		if(('a' <= *pp && *pp <= 'z') || ('A' <= *pp && *pp <= 'Z'))
		{
			*pp = Alphabet0[*pp];
		}
	}
}
