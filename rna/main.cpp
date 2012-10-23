#include <Python.h>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <algorithm> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>


#define SIBL 10
;
#define DEBUG_D 0;//0 - minimal debug;1-norm debug; 2 - max debug;
using namespace std;
string RNA_data;
int W_SIZE,Dmin;
float GAP_DMP;
vector<string> words;
ifstream myfile;
int counter=0;




char reversSymb(char curSymb)
{
	switch (curSymb)
	{
		case 'A':
			return 'T';
		case 'G':
			return 'C';
		case 'T':
			return 'A';
		case 'C':
			return 'G';
	};
	
	return 'x';
}


struct Pair {
    long first;
    long second;
    int length;
};

class Diag
{
public:
	Diag(long first,long second,long len)
	{
		tailX=first;
		headX=first+len;
		tailY=second+len;
		headY=second;
		tailLink=NULL;
		score=len;
		hasHead=false;
        isready=true;
		
	};
	
	
	~Diag()
	{
		;
		tailLink=NULL;
		
		
	};
	
	void debug() const
	{
	
		cout<<"TX:"<<tailX<< " TY:"<<tailY<<" HX:"<<headX<<" HY:"<<headY<<" S:"<<score;
	};
	bool hasTail() const
	{
		return tailLink;
	};

	float canLink(Diag * newHead )
	{
		if(this==newHead)return -999;
		
		long tmX=newHead->headX;
		long tmY=newHead->headY;
		long nhtY=newHead->tailY;
		long nhtX=newHead->tailX;

		
		if(!(tmX>headX && tmY<headY && nhtX>tailX && nhtY<tailY  ))return -555;
		
		if((nhtX>=headX) && (nhtY<=headY))//Целиком внутри
		{
			
			int Dx=nhtX-headX;
			int Dy=headY-nhtY;
			float jump;
			if(Dx==Dy)Dx=Dx/2;
			if(Dx==0 || Dy==0)
			 {
				 jump=abs((headX+headY)-(tmX+tmY));
			 }
			else jump=Dx+Dy;
			float score=newHead->totalLen()-(jump*GAP_DMP);
			return score;
		};
		if((nhtX<headX)&&(nhtY>headY))//Двойное пересечение
		{
			
			int Dx=headX-nhtX;
			int Dy=nhtY-headY;
			float jump;
			//if(Dx==Dy)Dx=0;
			if(Dx==0 || Dy==0)
			{
				jump=abs((headX+headY)-(tmX+tmY));
			}
			else jump=Dx+Dy;
			float score=newHead->totalLen()-(jump*GAP_DMP);
			return score;
			
		};
		if((nhtX<headX)&&(nhtY<=headY))//пересечение по X
		{
			
			int Dx=headX-nhtX;
			int Dy=nhtY-headY;
			float jump;
			//if(Dx==Dy)Dx=0;
			if(Dx==0 || Dy==0)
			{
				jump=abs((headX+headY)-(tmX+tmY));
			}
			else jump=Dx+Dy;
			float score=newHead->totalLen()-(jump*GAP_DMP);
			return score;
			
		};
		
		if((nhtX<headX))//пересечение по Y
		{
			
			int Dx=headX-nhtX;
			int Dy=headY-nhtY;
			float jump;
			//if(Dx==Dy)Dx=0;
			if(Dx==0 || Dy==0)
			{
				jump=abs((headX+headY)-(tmX+tmY));
			}
			else jump=Dx+Dy;
			float score=newHead->totalLen()-(jump*GAP_DMP);
			return score;
			
		};
		return -1000;
	};
	int diagScore() const
	{
		return score;
	};
	void linked()
	{
		hasHead=true;
	};
	bool isLinked()
	{
		return hasHead;
	};
	void breakLink()
	{
		hasHead=false;
		tailLink=NULL;
		score=totalLen();
	};
	void link(Diag * newTail,int scr)
	{
		if(newTail->isLinked() && hasTail())
		{
			return;
		}
		if(tailLink)cout<<"HAS TAIL!!!"<<endl;
		if(newTail==this)
		{
			cout<<"ptr circle!!!!"<<endl;};
		
		tailLink=newTail;
		newTail->linked(); 
		score=score+scr;
	};
	bool hasTail()
	{

		if(tailLink)return true;
		return false;
	}
    long tailMinX()
	{
		if(hasTail())
			return tailLink->tailMinX();
		return tailX;
	}
	
