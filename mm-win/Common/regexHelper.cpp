#include "regexHelper.h"
#include <windows.h>

bool CRegexHelper::IsValidEmailAddr( const char* strEmailAddr )
{
	regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");	
	return regex_match( strEmailAddr, pattern );
}

bool CRegexHelper::IsValidMobileNo(const char* strPhoneNo)
{
	//regex pattern("((130|131|132|133||134|135|136|137|138|139|145|147|150|151|152|154|157|158|159|176|177|178|181|182|183|184|187|188|155|156|185|186|153|180|189)\\d{8})");
	regex pattern("^[1][3-8][0-9]{9}$");
	return regex_match( strPhoneNo, pattern);
}


bool CRegexHelper::IsFirstPYMatch(const char* strText, const char* strPattern)
{
	tstring strFirstPYText;
	GenFirstPYLetter(strText, strFirstPYText);

	int patternLen = strlen(strPattern) + 1;
	char* lpBuf = new char[patternLen];
	memset(lpBuf, 0, patternLen);
	for(int idx = 0; idx < strlen(strPattern); idx++)
		lpBuf[idx] = toupper(strPattern[idx]);

	regex pattern(lpBuf);	
	delete []lpBuf;
	return  regex_search( strFirstPYText, pattern );
}


void  CRegexHelper::FirstLetter(int nCode, std::tstring& strLetter)  
{  
	if(nCode >= 1601 && nCode < 1637) strLetter = "A";  
	if(nCode >= 1637 && nCode < 1833) strLetter = "B";  
	if(nCode >= 1833 && nCode < 2078) strLetter = "C";  
	if(nCode >= 2078 && nCode < 2274) strLetter = "D";  
	if(nCode >= 2274 && nCode < 2302) strLetter = "E";  
	if(nCode >= 2302 && nCode < 2433) strLetter = "F";  
	if(nCode >= 2433 && nCode < 2594) strLetter = "G";  
	if(nCode >= 2594 && nCode < 2787) strLetter = "H";  
	if(nCode >= 2787 && nCode < 3106) strLetter = "J";  
	if(nCode >= 3106 && nCode < 3212) strLetter = "K";  
	if(nCode >= 3212 && nCode < 3472) strLetter = "L";  
	if(nCode >= 3472 && nCode < 3635) strLetter = "M";  
	if(nCode >= 3635 && nCode < 3722) strLetter = "N";  
	if(nCode >= 3722 && nCode < 3730) strLetter = "O";  
	if(nCode >= 3730 && nCode < 3858) strLetter = "P";  
	if(nCode >= 3858 && nCode < 4027) strLetter = "Q";  
	if(nCode >= 4027 && nCode < 4086) strLetter = "R";  
	if(nCode >= 4086 && nCode < 4390) strLetter = "S";  
	if(nCode >= 4390 && nCode < 4558) strLetter = "T";  
	if(nCode >= 4558 && nCode < 4684) strLetter = "W";  
	if(nCode >= 4684 && nCode < 4925) strLetter = "X";  
	if(nCode >= 4925 && nCode < 5249) strLetter = "Y";  
	if(nCode >= 5249 && nCode < 5590) strLetter = "Z";  
}  

void  CRegexHelper::GenFirstPYLetter(std::tstring strName, std::tstring&strResult )  
{  
	strResult.clear();
	if (strName.empty())  	return;  

	for (size_t i=0; i < strName.size()-1; i++)  
	{  
		if ( (unsigned char)strName[i] < 0x80 )  
		{  
			if ((unsigned char)strName[i] >= 'a' && (unsigned char)strName[i] <='z')
			{
				strName[i] -= ('a' - 'A');
			}
			strResult += strName[i];  
			continue;  
		}  
		int  nCode;  
		unsigned char ucHigh = (unsigned char)strName[i];  
		unsigned char ucLow  = (unsigned char)strName[i+1];  

		if ( ucHigh < 0xa1 || ucLow < 0xa1)  
			continue;  
		else  
			// Treat code by section-position as an int type parameter,  
			// so make following change to nCode.  
			nCode = (ucHigh - 0xa0) * 100 + ucLow - 0xa0;  

		std::tstring strRet;  
		FirstLetter(nCode, strRet);  
		strResult += strRet;  
		i++;  
	}  
}  

bool CRegexHelper::IsContainChineseCharacter( tstring strTxt )
{
	regex pattern("(.*[\u4E00-\u9FA5]+)|([\u4E00-\u9FA5]+.*)");
	return  regex_search( strTxt, pattern );
}

bool CRegexHelper::IsEmptyString( tstring strTxt )
{
	regex pattern("(^\\s*$)");	
	return regex_match( strTxt, pattern );
}

