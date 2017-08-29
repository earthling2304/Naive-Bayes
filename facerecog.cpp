#include<iostream>
#include<algorithm>
#include<iterator>
#include<fstream>
#include<string>
#include<cstring>
#include<cmath>
/*other headers*/

using namespace std;

float findpf(string str){
	fstream aFile;
	aFile.open(str.c_str(), ios::out|ios::in);
	int lines_count= count(istreambuf_iterator<char>(aFile), istreambuf_iterator<char>(), '\n');
	aFile.seekg(0, ios::beg);
	int f = count(istreambuf_iterator<char>(aFile), istreambuf_iterator<char>(), '1');
	aFile.seekg(0, ios::beg);
	float pF = float(f)/float(lines_count);
	aFile.close();
	return pF;
}

double probarrayF[70][60]={0}; // each element on the array contains p(xi|F) for 4200 attributes
double probarrayNF[70][60]={0}; // each element on the array contains p(xi|F) for 4200 attributes
double pF, pNF;

int calculatePriorF(string str1, string str2){
	//We need two file ptrs- both instream.
	ifstream flabels, fdata;
	
	flabels.open(str1.c_str(), ios::in);
	fdata.open(str2.c_str(), ios::in);
	int f = count(istreambuf_iterator<char>(flabels), istreambuf_iterator<char>(), '1');
	flabels.seekg(0, ios::beg);
	/*iterate through each line of flabels, see if it's 1. 
	If yes, then scan through next 70 lines in fdata. 
	If not, read next line in flabels and skip 70 lines in fdata. 
	Repeat till EOF.
	*/
	char str[2];
	int i,j;
	int count = 0;
	/*
ofstream newfile;
newfile.open("newfile", ios::out); //YAY IT'S WORKING
*/
	while(flabels.good()){
		flabels.getline(str, 2);
		char a = str[0];
		//cout<<a<<endl;
		if(a=='1'){ //scan through 70 lines on fdata
			char buf[61];
			fdata.seekg(0, ios::beg);
			for(i=0;i<70*count;i++)
				fdata.getline(buf,61);				
			for(i=0;i<70;i++){
				fdata.getline(buf, 61);
				//newfile<<buf<<endl;
				for(j=0;j<60&& buf[j]!='\n';j++){
					if(buf[j] == '#') probarrayF[i][j]++;
				}			
			}
			
		}
		
		count++;
	}

	for(i=0;i<70;i++){
		for(j=0;j<60;j++){
			//probarrayF[i][j] *=100;
			probarrayF[i][j] /= f;
			//cout<<probarrayF[i][j]<<" ";
			//cout<<probarrayNF[i][j]<<" ";
		}
		//cout<<endl;
	}

//cout<<f<<endl;
	fdata.close();
	flabels.close();
	return f;
}

int calculatePriorNF(string str1, string str2){
	//We need two file ptrs- both instream.
	ifstream flabels, fdata;
	
	flabels.open(str1.c_str(), ios::in);
	fdata.open(str2.c_str(), ios::in);
	int nf = count(istreambuf_iterator<char>(flabels), istreambuf_iterator<char>(), '0');
	flabels.seekg(0, ios::beg);
	/*iterate through each line of flabels, see if it's 1. 
	If yes, then scan through next 70 lines in fdata. 
	If not, read next line in flabels and skip 70 lines in fdata. 
	Repeat till EOF.
	*/
	char str[2];
	int i,j;
	int count = 0;
	/*
ofstream newfile;
newfile.open("newfile", ios::out); //YAY IT'S WORKING
*/
	while(flabels.good()){
		flabels.getline(str, 2);
		char a = str[0];
		//cout<<a<<endl;
		if(a=='0'){ //scan through 70 lines on fdata
			char buf[61];
			fdata.seekg(0, ios::beg);
			for(i=0;i<70*count;i++)
				fdata.getline(buf,61);				
			for(i=0;i<70;i++){
				fdata.getline(buf, 61);
				//newfile<<buf<<endl;
				for(j=0;j<60&& buf[j]!='\n';j++){
					if(buf[j] == '#') probarrayNF[i][j]++;
				}			
			}
			
		}
		
		count++;
	}
	for(i=0;i<70;i++){
		for(j=0;j<60;j++){
			//probarrayNF[i][j] *=100;
			probarrayNF[i][j] /= nf;
			
		}
		//cout<<endl;
	}


	fdata.close();
	flabels.close();
	return nf;
}