	long tailMaxY()
	{
		if(tailLink)return tailLink->tailMaxY();
		return tailY;
	}
	long totalLen()
	{
		return headX-tailMinX(); 
	};
	long tailX;
	long tailY;
	long headX;
	long headY;
private:
	Diag * tailLink;
	float score;
	bool hasHead;
	bool isready;
};

Pair createPair(long pos1,long pos2,int length)
{
	Pair toret;
	toret.first=pos1;
	toret.second=pos2;
	toret.length=length;
	return toret;
	
	
};


class MainDiagonal
{
public:
	MainDiagonal()
	{
	
	};
	~MainDiagonal()
	{
		clear();	
	};
	
	void debug() const
	{
		cout<<"MAIN DIAG DEBUG"<<endl;
		for(int i=0;i<diags.size();i++)
		{
			cout<<"  ";
			diags[i].debug();
		};
	};
	vector<Diag>* Diags(){
		return &diags;};
	bool empty()
	{
		return (diags.size()==0);
	};
	void clear()
	{
		diags.clear();
	};
private:
	vector<Diag> diags; 
	long len;
};

vector<MainDiagonal> diagonals;

class resultPair
{
public:
	resultPair(string word1, string word2)
	{
		w1=word1;
		w2=word2;
	};
	
	void append(Pair p)
	{
		pairs.push_back(p);
	};
	string debug() const
	{
		stringstream  toret;
		toret <<w1 <<" - "<<w2;
		for(int i=0;i<pairs.size();i++)
		{
			//toret<< " RP:"<<pairs[i].first<<":"<<pairs[i].second<<" ";
		};
		return toret.str();
		
	};
	string w1,w2;
	vector<Pair> pairs;
	
};

vector <resultPair> resultPr;


bool checkBack(long pos,long pos2)
{
	if(pos==0)return true;
	if(pos2+W_SIZE+1 >= RNA_data.length() )return true;
	if(RNA_data[pos-1]==reversSymb(RNA_data[pos2+W_SIZE]))return false;
	return true;
};

bool posOk(long pos1,long pos2,int length)
{
	return ((pos1+length+Dmin<pos2)   &&(checkBack(pos1,pos2)));	
};

class pairList
{
public:
	pairList(){firstEmpty=0;};
	~pairList()
	{
		pairs.clear();
	};
	void append(long pos1,long pos2)
	{
		pairs.push_back(createPair(pos1,pos2,W_SIZE));
	};
	
	string debug()
	{
		stringstream  toret;
		for(int i=0;i<pairs.size();i++)
		{
			toret<<" "<<pairs.at(i).first<<":"<<pairs.at(i).second;
		};
		return toret.str();
	};
	

	
	bool setSecondNLength(long second,int length)
	{
		bool toret=false;
		for(int i=firstEmpty;i<pairs.size();i++)
		{
			
			if(pairs.at(i).second==-1)
			{
				//firstEmpty=i;
				if((pairs.at(i).first+length+Dmin<second)   &&(checkBack(pairs.at(i).first,second)))
				{ pairs.at(i).second=second;
					pairs.at(i).length=length;
				    
					toret=true;
				};
			}
		};
		return toret;
	};
	int size()
	{
		return pairs.size();
	};
	long first(int num)
	{
		if(num<size() && num>-1)
			return pairs.at(num).first;
		else return -1;
	};
	void clean()
	{
		pairs.clear();
	};
	
	vector <Pair> pairs;
	int firstEmpty;
};




map<string,pairList> v_str;

char nextSymb(char curSymb)
{
	switch (curSymb)
	{
		case 'A':
			return 'T';
		case 'T':
			return 'G';
		case 'G':
			return 'C';
	};
	
	return 'x';
}


