#pragma once

class LogsManager {
	
//##############################################################################
//##------------------------------- ATTRIBUTES -------------------------------##
//##############################################################################

private:

	std::string m_FileName;
	std::string m_FilePath;
	bool m_TryToCreateDirectory = false;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################
	
public:

/*----------< CONSTRUCTORS >----------*/

	LogsManager() = default;
	/** Constructor of LogsManager class */
	/** @param fileName: The name + extension of the file to create, eg: "logs.txt" */
	/** @param finalFilePath: The final path of the file, eg: "test" \n => WizProductions/test/fileName.txt */
	explicit LogsManager(std::string fileName, const std::string& finalFilePath = "") :
			m_FileName(std::move(fileName))
	{
		m_FilePath = MiscUtils::GetAppDataFolder() + MiscUtils::m_CompanyAppFolder + finalFilePath;
		m_FileName = R"(\)" + m_FileName; // Add the backslash to the file name
		std::cout << "LogsManager(): Target file location is: " << m_FilePath << m_FileName << std::endl;
	}
	~LogsManager() = default;

/*------------------------------------*/

	
/* GETTERS */

/* SETTERS */



/* OTHERS FUNCTIONS */

	/** Creat the directory where the log file will be stored. */
	void CreateLogDirectory() const;

	/** Write a line in the log file @param append: write at end of the file.  */
	void WriteLineToFile(const std::string& line, const bool& append = true);

	/** Read the content of the file at store it in the std::string. */
	std::string ReadFile() const;

	/** Remove the log file. */
	void RemoveLogFile();
	

};


/*
Lire:

- caractère : get()
- line : getline()
- mot par mot : >>
- tout
# pouvoir bouger le curseur


Ecrire:
- texte par texte
# pouvoir append si besoin

open
close
valid
*/