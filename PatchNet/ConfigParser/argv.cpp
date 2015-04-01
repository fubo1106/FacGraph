
#include"argv.h"
#include"autores.h"

#include"err.h"

#include<stdio.h>
#include<functional>
#include<assert.h>
#include<tchar.h>

#define _CH_CVT _TX('%')

_FF_BEG

//=============================================================================================================

static char_t _cvt_special_char(char_t ch)
{
	char_t rch=ch;

	switch(ch)
	{
	case _TX('\"'):
	case _CH_CVT:
	case _TX('$'):
	case _TX('-'): 
	case _TX('/'):
		break;
	case _TX('n'):
		rch=_TX('\n');
		break;
	case _TX('t'):
		rch=_TX('\t');
		break;
	default:
//		FVT_WARNING("error_unknown_special_char",string_t(&ch,&ch+1));
		;
	}
	return rch;
}

static char_t *_skip_space(const char_t *pbeg,const char_t *pend)
{
	while(pbeg<pend&&_istspace((unsigned short)*pbeg))
		++pbeg;

	return (char_t*)pbeg;
}


static char_t *_skip_non_space(const char_t *pbeg,const char_t *pend)
{
	while(pbeg<pend&&!_istspace((unsigned short)*pbeg))
		++pbeg;

	return (char_t*)pbeg;
}

static char_t *_skip_dquotes(const char_t *pbeg, const char_t *pend)
{
	assert(pbeg<pend&&*pbeg==_TX('\"'));

	while(++pbeg!=pend)
	{
		if(*pbeg==_TX('\"')&&pbeg[-1]!=_CH_CVT)
			break;
	}
	if(pbeg==pend)
	{
	//	vfxReportError("error_unexpected_end_of_string",string_t(pbeg,pend));
	}
	return (char_t*)pbeg;
}

template<typename _OpT>
static char_t* _skip_non_str_until_ex(const char_t *pbeg, const char_t *pend, _OpT &op)
{
	bool quate=false;
	const char_t *px=pbeg;

	while(px<pend&&(quate||!op(*px)))
	{
		if(*px==_TX('"'))
		{//if encounter a string enclosed with double quotation mark
			if(px==pbeg||px[-1]!=_CH_CVT)
				quate=!quate;
		}
		++px;
	}
	if(quate)
	{
//		vfxReportError("error_unexpected_end_of_string",string_t(pbeg,pend));
	}
	return (char_t*)px;
}

static char_t *_skip_non_str_non_space(const char_t *pbeg,const char_t *pend)
{
	return _skip_non_str_until_ex(pbeg,pend,_istspace);
}
static char_t *_skip_non_str_until(const char_t *pbeg,const char_t *pend,char_t until)
{
	return _skip_non_str_until_ex(pbeg,pend,std::bind1st(std::equal_to<char_t>(),until));
}


static void _get_str_val(const char_t *pbeg, const char_t *px, string_t &str)
{
	const char_t *pwb=pbeg;

	for(;pwb!=px;++pwb)
	{
		if(*pwb==_TX('\"')&&(pwb==pbeg||pwb[-1]!=_CH_CVT))
			continue;
		else
			str.push_back(*pwb);
	}
}

static void _pro_arg(const char_t *pwb, const char_t *px, int nq, std::vector<string_t> &vstr)
{
	if(nq==0)
		vstr.push_back(string_t(pwb,px));
	else
	{
		vstr.push_back(string_t());
		vstr.back().reserve(px-pwb);

		_get_str_val(pwb,px,vstr.back());
	}
}


static void _read_strings(const char_t *pbeg, const char_t *pend, std::vector<string_t> &vstr)
{
	if(pbeg&&pend>pbeg)
	{
		int nq=0;
		const char_t *px=pbeg,*pwb=NULL;

		while(px!=pend)
		{
			if(!_istspace((ushort)*px))
			{
				if(!pwb)
				{
					pwb=px; nq=0;
				}

				if(*px==_TX('\"')&&px!=pbeg&&px[-1]!=_CH_CVT)
					++nq;
			}
			else
			{
				if(pwb&&(nq&1)==0)
				{
					_pro_arg(pwb,px,nq,vstr);
					pwb=NULL;
				}
			}
			++px;
		}

		if(pwb)
			_pro_arg(pwb,px,nq,vstr);
	}
}