string revWord(string curWord)
{
	string toret=curWord;
	for(int i=0;i<curWord.length();i++)
	{
		toret[curWord.length()-1-i]=reversSymb(curWord[i]);
	}
	return toret;
};


string nullTail(string word,int pos)
{
	string toret=word;
	for(int i=pos;i<word.length();i++)
	{
		// cout <<"null pos:" <<i<<endl;
		toret[i]='A';
	};
	return toret;
}

string nextWord(string word)
{
	int pos=word.length()-1;
	while(true){
		if(word.at(pos)!='C')
		{
			// cout << "pos:"<<pos<< endl;
			word[pos]=nextSymb(word.at(pos));
			return nullTail(word,pos+1);
		}
		else
		{
			if(pos>0)pos--;
			else
			{
				word='T'+word;
				pos=0;
				return nullTail(word,pos+1);
			};
		};
	};
	
}

pairList findWordFwd(string word)
{
	pairList toret;
	size_t found=RNA_data.find(word,0);
	while(found!=string::npos)
	{
		toret.append(long(found),-1);
		found=RNA_data.find(word,found+1);
	};
	return toret;
}

int createWordsList(int wordLen)
{
	v_str.clear();
	string word="A";
	while(word.length()<wordLen)// Create first word
	{
		word+='A';
	}
	
	int count=0;
	while (word.length()<wordLen+1)
    {
		word=nextWord(word);
		
		
		//cout<<word<<v_str[word].debug()<<endl;
		if(word.length()<=wordLen)
		{
			words.push_back(word);
			v_str[word]=findWordFwd(word);
		};
		count++;
    };
	return count;
}

void setPairs(long pos1, long pos2,string w1,string w2,resultPair *newPair)
{
	int len=w1.size();
	int c=0;
	//cout<<"Pos1:"<<pos1<<" Pos2:"<<pos2<<endl;
	//cout <<"sp w1:"<<w1<<endl;
	//cout <<"sp w2:"<<w2<<endl;
   // cout<< "s cw1:"<<RNA_data.substr(pos1, len)<<"("<<RNA_data.substr(pos1,len+2)<<")"<<endl;
	//cout<< "s cw2:"<<RNA_data.substr(pos2, len)<<endl;
	
	
	while((pos1+len<RNA_data.size())&&(pos2-c > 0)&&(RNA_data[pos1+len]==reversSymb(RNA_data[pos2-(c+1)])))
	{
		w1+=RNA_data[pos1+len];
		w2=RNA_data[pos2-c]+w2;
		c++;
		len=w1.size();
	};
	
	
	 if(posOk(pos1+len,pos2-c,len))
	   {
	
	   resultPair longPair=resultPair(w1,w2);	
	   longPair.append(createPair(pos1,pos2-c,len));
	   resultPr.push_back(longPair);
 	   //cout<<longPair.debug()<<endl;
		   int id_diag=pos2-c+len+pos1;
		//   cout<<"Creating Diag:"<<id_diag<<endl;;
		   diagonals.at(id_diag).Diags()->push_back(Diag(pos1,pos2-c,len));
		 //  cout<<"   sm diag id:"<<diagonals.at(id_diag).Diags()->size()-1;
    	}
	
	



}

		


void findPairs()
{
	resultPr.clear();
	for(int i=0;i<words.size();i++)
	{
		string word=words.at(i);
		int wp_len=v_str[word].size();
		if(wp_len==0)continue;
		string rev_str=revWord(word);
		int wr_len=v_str[rev_str].size();
		if(wr_len==0)continue;
		resultPair newPair=resultPair(words.at(i),rev_str);
		for(int j=0;j<wp_len;j++)
		{
			for(int k=0;k<wr_len;k++)
			{
			if(v_str[word].setSecondNLength(v_str[rev_str].first(k),W_SIZE))				
				setPairs(v_str[word].first(j),v_str[rev_str].first(k),words.at(i),rev_str,&newPair);
			};
				
		};
		resultPr.push_back(newPair);
		//cout<<newPair.debug()<<endl;
	};
};