tstring CRegexHelper::ConvertChineseUnicodeToPyt(tstring strText)  
{   
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, strText.c_str(), strlen( strText.c_str()), NULL, 0);  
	wchar_t* chrstr = new wchar_t[wcsLen + 1];  
	::MultiByteToWideChar(CP_ACP, NULL,  strText.c_str(), strlen( strText.c_str()), chrstr, wcsLen);  
	chrstr[wcsLen] = '\0';  

	const  int pyvalue[] = {-20319,-20317,-20304,-20295,-20292,-20283,-20265,-20257,-20242,-20230,-20051,-20036,-20032,-20026,  
		-20002,-19990,-19986,-19982,-19976,-19805,-19784,-19775,-19774,-19763,-19756,-19751,-19746,-19741,-19739,-19728,  
		-19725,-19715,-19540,-19531,-19525,-19515,-19500,-19484,-19479,-19467,-19289,-19288,-19281,-19275,-19270,-19263,  
		-19261,-19249,-19243,-19242,-19238,-19235,-19227,-19224,-19218,-19212,-19038,-19023,-19018,-19006,-19003,-18996,  
		-18977,-18961,-18952,-18783,-18774,-18773,-18763,-18756,-18741,-18735,-18731,-18722,-18710,-18697,-18696,-18526,  
		-18518,-18501,-18490,-18478,-18463,-18448,-18447,-18446,-18239,-18237,-18231,-18220,-18211,-18201,-18184,-18183,  
		-18181,-18012,-17997,-17988,-17970,-17964,-17961,-17950,-17947,-17931,-17928,-17922,-17759,-17752,-17733,-17730,  
		-17721,-17703,-17701,-17697,-17692,-17683,-17676,-17496,-17487,-17482,-17468,-17454,-17433,-17427,-17417,-17202,  
		-17185,-16983,-16970,-16942,-16915,-16733,-16708,-16706,-16689,-16664,-16657,-16647,-16474,-16470,-16465,-16459,  
		-16452,-16448,-16433,-16429,-16427,-16423,-16419,-16412,-16407,-16403,-16401,-16393,-16220,-16216,-16212,-16205,  
		-16202,-16187,-16180,-16171,-16169,-16158,-16155,-15959,-15958,-15944,-15933,-15920,-15915,-15903,-15889,-15878,  
		-15707,-15701,-15681,-15667,-15661,-15659,-15652,-15640,-15631,-15625,-15454,-15448,-15436,-15435,-15419,-15416,  
		-15408,-15394,-15385,-15377,-15375,-15369,-15363,-15362,-15183,-15180,-15165,-15158,-15153,-15150,-15149,-15144,  
		-15143,-15141,-15140,-15139,-15128,-15121,-15119,-15117,-15110,-15109,-14941,-14937,-14933,-14930,-14929,-14928,  
		-14926,-14922,-14921,-14914,-14908,-14902,-14894,-14889,-14882,-14873,-14871,-14857,-14678,-14674,-14670,-14668,  
		-14663,-14654,-14645,-14630,-14594,-14429,-14407,-14399,-14384,-14379,-14368,-14355,-14353,-14345,-14170,-14159,  
		-14151,-14149,-14145,-14140,-14137,-14135,-14125,-14123,-14122,-14112,-14109,-14099,-14097,-14094,-14092,-14090,  
		-14087,-14083,-13917,-13914,-13910,-13907,-13906,-13905,-13896,-13894,-13878,-13870,-13859,-13847,-13831,-13658,  
		-13611,-13601,-13406,-13404,-13400,-13398,-13395,-13391,-13387,-13383,-13367,-13359,-13356,-13343,-13340,-13329,  
		-13326,-13318,-13147,-13138,-13120,-13107,-13096,-13095,-13091,-13076,-13068,-13063,-13060,-12888,-12875,-12871,  
		-12860,-12858,-12852,-12849,-12838,-12831,-12829,-12812,-12802,-12607,-12597,-12594,-12585,-12556,-12359,-12346,  
		-12320,-12300,-12120,-12099,-12089,-12074,-12067,-12058,-12039,-11867,-11861,-11847,-11831,-11798,-11781,-11604,  
		-11589,-11536,-11358,-11340,-11339,-11324,-11303,-11097,-11077,-11067,-11055,-11052,-11045,-11041,-11038,-11024,  
		-11020,-11019,-11018,-11014,-10838,-10832,-10815,-10800,-10790,-10780,-10764,-10587,-10544,-10533,-10519,-10331,  
		-10329,-10328,-10322,-10315,-10309,-10307,-10296,-10281,-10274,-10270,-10262,-10260,-10256,-10254};  

	const char pystr[396][7] = {"a","ai","an","ang","ao","ba","bai","ban","bang","bao","bei","ben","beng","bi","bian","biao",  
		"bie","bin","bing","bo","bu","ca","cai","can","cang","cao","ce","ceng","cha","chai","chan","chang","chao","che","chen",  
		"cheng","chi","chong","chou","chu","chuai","chuan","chuang","chui","chun","chuo","ci","cong","cou","cu","cuan","cui",  
		"cun","cuo","da","dai","dan","dang","dao","de","deng","di","dian","diao","die","ding","diu","dong","dou","du","duan",  
		"dui","dun","duo","e","en","er","fa","fan","fang","fei","fen","feng","fo","fou","fu","ga","gai","gan","gang","gao",  
		"ge","gei","gen","geng","gong","gou","gu","gua","guai","guan","guang","gui","gun","guo","ha","hai","han","hang",  
		"hao","he","hei","hen","heng","hong","hou","hu","hua","huai","huan","huang","hui","hun","huo","ji","jia","jian",  
		"jiang","jiao","jie","jin","jing","jiong","jiu","ju","juan","jue","jun","ka","kai","kan","kang","kao","ke","ken",  
		"keng","kong","kou","ku","kua","kuai","kuan","kuang","kui","kun","kuo","la","lai","lan","lang","lao","le","lei",  
		"leng","li","lia","lian","liang","liao","lie","lin","ling","liu","long","lou","lu","lv","luan","lue","lun","luo",  
		"ma","mai","man","mang","mao","me","mei","men","meng","mi","mian","miao","mie","min","ming","miu","mo","mou","mu",  
		"na","nai","nan","nang","nao","ne","nei","nen","neng","ni","nian","niang","niao","nie","nin","ning","niu","nong",  
		"nu","nv","nuan","nue","nuo","o","ou","pa","pai","pan","pang","pao","pei","pen","peng","pi","pian","piao","pie",  
		"pin","ping","po","pu","qi","qia","qian","qiang","qiao","qie","qin","qing","qiong","qiu","qu","quan","que","qun",  
		"ran","rang","rao","re","ren","reng","ri","rong","rou","ru","ruan","rui","run","ruo","sa","sai","san","sang",  
		"sao","se","sen","seng","sha","shai","shan","shang","shao","she","shen","sheng","shi","shou","shu","shua",  
		"shuai","shuan","shuang","shui","shun","shuo","si","song","sou","su","suan","sui","sun","suo","ta","tai",  
		"tan","tang","tao","te","teng","ti","tian","tiao","tie","ting","tong","tou","tu","tuan","tui","tun","tuo",  
		"wa","wai","wan","wang","wei","wen","weng","wo","wu","xi","xia","xian","xiang","xiao","xie","xin","xing",  
		"xiong","xiu","xu","xuan","xue","xun","ya","yan","yang","yao","ye","yi","yin","ying","yo","yong","you",  
		"yu","yuan","yue","yun","za","zai","zan","zang","zao","ze","zei","zen","zeng","zha","zhai","zhan","zhang",  
		"zhao","zhe","zhen","zheng","zhi","zhong","zhou","zhu","zhua","zhuai","zhuan","zhuang","zhui","zhun","zhuo",  
		"zi","zong","zou","zu","zuan","zui","zun","zuo"};  

	int chrasc = 0;           

	char* pcReturnString = NULL;  

	//unicode转换为ANSI  
	int length = WideCharToMultiByte(CP_ACP, 0, chrstr, -1, 0, 0, 0, 0 );   
	char* nowchar = new char[length+1];  
	memset(nowchar,0,length+1);  
	WideCharToMultiByte( CP_ACP, 0, chrstr, -1, nowchar,length, 0, 0 );  

	//转换ANSI,字符部分不变，汉字转换成相应的拼音  
	char *returnstr = new char[6*length +1];  
	memset(returnstr,0,6*length +1);  

	int offset = 0;   
	for (int j = 0; j < length;) // 循环处理字节数组  
	{  
		if (nowchar[j] >= 0 && nowchar[j] < 128) // 非汉字处理  
		{ 
			returnstr[offset] = nowchar[j];  
			offset++;  
			j++;  
			continue;  
		}  

		// 汉字处理  
		chrasc = nowchar[j] * 256 + nowchar[j+1]+256;  

		if (chrasc > 0 && chrasc < 160)  
		{  
			returnstr[offset]= nowchar[j];  
			offset++;  
			j++;  
		}  
		else  
		{  
			for (int i = (sizeof(pyvalue)/sizeof(pyvalue[0]) - 1); i >= 0; i--)  
			{  
				if (pyvalue[i] <= chrasc)  
				{  
					strcpy(returnstr+offset ,pystr[i]);  

					offset +=strlen(pystr[i]);    
					break;  
				}  
			}  
			j +=2;  
		}  
	}  
	if(strlen(returnstr)>0)  
	{  
		pcReturnString = new char[strlen(returnstr)+1];  
		memset(pcReturnString,0,strlen(returnstr)+1);  
		strcpy(pcReturnString,returnstr);  
	}  

	delete []returnstr;  
	delete []nowchar; 

	tstring strReturn;
	if (pcReturnString != NULL)
	{
		strReturn = pcReturnString;
	}
	
	delete []pcReturnString;
	return strReturn;  
}  

bool CRegexHelper::IsValidOrgName( tstring strText )
{
	for (int i=0; i<strText.length(); i++)
	{
		int iValue = strText[i];
		if (iValue >=0 && iValue <=47)
			return false;

		if (iValue >= 58 && iValue <= 64)
			return false;

		if (iValue >= 91 && iValue <= 96)
			return false;

		if (iValue >= 123 && iValue <= 127)
			return false;
		
	}
	return true;
}