template<typename _ValT>
static int _parse_arg(const char_t *const *pps, const char_t *fmt, _ValT *pval, int count)
{//for generic type
	int i=0;

	for(;i<count;++i)
	{
		if(_stscanf(pps[i],fmt,pval+i)!=1)
			break;
	}
	return i;
}

static int _parse_arg(const char_t *const *pps,const char_t *fmt, string_t *pval, int count)
{//for string
	for(int i=0;i<count;++i)
		pval[i]=pps[i];
	return count;
}
static int _parse_arg(const char_t *const *pps,const char_t *fmt, char_t *pval, int count)
{//for char_t, need not to be separated by blank
	const char_t *ps=pps[0];
	int i=0;

	for(;i<count&&ps[i]&&!_istspace((ushort)ps[i]);++i)
	{
		pval[i]=ps[i];
	}
	return i;
}

static int _parse_arg(const char_t *const *pps, const char_t *fmt, bool *pval, int count)
{//for bool
	for(int i=0;i<count;++i)
		pval[i]=true; //default to be true

	if(pps&&pps[0])
	{
		const char_t *ps=pps[0];

		for(int i=0;i<count&&ps[i];++i)
		{//'+' for true, and '-' for false
			if(ps[i]==_TX('-'))
				pval[i]=false;
			else
				if(ps[i]!=_TX('+'))
				{
			//		vfxReportError("error_unknown_bool_char",ps);
					return i;
				}
		}	
	}
	
	return count;
}

template<typename _ValT>
static int _parse_arg_ex(IArgSet *pas, int request, const char_t *fmt, const char_t *var, _ValT *pval, int count,int pos)
{
	const char_t *const *pps=pas->Query(var,request,pos);
	int nread=0;

	if(!pps&&count>0) //if not found
		pas->OnFailed(var);
	else
	{
		nread=_parse_arg(pps,fmt,pval,count);
		if(nread!=count)
			pas->OnFailed(var);
	}
	return nread;
}

IArgSet::IArgSet()
:m_bAllowExcept(true)
{
}
bool IArgSet::Contain(const char_t *var)
{
	int flag=0;
	this->Query(var,1,0,&flag);

	return (flag&ASF_NOT_FOUND)==0;
}
void IArgSet::AllowException(bool bAllow)
{
	m_bAllowExcept=bAllow;
}
int IArgSet::Get(const char_t *var, bool *val, int count, int pos )
{
	return _parse_arg(this->Query(var,1,pos),_TX(""),val,count);
}
int IArgSet::Get(const char_t *var, char_t *val, int count, int pos )
{
	return _parse_arg_ex(this,1,_TX(""),var,val,count,pos);
}
int IArgSet::Get(const char_t *var, int *val, int count, int pos )
{
	return _parse_arg_ex(this,count,_TX("%d"),var,val,count,pos);
}
int IArgSet::Get(const char_t *var, float *val, int count, int pos )
{
	return _parse_arg_ex(this,count,_TX("%f"),var,val,count,pos);
}
int IArgSet::Get(const char_t *var, double *val, int count, int pos )
{
	return _parse_arg_ex(this,count,_TX("%lf"),var,val,count,pos);
}
int IArgSet::Get(const char_t *var, string_t *val, int count, int pos )
{
	return _parse_arg_ex(this,count,_TX(""),var,val,count,pos);
}

template<typename _SrcT, typename _DestT>
static int _argset_cast_get(IArgSet *parg, const char_t *var, _DestT *val, int count, int pos)
{
	AutoArrayPtr<_SrcT> psrc(new _SrcT[count]);
	int nr=parg->Get(var,psrc,count,pos);

	for(int i=0;i<nr;++i)
		val[i]=(_DestT)psrc[i];

	return nr;
}

int IArgSet::Get(const char_t *var, unsigned char *val, int count, int pos )
{
	return _argset_cast_get<int>(this,var,val,count,pos);
}