void doDiag(long diag_id,int smd_id)
{
	//cout<<"--LinK:"<<diag_id<<endl;
	Diag*  curDiag=&diagonals[diag_id].Diags()->at(smd_id);
	Diag*  topDiag=NULL;
	float top_score=-1;
	//if(curDiag->isLinked())return;
	long to=diag_id+SIBL;
	long from=diag_id-SIBL;
	if(to>diagonals.size())to=diagonals.size();
	if(from<0)from=0;
	for(int i=from;i<to;i++)
	{
		if(diagonals[i].empty())continue;
		for(int j=0;j<diagonals[i].Diags()->size();j++)
		{
			//cout<<"Link:"<<diag_id<<" SIZE:"<<diagonals[i].Diags()->size()<<" sm:"<<smd_id;
			if(i==diag_id && j==smd_id)continue;
			if(diagonals[i].Diags()->at(j).hasTail())continue;
			float score=curDiag->canLink(&diagonals[i].Diags()->at(j));
			if(score>top_score)
			{
				top_score=score;
				topDiag=&diagonals[i].Diags()->at(j);
			};
			//cout<<"Link:"<<diag_id<<" sm:"<<smd_id<<" 2:"<<i<<":"<<j<<"  score:"<<score<<endl;
		};
		if(top_score>0)
		{
			topDiag->link(curDiag,top_score);
			//if(score>6)cout<<"|TХ|"<<curDiag->tailMinX()<<"|HX|"<<curDiag->headX<<"|HY|"<<curDiag->tailY<<"|TY|"<<curDiag->tailMaxY()<<"|SC|"<<score<<endl;
			return;
		};
	};	
	
};
void diagGlue()
{
	for(int dg=0;dg<diagonals.size();dg++)
	{
		if(dg==188)
		{
		//	cout<<188;
		};
		if(diagonals[dg].empty())continue;
		for(int i=0;i<diagonals[dg].Diags()->size();i++)
		{
			doDiag(dg,i);
		};
	};
}; 

string getFormFile(long start,long frame)
{
	

	char c;
	c=myfile.peek();;
	myfile.seekg(start);
	string toret;
	while(myfile.tellg()<start+frame && !myfile.eof())
		  {
			  int pos=myfile.tellg();
			  // c=myfile.peek();;
			  myfile>>c;
			 // cout<<"C:"<<c<<endl;
			  if(c=='a')c='A';
			  if(c=='t' || c=='u' || c=='U')c='T';
			  if(c=='g')c='G';
			  if(c=='c')c='C';
			  if(c=='.' ||c==' ' || c=='\n' || c=='>')continue;
			  toret+=c;
		  };
	return toret;
};

string mainString;

string getFormString(string * data,long start,long frame)
{
	long pos=start;
	char c;
	string toret;
	while(pos<data->size() && pos<start+frame)
	{
		c=data->at(pos);
		if(c=='a')c='A';
		if(c=='t' || c=='u' || c=='U')c='T';
		if(c=='g')c='G';
		if(c=='c')c='C';
		if(c=='.' ||c==' ' || c=='\n' || c=='>')continue;
		toret+=c;
		pos++;
	};
	return toret;
};

bool compare( const Diag* a, const Diag* b)
{

	
	return a->diagScore() > b->diagScore();
	
};
vector<Diag*>  diagsList;
void createDiagsList()
{
	int count=0;
	diagsList.clear();
	for(int i=0;i<diagonals.size();i++)
	{
		if(diagonals[i].empty())continue;
		//cout << endl;
	//	cout<<"Diag summ="<<i<<endl;
		int k=0;
		for(k;k<diagonals[i].Diags()->size();k++)
		{diagsList.push_back(&diagonals[i].Diags()->at(k));
			count++;
		}
		//cout<<"Diags count:"<<k<<endl;
	};
	//cout<<"Diags count:"<<count<<endl;
	sort(diagsList.begin(), diagsList.end(),compare);
		
};


