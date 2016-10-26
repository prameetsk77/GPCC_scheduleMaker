#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <set>

#define DEBUG 0

using namespace std;

class Fields{
public:

	Fields(){isempty = true;};

	Fields(const string& block){
		string line;
		stringstream blockStream(block);

		getline(blockStream, line);
		time = line.substr(0, line.find("with"));

		hour = stoi(line.substr(0, line.find(":")), NULL);
		min = stoi(line.substr(line.find(":") + 1, 2), NULL);

		if(hour < 8)
			hour = hour + 12;

		if(time.find("AM") != string::npos)
			isAM = 1;
		else if(time.find("PM") != string::npos)
			isAM =0;
		else 
			throw runtime_error("AM or PM not found");

		name = line.substr(line.find("with") + 5);

		#if DEBUG
		cout << time << " i got \t " << "|"<< hour << ":"<< min << "| " <<  flush;
		if(isAM)
			cout << "AM"<< flush;
		else
			cout << "PM"<< flush;
		cout << " name" << name << endl;
		#endif

		isempty = false;
		this->block = block;

		elapsed_time = hour* 100 + min;
	}

	bool operator<(const Fields& rhs) const {
		if(elapsed_time != rhs.elapsed_time)
			return elapsed_time < rhs.elapsed_time;

		return greaterName(rhs);
	}

	void makeSchedule(string fileName);

	bool isfilled(void){
		return !isempty;
	}

	bool setFilled(bool val){
		isempty = !val;
	}

	string getName()const{
		return name;
	}
	string getTime()const {
		return time;
	}

	int gethour() const {return hour;}

	int getmin() const {return min;}

	string getBlock() const {return block;}
private:

	bool isempty;
	string time;

	string name;

	int hour;
	int min;
	int isAM;

	unsigned int elapsed_time;

	string block;

	void getTime(const char* line);

	bool greaterName(const Fields& rhs) const{
		int cmp = name.compare(rhs.name);
		if(cmp <= 0)
			return true;
		else
			return false;
	}

};

class Schedule{
public:

	Schedule(const string& name){

		fileStream.open(name, ios::in);
		if (!fileStream.is_open()){
			throw runtime_error("ERROR: File could not be opened");
			return;
		}
		readSchedule();

		set_iterator = fieldSet.begin();
	};

	~Schedule(){
		fileStream.close();
	}

	void print(ostream& os)const{
		for(auto itr = fieldSet.begin(); itr!=fieldSet.end(); itr++){ 
				os << "|" << (*itr).getName() << "|" <<  " at time " << (*itr).getTime() << endl;
				//os << "hour: " << (*itr).gethour() << " min: " << (*itr).getmin() << endl;
			}
	}

	void merge(Schedule& toMerge){

		Fields toAdd;
		toMerge.getNext(toAdd);
		while(toAdd.isfilled()){
			#if DEBUG
			cout << "inserting " << toAdd.getBlock() << endl;
			#endif
			fieldSet.insert(toAdd);
			toMerge.getNext(toAdd);
		}
	}

	void getNext(Fields& toAdd) {
		if(set_iterator == fieldSet.end()){
			toAdd.setFilled(false);
			return;
		}

		toAdd = *set_iterator;
		set_iterator++;
		return;
	}

	void makeSchedule(string& fileName);
private:
	multiset<Fields>::iterator set_iterator;
	ifstream fileStream;
	set<Fields> fieldSet;

	void readSchedule(void);

};

void Schedule::readSchedule(void){
	int eofCheck = fileStream.peek();
	if(eofCheck == EOF){
		throw runtime_error("file content empty");
		return;
	}
	while(!fileStream.eof()){
		int lastChar;
		string line;
		stringstream fieldStream;
		int i;

		do{
			getline(fileStream, line);
			i++;
			fieldStream << line ;
			fieldStream << "\n" ;
			lastChar = fileStream.peek();
		}while(lastChar != '\n' && lastChar != EOF);
		if(lastChar == '\n'){
			getline(fileStream, line);
			fieldStream << "\n" ;
		}

		if(i < 5)
			continue;

		#if DEBUG
		cout << "*****************************************************************\n";
		cout << fieldStream.str();
		#endif

		Fields newfield(fieldStream.str());
		fieldSet.insert(newfield);
		

		#if DEBUG
		cout << "fieldSet.size() " <<fieldSet.size() << endl;
		#endif

		i=0;
	}
}

void Schedule::makeSchedule(string& fileName){
	ofstream newFile;
	newFile.open(fileName, ios::app);

	if(!newFile.is_open())
		throw runtime_error("cannot make new file");

	for(auto itr = fieldSet.begin(); itr != fieldSet.end(); itr++)
		newFile << (*itr).getBlock();

	newFile.close();
}


int main(int argc, char** argv){

	cout << "\nThis program can sort, interleave schedule and also merge schedules." << endl;
	int num;

	cout << "What is the name of the first file you want to merge ?" << endl;
	string firstFile;

	cin >> firstFile;
	Schedule firstSchedule(firstFile);

	cout << "How many files do you want to sort/merge ? " << endl;

	cin >> num;

	for(int i=1;i<num; i++){
		string fileName;
		cout << "File " << i+1 << " name ?" << endl;
		cin >> fileName;
		Schedule schedules(fileName);
		firstSchedule.merge(schedules);
	}

	string op_File = "ScheduleOutput.txt";
	firstSchedule.makeSchedule(op_File);

	cout << "\nMerge Done. Check the file- " << op_File << " for output." << endl;

	// Schedule givenSchedule("schedule.txt");
	// givenSchedule.print(cout);
	// cout << "************************************************\n"<<endl;
	// Schedule givenSchedule2("schedule2.txt");
	// givenSchedule2.print(cout);
	// cout << "************************************************\n"<<endl;

	// givenSchedule.merge(givenSchedule2);
	// givenSchedule.print(cout);

	// string opFile = "opFile.txt";

	// givenSchedule.makeSchedule(opFile);

}