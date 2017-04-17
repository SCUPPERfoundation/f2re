
#include <fstream>
#include <iostream>
#include <string>

#include <unistd.h>

std::string GetNextDirectory(const std::string& dir);
bool			OutputOwnerLine(const std::string& file_name,const std::string& star_name);

int main()
{
	char c_dir[256];
	std::cout << getcwd(c_dir,256) << std::endl;
	std::string	dir(c_dir);

	std::string next_dir;
	for(;;)
	{
		next_dir = GetNextDirectory(dir);
		if(next_dir == "-end-")
			break;
		std::string next_file = next_dir + "/space.inf";
		OutputOwnerLine(next_file,next_dir);
	}
	return 0;
}

std::string GetNextDirectory(const std::string& dir)
{
	static std::ifstream	dir_file;
	static bool flag = false;
	if(!flag)
	{
		std::string file_name = dir + "/dirs.txt";
		dir_file.open(file_name.c_str());
		if(!dir_file)
			return "-end-";
		flag = true;
	}

	char dir_name[100];
	dir_file.getline(dir_name,100);
	std::string dir_str(dir_name);
	if(dir_str.compare("-end-") == 0)
	{
		dir_file.close();
		return dir_str;
	}

	dir_str.insert(0,"/");
	dir_str.insert(0,dir);
	return(dir_str);
}

bool	OutputOwnerLine(const std::string& file_name,const std::string& star_name)
{

	std::ifstream	inf_file(file_name.c_str());
	if(!inf_file)
	{
		std::cerr << "***" << file_name << " not found!***\n";
		return(false);
	}

	char xml_text[512];
	inf_file.getline(xml_text,512);	// skip first line - xml start stuff
	inf_file.getline(xml_text,512);	// this is the one we want!
	std::cout << star_name << "   " << xml_text << "\n";
	return true;
}