void IArgSet::OnFailed(const char_t *var)
{
	if(m_bAllowExcept)
		this->OnException(var);
}
void IArgSet::OnException(const char_t *var)
{
	FF_EXCEPTION(ERR_INVALID_ARGUMENT,var);
}
//========================================================================
IArgSetCombined::IArgSetCombined()
:m_pNext(NULL)
{
}
void IArgSetCombined::SetNext(IArgSet *pNext)
{
	m_pNext=pNext;
}
//========================================================================

static void _cvt_cmd_string(char_t *pbeg, char_t *pend)
{//convert the command-line input
	char_t *prx=pbeg, *pwx=pbeg;

	while(prx<pend)
	{
		if(*prx==_CH_CVT&&prx+1!=pend)
		{//a double quotation mark that is not the beginning of a string
			++prx;
			*pwx=_cvt_special_char(*prx);
		}
		else
			if(*prx==_TX('"'))
			{//ignore this double quotation mark
				++prx; continue;
			}
			else
				*pwx=*prx;

		++pwx;
		++prx;
	}
	*pwx=_TX('\0');
}

static void _create_arg_index(char_t *pbeg,char_t *pend, std::vector<const char_t *> &idx)
{
	assert(pbeg==pend||_istspace((ushort)pend[-1]));

	char_t *px=pbeg;

	while(px<pend)
	{
		char_t *pbx=_skip_space(px,pend);
		char_t *pex=_skip_non_str_non_space(pbx,pend);

		if(pbx!=pex)
		{
			_cvt_cmd_string(pbx,pex);
			idx.push_back(pbx);
			px=pex+1;
		}
		else
		{
			assert(pex==pend);

			px=pend;
		}
	}
}



class vfxArgInf
{
public:
	string_t  m_name;		 
	string_t  m_value;
	string_t  m_comment;  //comment to the varible
};

struct _ArgItem
	:vfxArgInf
{
	std::vector<const char_t*>  m_idx;
public:

	void ParseValue(string_t &val)
	{
		if(!val.empty()&&m_idx.empty())
		{
			if(!_istspace((ushort)*val.rbegin()))
			{//append a blank in order to reserve space for '\0' of the last string.
				val.push_back(_TX(' '));
			}
			_create_arg_index(&val[0],&val[0]+val.size(),m_idx);
			
			if(m_idx.empty()) //if contains no string
				val.clear();
		}
	}
	void ParseValue()
	{
		this->ParseValue(m_value);
	}
};

static void _split_arg(const char_t *pbeg, const char_t *pend, std::vector<_ArgItem> &vArg, bool bAddHead)
{
	const char_t *px=pbeg,*pbx=NULL;
	int nhead=0; //number of anonymous varibles, which must be placed at the start of command-line
	int ni=0;    //number of named varibles, which can be placed anywhere after anonymous varibles
	int nq=0;   //number of quotes

	while(px<pend)
	{	
		if(
			(*px==_TX('-')&&px+1!=pend&&_istalpha((ushort)px[1])||*px==_TX('/'))&&(px==pbeg||_istspace((ushort)px[-1]))
			)
		{//the beginning of a named varible, either the following two cases:
		 // 1) '-' followed by alphabetic letters ('-' followed by number may be an negative number)
		 // 2) '/' followed by any character

			if((nq&1)==0) //if not in double quotes
			{
				if(pbx)
				{//save the value of previous varible
					vArg.back().m_value.append(pbx,px);
				}
				pbx=_skip_non_space(px,pend); //the end of the varible name
				
				vArg.push_back(_ArgItem());
				vArg.back().m_name=string_t(px+1,pbx);
				px=pbx;
				++ni; 
			}
		}
		else
			if(ni==0&&bAddHead&&!_istspace((ushort)*px))
			{//the beginning of an anonymous varible
				const char_t *px0=px;
				px=_skip_non_str_non_space(px,pend);

				char_t buf[16];
				_stprintf(buf,_TX("#%d"),nhead); //make an name for the varible

				vArg.push_back(_ArgItem());
				vArg.back().m_name=buf;
				vArg.back().m_value=string_t(px0,px);
				++nhead;
			}
			else
			{
				if(*px==_TX('\"')&&(px==pbeg||px[-1]!=_CH_CVT))
				{
					++nq;
				}
			}
		++px;
	}
	
	if(pbx&&ni>0)
	{//save the value of the last varible
		vArg.back().m_value.append(pbx,pend);
	}
}