vector<Diag*> cleanList;
bool conflict(Diag* a,Diag * b)
{
	long curTail=a->tailMinX();
    long newTailX=b->tailMinX();
	
	return !((curTail > b->tailMaxY()) || (a->headX < b->tailMinX() && a->headY > b->tailMaxY()) ||(a->tailMaxY()<b->tailMinX() ));
};
bool checkConfl(Diag* b)
{
	for(int i=0;i<cleanList.size();i++)
	{
		if(conflict(cleanList.at(i),b))return true;
	};
	return false;
};
void createCleanList()
{
	cleanList.clear();
  	for(int i=0;i<diagsList.size();i++)
    {
		
	//	cout<<"Cur pos:"<<i<<endl;
		if(!checkConfl(diagsList.at(i)))cleanList.push_back(diagsList.at(i));
	}
}

struct result
{
	long start;
	long end;
	int score;
    inline bool operator==(const result &other) {
        return start==other.start && end==other.end && score==other.score;
    }
};

typedef std::vector<result> myres;
myres doAll(string data )
{
	
	long frame=2000;
	int jam=1000;
	long curpos=0,respos;
	GAP_DMP=0.333;
	while(curpos<data.size())
	{
		words.clear();
		diagonals.clear();
		RNA_data=getFormString(&data,curpos,frame);
		
		diagonals.resize(2*RNA_data.length());
		//	cout <<"Data len:"<<RNA_data.length()<<endl;
		createWordsList(W_SIZE); //Создаем писок слов к каждому слову где встречается.
		//	cout<<"get_pairs"<<endl;
		findPairs();
	
		diagGlue();
		

		
		
		
		
		//	cout<<"Start:"<< curpos<<"W LEN:"<<W_SIZE<<" GAP:1/"<<GAP_DMP<<endl;
		
		createDiagsList();
		createCleanList();
		for(int i=0;i<cleanList.size();i++)
		{
			cout<<i<<","<<cleanList.at(i)->tailMinX()+curpos<<","<<cleanList.at(i)->tailMaxY()+curpos<<","<<cleanList.at(i)->diagScore()<<endl;
		};
		curpos=curpos+frame-jam;
	};

    myres test;
    result test_elem1, test_elem2;

    test_elem1.start = 123;
    test_elem1.end = 456;
    test_elem1.score = 789;

    test_elem2.start = 234;
    test_elem2.end = 567;
    test_elem2.score = 890;

    test.push_back(test_elem1);
    test.push_back(test_elem2);
    return test;
	
};


void doMultiFrame()
{
	cout << "Multiframe Mode"<<endl;

	long frame=2000;
	int jam=1000;
	long curpos=0,respos;
	GAP_DMP=0.333;
	cout<<"FRAME:"<<frame<<" JAM:"<<jam<<" GAP:"<<GAP_DMP<<" SIBL 10"<<endl;
	while(!myfile.eof())
	{
		//cout <<endl;
	words.clear();	
    diagonals.clear();
	RNA_data=getFormFile(curpos,frame);
		
	diagonals.resize(2*RNA_data.length());
	//	cout <<"Data len:"<<RNA_data.length()<<endl;
	createWordsList(W_SIZE); //Создаем писок слов к каждому слову где встречается.
	//	cout<<"get_pairs"<<endl;
	findPairs();
    
		if(curpos==8000)
		{
	//		cout << endl;
		};
	//	cout<<"Diag glue";
	diagGlue();
		
	int	counter=0;
	int	maxScore=-1;
	int	linkCount=0;
		
		

		
	//	cout<<"Start:"<< curpos<<"W LEN:"<<W_SIZE<<" GAP:1/"<<GAP_DMP<<endl;
		
		createDiagsList();
		createCleanList();
		for(int i=0;i<cleanList.size();i++)
		{
			cout<<i<<","<<cleanList.at(i)->tailMinX()+curpos<<","<<cleanList.at(i)->tailMaxY()+curpos<<","<<cleanList.at(i)->diagScore()<<endl;
		};
		curpos=curpos+frame-jam;
	};
	cout << "End of Multiframe Mode"<<endl;

};



BOOST_PYTHON_MODULE(_rna){
using namespace boost::python;


class_<result>("result")
          .def_readwrite("start", &result::start)
          .def_readwrite("end", &result::end)
          .def_readwrite("score", &result::score);

class_< myres >("myres")
          .def( vector_indexing_suite<myres>() );


def("doAll",&doAll);
}
