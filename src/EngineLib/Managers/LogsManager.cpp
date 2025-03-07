#include "pch.h"
#include "LogsManager.h"

void LogsManager::CreateLogDirectory() const {
	try {
		if (std::filesystem::create_directories(m_FilePath)) {
			std::cout << "LogsManager(CreateDir): The directory \"" << m_FilePath << "\" is created." << std::endl;
		} else {
			std::cerr << "LogsManager(CreateDir): The directory \"" << m_FilePath << "\" already exists or cannot be created." << std::endl;
		}
	} catch (const std::exception& ex) {
		std::cerr << "LogsManager(): Exception during directory creation: " << ex.what() << std::endl;
	}
}


void LogsManager::WriteLineToFile(const std::string& line, const bool& append) {

#ifdef _DEBUG
	std::ofstream logFile;
	if (append) { //Append at end of the file
		logFile.open(m_FilePath + m_FileName, std::ios::app);
	}
	else { //Write a new file
		logFile.open(m_FilePath + m_FileName);
	}
	
	if (!logFile) { //Error to read the file
		std::cerr << "LogsManager(Write): Cannot open the logs file \"" << m_FilePath << m_FileName << "\"." << std::endl;

		if (m_TryToCreateDirectory) { //If already tried to create the directory
			std::cerr << "LogsManager(Write): Cannot create the directory and the logs file at \"" << m_FilePath << m_FileName << "\"." << std::endl;
			return;
		}

		m_TryToCreateDirectory = true; //Set the flag to true
		CreateLogDirectory(); //Create the directory
		
		WriteLineToFile(line, append); //Retry to write the line
		
		return;
	}
	
	logFile << line; //Write the line in the file
	//std::cout << "LogsManager(Write): Line:  \"" << line << "\" written in the logs file." << std::endl;
	
	logFile.close();  //Close the file

#endif
}


std::string LogsManager::ReadFile() const {

	std::ifstream file(m_FilePath + m_FileName);
	std::string extractedLines;
	
	if (!file) { //If not able to open the file
		std::cerr << "LogsManager(Read): Cannot open the logs file \"" << m_FilePath << m_FileName << "\"." << std::endl;
	}

	while (std::getline(file, extractedLines)) { //Read the file line by line
		std::cout << "LogsManager(Read): Line:  \"" << extractedLines << "\" read from the logs file." << std::endl;
	}

	file.close(); //Close the file
	
	return extractedLines;
}

void LogsManager::RemoveLogFile() {
	if (std::filesystem::remove(m_FilePath + m_FileName)) { //Remove the file
		std::cout << "LogsManager(Remove): The logs file \"" << m_FilePath << m_FileName << "\" is removed." << std::endl;
		m_TryToCreateDirectory = false; //Reset the flag
	} else {
		std::cerr << "LogsManager(Remove): Cannot remove the logs file \"" << m_FilePath << m_FileName << "\"." << std::endl;
	}	
}