class CommandArgSet::_CImp
{
	typedef std::vector<_ArgItem>::iterator _ItrT;
public:
	std::vector<_ArgItem>  m_vArg;
//	int m_first_named;
public:
	_CImp()
//		:m_first_named(0)
	{
	}
	void SetArg(const string_t &arg)
	{
		m_vArg.resize(0);

		if(!arg.empty())
		{
			_split_arg(&arg[0],&arg[0]+arg.size(),m_vArg,true);

			//search the first named
			//int i=0;
			//for(; i<(int)m_vArg.size(); ++i)
			//{
			//	const string_t &vi(m_vArg[i].m_value);
			//	if(!vi.empty() && (vi[0]==_TX('-')||vi[0]==_TX('/')) )
			//	{
			//		break;
			//	}
			//}
			//m_first_named=i;
		}
	}

	_ItrT _find(const char_t *var)
	{
		_ItrT end(m_vArg.end()),itr(m_vArg.begin());

		for(;itr!=end;++itr)
		{
			if(_tcscmp(itr->m_name.c_str(),var)==0)
				break;
		}
		return itr;
	}

	//@bfound : whether the varible is found, note that the return value may be NULL even when the varible exists,
	// which may be for empty value or invalid @count or @pos
	const char_t *const* Query(const char_t *var, int count, int pos, bool &bfound)
	{
		static char_t nbuf[16];

		_ItrT itr(m_vArg.end());

		if(var && var[0]==_TX('#') && pos!=0)
		{//if @pos is specified for #0 #1 #2 ...
			int ii=0;
			if(_stscanf(var+1,_TX("%d"),&ii)==1)
			{
		//		if(ii+pos<m_first_named) //in the range of anonymous
				{
					_stprintf(nbuf,_TX("#%d"),ii+pos); //add pos to the index, in order to support GetVector(...) of #0 #1 #2 ...
					itr=this->_find(nbuf);
					pos=0;
				}
			}
		}
		else
			itr=this->_find(var);

		const char_t *const *pr=NULL;
		
		if(itr!=m_vArg.end())
		{
			itr->ParseValue();
			
			if(pos+count<=(int)itr->m_idx.size())
				pr=&itr->m_idx[pos];

			bfound=true;
		}
		else
			bfound=false;

		return pr;
	}

	_ArgItem* Find(const char_t *var)
	{
		_ItrT itr(this->_find(var));

		return  itr==m_vArg.end()? NULL:&*itr;
	}
};

//VFX_IMPLEMENT_COPY_FUNC(CommandArgSet);

CommandArgSet::CommandArgSet()
:m_pImp(new _CImp)
{
}
CommandArgSet::~CommandArgSet()
{
	delete m_pImp;
}
void CommandArgSet::SetArg(const string_t &arg)
{
	m_pImp->SetArg(arg);
}

void CommandArgSet::SetArg(int argc, char_t *argv[])
{
	string_t val;

	for(int i=1; i<argc; ++i)
	{
		val+=argv[i];
		val.push_back(_TX(' '));
	}

	this->SetArg(val);
}

const char_t *const* CommandArgSet::Query(const char_t *var, int count, int pos,int *flag)
{
	char_t buf[32];
	if(!var)
	{
		_stprintf(buf,_TX("#%d"),pos);
		var=buf;
		pos=0;
	}

	bool bfound;
	const char_t *const* pr=m_pImp->Query(var,count,pos,bfound);

	if(!bfound&&m_pNext)
		pr=m_pNext->Query(var,count,pos);

	if(flag&&!bfound)
		*flag|=ASF_NOT_FOUND;

	return pr;
}
int CommandArgSet::NArg() const
{
	return (int)m_pImp->m_vArg.size();
}
//const vfxArgInf* CommandArgSet::GetArgInf(int idx)
//{
//	return unsigned(idx)<(unsigned)this->NArg()? &m_pImp->m_vArg[idx]:NULL;
//}


_FF_END