void calculatePost(){
	//test data ka scanning
	ifstream fdata, flabels;
	fdata.open("facedatatest", ios::in);
	flabels.open("facedatatestlabels", ios::in);
	int patches = count(istreambuf_iterator<char>(flabels), istreambuf_iterator<char>(), '\n');
	flabels.close();
	//cout<<patches<<endl;
	int count = 0; // which patch are we looking at?
	char buf[61];
	int i,j,k;
	double postF,postNF;
	ofstream out;
	out.open("result", ios::out);	
	for(k=0;k<patches;k++){
		postF = log(pF);
		postNF = log(pNF);
		fdata.seekg(0, ios::beg);
		for(i=0;i<70*k;i++) fdata.getline(buf, 61);
		for(i=0;i<70;i++){
			fdata.getline(buf, 61);
			//cout<<buf<<endl;
			for(j=0;j<60;j++){
				if(buf[j] == '#'){
					postF += log(probarrayF[i][j]);
					postNF += log(probarrayNF[i][j]);

				}
				
				if(buf[j] == ' '){
					postF += log(1-probarrayF[i][j]);
					postNF += log(1-probarrayNF[i][j]);
					
				}
				//cout<<postF<<" "<<postNF<<endl;	
			}		
		}
	
	if(postF>=postNF) out<<'1'<<endl;
	else out<<'0'<<endl;
	}
fdata.close();
out.close();	
}
int TP=0,TN=0,FP=0,FN=0;
void confusionMatrix(){
	ifstream f1,f2;
	f1.open("facedatatestlabels", ios::in);
	f2.open("result", ios::in);
	char buf1[2], buf2[2];
	int d = 0;
	int i,j;
	char dummy[61];
	ofstream falpo;
	falpo.open("FP", ios::out);
	ofstream falneg;
	falneg.open("FN", ios::out);
	while(f1.good()&&f2.good()){
		f1.getline(buf1,2);
		f2.getline(buf2,2);
		if(buf1[0] == '1' && buf2[0] == '1') TP++;
		else if(buf1[0] == '0' && buf2[0] == '0') TN++;	
		else if(buf1[0] == '0' && buf2[0] == '1'){ FP++;
			
			ifstream tstdata;
			tstdata.open("facedatatest", ios::in);
			
			for(i=0;i<d*70;i++)
				tstdata.getline(dummy, 61);
			for(i=0;i<70;i++){
				tstdata.getline(dummy, 61);
				falpo<<dummy<<endl;
			}
			tstdata.close();

		}
		else if(buf1[0] == '1' && buf2[0] == '0'){ FN++;
			ifstream tstdata;
			tstdata.open("facedatatest", ios::in);
			
			for(i=0;i<d*70;i++)
				tstdata.getline(dummy, 61);
			for(i=0;i<70;i++){
				tstdata.getline(dummy, 61);
				falneg<<dummy<<endl;
			}
			tstdata.close();

		}	
		d++;
	}
	falpo.close();
	falneg.close();
	cout<<"TP "<<TP<<endl;
	cout<<"TN "<<TN<<endl;
	cout<<"FP "<<FP<<endl;
	cout<<"FN "<<FN<<endl;
	float accuracy = (float)(TP+TN)*100/(TP+FN+TN+FP);
	cout<<accuracy<<"%"<<endl;
}

int main(){
	// find pF - P(F)
	string str1 = "facedatatrainlabels";
	string str2 = "facedatatrain";
	pF = findpf(str1);
	pNF = 1-pF;
	//cout<<pF<<" "<<pNF<<endl;
	/*Divide into patches and work with each to determine the prior*/
	int f= calculatePriorF(str1, str2);
	int nf = calculatePriorNF(str1, str2); 

	int i,j;
/*for(i=0;i<70;i++){
for(j=0;j<60;j++){
cout<<probarrayNF[i][j]<<" ";
}
cout<<endl;
}*/

	//Testing the data -  read each patch, classify.
	calculatePost(); 
	//Find errors
	confusionMatrix();
	return 0;
}
