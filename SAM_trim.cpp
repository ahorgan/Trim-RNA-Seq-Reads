#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<stdlib.h>
using namespace std;

int main(int agrc, char **argv)
{
	ifstream inFile;
	inFile.open(argv[1]);
	int track = 0;
	string buffer;
	string trimmed_read;
	string md_field;
	vector<int> A;
	int *sum;
	int *backtrack;
	int pos, m_pos;
	int start, size;

	while(inFile >> buffer)
	{
		//CIGAR string
		if(track % 19 == 5)
		{
			pos = buffer.find_first_of("SM");
			if(buffer[pos] == 'M')
			{
				start = 0;
				size = atoi((buffer.substr(0, pos + 1)).c_str());
			}
			else
			{
				start = atoi((buffer.substr(0, pos + 1)).c_str());
				m_pos = buffer.find_first_of('M');
				size = atoi((buffer.substr(pos + 1, m_pos - pos - 1)).c_str());
			}
		}
		// trim read using CIGAR
		else if(track % 19 == 9)
		{
			trimmed_read = buffer.substr(start, size);
		}
		// convert MD to array A
		else if(track % 19 == 17)
		{
			A.clear();
			md_field = buffer.substr(5);
			//cout << md_field << endl;
			pos = 0;
			int i = 0;
			while((pos = md_field.find_first_of("ACGT", i)) != string::npos)
			{
				A.push_back(atoi((md_field.substr(i, pos - i)).c_str()));
				i = pos + 1;
				A.push_back(-5);
			}
			A.push_back(atoi((md_field.substr(i)).c_str()));
			/*for(int i = 0; i < A.size(); i++)
				cout << A[i] << " ";
			cout << endl;*/
			// dynamic programming algorithm
			sum = new int[A.size()];
			backtrack = new int[A.size()];
			sum[0] = A[0];
			backtrack[0] = 0;

			// fill table
			for(int i = 1; i < A.size(); i++)
			{
				sum[i] = sum[i-1] + A[i];
				if(A[i] > sum[i])
				{
					sum[i] = A[i];
					backtrack[i] = i;
				}
				else
					backtrack[i] = backtrack[i-1];

			}
			/*for(int i = 0; i < A.size(); i++)
				cout << sum[i] << ":" << backtrack[i] << "  ";
			cout << endl;*/
			// find max in table
			int max = sum[0];
			int index = 0;
			for(int i = 1; i < A.size(); i++)
			{
				if(sum[i] > max)
				{
					max = sum[i];
					index = i;
				}
			}
			pos = 0;
			for(int i = 0; i < backtrack[index]; i++)
			{
				if(A[i] == -5)
					pos++;
				else
					pos += A[i];
			}
			size = 0;
			for(int i = backtrack[index]; i <= index; i++)
			{
				if(A[i] == -5)
					size++;
				else
					size += A[i];
			}

			if(size >= 25)
			{
				trimmed_read = trimmed_read.substr(pos, size);
				cout << trimmed_read << endl;
			}
		}
		track++;
	}
	inFile.close();
	return 0;
}
